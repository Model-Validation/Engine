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

/*! \file portfolio/builders/fxpartialtimebarrieroption.hpp
    \brief
    \ingroup portfolio
*/

#pragma once

#include <ored/portfolio/builders/cachingenginebuilder.hpp>
#include <ored/portfolio/enginefactory.hpp>
#include <ored/utilities/to_string.hpp>
#include <ql/experimental/barrieroption/vannavolgapartialtimebarrierengine.hpp>
#include <ql/pricingengines/barrier/analyticpartialtimebarrieroptionengine.hpp>
#include <ql/processes/blackscholesprocess.hpp>
#include <qle/termstructures/blackdeltautilities.hpp>

namespace ore {
namespace data {
using std::string;

//! Engine Builder for European FX Partial-Time Barrier Options
/*! Pricing engines are cached by currency pair

    \ingroup portfolio
 */
class FxPartialTimeBarrierOptionAnalyticEngineBuilder
    : public ore::data::CachingPricingEngineBuilder<string, const Currency&, const Currency&> {
public:
    FxPartialTimeBarrierOptionAnalyticEngineBuilder()
        : CachingEngineBuilder("GarmanKohlhagen", "AnalyticPartialTimeBarrierOptionEngine",
                               {"FxPartialTimeBarrierOption"}) {}

protected:
    string keyImpl(const Currency& forCcy, const Currency& domCcy) override {
        return forCcy.code() + domCcy.code();
    }

    QuantLib::ext::shared_ptr<PricingEngine> engineImpl(const Currency& forCcy, const Currency& domCcy) override {
        string pair = forCcy.code() + domCcy.code();

        QuantLib::ext::shared_ptr<GeneralizedBlackScholesProcess> gbsp = QuantLib::ext::make_shared<GeneralizedBlackScholesProcess>(
            market_->fxSpot(pair, configuration(ore::data::MarketContext::pricing)),
            market_->discountCurve(forCcy.code(),
                                   configuration(ore::data::MarketContext::pricing)), // dividend yield ~ foreign yield
            market_->discountCurve(domCcy.code(), configuration(ore::data::MarketContext::pricing)),
            market_->fxVol(pair, configuration(ore::data::MarketContext::pricing)));
        return QuantLib::ext::make_shared<QuantLib::AnalyticPartialTimeBarrierOptionEngine>(gbsp);
    }
};

//! Engine builder for European FX Partial-Time Barrier Options using Vanna-Volga Barrier Engine
class FxPartialTimeBarrierOptionVVEngineBuilder
    : public ore::data::CachingPricingEngineBuilder<string, const Currency&, const Currency&, const Date&,
                                                    const Date&> {
public:
    FxPartialTimeBarrierOptionVVEngineBuilder()
        : CachingEngineBuilder("GarmanKohlhagen", "VannaVolgaPartialTimeBarrierEngine",
                               {"FxPartialTimeBarrierOption"}) {}

protected:
    string keyImpl(const Currency& forCcy, const Currency& domCcy, const Date& expiryDate,
                   const Date& paymentDate) override {
        return forCcy.code() + "/" + domCcy.code() + "/" + ore::data::to_string(expiryDate) + "/" +
               ore::data::to_string(paymentDate);
    }

    QuantLib::ext::shared_ptr<PricingEngine> engineImpl(const Currency& forCcy, const Currency& domCcy,
                                                        const Date& expiryDate, const Date& paymentDate) override {
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
        Time ttm = gbsp->blackVolatility()->timeFromReference(expiryDate);
        Real forward = spotFX->value() * foreignTS->discount(ttm) / domesticTS->discount(ttm);

        Handle<DeltaVolQuote> atmVol(QuantLib::ext::make_shared<DeltaVolQuote>(
            Handle<Quote>(
                QuantLib::ext::make_shared<SimpleQuote>(gbsp->blackVolatility()->blackVol(expiryDate, forward))),
            DeltaVolQuote::DeltaType::Spot, ttm,
            DeltaVolQuote::AtmType::AtmDeltaNeutral) // TODO AtmSpot, AtmFwd, or AtmDeltaNeutral?
        );

        DLOG("At forward: " << forward << " " << gbsp->blackVolatility()->blackVol(expiryDate, forward));

        Real strike25Put = QuantExt::getStrikeFromDelta(Option::Type::Put, -0.25, DeltaVolQuote::DeltaType::Spot,
                                                        spotFX->value(), domesticTS->discount(ttm),
                                                        foreignTS->discount(ttm), *gbsp->blackVolatility(), ttm);
        Real strike25Call = QuantExt::getStrikeFromDelta(Option::Type::Call, 0.25, DeltaVolQuote::DeltaType::Spot,
                                                         spotFX->value(), domesticTS->discount(ttm),
                                                         foreignTS->discount(ttm), *gbsp->blackVolatility(), ttm);
        Handle<DeltaVolQuote> vol25Put(
            QuantLib::ext::make_shared<DeltaVolQuote>(-0.25,
                                                      Handle<Quote>(QuantLib::ext::make_shared<SimpleQuote>(
                                                          gbsp->blackVolatility()->blackVol(expiryDate, strike25Put))),
                                                      ttm, DeltaVolQuote::DeltaType::Spot));
        Handle<DeltaVolQuote> vol25Call(
            QuantLib::ext::make_shared<DeltaVolQuote>(0.25,
                                                      Handle<Quote>(QuantLib::ext::make_shared<SimpleQuote>(
                                                          gbsp->blackVolatility()->blackVol(expiryDate, strike25Call))),
                                                      ttm, DeltaVolQuote::DeltaType::Spot));

        bool adaptVanDelta = false;  // Default false
        Real bsPriceWithSmile = 0.0; // Default 0.0

        return QuantLib::ext::make_shared<QuantLib::VannaVolgaPartialTimeBarrierEngine>(
            atmVol, vol25Put, vol25Call, spotFX, domesticTS, foreignTS, adaptVanDelta, bsPriceWithSmile);
    }
};

} // namespace data
} // namespace ore
