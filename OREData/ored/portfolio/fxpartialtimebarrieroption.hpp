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

/*! \file portfolio/fxpartialbarrieroption.hpp
   \brief FX Partial-Time Barrier Option data model and serialization
   \ingroup portfolio
*/

#pragma once

#include <ored/portfolio/barrierdata.hpp>
#include <ored/portfolio/enginefactory.hpp>
#include <ored/portfolio/fxderivative.hpp>
#include <ored/portfolio/optiondata.hpp>
#include <ql/instruments/barriertype.hpp>

namespace ore {
namespace data {
using std::string;

//! Serializable FX Partial-Time Barrier Option
/*!
  \ingroup tradedata
*/
class FxPartialTimeBarrierOption : public FxSingleAssetDerivative {
public:
    //! Default constructor
    FxPartialTimeBarrierOption()
        : ore::data::Trade("FxPartialTimeBarrierOption"), FxSingleAssetDerivative(""), boughtAmount_(0.0),
          soldAmount_(0.0) {}
    //! Constructor
    FxPartialTimeBarrierOption(Envelope& env, OptionData option, BarrierData barrier, string boughtCurrency,
                               double boughtAmount, string soldCurrency, double soldAmount, string startDate = "",
                               string endDate = "", string calendar = "", string fxIndex = "")
        : ore::data::Trade("FxPartialTimeBarrierOption", env),
          FxSingleAssetDerivative("", env, boughtCurrency, soldCurrency), option_(option), barrier_(barrier),
          startDate_(startDate), endDate_(endDate), calendar_(calendar), fxIndex_(fxIndex), boughtAmount_(boughtAmount),
          soldAmount_(soldAmount) {}

    //! Build QuantLib/QuantExt instrument, link pricing engine
    void build(const QuantLib::ext::shared_ptr<EngineFactory>&) override;

    //! \name Inspectors
    //@{
    const OptionData& option() const { return option_; }
    const BarrierData& barrier() const { return barrier_; }
    double boughtAmount() const { return boughtAmount_; }
    double soldAmount() const { return soldAmount_; }
    const string& startDate() const { return startDate_; }
    const string& endDate() const { return endDate_; }
    const string& calendar() const { return calendar_; }
    const string& fxIndex() const { return fxIndex_; }
    //@}

    //! \name Serialisation
    //@{
    virtual void fromXML(XMLNode* node) override;
    virtual XMLNode* toXML(ore::data::XMLDocument& doc) const override;
    //@}
private:
    bool checkBarrier(Real spot, Barrier::Type type, Real level);
    OptionData option_;
    BarrierData barrier_;
    string startDate_;
    string endDate_;
    string calendar_;
    string fxIndex_;
    double boughtAmount_;
    double soldAmount_;
};
} // namespace data
} // namespace ore
