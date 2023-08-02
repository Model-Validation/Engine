/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2022 Skandinaviska Enskilda Bankan AB (publ)

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/license.shtml>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#include <qle/termstructures/svivolsurface.hpp>
#include <algorithm>
// #include <iostream>

using namespace std;
using namespace QuantLib;

namespace QuantExt {
    SviVolSurface::SviVolSurface(const Date& referenceDate,
                                 const Handle<Quote>& spot,
                                 const Handle<YieldTermStructure>& riskFreeTS,
                                 const Handle<YieldTermStructure>& dividendTS,
                                 const std::vector<Date>& expiries,
                                 const std::vector<std::vector<double> >& smileSectionParameterSets,
                                 const Calendar& cal,
                                 BusinessDayConvention bdc,
                                 const DayCounter& dc)
    : BlackVarianceTermStructure(referenceDate, cal, bdc, dc), spot_(spot), rf_(riskFreeTS),
      div_(dividendTS), expiries_(expiries), smileSectionParameterSets_(smileSectionParameterSets) {
        QL_REQUIRE(expiries_.size() > 0, "VolatilitySviSurfaceConfig expects one or more expiries");
        QL_REQUIRE(
            smileSectionParameterSets_.size() > 0,
            "VolatilitySviSurfaceConfig expects one or more SVI smile section parameter sets");
        QL_REQUIRE(expiries_.size() == smileSectionParameterSets_.size(),
                   "Expected an equal amount of expiries andSVI smile section parameter sets");
        QL_REQUIRE(std::is_sorted(expiries_.begin(), expiries_.end()),
                   "The SviSmileSections must be sorted by expiry, in increasing order");

        for (Size i = 0; i < expiries_.size(); ++i) {
            smileSections_.push_back(ext::make_shared<SviSmileSection>(
                expiries[i], forward(expiries[i]), smileSectionParameterSets_[i], dayCounter()));
            expiryTimes_.push_back(smileSections_[i]->exerciseTime());
        }
    }

    Real SviVolSurface::blackVarianceImpl(Time t, Real strike) const {
        // Hard-coded variance term interpolation

        Time t1;
        Time t2;
        Real var_t1;
        Real var_t2;
        Real var_t;
        Real moneyness = strike / (spot_->value() * div_->discount(t) / rf_->discount(t));
        // cout << endl;
        // cout << "SVIVol -- time: " << t << ", strike: " << strike << ", moneyness: " << moneyness << ", spot: " << spot_->value() << ", div_df: " << div_->discount(t) << ", rf_df: " << rf_->discount(t) << endl;

        if (t <= expiryTimes_.front()) {
            t1 = 0.0;
            t2 = expiryTimes_.front();
            var_t1 = 0.0;
            var_t2 = smileSections_[0]->variance(moneyness * smileSections_[0]->atmLevel());
            // cout << "SVIVol -- t1: " << t1 << ", t2: " << t2 << ", var_t1: " << var_t1 << ", var_t2: " << var_t2 << endl;
        } else if (t <= expiryTimes_.back()) {
            Size i1 = 0, i2 = 0;
            for (Size i = 0; i < smileSections_.size() - 1; ++i) {
                if (smileSections_[i]->exerciseTime() < t && t <= smileSections_[i+1]->exerciseTime()) {
                    i1 = i;
                    i2 = i + 1;
                    break;
                }
            }
            t1 = smileSections_[i1]->exerciseTime();
            t2 = smileSections_[i2]->exerciseTime();
            var_t1 = smileSections_[i1]->variance(moneyness * smileSections_[i1]->atmLevel());
            var_t2 = smileSections_[i2]->variance(moneyness * smileSections_[i2]->atmLevel());
            // cout << "SVIVol --  t1: " << t1 << ", t2: " << t2 << ", var_t1: " << var_t1 << ", var_t2: " << var_t2 << endl;
        } else {
            // cout << "SVIVol --  t1: " << smileSections_.back()->exerciseTime() << endl;
            var_t = smileSections_.back()->variance(moneyness * smileSections_.back()->atmLevel()) *
                   t / smileSections_.back()->exerciseTime();
            // cout << "SVIVol -- var_t: " << var_t << endl;
            // cout << flush;
            return var_t;
        }
        var_t = var_t1 + (var_t2 - var_t1) * (t - t1) / (t2 - t1);
        // cout << "SVIVol -- var_t: " << var_t << endl;
        // cout << flush;
        return var_t;
    }

    Real SviVolSurface::forward(const Date& expiry) {
        return spot_->value() * div_->discount(expiry) / rf_->discount(expiry);
    }
}