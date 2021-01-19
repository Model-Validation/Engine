/*
 Copyright (C) 2020 Fredrik Gerdin Börjesson
 All rights reserved.
*/

/*! \file ored/portfolio/asianoption.hpp
\brief asian option representation
\ingroup tradedata
*/

#include <ored/portfolio/builders/asianoption.hpp>
#include <ored/portfolio/asianoption.hpp>
#include <ored/portfolio/optionwrapper.hpp>
#include <ored/utilities/log.hpp>
#include <ql/instruments/asianoption.hpp>
#include <ql/instruments/averagetype.hpp>
#include <ql/errors.hpp>

namespace ore {
namespace data {

void AsianOptionTrade::build(const boost::shared_ptr<EngineFactory>& engineFactory) {
    QuantLib::Date today = engineFactory->market()->asofDate();
    Currency ccy = parseCurrency(currency_);

    QL_REQUIRE(tradeActions().empty(), "TradeActions not supported for AsianOption");

    // Payoff
    Option::Type type = parseOptionType(option_.callPut());
    boost::shared_ptr<StrikedTypePayoff> payoff(new PlainVanillaPayoff(type, strike_));

    QuantLib::Exercise::Type exerciseType = parseExerciseType(option_.style());
    QL_REQUIRE(option_.exerciseDates().size() == 1, "Invalid number of excercise dates");
    expiryDate_ = parseDate(option_.exerciseDates().front());

    // Set the maturity date equal to the expiry date. It may get updated below if option is cash settled with
    // payment after expiry.
    maturity_ = expiryDate_;

    // Exercise
    boost::shared_ptr<Exercise> exercise;
    switch (exerciseType) {
    case QuantLib::Exercise::Type::European: {
        exercise = boost::make_shared<EuropeanExercise>(expiryDate_);
        break;
    }
    case QuantLib::Exercise::Type::American: {
        ELOG("The implemented Asian pricing engines do not support American options.");
        QL_FAIL("The implemented Asian pricing engines do not support American options.");
        //exercise = boost::make_shared<AmericanExercise>(expiryDate_, option_.payoffAtExpiry());
        break;
    }
    default:
        QL_FAIL("Option style " << option_.style() << " is not supported");
    }

    // Create the instrument and the populate the name for the engine builder.
    boost::shared_ptr<Instrument> asian;
    string tradeTypeBuilder = tradeType_;
    tradeTypeBuilder += (exerciseType == Exercise::European) ? "" : "American";
    Settlement::Type settlementType = parseSettlementType(option_.settlement());

    bool isEuropDeferredCS = false;
    if (exerciseType == Exercise::European && settlementType == Settlement::Cash) {
        // We have a European cash-settled option

        // Get payment data
        const boost::optional<OptionPaymentData>& opd = option_.paymentData();
        Date paymentDate = expiryDate_;
        if (opd) {
            if (opd->rulesBased()) {
                const Calendar& cal = opd->calendar();
                QL_REQUIRE(cal != Calendar(), "Need non-empty calendar for rules based payment date.");
                paymentDate = cal.advance(expiryDate_, opd->lag(), Days, opd->convention());
            } else {
                const vector<Date>& dates = opd->dates();
                QL_REQUIRE(dates.size() == 1, "Need exactly one payment date for cash settled European option.");
                paymentDate = dates[0];
            }
            QL_REQUIRE(paymentDate >= expiryDate_, "Payment date must be greater than or equal to expiry date.");
        }
        
        if (paymentDate > expiryDate_) {
            isEuropDeferredCS = true;
            tradeTypeBuilder += "EuropeanCS";
            // TODO: If support is needed/added, add logic from ored/portfolio/vanillaoption.cpp for updating of maturity date.
            ELOG("Asian options do not support deferred cash settlement payments, "
                 << "i.e. where payment date > maturity date.");
            QL_FAIL("Asian options do not support deferred cash-settlement payments, i.e. where payment date > "
                    "maturity date.");
        }
    }

    // Create builder in advance to determine if continuous or discrete asian option.
    std::string averageType = option_.asianData()->averageType();
    std::string asianType = option_.asianData()->asianType();
    tradeTypeBuilder += averageType; // Add Arithmetic/Geometric
    tradeTypeBuilder += asianType;   // Add Price/Strike
    boost::shared_ptr<EngineBuilder> builder = engineFactory->builder(tradeTypeBuilder);
    QL_REQUIRE(builder, "No builder found for " << tradeTypeBuilder);
    // TODO: Add check for processType tag in advance?
    std::string processType = engineFactory->engineData()->engineParameters(tradeTypeBuilder).at("ProcessType");
    QL_REQUIRE(processType != "", "ProcessType must be configured.");

    QuantLib::Average::Type qlAverageType; // TODO: Improve aesthetics
    if (averageType == "Geometric") {
        qlAverageType = QuantLib::Average::Type::Geometric;
    } else if (averageType == "Arithmetic") {
        qlAverageType = QuantLib::Average::Type::Arithmetic;
    }
    if (!isEuropDeferredCS) {
        // If the option is not a European deferred CS contract, i.e. the standard European/American physical/cash-settled ones. 
        if (processType == "Discrete") {
            Real runningAccumulator = 0;
            if (averageType == "Geometric") {
                runningAccumulator = 1;
            }
            Size pastFixings = 0;
            std::vector<QuantLib::Date> fixingDates = option_.asianData()->fixingDates();
            for (QuantLib::Date fixingDate : fixingDates) {
                if (fixingDate < today) {
                    // TODO: Validate if correct loading of fixings!
                    Real fixingValue = engineFactory->market()->equityCurve(assetName_)->fixing(fixingDate);
                    if (averageType == "Geometric") {
                        runningAccumulator *= fixingValue;
                    } else if (averageType == "Arithmetic") {
                        runningAccumulator += fixingValue;
                    }
                    ++pastFixings;
                }
            }

            asian = boost::make_shared<QuantLib::DiscreteAveragingAsianOption>(
                qlAverageType, runningAccumulator, pastFixings, fixingDates, payoff, exercise);
        } else if (processType == "Continuous") {
            asian = boost::make_shared<QuantLib::ContinuousAveragingAsianOption>(qlAverageType, payoff, exercise);
        }
    } else {
        ELOG("Asian options do not support deferred cash settlement payments, "
             << "i.e. where payment date > maturity date.");
        QL_FAIL("Asian options do not support deferred cash-settlement payments, i.e. where payment date > "
                << "maturity date.");
    }


    // Only try to set an engine on the option instrument if it is not expired. This avoids errors in
    // engine builders that rely on the expiry date being in the future.
    string configuration = Market::defaultConfiguration;
    if (!asian->isExpired()) {
        boost::shared_ptr<AsianOptionEngineBuilder> asianOptionBuilder =
            boost::dynamic_pointer_cast<AsianOptionEngineBuilder>(builder);

        asian->setPricingEngine(asianOptionBuilder->engine(assetName_, ccy, expiryDate_, strike_));

        configuration = asianOptionBuilder->configuration(MarketContext::pricing);
    } else {
        DLOG("No engine attached for option on trade " << id() << " with expiry date " << io::iso_date(expiryDate_)
                                                       << " because it is expired.");
    }

    Position::Type positionType = parsePositionType(option_.longShort());
    Real bsInd = (positionType == QuantLib::Position::Long ? 1.0 : -1.0);
    Real mult = quantity_ * bsInd;

    // If premium data is provided
    // 1) build the fee trade and pass it to the instrument wrapper for pricing
    // 2) add fee payment as additional trade leg for cash flow reporting
    std::vector<boost::shared_ptr<Instrument>> additionalInstruments;
    std::vector<Real> additionalMultipliers;
    if (option_.premiumPayDate() != "" && option_.premiumCcy() != "") {
        Real premiumAmount = -bsInd * option_.premium(); // pay if long, receive if short
        Currency premiumCurrency = parseCurrency(option_.premiumCcy());
        Date premiumDate = parseDate(option_.premiumPayDate());
        addPayment(additionalInstruments, additionalMultipliers, premiumDate, premiumAmount, premiumCurrency, ccy,
                   engineFactory, configuration);
        DLOG("Option premium added for asian option " << id());
    }

    // TODO: Use something like EuropeanOptionWrapper instead? After all, an asian option is not
    // a vanilla instrument. However, the available European-/AmericanOptionWrapper is not directly
    // applicable as it calls for both an instrument and an underlying instrument.
    // Given how the asians are priced, though, we still manage fine using the VanillaInstrument wrapper
    // for the time being.
    instrument_ = boost::shared_ptr<InstrumentWrapper>(
        new VanillaInstrument(asian, mult, additionalInstruments, additionalMultipliers));
    //instrument_ = boost::shared_ptr<InstrumentWrapper>(
    //    boost::make_shared<EuropeanOptionWrapper>(...));

    npvCurrency_ = currency_;

    // Notional - we really need todays spot to get the correct notional.
    // But rather than having it move around we use strike * quantity
    notional_ = strike_ * quantity_;
    notionalCurrency_ = currency_;
}

void AsianOptionTrade::fromXML(XMLNode* node) { Trade::fromXML(node); }

XMLNode* AsianOptionTrade::toXML(XMLDocument& doc) {
    XMLNode* node = Trade::toXML(doc);
    return node;
}

} // namespace data
} // namespace ore
