/*
 Copyright (C) 2026 Skandinaviska Enskilda Banken AB (publ)
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

#include <qle/termstructures/fxforwardyieldtermstructure.hpp>
#include <qle/utilities/time.hpp>
#include <ql/time/calendars/jointcalendar.hpp>

using namespace std;
using namespace QuantLib;

namespace QuantExt {

FxForwardYieldTermStructure::FxForwardYieldTermStructure(
    const QuantLib::ext::shared_ptr<PriceTermStructure>& fxForwardCurve,
    const QuantLib::ext::shared_ptr<QuantLib::YieldTermStructure>& foreignDiscount,
    const QuantLib::Handle<QuantLib::Quote>& fxSpotQuote, bool invertedQuotation,
    const QuantLib::Calendar& advanceCalendar, const QuantLib::Calendar& tradingCaledar, QuantLib::Natural spotDays,
    QuantLib::BusinessDayConvention bdc)
    : priceCurve_(fxForwardCurve), discount_(foreignDiscount), fxSpotQuote_(fxSpotQuote),
      invertedQuotation_(invertedQuotation), advanceCalendar_(advanceCalendar), tradingCalendar_(tradingCaledar),
      spotDays_(spotDays), bdc_(bdc) {
    QL_REQUIRE(
        priceCurve_->referenceDate() == discount_->referenceDate(),
        "PriceTermStructureAdapter: The reference date of the discount curve and price curve should be the same");

    registerWith(priceCurve_);
    registerWith(discount_);
    registerWith(fxSpotQuote_);
}

Date FxForwardYieldTermStructure::maxDate() const {
    // Take the min of the two underlying curves' max date
    // Extrapolation will be determined by each underlying curve individually
    return min(priceCurve_->maxDate(), discount_->maxDate());
}

const Date& FxForwardYieldTermStructure::referenceDate() const {
    QL_REQUIRE(
        priceCurve_->referenceDate() == discount_->referenceDate(),
        "PriceTermStructureAdapter: The reference date of the discount curve and price curve should be the same");
    return priceCurve_->referenceDate();
}

DayCounter FxForwardYieldTermStructure::dayCounter() const { return priceCurve_->dayCounter(); }

const QuantLib::ext::shared_ptr<PriceTermStructure>& FxForwardYieldTermStructure::priceCurve() const {
    return priceCurve_;
}

const QuantLib::ext::shared_ptr<QuantLib::YieldTermStructure>& FxForwardYieldTermStructure::discountCurve() const {
    return discount_;
}
const QuantLib::Handle<QuantLib::Quote>& FxForwardYieldTermStructure::fxSpotQuote() const { return fxSpotQuote_; }

bool FxForwardYieldTermStructure::invertedQuotation() const { return invertedQuotation_; }

const QuantLib::Calendar& FxForwardYieldTermStructure::advanceCalendar() const { return advanceCalendar_; }

const QuantLib::Calendar& FxForwardYieldTermStructure::tradingCalendar() const { return tradingCalendar_; }

QuantLib::Natural FxForwardYieldTermStructure::spotDays() const { return spotDays_; }

QuantLib::BusinessDayConvention FxForwardYieldTermStructure::businessDayConvention() const { return bdc_; }

DiscountFactor FxForwardYieldTermStructure::discountImpl(Time t) const {
    if (t == 0.0)
        return 1.0;

    Time tBound = t;
    Time tMin = priceCurve()->minTime();
    Time tMax = priceCurve()->maxTime();

    Date spotDate = advanceCalendar().advance(referenceDate(), spotDays() * Days, businessDayConvention());
    spotDate = JointCalendar(advanceCalendar(), tradingCalendar()).adjust(spotDate, businessDayConvention());
    Time tSpot = timeFromReference(spotDate);

    Real discountedSpotRate;
    if (tMin == 0) {
        // The spot date is contained within the underlying FX forward price curve, so we can more
        // easily determine the discounted FX spot rate (today's rate/cash rate)
        discountedSpotRate = priceCurve()->price(0, false);
    } else {
        // In this case, tMin > tSpot, so we need to determine the zero rate between these two,
        // then use it to discount the spot price back to the reference date.
        DiscountFactor foreignDfTSpot = discountCurve()->discount(spotDate, true);
        DiscountFactor foreignDfTMin = discountCurve()->discount(lowerDate(tMin, referenceDate(), dayCounter()), true);
        Real forwardPriceAtTMin = priceCurve()->price(tMin, false);
        Real fxSpot = fxSpotQuote()->value();

        // Calculate the domestic Df
        DiscountFactor domesticDfTMinToTSpot =
            (invertedQuotation() ? forwardPriceAtTMin / fxSpot : fxSpot / forwardPriceAtTMin) *
            foreignDfTSpot / foreignDfTMin;
        Rate zeroRate = -std::log(domesticDfTMinToTSpot) / (tSpot - tMin);
        discountedSpotRate = fxSpotQuote()->value() * std::exp(-zeroRate * tSpot) / foreignDfTSpot;
        if (t < tMin)
            return std::exp(-zeroRate * t); // Return early with flat extrapolation
    }

    if (tBound > tMax)
        tBound = tMax;
    // Reversing the conversion to year fraction below since the discount curve may have a different DC method
    DiscountFactor discount = discountCurve()->discount(lowerDate(tBound, referenceDate(), dayCounter()), true);
    Real forwardPrice = priceCurve()->price(tBound, true);
    DiscountFactor resultDf =
        discount * (invertedQuotation() ? discountedSpotRate / forwardPrice : forwardPrice / discountedSpotRate);

    if (t > tMax) {
        // todo handle the flat zero extrapolation for rate retrieval beyond the last FX forward pillar
        Real flatZeroRate = -std::log(resultDf) / (tBound);
        resultDf = std::exp(-flatZeroRate * t);
    }
    return resultDf;
}

} // namespace QuantExt
