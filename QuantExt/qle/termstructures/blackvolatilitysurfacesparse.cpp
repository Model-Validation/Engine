/*
 Copyright (C) 2022 Skandinaviska Enskilda Banken AB (publ)
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

#include <qle/termstructures/blackvolatilitysurfacesparse.hpp>

using namespace std;
using namespace QuantLib;

namespace QuantExt {

BlackVolatilitySurfaceSparse::BlackVolatilitySurfaceSparse(const Date& referenceDate, const Calendar& cal,
                                                           const vector<Date>& dates, const vector<Real>& strikes,
                                                           const vector<Volatility>& volatilities,
                                                           const DayCounter& dayCounter, bool lowerStrikeConstExtrap,
                                                           bool upperStrikeConstExtrap, bool timeFlatExtrapolation)
    : BlackVolatilityTermStructure(referenceDate, cal), OptionInterpolator2d<Linear, Linear>(referenceDate, dayCounter,
                                                                                             lowerStrikeConstExtrap,
                                                                                             upperStrikeConstExtrap),
      timeFlatExtrapolation_(timeFlatExtrapolation) {

    QL_REQUIRE((strikes.size() == dates.size()) && (dates.size() == volatilities.size()),
               "dates, strikes and volatilities vectors not of equal size.");

    initialise(dates, strikes, volatilities);
}

QuantLib::Real BlackVolatilitySurfaceSparse::blackVolImpl(QuantLib::Time t, QuantLib::Real strike) const {
    QuantLib::Time tb = times().back();
    QuantLib::Time tf = times().front();

    if (timeFlatExtrapolation_ && t < tf) {
        return getValue(tf, strike);
    } else if (timeFlatExtrapolation_ && t > tb) {
        return getValue(tb, strike);
    } else {
        return getValue(t, strike);
    }
};
} // namespace QuantExt
