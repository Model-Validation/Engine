/*
 Copyright (C) 2024 Quaternion Risk Management Ltd
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

/*! \file scenario/scenarioutilities.hpp
    \brief Scenario utility functions
    \ingroup scenario
*/

#pragma once

#include <orea/scenario/scenario.hpp>

#include <set>

namespace ore {
namespace analytics {

QuantLib::Real getDifferenceScenario(const RiskFactorKey::KeyType keyType, const QuantLib::Real v1,
                                     const QuantLib::Real v2);

QuantLib::ext::shared_ptr<Scenario> getDifferenceScenario(const QuantLib::ext::shared_ptr<Scenario>& s1,
                                                          const QuantLib::ext::shared_ptr<Scenario>& s2,
                                                          const QuantLib::Date& targetScenarioAsOf = QuantLib::Date(),
                                                          const QuantLib::Real targetScenarioNumeraire = 0.0,
                                                          const bool allowAdditionalKeysInS2 = false);

QuantLib::Real addDifferenceToScenario(const RiskFactorKey::KeyType keyType, const QuantLib::Real v,
                                       const QuantLib::Real d);

QuantLib::ext::shared_ptr<Scenario> addDifferenceToScenario(const QuantLib::ext::shared_ptr<Scenario>& s,
                                                            const QuantLib::ext::shared_ptr<Scenario>& d,
                                                            const QuantLib::Date& targetScenarioAsOf = QuantLib::Date(),
                                                            const QuantLib::Real targetScenarioNumeraire = 0.0);

QuantLib::ext::shared_ptr<Scenario>
recastScenario(const QuantLib::ext::shared_ptr<Scenario>& scenario,
               const std::map<std::pair<RiskFactorKey::KeyType, std::string>, std::vector<std::vector<QuantLib::Real>>>&
                   oldCoordinates,
               const std::map<std::pair<RiskFactorKey::KeyType, std::string>, std::vector<std::vector<QuantLib::Real>>>&
                   newCoordinates);

QuantLib::ext::shared_ptr<Scenario> recastScenario(
    const QuantLib::ext::shared_ptr<Scenario>& scenario,
    const std::map<std::pair<RiskFactorKey::KeyType, std::string>, std::vector<std::vector<QuantLib::Real>>>&
        oldCoordinates,
    const std::set<std::tuple<RiskFactorKey::KeyType, std::string, std::vector<std::vector<QuantLib::Real>>>>&
        newCoordinates);

QuantLib::Real sanitizeScenarioValue(const RiskFactorKey::KeyType keyType, const bool isPar,
                                     const QuantLib::Real rawValue);

} // namespace analytics
} // namespace ore
