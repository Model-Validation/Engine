/*
  Copyright (C) 2017 Quaternion Risk Management Ltd
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

/*! \file ored/portfolio/builders/fxoption.hpp
    \brief
    \ingroup portfolio
*/

#pragma once

#include <boost/make_shared.hpp>
#include <ored/portfolio/builders/cachingenginebuilder.hpp>
#include <ored/portfolio/enginefactory.hpp>
#include <ored/utilities/parsers.hpp>
#include <ored/utilities/to_string.hpp>
#include <ql/experimental/barrieroption/vannavolgabarrierengine.hpp>
#include <ql/pricingengines/barrier/fdblackscholesbarrierengine.hpp>
#include <ql/processes/blackscholesprocess.hpp>
#include <qle/pricingengines/analyticbarrierengine.hpp>
#include <qle/termstructures/blackdeltautilities.hpp>
#include <qle/termstructures/blackmonotonevarvoltermstructure.hpp>

namespace ore {
namespace data {
using namespace std;
using namespace QuantLib;

//! Engine Builder for European FX Barrier Options
/*! Pricing engines are cached by currency pair

    \ingroup portfolio
 */
class FxBarrierOptionEngineBuilder
    : public ore::data::CachingPricingEngineBuilder<string, const Currency&, const Currency&, const Date&,
                                                    const Date&> {

protected:
    FxBarrierOptionEngineBuilder(const string& model, const string& engine)
        : CachingEngineBuilder(model, engine, {"FxBarrierOption"}) {}

    virtual string keyImpl(const Currency& forCcy, const Currency& domCcy, const Date& expiryDate,
                           const Date& paymentDate) override {
        return forCcy.code() + "/" + domCcy.code() + "/" + ore::data::to_string(expiryDate) + "/" +
               ore::data::to_string(paymentDate);
    }

    boost::shared_ptr<GeneralizedBlackScholesProcess>
    getBlackScholesProcess(const Currency& forCcy, const Currency& domCcy, const std::vector<Time>& timePoints = {}) {
        const string& ccyPairCode = forCcy.code() + domCcy.code();
        Handle<BlackVolTermStructure> vol =
            market_->fxVol(ccyPairCode, configuration(ore::data::MarketContext::pricing));
        if (!timePoints.empty()) {
            vol = Handle<BlackVolTermStructure>(
                boost::make_shared<QuantExt::BlackMonotoneVarVolTermStructure>(vol, timePoints));
            vol->enableExtrapolation();
        }
        return boost::make_shared<GeneralizedBlackScholesProcess>(
            market_->fxSpot(ccyPairCode, configuration(ore::data::MarketContext::pricing)),
            market_->discountCurve(forCcy.code(), configuration(ore::data::MarketContext::pricing)),
            market_->discountCurve(domCcy.code(), configuration(ore::data::MarketContext::pricing)), vol);
    }
};

class FxBarrierOptionAnalyticEngineBuilder : public FxBarrierOptionEngineBuilder {
public:
    FxBarrierOptionAnalyticEngineBuilder() : FxBarrierOptionEngineBuilder("GarmanKohlhagen", "AnalyticBarrierEngine") {}

protected:
    virtual boost::shared_ptr<PricingEngine> engineImpl(const Currency& forCcy, const Currency& domCcy,
                                                        const Date& expiryDate, const Date& paymentDate) override {
        boost::shared_ptr<GeneralizedBlackScholesProcess> gbsp = getBlackScholesProcess(forCcy, domCcy);
        return boost::make_shared<QuantExt::AnalyticBarrierEngine>(gbsp, paymentDate);
    }
};

class FxBarrierOptionFDEngineBuilder : public FxBarrierOptionEngineBuilder {
public:
    FxBarrierOptionFDEngineBuilder() : FxBarrierOptionEngineBuilder("GarmanKohlhagen", "FdBlackScholesBarrierEngine") {}

protected:
    virtual boost::shared_ptr<PricingEngine> engineImpl(const Currency& forCcy, const Currency& domCcy,
                                                        const Date& expiryDate, const Date& paymentDate) override {
        // We follow the way FdBlackScholesBarrierEngine determines maturity for time grid generation
        Handle<YieldTermStructure> riskFreeRate =
            market_->discountCurve(domCcy.code(), configuration(ore::data::MarketContext::pricing));
        Time expiry = riskFreeRate->dayCounter().yearFraction(riskFreeRate->referenceDate(),
                                                              std::max(riskFreeRate->referenceDate(), expiryDate));

        FdmSchemeDesc scheme = ore::data::parseFdmSchemeDesc(engineParameter("Scheme"));
        Size tGrid = std::max<Size>(1, (Size)(ore::data::parseInteger(engineParameter("TimeGridPerYear")) * expiry));
        Size xGrid = ore::data::parseInteger(engineParameter("XGrid"));
        Size dampingSteps = ore::data::parseInteger(engineParameter("DampingSteps"));
        bool monotoneVar = ore::data::parseBool(engineParameter("EnforceMonotoneVariance", {}, false, "true"));

        boost::shared_ptr<GeneralizedBlackScholesProcess> gbsp;

        if (monotoneVar) {
            // Replicate the construction of time grid in FiniteDifferenceModel::rollbackImpl
            // This time grid is required to build a BlackMonotoneVarVolTermStructure which
            // ensures monotonic variance along the time grid
            std::vector<Time> timePoints(tGrid + 1);
            Array timePointsArray(tGrid, expiry, -expiry / tGrid);
            timePoints[0] = 0.0;
            for (Size i = 0; i < tGrid; i++)
                timePoints[timePoints.size() - i - 1] = timePointsArray[i];
            timePoints.insert(std::upper_bound(timePoints.begin(), timePoints.end(), 0.99 / 365), 0.99 / 365);
            gbsp = getBlackScholesProcess(forCcy, domCcy, timePoints);
        } else {
            gbsp = getBlackScholesProcess(forCcy, domCcy);
        }
        return boost::make_shared<FdBlackScholesBarrierEngine>(gbsp, tGrid, xGrid, dampingSteps, scheme);
    }
};


//! Engine Builder for Fx Barrier Options using Vanna-Volga Barrier Engine
/*! Pricing engines are cached by currency pair/currency/expiry
    \ingroup builders
 */
class FxBarrierOptionVVEngineBuilder : public FxBarrierOptionEngineBuilder {
public:
    FxBarrierOptionVVEngineBuilder() : FxBarrierOptionEngineBuilder("GarmanKohlhagen", "VannaVolgaBarrierEngine") {}

protected:
    virtual boost::shared_ptr<PricingEngine> engineImpl(const Currency& forCcy, const Currency& domCcy,
                                                        const Date& expiryDate, const Date& paymentDate) override {
        string pair = forCcy.code() + domCcy.code();
        boost::shared_ptr<GeneralizedBlackScholesProcess> gbsp = boost::make_shared<GeneralizedBlackScholesProcess>(
            market_->fxSpot(pair, configuration(ore::data::MarketContext::pricing)), // TODO: fxRate or fxSpot?
            market_->discountCurve(forCcy.code(),
                                   configuration(ore::data::MarketContext::pricing)), // dividend yield ~ foreign yield
            market_->discountCurve(domCcy.code(), configuration(ore::data::MarketContext::pricing)),
            market_->fxVol(pair, configuration(ore::data::MarketContext::pricing)));
        Handle<YieldTermStructure> domesticTS = gbsp->riskFreeRate();
        Handle<YieldTermStructure> foreignTS = gbsp->dividendYield();
        Handle<Quote> spotFX = gbsp->stateVariable();
        Time ttm = gbsp->blackVolatility()->timeFromReference(expiryDate);
        Real forward = spotFX->value() * foreignTS->discount(ttm) / domesticTS->discount(ttm);

        Handle<DeltaVolQuote> atmVol(boost::make_shared<DeltaVolQuote>(
            Handle<Quote>(boost::make_shared<SimpleQuote>(
                                                  gbsp->blackVolatility()->blackVol(expiryDate, forward))),
            DeltaVolQuote::DeltaType::Spot, ttm, DeltaVolQuote::AtmType::AtmDeltaNeutral) // TODO AtmSpot, AtmFwd, or AtmDeltaNeutral?
        );

        DLOG("At forward: " << forward << " " << gbsp->blackVolatility()->blackVol(expiryDate, forward));

        Real strike25Put = QuantExt::getStrikeFromDelta(Option::Type::Put, -0.25, DeltaVolQuote::DeltaType::Spot,
                                                        spotFX->value(), domesticTS->discount(ttm),
                                                        foreignTS->discount(ttm), *gbsp->blackVolatility(), ttm);
        Real strike25Call = QuantExt::getStrikeFromDelta(Option::Type::Call, 0.25, DeltaVolQuote::DeltaType::Spot,
                                                         spotFX->value(), domesticTS->discount(ttm),
                                                         foreignTS->discount(ttm), *gbsp->blackVolatility(), ttm);
        Handle<DeltaVolQuote> vol25Put(boost::make_shared<DeltaVolQuote>(
            -0.25,
            Handle<Quote>(boost::make_shared<SimpleQuote>(gbsp->blackVolatility()->blackVol(expiryDate, strike25Put))),
            ttm, DeltaVolQuote::DeltaType::Spot));
        Handle<DeltaVolQuote> vol25Call(boost::make_shared<DeltaVolQuote>(
            0.25,
            Handle<Quote>(boost::make_shared<SimpleQuote>(gbsp->blackVolatility()->blackVol(expiryDate, strike25Call))),
            ttm, DeltaVolQuote::DeltaType::Spot));

        bool adaptVanDelta = false;  // Default false
        Real bsPriceWithSmile = 0.0; // Default 0.0

        return boost::make_shared<QuantLib::VannaVolgaBarrierEngine>(atmVol, vol25Put, vol25Call, spotFX, domesticTS,
                                                                     foreignTS, adaptVanDelta, bsPriceWithSmile);
    }
};

} // namespace data
} // namespace oreplus
