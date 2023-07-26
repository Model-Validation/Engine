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

/*! \file svivolsurface.hpp
    \brief SVI volatility (smile) surface
*/

#ifndef quantext_svi_vol_surface_hpp
#define quantext_svi_vol_surface_hpp

#include <ql/experimental/volatility/sviinterpolatedsmilesection.hpp>
#include <ql/experimental/volatility/svismilesection.hpp>
#include <ql/termstructures/volatility/equityfx/blackvoltermstructure.hpp>
#include <ql/termstructures/yieldtermstructure.hpp>

namespace QuantExt {
    using namespace QuantLib;


    //! SVI volatility (smile) surface
    /*! blah blah

        \test None yet
     */
    class SviVolSurface : public BlackVarianceTermStructure {
    public:
        //! \name Constructors
        //@{
        /*! @param todo TO DO
         */
        SviVolSurface(const Date& referenceDate,
            const Handle<Quote>& spot,
            const Handle<YieldTermStructure>& riskFreeTS,
            const Handle<YieldTermStructure>& dividendTS,
            const std::vector<Date>& expiries,
            const std::vector<std::vector<double> >& smileSectionParameterSets,
            const Calendar& cal = Calendar(),
            BusinessDayConvention bdc = Following,
            const DayCounter& dc = DayCounter());
        //@}

        // Inherited via BlackVarianceTermStructure
        //! Inspectors
        //@{
        Date maxDate() const override;
        Real minStrike() const override;
        Real maxStrike() const override;
        //@}
    protected:
        Real blackVarianceImpl(Time t, Real strike) const override;

    private:
        Real forward(const Date& expiry);

        Handle<Quote> spot_;
        Handle<YieldTermStructure> rf_;
        Handle<YieldTermStructure> div_;
        std::vector<Date> expiries_;
        std::vector<Time> expiryTimes_;
        std::vector<std::vector<double> > smileSectionParameterSets_;
        std::vector<ext::shared_ptr<SviSmileSection> > smileSections_;
    };

    inline Date SviVolSurface::maxDate() const { return Date::maxDate(); }

    inline Real SviVolSurface::minStrike() const { return 0.0; }

    inline Real SviVolSurface::maxStrike() const { return QL_MAX_REAL; }
}
#endif