/*
 Copyright (C) 2026 Quaternion Risk Management Ltd
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

#pragma once

#include <ql/instruments/yearonyearinflationswap.hpp>
#include <ql/termstructures/bootstraphelper.hpp>
#include <ql/termstructures/inflationtermstructure.hpp>

namespace QuantExt {

//! Year-on-year inflation-swap helper using a zero inflation curve as bootstrap target.
class MixedYearOnYearInflationSwapHelper
    : public QuantLib::RelativeDateBootstrapHelper<QuantLib::ZeroInflationTermStructure> {
public:
    MixedYearOnYearInflationSwapHelper(const QuantLib::Handle<QuantLib::Quote>& quote,
                                       const QuantLib::Period& swapObsLag,
                                       const QuantLib::Date& maturity,
                                       QuantLib::Calendar calendar,
                                       QuantLib::BusinessDayConvention paymentConvention,
                                       QuantLib::DayCounter dayCounter,
                                       const QuantLib::ext::shared_ptr<QuantLib::ZeroInflationIndex>& zii,
                                       QuantLib::CPI::InterpolationType interpolation);

    void setTermStructure(QuantLib::ZeroInflationTermStructure*) override;
    QuantLib::Real impliedQuote() const override;

    QuantLib::ext::shared_ptr<QuantLib::YearOnYearInflationSwap> swap() const { return yyiis_; }

protected:
    void initializeDates() override;

private:
    QuantLib::Period swapObsLag_;
    QuantLib::Date maturity_;
    QuantLib::Calendar calendar_;
    QuantLib::BusinessDayConvention paymentConvention_;
    QuantLib::DayCounter dayCounter_;
    QuantLib::ext::shared_ptr<QuantLib::YoYInflationIndex> yii_;
    QuantLib::CPI::InterpolationType interpolation_;
    QuantLib::ext::shared_ptr<QuantLib::YearOnYearInflationSwap> yyiis_;
    QuantLib::Handle<QuantLib::YieldTermStructure> nominalTermStructure_;
    QuantLib::RelinkableHandle<QuantLib::ZeroInflationTermStructure> termStructureHandle_;
};

} // namespace QuantExt
