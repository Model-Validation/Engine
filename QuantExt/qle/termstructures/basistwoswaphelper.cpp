/*
 Copyright (C) 2016 Quaternion Risk Management Ltd
 All rights reserved.

 This file is part of ORE, a free-software/open-source library
 for transparent pricing and risk analysis - http://opensourcerisk.org

 ORE is free software: you can redistribute it and/or modify it
 under the terms of the Modified BSD License.  You should have received a
 copy of the license along with this program.
 The license is also available online at <http://opensourcerisk.org>

 This program is distributed on the basis that it will form a useful
 contribution to risk analytics and model standardisation, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. See the license for more details.
*/

#include <ql/cashflows/floatingratecoupon.hpp>
#include <ql/cashflows/iborcoupon.hpp>
#include <ql/instruments/makevanillaswap.hpp>
#include <ql/utilities/null_deleter.hpp>
#include <qle/termstructures/basistwoswaphelper.hpp>

namespace QuantExt {

BasisTwoSwapHelper::BasisTwoSwapHelper(const Handle<Quote>& spread, const Period& swapTenor, const Calendar& calendar,
                                       // Long tenor swap
                                       Frequency longFixedFrequency, BusinessDayConvention longFixedConvention,
                                       const DayCounter& longFixedDayCount,
                                       const QuantLib::ext::shared_ptr<IborIndex>& longIndex, bool longIndexGiven,
                                       // Short tenor swap
                                       Frequency shortFixedFrequency, BusinessDayConvention shortFixedConvention,
                                       const DayCounter& shortFixedDayCount,
                                       const QuantLib::ext::shared_ptr<IborIndex>& shortIndex, bool longMinusShort,
                                       bool shortIndexGiven,
                                       // Discount curve
                                       const Handle<YieldTermStructure>& discountingCurve, bool discountCurveGiven)
    : RelativeDateRateHelper(spread), swapTenor_(swapTenor), calendar_(calendar),
      longFixedFrequency_(longFixedFrequency), longFixedConvention_(longFixedConvention),
      longFixedDayCount_(longFixedDayCount), longIndex_(longIndex), longIndexGiven_(longIndexGiven),
      shortFixedFrequency_(shortFixedFrequency), shortFixedConvention_(shortFixedConvention),
      shortFixedDayCount_(shortFixedDayCount), shortIndex_(shortIndex), longMinusShort_(longMinusShort),
      shortIndexGiven_(shortIndexGiven), discountHandle_(discountingCurve), discountCurveGiven_(discountCurveGiven) {

    QL_REQUIRE(longIndex_->tenor() >= shortIndex_->tenor(),
               "Tenor of longIndex should be at least tenor of shortIndex.");

    QL_REQUIRE(!(longIndexGiven_ && shortIndexGiven_ && discountCurveGiven_),
               "BasisTwoSwaphelper: Have all curves nothing to solve for.");

    if (longIndexGiven_ && !shortIndexGiven_) {
        shortIndex_ = shortIndex_->clone(termStructureHandle_);
        shortIndex_->unregisterWith(termStructureHandle_);
    } else if (!longIndexGiven_ && shortIndexGiven_) {
        longIndex_ = longIndex_->clone(termStructureHandle_);
        longIndex_->unregisterWith(termStructureHandle_);
    } else if (!longIndexGiven_ && !shortIndexGiven_) {
        QL_FAIL("Need at least one of the indices to have a valid curve.");
    }

    registerWith(longIndex_);
    registerWith(shortIndex_);
    registerWith(discountHandle_);
    initializeDates();
}

void BasisTwoSwapHelper::initializeDates() {

    /* Important to use a fixed rate of 0.0 here to avoid the calculation
       of the atm swap rate in MakeVanillaSwap operator ...(). If it is
       Null, you get an exception because the discountRelinkableHandle_
       is initially empty. */
    longSwap_ = MakeVanillaSwap(swapTenor_, longIndex_, 0.0)
                    .withDiscountingTermStructure(discountRelinkableHandle_)
                    .withFixedLegDayCount(longFixedDayCount_)
                    .withFixedLegTenor(Period(longFixedFrequency_))
                    .withFixedLegConvention(longFixedConvention_)
                    .withFixedLegTerminationDateConvention(longFixedConvention_)
                    .withFixedLegCalendar(calendar_)
                    .withFloatingLegCalendar(calendar_);

    shortSwap_ = MakeVanillaSwap(swapTenor_, shortIndex_, 0.0)
                     .withDiscountingTermStructure(discountRelinkableHandle_)
                     .withFixedLegDayCount(shortFixedDayCount_)
                     .withFixedLegTenor(Period(shortFixedFrequency_))
                     .withFixedLegConvention(shortFixedConvention_)
                     .withFixedLegTerminationDateConvention(shortFixedConvention_)
                     .withFixedLegCalendar(calendar_)
                     .withFloatingLegCalendar(calendar_);

    earliestDate_ = std::min(longSwap_->startDate(), shortSwap_->startDate());
    latestDate_ = std::max(longSwap_->maturityDate(), shortSwap_->maturityDate());

/* May need to adjust latestDate_ if you are projecting libor based
   on tenor length rather than from accrual date to accrual date. */
    if (!IborCoupon::Settings::instance().usingAtParCoupons()) {
        if (termStructureHandle_ == shortIndex_->forwardingTermStructure()) {
            QuantLib::ext::shared_ptr<FloatingRateCoupon> lastFloating =
                QuantLib::ext::dynamic_pointer_cast<FloatingRateCoupon>(shortSwap_->floatingLeg().back());
            Date fixingValueDate = shortIndex_->valueDate(lastFloating->fixingDate());
            Date endValueDate = shortIndex_->maturityDate(fixingValueDate);
            latestDate_ = std::max(latestDate_, endValueDate);
        }
        if (termStructureHandle_ == longIndex_->forwardingTermStructure()) {
            QuantLib::ext::shared_ptr<FloatingRateCoupon> lastFloating =
                QuantLib::ext::dynamic_pointer_cast<FloatingRateCoupon>(longSwap_->floatingLeg().back());
            Date fixingValueDate = longIndex_->valueDate(lastFloating->fixingDate());
            Date endValueDate = longIndex_->maturityDate(fixingValueDate);
            latestDate_ = std::max(latestDate_, endValueDate);
        }
    }
}

void BasisTwoSwapHelper::setTermStructure(YieldTermStructure* t) {

    bool observer = false;

    QuantLib::ext::shared_ptr<YieldTermStructure> temp(t, null_deleter());
    termStructureHandle_.linkTo(temp, observer);

    if (!discountCurveGiven_)
        discountRelinkableHandle_.linkTo(temp, observer);
    else
        discountRelinkableHandle_.linkTo(*discountHandle_, observer);

    RelativeDateRateHelper::setTermStructure(t);
}

Real BasisTwoSwapHelper::impliedQuote() const {
    QL_REQUIRE(termStructure_ != 0, "Termstructure not set");
    longSwap_->deepUpdate();
    shortSwap_->deepUpdate();
    if (longMinusShort_)
        return longSwap_->fairRate() - shortSwap_->fairRate();
    else
        return shortSwap_->fairRate() - longSwap_->fairRate();
}

void BasisTwoSwapHelper::accept(AcyclicVisitor& v) {
    Visitor<BasisTwoSwapHelper>* v1 = dynamic_cast<Visitor<BasisTwoSwapHelper>*>(&v);
    if (v1 != 0)
        v1->visit(*this);
    else
        RateHelper::accept(v);
}
} // namespace QuantExt
