/*
 Copyright (C) 2016 Quaternion Risk Management Ltd
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

/*! \file scenario/scenario.hpp
    \brief Scenario class
    \ingroup scenario
*/

#pragma once

#include <qle/termstructures/scenario.hpp>

namespace ore::analytics {

// these classes were moved to QuantExt

using RiskFactorKey = QuantExt::RiskFactorKey;
using Scenario = QuantExt::Scenario;
using ShiftScheme = QuantExt::ShiftScheme;
using ShiftType = QuantExt::ShiftType;

} // namespace ore::analytics
