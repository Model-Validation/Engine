/*
 Copyright (C) 2020 Fredrik Gerdin Börjesson
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

/*! \file portfolio/builders/fxasianoption.hpp
    \brief Engine builder for fx asian options
    \ingroup builders
*/

#pragma once

#include <ored/portfolio/builders/asianoption.hpp>
#include <ored/utilities/parsers.hpp>

namespace ore {
namespace data {

//! Discrete Monte Carlo Engine Builder for European Asian Fx Arithmetic Average Price Options
/*! Pricing engines are cached by asset/currency

    \ingroup builders
 */
class FxEuropeanAsianOptionMCDAAPEngineBuilder : public EuropeanAsianOptionMCDAAPEngineBuilder {
public:
    FxEuropeanAsianOptionMCDAAPEngineBuilder()
        : EuropeanAsianOptionMCDAAPEngineBuilder("BlackScholesMerton", {"FxAsianOptionArithmeticPrice"},
                                                 AssetClass::EQ) {}
};

//! Discrete Monte Carlo Engine Builder for European Asian Fx Arithmetic Average Strike Options
/*! Pricing engines are cached by asset/currency

    \ingroup builders
 */
class FxEuropeanAsianOptionMCDAASEngineBuilder : public EuropeanAsianOptionMCDAASEngineBuilder {
public:
    FxEuropeanAsianOptionMCDAASEngineBuilder()
        : EuropeanAsianOptionMCDAASEngineBuilder("BlackScholesMerton", {"FxAsianOptionArithmeticStrike"},
                                                 AssetClass::EQ) {}
};

//! Discrete Monte Carlo Engine Builder for European Asian Fx Geometric Average Price Options
/*! Pricing engines are cached by asset/currency

    \ingroup builders
 */
class FxEuropeanAsianOptionMCDGAPEngineBuilder : public EuropeanAsianOptionMCDGAPEngineBuilder {
public:
    FxEuropeanAsianOptionMCDGAPEngineBuilder()
        : EuropeanAsianOptionMCDGAPEngineBuilder("BlackScholesMerton", {"FxAsianOptionArithmeticPrice"},
                                                 AssetClass::EQ) {}
};

//! Discrete Analytic Engine Builder for European Asian Fx Geometric Average Price Options
/*! Pricing engines are cached by asset/currency

    \ingroup builders
 */
class FxEuropeanAsianOptionADGAPEngineBuilder : public EuropeanAsianOptionADGAPEngineBuilder {
public:
    FxEuropeanAsianOptionADGAPEngineBuilder()
        : EuropeanAsianOptionADGAPEngineBuilder("BlackScholesMerton", {"FxAsianOptionGeometricPrice"},
                                                AssetClass::EQ) {}
};

//! Discrete Analytic Engine Builder for European Asian Fx Geometric Average Strike Options
/*! Pricing engines are cached by asset/currency

    \ingroup builders
 */
class FxEuropeanAsianOptionADGASEngineBuilder : public EuropeanAsianOptionADGASEngineBuilder {
public:
    FxEuropeanAsianOptionADGASEngineBuilder()
        : EuropeanAsianOptionADGASEngineBuilder("BlackScholesMerton", {"FxAsianOptionGeometricStrike"},
                                                AssetClass::EQ) {}
};

//! Continuous Analytic Engine Builder for European Asian Fx Geometric Average Price Options
/*! Pricing engines are cached by asset/currency

    \ingroup builders
 */
class FxEuropeanAsianOptionACGAPEngineBuilder : public EuropeanAsianOptionACGAPEngineBuilder {
public:
    FxEuropeanAsianOptionACGAPEngineBuilder()
        : EuropeanAsianOptionACGAPEngineBuilder("BlackScholesMerton", {"FxAsianOptionGeometricPrice"},
                                                AssetClass::EQ) {}
};

} // namespace data
} // namespace ore
