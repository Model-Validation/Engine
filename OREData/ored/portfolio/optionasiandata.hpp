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

/*! \file ored/portfolio/optionasiandata.hpp
    \brief asian option data model and serialization
    \ingroup tradedata
*/

#pragma once

#include <ored/utilities/xmlutils.hpp>
#include <ql/time/date.hpp>

namespace ore {
namespace data {

/*! Serializable object holding asian option data for options with payoff type Asian.
    \ingroup tradedata
*/
class OptionAsianData : public XMLSerializable {
public:

    //! Default constructor
    OptionAsianData();

    //! Constructor taking an average type, average mode, and vector of fixing string dates.
    OptionAsianData(const std::string& asianType, const std::string& averageType,
                    const std::vector<std::string>& strFixingDates);
    //! Constructor taking an average type, average mode, and vector of fixing dates.
    OptionAsianData(const std::string& asianType, const std::string& averageType,
                    const std::vector<QuantLib::Date>& fixingDates);

    //! \name Inspectors
    //@{
    const std::string& asianType() const { return asianType_;  }
    const std::string& averageType() const { return averageType_;  }
    const std::vector<QuantLib::Date> fixingDates() const { return fixingDates_; }
    //@}

    //! \name Serialisation
    //@{
    virtual void fromXML(XMLNode* node) override;
    virtual XMLNode* toXML(XMLDocument& doc) override;
    //@}

private:
    std::vector<std::string> strFixingDates_;

    std::string asianType_;
    std::string averageType_;
    std::vector<QuantLib::Date> fixingDates_;

    //! Initialisation
    void init();
};

} // namespace data
} // namespace ore
