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

#include <ored/portfolio/optionasiandata.hpp>
#include <ored/utilities/parsers.hpp>
#include <algorithm>

namespace ore {
namespace data {

OptionAsianData::OptionAsianData() : asianType_("") {}

OptionAsianData::OptionAsianData(const string& asianType, const string& averageType,
                                 const vector<string>& strFixingDates)
    : asianType_(asianType), averageType_(averageType), strFixingDates_(strFixingDates) {
    init();
}

OptionAsianData::OptionAsianData(const string& asianType, const string& averageType,
                                 const vector<QuantLib::Date>& fixingDates)
    : asianType_(asianType), averageType_(averageType), fixingDates_(fixingDates) {}

void ore::data::OptionAsianData::fromXML(XMLNode* node) {
    XMLUtils::checkNode(node, "AsianData");
    asianType_ = XMLUtils::getChildValue(node, "AsianType", true);
    averageType_ = XMLUtils::getChildValue(node, "AverageType", true);
    strFixingDates_ = XMLUtils::getChildrenValues(node, "FixingDates", "FixingDate", true);
    QL_REQUIRE(strFixingDates_.size() > 0, "Expected 1 or more FixingDate for AsianData.");
    init();
}

XMLNode* OptionAsianData::toXML(XMLDocument& doc) { 
    XMLNode* node = doc.allocNode("AsianData");
    XMLUtils::addChild(doc, node, "AsianType", asianType_);
    XMLUtils::addChild(doc, node, "AverageType", averageType_);
    XMLUtils::addChildren(doc, node, "FixingDates", "FixingDate", strFixingDates_);
    return node;
}

void OptionAsianData::init() {
    // TODO: FIX!
    //fixingDates_ = vector<QuantLib::Date>(strFixingDates_.size());
    fixingDates_.resize(strFixingDates_.size());
    std::transform(strFixingDates_.begin(), strFixingDates_.end(), fixingDates_.begin(),
                   [](string strDate) -> QuantLib::Date { return parseDate(strDate); });
}

} // namespace data
} // namespace ore
