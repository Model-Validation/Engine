/*
 Copyright (C) 2018 Quaternion Risk Management Ltd
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

#include <ql/event.hpp>

#include <qle/pricingengines/discountingcommodityforwardengine.hpp>

using namespace std;
using namespace QuantLib;

namespace QuantExt {

DiscountingCommodityForwardEngine::DiscountingCommodityForwardEngine(const Handle<YieldTermStructure>& discountCurve,
                                                                     boost::optional<bool> includeSettlementDateFlows,
                                                                     const Date& npvDate)
    : discountCurve_(discountCurve), includeSettlementDateFlows_(includeSettlementDateFlows),
      npvDate_(npvDate) {

    registerWith(discountCurve_);
}

void DiscountingCommodityForwardEngine::calculate() const {

    const auto& index = arguments_.index;
    Date npvDate = npvDate_;
    if (npvDate == Null<Date>()) {
        const auto& priceCurve = index->priceCurve();
        QL_REQUIRE(!priceCurve.empty(), "DiscountingCommodityForwardEngine: need a non-empty price curve.");
        npvDate = priceCurve->referenceDate();
    }

    const Date& maturity = arguments_.maturityDate;
    Date paymentDate = maturity;
    if (!arguments_.physicallySettled && arguments_.paymentDate != Date()) {
        paymentDate = arguments_.paymentDate;
    }

    results_.value = 0.0;
    if (!detail::simple_event(paymentDate).hasOccurred(Date(), includeSettlementDateFlows_)) {

        Real buySell = arguments_.position == Position::Long ? 1.0 : -1.0;
        Real forwardPrice = index->fixing(maturity);
        
        if (!arguments_.observationDates.empty()) {
            std::vector<Real> fixings;
            forwardPrice = 0; // Reset and accumulate all observed fixings instead
            for (Date obsDate : arguments_.observationDates) {
                fixings.push_back(index->fixing(obsDate));
                forwardPrice += index->fixing(obsDate);
            }
            
            forwardPrice /= arguments_.observationDates.size();
            results_.additionalResults["fixings"] = fixings;
        }
        results_.value = arguments_.quantity * buySell * (forwardPrice - arguments_.strike) *
                         discountCurve_->discount(paymentDate) / discountCurve_->discount(npvDate);
        results_.additionalResults["forwardPrice"] = forwardPrice;
        results_.additionalResults["discountPaymentDate"] = discountCurve_->discount(paymentDate);
        results_.additionalResults["discountNpvDate"] = discountCurve_->discount(npvDate);
        results_.additionalResults["currentNotional"] = arguments_.strike * arguments_.quantity;
    }
}

} // namespace QuantExt
