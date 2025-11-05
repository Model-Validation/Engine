/*
  Copyright (C) 2019 Quaternion Risk Management Ltd
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

/*! \file ored/portfolio/builders/fxdoublebarrieroption.hpp
    \brief
    \ingroup portfolio
*/

#pragma once

#include <boost/make_shared.hpp>
#include <ored/portfolio/builders/cachingenginebuilder.hpp>
#include <ored/portfolio/enginefactory.hpp>
#include <ored/utilities/parsers.hpp>
#include <ored/utilities/to_string.hpp>
#include <ql/experimental/barrieroption/suowangdoublebarrierengine.hpp>
#include <ql/experimental/barrieroption/vannavolgadoublebarrierengine.hpp>
#include <ql/pricingengines/barrier/fdblackscholesbarrierengine.hpp>
#include <ql/processes/blackscholesprocess.hpp>
#include <qle/pricingengines/analyticdoublebarrierengine.hpp>
#include <qle/termstructures/blackdeltautilities.hpp>
#include <qle/termstructures/blackmonotonevarvoltermstructure.hpp>
#include <qle/termstructures/fxblackvolsurface.hpp>

namespace ore {
namespace data {
using namespace std;
using namespace QuantLib;

//! Engine Builder for European FX Double Barrier Options
/*! Pricing engines are cached by currency pair

    \ingroup portfolio
 */
class FxDoubleBarrierOptionEngineBuilder
    : public ore::data::CachingPricingEngineBuilder<string, const Currency&, const Currency&, const Date&> {

protected:
    FxDoubleBarrierOptionEngineBuilder(const string& model, const string& engine)
        : CachingEngineBuilder(model, engine, {"FxDoubleBarrierOption"}) {}

    virtual string keyImpl(const Currency& forCcy, const Currency& domCcy, const Date& paymentDate) override {
        return forCcy.code() + domCcy.code() + ore::data::to_string(paymentDate);
    }

    QuantLib::ext::shared_ptr<GeneralizedBlackScholesProcess>
    getBlackScholesProcess(const Currency& forCcy, const Currency& domCcy, const std::vector<Time>& timePoints = {}) {
        const string pair = forCcy.code() + domCcy.code();
        Handle<BlackVolTermStructure> vol = market_->fxVol(pair, configuration(ore::data::MarketContext::pricing));
        if (!timePoints.empty()) {
            vol = Handle<BlackVolTermStructure>(
                QuantLib::ext::make_shared<QuantExt::BlackMonotoneVarVolTermStructure>(vol, timePoints));
            vol->enableExtrapolation();
        }
        return QuantLib::ext::make_shared<GeneralizedBlackScholesProcess>(
            market_->fxSpot(pair, configuration(ore::data::MarketContext::pricing)),
            market_->discountCurve(forCcy.code(),
                                   configuration(ore::data::MarketContext::pricing)), // dividend yield ~ foreign yield
            market_->discountCurve(domCcy.code(), configuration(ore::data::MarketContext::pricing)), vol);
    }
};

//! Analytical Engine Builder for FX Double Barrier Options
/*! Pricing engines are cached by currency pair

    \ingroup portfolio
 */
class FxDoubleBarrierOptionAnalyticEngineBuilder : public FxDoubleBarrierOptionEngineBuilder {
public:
    FxDoubleBarrierOptionAnalyticEngineBuilder()
        : FxDoubleBarrierOptionEngineBuilder("GarmanKohlhagen", "AnalyticDoubleBarrierEngine") {}

protected:
    virtual QuantLib::ext::shared_ptr<PricingEngine> engineImpl(const Currency& forCcy, const Currency& domCcy, const Date& paymentDate) override {
        QuantLib::ext::shared_ptr<GeneralizedBlackScholesProcess> gbsp = getBlackScholesProcess(forCcy, domCcy);
        return QuantLib::ext::make_shared<QuantExt::AnalyticDoubleBarrierEngine>(gbsp, paymentDate);
    }
};

//! Engine Builder for Fx Double Barrier Options using Vanna-Volga Double Barrier Engine
/*! Pricing engines are cached by currency pair/currency/payment date
    \ingroup builders
 */
class FxDoubleBarrierOptionVVEngineBuilder : public FxDoubleBarrierOptionEngineBuilder {
public:
    FxDoubleBarrierOptionVVEngineBuilder()
        : FxDoubleBarrierOptionEngineBuilder("GarmanKohlhagen", "VannaVolgaDoubleBarrierEngine") {}

protected:
    virtual QuantLib::ext::shared_ptr<PricingEngine> engineImpl(const Currency& forCcy, const Currency& domCcy,
                                                                const Date& paymentDate) override {
        string pair = forCcy.code() + domCcy.code();
        QuantLib::ext::shared_ptr<GeneralizedBlackScholesProcess> gbsp =
            QuantLib::ext::make_shared<GeneralizedBlackScholesProcess>(
                market_->fxSpot(pair, configuration(ore::data::MarketContext::pricing)), // TODO: fxRate or fxSpot?
                market_->discountCurve(
                    forCcy.code(),
                    configuration(ore::data::MarketContext::pricing)), // dividend yield ~ foreign yield
                market_->discountCurve(domCcy.code(), configuration(ore::data::MarketContext::pricing)),
                market_->fxVol(pair, configuration(ore::data::MarketContext::pricing)));
        Handle<YieldTermStructure> domesticTS = gbsp->riskFreeRate();
        Handle<YieldTermStructure> foreignTS = gbsp->dividendYield();
        Handle<Quote> spotFX = gbsp->stateVariable();
        Time ttm = gbsp->blackVolatility()->timeFromReference(paymentDate);
        Real forward = spotFX->value() * foreignTS->discount(ttm) / domesticTS->discount(ttm);

        Handle<DeltaVolQuote> atmVol(QuantLib::ext::make_shared<DeltaVolQuote>(
            Handle<Quote>(
                QuantLib::ext::make_shared<SimpleQuote>(gbsp->blackVolatility()->blackVol(paymentDate, forward))),
            DeltaVolQuote::DeltaType::Spot, ttm,
            DeltaVolQuote::AtmType::AtmDeltaNeutral) // TODO AtmSpot, AtmFwd, or AtmDeltaNeutral?
        );

        DLOG("At forward: " << forward << " " << gbsp->blackVolatility()->blackVol(paymentDate, forward));

        Real strike25Put = QuantExt::getStrikeFromDelta(Option::Type::Put, -0.25, DeltaVolQuote::DeltaType::Spot,
                                                        spotFX->value(), domesticTS->discount(ttm),
                                                        foreignTS->discount(ttm), *gbsp->blackVolatility(), ttm);
        Real strike25Call = QuantExt::getStrikeFromDelta(Option::Type::Call, 0.25, DeltaVolQuote::DeltaType::Spot,
                                                         spotFX->value(), domesticTS->discount(ttm),
                                                         foreignTS->discount(ttm), *gbsp->blackVolatility(), ttm);
        Handle<DeltaVolQuote> vol25Put(
            QuantLib::ext::make_shared<DeltaVolQuote>(-0.25,
                                                      Handle<Quote>(QuantLib::ext::make_shared<SimpleQuote>(
                                                          gbsp->blackVolatility()->blackVol(paymentDate, strike25Put))),
                                                      ttm, DeltaVolQuote::DeltaType::Spot));
        Handle<DeltaVolQuote> vol25Call(QuantLib::ext::make_shared<DeltaVolQuote>(
            0.25,
            Handle<Quote>(
                QuantLib::ext::make_shared<SimpleQuote>(gbsp->blackVolatility()->blackVol(paymentDate, strike25Call))),
            ttm, DeltaVolQuote::DeltaType::Spot));

        bool adaptVanDelta = false;  // Default false
        Real bsPriceWithSmile = 0.0; // Default 0.0
        int series = 5;
        string underlyingEngine = engineParameter("UnderlyingEngine", {}, false, "AnalyticDoubleBarrierEngine");
        if (underlyingEngine == "AnalyticDoubleBarrierEngine") {
            return QuantLib::ext::make_shared<VannaVolgaDoubleBarrierEngine<QuantLib::AnalyticDoubleBarrierEngine>>(
                atmVol, vol25Put, vol25Call, spotFX, domesticTS, foreignTS, adaptVanDelta, bsPriceWithSmile, series);
        } else if (underlyingEngine == "SuoWangDoubleBarrierEngine") {
            return QuantLib::ext::make_shared<VannaVolgaDoubleBarrierEngine<QuantLib::SuoWangDoubleBarrierEngine>>(
                atmVol, vol25Put, vol25Call, spotFX, domesticTS, foreignTS, adaptVanDelta, bsPriceWithSmile, series);
        } else {
            QL_FAIL("Expected UnderlyingEngine to be either 'AnalyticDoubleBarrierEngine' or "
                    "'SuoWangDoubleBarrierEngine'.");
        }
    }
};


} // namespace data
} // namespace oreplus
