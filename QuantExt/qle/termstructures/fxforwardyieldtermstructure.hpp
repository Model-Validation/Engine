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

/*! \file qle/termstructures/fxforwardyieldtermstructure.hpp
    \brief Yield term structure implied from FX forward prices
*/

#ifndef quantext_fx_forward_yield_term_structure_hpp
#define quantext_fx_forward_yield_term_structure_hpp

#include <qle/termstructures/pricetermstructureadapter.hpp>

namespace QuantExt {

//! Yield term structure implied from FX forward prices with flat zero rate extrapolation
/*! This class extends QuantLib::YieldTermStructure to convert FX forward prices
    from an InterpolatedPriceCurve into domestic currency discount factors.

    It always applies flat zero rate extrapolation for times before the first
    or after the last pillar date of the underlying FX forward price curve.

    The conversion follows the standard FX forward pricing relationship:
    \f[
    F(0, t) = S(0) \frac{P_{\text{for}}(0, t)}{P_{\text{dom}}(0, t)}
    \f]

    When \c invertedQuotation is false (the default), the implied discount factor is:
    \f[
    P_{\text{dom}}(0, t) = P_{\text{for}}(0, t) \frac{F(0, t)}{S(0)}
    \f]

    When \c invertedQuotation is true:
    \f[
    P_{\text{dom}}(0, t) = P_{\text{for}}(0, t) \frac{S(0)}{F(0, t)}
    \f]

    For times outside the range of the FX forward price curve, flat zero rate
    extrapolation is applied using the zero rate at the nearest boundary pillar. The
    FX spot rate is discounted back to the reference date such that all discounts are
    correctly express
*/
class FxForwardYieldTermStructure : public QuantLib::YieldTermStructure {

public:
    //! Construct with an explicit FX spot quote and spot date calculation data
    FxForwardYieldTermStructure(const QuantLib::ext::shared_ptr<PriceTermStructure>& fxForwardCurve,
                                const QuantLib::ext::shared_ptr<QuantLib::YieldTermStructure>& foreignDiscount,
                                const QuantLib::Handle<QuantLib::Quote>& fxSpotQuote, bool invertedQuotation,
                                const QuantLib::Calendar& advanceCalendar, const QuantLib::Calendar& tradingCalendar,
                                QuantLib::Natural spotDays, QuantLib::BusinessDayConvention bdc);

    //! \name TermStructure interface
    //@{
    QuantLib::Date maxDate() const override;
    const QuantLib::Date& referenceDate() const override;
    QuantLib::DayCounter dayCounter() const override;
    //@}

    //! \name Inspectors
    //@{
    const QuantLib::ext::shared_ptr<PriceTermStructure>& priceCurve() const;
    const QuantLib::ext::shared_ptr<QuantLib::YieldTermStructure>& discountCurve() const;
    const QuantLib::Handle<QuantLib::Quote>& fxSpotQuote() const;
    bool invertedQuotation() const;
    const QuantLib::Calendar& advanceCalendar() const;
    const QuantLib::Calendar& tradingCalendar() const;
    QuantLib::Natural spotDays() const;
    QuantLib::BusinessDayConvention businessDayConvention() const;
    //@}
protected:
    //! \name YieldTermStructure interface
    //@{
    QuantLib::DiscountFactor discountImpl(QuantLib::Time t) const override;
    //@}

private:
    QuantLib::ext::shared_ptr<PriceTermStructure> priceCurve_;
    QuantLib::ext::shared_ptr<QuantLib::YieldTermStructure> discount_;
    QuantLib::Handle<QuantLib::Quote> fxSpotQuote_;
    bool invertedQuotation_;
    QuantLib::Calendar advanceCalendar_, tradingCalendar_;
    QuantLib::Natural spotDays_;
    QuantLib::BusinessDayConvention bdc_;
};

} // namespace QuantExt

#endif
