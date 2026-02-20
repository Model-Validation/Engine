/*
 Copyright (C) 2026 Quaternion Risk Management Ltd
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

#include <qle/indexes/inflationindexwrapper.hpp>
#include <qle/termstructures/inflation/mixedinflationhelpers.hpp>

#include <ql/indexes/inflationindex.hpp>
#include <ql/pricingengines/swap/discountingswapengine.hpp>
#include <ql/utilities/null_deleter.hpp>
#include <ql/termstructures/yield/flatforward.hpp>
#include <ql/termstructures/inflationtermstructure.hpp>
#include <ql/termstructures/bootstraphelper.hpp>
#include <utility>
#include <boost/smart_ptr/make_shared_object.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <ql/errors.hpp>
#include <ql/handle.hpp>
#include <ql/instruments/swap.hpp>
#include <ql/instruments/yearonyearinflationswap.hpp>
#include <ql/quote.hpp>
#include <ql/termstructures/yieldtermstructure.hpp>
#include <ql/time/businessdayconvention.hpp>
#include <ql/time/calendar.hpp>
#include <ql/time/calendars/nullcalendar.hpp>
#include <ql/time/date.hpp>
#include <ql/time/dategenerationrule.hpp>
#include <ql/time/daycounter.hpp>
#include <ql/time/daycounters/actual365fixed.hpp>
#include <ql/time/period.hpp>
#include <ql/time/schedule.hpp>
#include <ql/time/timeunit.hpp>
#include <ql/types.hpp>

namespace QuantExt {

using namespace QuantLib;

MixedYearOnYearInflationSwapHelper::MixedYearOnYearInflationSwapHelper(
    const Handle<Quote>& quote, const Period& swapObsLag, const Date& maturity, Calendar calendar,
    BusinessDayConvention paymentConvention, DayCounter dayCounter, const ext::shared_ptr<ZeroInflationIndex>& zii,
    CPI::InterpolationType interpolation)
    : RelativeDateBootstrapHelper<ZeroInflationTermStructure>(quote, false), swapObsLag_(swapObsLag),
      maturity_(maturity), calendar_(std::move(calendar)), paymentConvention_(paymentConvention),
      dayCounter_(std::move(dayCounter)), interpolation_(interpolation),
      nominalTermStructure_(std::move( // any nominal term structure will give the same result;
                                       // when calculating the fair rate, the equal discount factors
                                       // for the payments on the two legs will cancel out.
          Handle<YieldTermStructure>(ext::make_shared<FlatForward>(0, NullCalendar(), 0.0, Actual365Fixed())))) {

    auto ziiClone = zii->clone(termStructureHandle_);
    ziiClone->unregisterWith(termStructureHandle_);

    yii_ = ext::make_shared<YoYInflationIndexWrapper>(ziiClone);

    if (detail::CPI::isInterpolated(interpolation_, yii_)) {
        Period pShift(yii_->frequency());
        QL_REQUIRE(swapObsLag_ - pShift >= zii->availabilityLag(),
                   "inconsistency between swap observation lag "
                       << swapObsLag_ << ", index period " << pShift << " and index availability "
                       << zii->availabilityLag() << ": need (obsLag-index period) >= availLag");
    }

    registerWith(yii_);
    registerWith(nominalTermStructure_);
    MixedYearOnYearInflationSwapHelper::initializeDates();
}

Real MixedYearOnYearInflationSwapHelper::impliedQuote() const {
    yyiis_->deepUpdate();
    return yyiis_->fairRate();
}

void MixedYearOnYearInflationSwapHelper::initializeDates() {
    //! The start date of the swap is calculated by advance back one year from maturity date.
    //! The latest date is then calculated by advancing one year from start.
    Date startDate = calendar_.advance(maturity_, Period(-1, Years), paymentConvention_);
    Date endDate = calendar_.advance(startDate, Period(1, Years), paymentConvention_);
    Schedule schedule =
        Schedule({startDate, endDate}); // the schedule is simple enough and they are the same for both legs

    yyiis_ = ext::make_shared<YearOnYearInflationSwap>(
        Swap::Payer, 1000000.0, schedule, quote().empty() || !quote()->isValid() ? 0.0 : quote()->value(), dayCounter_,
        schedule, yii_, swapObsLag_, interpolation_, 0.0, dayCounter_, calendar_, paymentConvention_);
    //! The swap schedule defines cashflow timing and payment dates. For bootstrapping, the dates need to take the 
    //! observation lag into account to be inline with the logic of ZeroCouponInflationSwapHelper.
    auto fixingPeriod = inflationPeriod(maturity_ - swapObsLag_, yii_->frequency());
    earliestDate_ = latestDate_ = fixingPeriod.first;
    yyiis_->setPricingEngine(ext::make_shared<DiscountingSwapEngine>(nominalTermStructure_));
}

void MixedYearOnYearInflationSwapHelper::setTermStructure(ZeroInflationTermStructure* z) {
    bool observer = false;

    ext::shared_ptr<ZeroInflationTermStructure> temp(z, null_deleter());
    termStructureHandle_.linkTo(std::move(temp), observer);

    RelativeDateBootstrapHelper<ZeroInflationTermStructure>::setTermStructure(z);
}

} // namespace QuantExt
