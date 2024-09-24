/*
 Copyright (C) 2023 Skandinaviska Enskilda Banken AB (publ)
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

#include <algorithm>
#include <numeric>
#include <ql/time/schedule.hpp>
#include <qle/termstructures/blackvolsurfacewithtimeweight.hpp>

namespace QuantExt {

BlackVolatilityWithTimeWeighting::BlackVolatilityWithTimeWeighting(
    const QuantLib::ext::shared_ptr<BlackVolTermStructure>& surface, const std::vector<Date>& skewDates,
    const std::unordered_map<Weekday, Real>& weekdayWeights, const std::unordered_map<Date, Real>& eventWeights)
    : surface_(surface), skewDates_(skewDates), weekdayWeights_(weekdayWeights), eventWeights_(eventWeights) {}

Volatility BlackVolatilityWithTimeWeighting::blackVolImpl(Time t, Real strike) const {
    // Not a neat conversion but to work with the weekday weights (especially) and event dates,
    // having t as a QL::Date makes for nicer handling below.
    Date expiryDate = referenceDate() + std::round(t * 365) * Days;

    // std::vector<Time> skewTimes;
    // std::transform(skewDates_.begin(), skewDates_.end(), skewTimes.begin(),
    //                [this](Date d) { return timeFromReference(d); });

    if (std::find(skewDates_.begin(), skewDates_.end(), expiryDate) != skewDates_.end())
        return surface_->blackVol(t, strike);

    if (expiryDate <= skewDates_.front()) {
        // Date firstSkewDate = skewDates_.front();
        return surface_->blackVol(t, strike); // * W(0, t) / W(0, T1);
    } else if (expiryDate >= skewDates_.back()) {
        // Date maxSkewDate = skewDates_.back();
        return surface_->blackVol(t, strike); // * W(0, t) / W(0, TN);
    } else {
        auto nextSkewIndex =
            std::distance(skewDates_.begin(), std::lower_bound(skewDates_.begin(), skewDates_.end(), expiryDate));
        QL_REQUIRE(nextSkewIndex != 0, "Next skew's index cannot be 0.");
        Date prevSkewDate = skewDates_[nextSkewIndex - 1];
        Date nextSkewDate = skewDates_[nextSkewIndex];
        if (prevSkewDate == expiryDate)
            return surface_->blackVol(prevSkewDate, strike);

        Real tWeight = accruedWeightBetween(prevSkewDate + Period(1, Days), expiryDate) /
                       accruedWeightBetween(prevSkewDate + Period(1, Days), nextSkewDate);
        Real prevVariance = surface_->blackVariance(prevSkewDate, strike);
        Real nextVariance = surface_->blackVariance(nextSkewDate, strike);
        return std::sqrt((prevVariance * (1 - tWeight) + nextVariance * tWeight) / t);
    }
}

Real BlackVolatilityWithTimeWeighting::accruedWeightBetween(Date prevDate, Date endDate) const {
    if (prevDate == endDate)
        return eventWeights_.count(prevDate) == 1 ? eventWeights_.at(prevDate) : weekdayWeights_.at(prevDate.weekday());

    std::vector<Date> dates;
    dates.reserve(endDate - prevDate + 1);
    for (int i = 0; i < endDate - prevDate + 1; ++i) {
        dates.push_back(prevDate + i * Days);
    }
    Real accruedWeight = std::accumulate(dates.begin(), dates.end(), Real(0.0), [&](Real accWeight, const Date d) {
            return accWeight + (eventWeights_.count(d) == 1 ? eventWeights_.at(d) : weekdayWeights_.at(d.weekday()));
        });

    return accruedWeight;
}

} // namespace QuantExt