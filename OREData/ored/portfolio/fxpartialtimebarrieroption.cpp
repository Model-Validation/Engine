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

#include <ored/portfolio/builders/fxoption.hpp>
#include <ored/portfolio/builders/fxpartialtimebarrieroption.hpp>
#include <ored/portfolio/enginefactory.hpp>
#include <ored/portfolio/fxpartialtimebarrieroption.hpp>
#include <ored/utilities/indexparser.hpp>
#include <ored/utilities/parsers.hpp>
#include <ql/errors.hpp>
#include <ql/exercise.hpp>
#include <ql/experimental/exoticoptions/partialtimebarrieroption.hpp>
#include <ql/instruments/compositeinstrument.hpp>
#include <ql/instruments/vanillaoption.hpp>
#include <qle/indexes/fxindex.hpp>

using namespace QuantLib;
using namespace QuantExt;

namespace ore {
namespace data {

void FxPartialTimeBarrierOption::build(const QuantLib::ext::shared_ptr<EngineFactory>& engineFactory) {

    const QuantLib::ext::shared_ptr<Market> market = engineFactory->market();

    // Only European Single Barrier supported for now
    QL_REQUIRE(option_.style() == "European", "Option Style unknown: " << option_.style());
    QL_REQUIRE(option_.exerciseDates().size() == 1, "Invalid number of excercise dates");
    QL_REQUIRE(barrier_.levels().size() == 1, "Invalid number of barrier levels");
    QL_REQUIRE(barrier_.style().empty() || barrier_.style() == "PartialTime",
               "Only partial-time barrier style suppported");
    QL_REQUIRE(tradeActions().empty(), "TradeActions not supported for FxPartialTimeBarrierOption");

    Currency boughtCcy = parseCurrency(boughtCurrency_);
    Currency soldCcy = parseCurrency(soldCurrency_);
    // Real level = barrier_.levels()[0].value();
    Real rebate = barrier_.rebate();
    QL_REQUIRE(rebate >= 0, "Rebate must be non-negative");

    Real strike = soldAmount_ / boughtAmount_;
    Option::Type type = parseOptionType(option_.callPut());

    // Exercise
    Date expiryDate = parseDate(option_.exerciseDates().front());
    Date paymentDate = expiryDate;

    QuantLib::ext::shared_ptr<Exercise> exercise = QuantLib::ext::make_shared<EuropeanExercise>(expiryDate);

    const QuantLib::ext::optional<OptionPaymentData>& opd = option_.paymentData();
    if (opd) {
        if (opd->rulesBased()) {
            const Calendar& cal = opd->calendar();
            QL_REQUIRE(cal != Calendar(), "Need a non-empty calendar for rules based payment date.");
            paymentDate = cal.advance(expiryDate, opd->lag(), Days, opd->convention());
        } else {
            const vector<Date>& dates = opd->dates();
            QL_REQUIRE(dates.size() == 1, "Need exactly one payment date for cash settled European option.");
            paymentDate = dates[0];
        }
        QL_REQUIRE(paymentDate >= expiryDate, "Payment date must be greater than or equal to expiry date.");
    }
    QuantLib::ext::shared_ptr<Instrument> ptBarrier;
    QuantLib::ext::shared_ptr<Instrument> rebateInstrument;

    // bool exercised = false;
    // Real exercisePrice = Null<Real>();
    Barrier::Type barrierType = parseBarrierType(barrier_.type());

    Option::Type rebateType;
    if (barrierType == Barrier::Type::UpIn || barrierType == Barrier::Type::DownOut) {
        // Payoff - Up&Out / Down&In Digital Option with barrier B payoff rebate
        rebateType = Option::Put;
    } else {
        // Payoff - Up&In / Down&Out Digital Option with barrier B payoff rebate
        rebateType = Option::Call;
    }

    PartialBarrier::Range barrierRangeType;
    Date coverEventDate;
    if (endDate_ != "" && parseDate(endDate_) < expiryDate) {
        DLOG("Mapped partial-time barrier type 'Start'.");
        QL_REQUIRE(endDate_ != "", "End date must be given for 'Start' style partial-time barriers.");
        coverEventDate = parseDate(endDate_);
        barrierRangeType = PartialBarrier::Start;
    } else {
        DLOG("Mapped partial-time barrier type 'End'.");
        QL_REQUIRE(startDate_ != "", "Start date must be given for 'EndB2' style partial-time barriers.");
        coverEventDate = parseDate(startDate_);
        barrierRangeType = PartialBarrier::EndB2;
    }

    if (paymentDate > expiryDate) {
        /*
        // Has the option been marked as exercised
        const QuantLib::ext::optional<OptionExerciseData>& oed = option_.exerciseData();
        if (oed) {
            QL_REQUIRE(oed->date() == expiryDate, "The supplied exercise date ("
                                                      << io::iso_date(oed->date())
                                                      << ") should equal the option's expiry date ("
                                                      << io::iso_date(expiryDate) << ").");
            exercised = true;
            exercisePrice = oed->price();
        }

        QuantLib::ext::shared_ptr<FxIndex> fxIndex;
        if (option_.isAutomaticExercise()) {
            QL_REQUIRE(!fxIndex_.empty(), "FX european barrier option trade with delay payment "
                                              << id() << ": the FXIndex node needs to be populated.");
            fxIndex = buildFxIndex(fxIndex_, soldCcy.code(), boughtCcy.code(), engineFactory->market(),
                                   engineFactory->configuration(MarketContext::pricing));
            requiredFixings_.addFixingDate(expiryDate, fxIndex_, paymentDate);
        }

        // vanillaK = QuantLib::ext::make_shared<CashSettledEuropeanOption>(
        //     type, strike, expiryDate, paymentDate, option_.isAutomaticExercise(), fxIndex, exercised, exercisePrice);
        rebateInstrument = QuantLib::ext::make_shared<CashSettledEuropeanOption>(rebateType, level, rebate, expiryDate,
                                                                         paymentDate, option_.isAutomaticExercise(),
                                                                         fxIndex, exercised, exercisePrice);*/
    } else {
        // Payoff - European Option with strike K
        QuantLib::ext::shared_ptr<StrikedTypePayoff> payoff(new PlainVanillaPayoff(type, strike));

        ptBarrier =
            QuantLib::ext::make_shared<PartialTimeBarrierOption>(barrierType, barrierRangeType, barrier_.levels()[0].value(),
                                                         barrier_.rebate(), coverEventDate, payoff, exercise);
        // rebateInstrument = QuantLib::ext::make_shared<VanillaOption>(rebatePayoff, exercise);
    }

    // This is for when/if a PayoffCurrency is added to the instrument,
    // which would require flipping the underlying currency pair
    // const bool flipResults = false;

    // set pricing engines
    QuantLib::ext::shared_ptr<EngineBuilder> builder;
    QuantLib::ext::shared_ptr<EngineBuilder> ptBarrierBuilder;
    QuantLib::ext::shared_ptr<VanillaOptionEngineBuilder> fxOptBuilder;

    if (paymentDate > expiryDate) {
        // builder = engineFactory->builder("FxOptionEuropeanCS");
        // QL_REQUIRE(builder, "No builder found for FxOptionEuropeanCS");
        // fxOptBuilder = QuantLib::ext::dynamic_pointer_cast<FxEuropeanCSOptionEngineBuilder>(builder);

        // ptBarrierBuilder = engineFactory->builder("FxPartialTimeBarrierOptionCS");
        // QL_REQUIRE(ptBarrierBuilder, "No builder found for FxPartialTimeBarrierOptionCS");
        // auto fxDigitalOptBuilder = QuantLib::ext::dynamic_pointer_cast<FxDigitalCSOptionEngineBuilder>(digitalBuilder);
        //  ptBarrier->setPricingEngine(fxDigitalOptBuilder->engine(boughtCcy, soldCcy));
        //  rebateInstrument->setPricingEngine(fxDigitalOptBuilder->engine(boughtCcy, soldCcy));
    } else {
        builder = engineFactory->builder("FxOption");
        QL_REQUIRE(builder, "No builder found for FxOption");
        fxOptBuilder = QuantLib::ext::dynamic_pointer_cast<FxEuropeanOptionEngineBuilder>(builder);

        ptBarrierBuilder = engineFactory->builder("FxPartialTimeBarrierOption");
        QL_REQUIRE(ptBarrierBuilder, "No builder found for FxPartialTimeBarrierOption");
        auto fxPtBarrierOptBuilder =
            QuantLib::ext::dynamic_pointer_cast<FxPartialTimeBarrierOptionAnalyticEngineBuilder>(ptBarrierBuilder);
        ptBarrier->setPricingEngine(fxPtBarrierOptBuilder->engine(boughtCcy, soldCcy));
        // rebateInstrument->setPricingEngine(fxDigitalOptBuilder->engine(boughtCcy, soldCcy, flipResults));
    }

    // Add additional premium payments
    Position::Type positionType = parsePositionType(option_.longShort());
    Real bsInd = (positionType == QuantLib::Position::Long ? 1.0 : -1.0);
    Real mult = boughtAmount_ * bsInd;

    std::vector<QuantLib::ext::shared_ptr<Instrument>> additionalInstruments;
    std::vector<Real> additionalMultipliers;
    Date lastPremiumDate = addPremiums(additionalInstruments, additionalMultipliers, mult, option_.premiumData(), bsInd,
                                       soldCcy, engineFactory, ptBarrierBuilder->configuration(MarketContext::pricing));

    instrument_ = QuantLib::ext::shared_ptr<InstrumentWrapper>(
        new VanillaInstrument(ptBarrier, mult, additionalInstruments, additionalMultipliers));

    npvCurrency_ = soldCurrency_; // sold is the domestic
    notional_ = soldAmount_;
    notionalCurrency_ = soldCurrency_;
    maturity_ = std::max({lastPremiumDate, paymentDate}); // delayed pay date is only affecting the maturity

    additionalData_["boughtCurrency"] = boughtCurrency_;
    additionalData_["boughtAmount"] = boughtAmount_;
    additionalData_["soldCurrency"] = soldCurrency_;
    additionalData_["soldAmount"] = soldAmount_;
    if (!fxIndex_.empty())
        additionalData_["FXIndex"] = fxIndex_;

    // ISDA taxonomy
    // ISDA taxonomy
    additionalData_["isdaAssetClass"] = string("Foreign Exchange");
    additionalData_["isdaBaseProduct"] = string("Simple Exotic");
    additionalData_["isdaSubProduct"] = string("Barrier");
    additionalData_["isdaTransaction"] = string("");
}

void FxPartialTimeBarrierOption::fromXML(XMLNode* node) {
    Trade::fromXML(node);
    XMLNode* fxNode = XMLUtils::getChildNode(node, "FxPartialTimeBarrierOptionData");
    QL_REQUIRE(fxNode, "No FxPartialTimeBarrierOptionData Node");
    option_.fromXML(XMLUtils::getChildNode(fxNode, "OptionData"));
    barrier_.fromXML(XMLUtils::getChildNode(fxNode, "BarrierData"));
    startDate_ = XMLUtils::getChildValue(fxNode, "StartDate", false);
    endDate_ = XMLUtils::getChildValue(fxNode, "EndDate", false);
    boughtCurrency_ = XMLUtils::getChildValue(fxNode, "BoughtCurrency", true);
    soldCurrency_ = XMLUtils::getChildValue(fxNode, "SoldCurrency", true);
    boughtAmount_ = XMLUtils::getChildValueAsDouble(fxNode, "BoughtAmount", true);
    soldAmount_ = XMLUtils::getChildValueAsDouble(fxNode, "SoldAmount", true);
    fxIndex_ = XMLUtils::getChildValue(fxNode, "FXIndex", false, "");
}

XMLNode* FxPartialTimeBarrierOption::toXML(XMLDocument& doc) const {
    XMLNode* node = Trade::toXML(doc);
    XMLNode* fxNode = doc.allocNode("FxPartialTimeBarrierOptionData");
    XMLUtils::appendNode(node, fxNode);

    XMLUtils::appendNode(fxNode, option_.toXML(doc));
    XMLUtils::appendNode(fxNode, barrier_.toXML(doc));
    if (startDate_ != "")
        XMLUtils::addChild(doc, fxNode, "StartDate", startDate_);
    if (endDate_ != "")
        XMLUtils::addChild(doc, fxNode, "EndDate", endDate_);
    XMLUtils::addChild(doc, fxNode, "BoughtCurrency", boughtCurrency_);
    XMLUtils::addChild(doc, fxNode, "BoughtAmount", boughtAmount_);
    XMLUtils::addChild(doc, fxNode, "SoldCurrency", soldCurrency_);
    XMLUtils::addChild(doc, fxNode, "SoldAmount", soldAmount_);

    if (!fxIndex_.empty())
        XMLUtils::addChild(doc, fxNode, "FXIndex", fxIndex_);

    return node;
}
} // namespace data
} // namespace ore
