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

/*! \file blackvariancesurfacesparse.hpp
 \brief Sparse Black volatility surface modelled as volatility surface
 */

#ifndef quantext_black_volatility_surface_sparse_hpp
#define quantext_black_volatility_surface_sparse_hpp

#include <ql/math/interpolations/linearinterpolation.hpp>
#include <ql/termstructures/volatility/equityfx/blackvoltermstructure.hpp>
#include <ql/time/daycounters/actual365fixed.hpp>
#include <qle/interpolators/optioninterpolator2d.hpp>

namespace QuantExt {

//! Black volatility surface based on sparse matrix.
//!  \ingroup termstructures
class BlackVolatilitySurfaceSparse : public QuantLib::BlackVolatilityTermStructure,
                                     public OptionInterpolator2d<QuantLib::Linear, QuantLib::Linear> {

public:
    BlackVolatilitySurfaceSparse(const QuantLib::Date& referenceDate, const QuantLib::Calendar& cal,
                                 const std::vector<QuantLib::Date>& dates, const std::vector<QuantLib::Real>& strikes,
                                 const std::vector<QuantLib::Volatility>& volatilities,
                                 const QuantLib::DayCounter& dayCounter, bool lowerStrikeConstExtrap = true,
                                 bool upperStrikeConstExtrap = true, bool timeFlatExtrapolation = false);

    enum class TimeInterpolationMethod { Linear, Flat };
    //! \name TermStructure interface
    //@{
    QuantLib::Date maxDate() const { return QuantLib::Date::maxDate(); }
    const QuantLib::Date& referenceDate() const { return OptionInterpolator2d::referenceDate(); }
    QuantLib::DayCounter dayCounter() const { return OptionInterpolator2d::dayCounter(); }
    //@}
    //! \name VolatilityTermStructure interface
    //@{
    QuantLib::Real minStrike() const { return 0; }
    QuantLib::Real maxStrike() const { return QL_MAX_REAL; }
    //@}

    //! \name Visitability
    //@{
    virtual void accept(QuantLib::AcyclicVisitor&);
    //@}

protected:
    virtual QuantLib::Real blackVolImpl(QuantLib::Time t, QuantLib::Real strike) const;

    bool timeFlatExtrapolation_;
};

// inline definitions

inline void BlackVolatilitySurfaceSparse::accept(QuantLib::AcyclicVisitor& v) {
    QuantLib::Visitor<BlackVolatilitySurfaceSparse>* v1 =
        dynamic_cast<QuantLib::Visitor<BlackVolatilitySurfaceSparse>*>(&v);
    if (v1 != 0)
        v1->visit(*this);
    else
        QuantLib::BlackVolatilityTermStructure::accept(v);
}
} // namespace QuantExt

#endif
