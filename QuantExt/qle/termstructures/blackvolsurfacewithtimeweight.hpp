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

/*! \file qle/termstructures/blackvolsurfacewithtimeweight.hpp
    \brief Wrapper class for a BlackVolTermStructure that easily provides time-weighting.
    \ingroup termstructures
*/

#ifndef quantext_blackvolsurfacewithtimeweight_hpp
#define quantext_blackvolsurfacewithtimeweight_hpp

#include <ql/quote.hpp>
#include <ql/termstructures/volatility/equityfx/blackvoltermstructure.hpp>
#include <ql/termstructures/yieldtermstructure.hpp>
#include <unordered_map>

namespace QuantExt {
using namespace QuantLib;

//! Wrapper class for a BlackVolTermStructure that easily provides time-weighted vols.
/*! This class implements BlackVolatilityTermStructure and takes a surface (well, any BlackVolTermStructure) as an
    input.
 */
//!\ingroup termstructures

class BlackVolatilityWithTimeWeighting : public BlackVolatilityTermStructure {
public:
    //! Constructor. This is a floating term structure (settlement days is zero)
    BlackVolatilityWithTimeWeighting(const QuantLib::ext::shared_ptr<BlackVolTermStructure>& surface,
                                     const std::vector<Date>& skewDates,
                                     const std::unordered_map<Weekday, Real>& weekdayWeights,
                                     const std::unordered_map<Date, Real>& eventWeights);

    //! \name TermStructure interface
    //@{
    DayCounter dayCounter() const override { return surface_->dayCounter(); }
    Date maxDate() const override { return surface_->maxDate(); }
    Time maxTime() const override { return surface_->maxTime(); }
    const Date& referenceDate() const override { return surface_->referenceDate(); }
    Calendar calendar() const override { return surface_->calendar(); }
    Natural settlementDays() const override { return surface_->settlementDays(); }
    //@}

    //! \name VolatilityTermStructure interface
    //@{
    Rate minStrike() const override { return surface_->minStrike(); }
    Rate maxStrike() const override { return surface_->maxStrike(); }
    //@}

    //! \name Inspectors
    //@{
    QuantLib::ext::shared_ptr<BlackVolTermStructure> surface() const { return surface_; }
    std::vector<Date> skewDates() const { return skewDates_; }
    std::unordered_map<Weekday, Real> weekdayWeights() const { return weekdayWeights_; }
    std::unordered_map<Date, Real> eventWeights() const { return eventWeights_; }
    //@}

protected:
    Volatility blackVolImpl(Time t, Real strike) const override;

private:
    QuantLib::ext::shared_ptr<BlackVolTermStructure> surface_;
    std::vector<Date> skewDates_;
    std::unordered_map<Weekday, Real> weekdayWeights_;
    std::unordered_map<Date, Real> eventWeights_;

    //! Helpers to calculates time weights for interpolation
    //@{
    Real accruedWeightBetween(Date prevDate, Date endDate) const;
    //@}
};

} // namespace QuantExt

#endif