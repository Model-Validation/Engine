/*
 Copyright (C) 2019 Quaternion Risk Management Ltd
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

#include <ored/utilities/calendaradjustmentconfig.hpp>
#include <ored/utilities/calendarparser.hpp>
#include <ored/utilities/parsers.hpp>
#include <ored/utilities/to_string.hpp>
#include <ql/time/calendar.hpp>
#include <string>
namespace ore {
namespace data {

CalendarAdjustmentConfig::CalendarAdjustmentConfig() {}

void CalendarAdjustmentConfig::addHolidays(const string& calname, const Date& d) {
    additionalHolidays_[normalisedName(calname)].insert(d);
}

void CalendarAdjustmentConfig::addBusinessDays(const string& calname, const Date& d) {
    additionalBusinessDays_[normalisedName(calname)].insert(d);
}

void CalendarAdjustmentConfig::addBaseCalendar(const string& calname, const string& s) {
    baseCalendars_[normalisedName(calname)] = s;
}

void CalendarAdjustmentConfig::addOverride(const string& calname, const Date& start, const Date& end) {
    QL_REQUIRE(start != Date() && end != Date() && start <= end,
               "CalendarAdjustmentConfig: invalid override window [" << start << ", " << end << "] for calendar "
                                                                     << calname);
    overrideWindows_[normalisedName(calname)] = std::make_pair(start, end);
}

bool CalendarAdjustmentConfig::isOverride(const string& calname) const {
    return overrideWindows_.find(normalisedName(calname)) != overrideWindows_.end();
}

std::pair<Date, Date> CalendarAdjustmentConfig::getOverrideWindow(const string& calname) const {
    auto it = overrideWindows_.find(normalisedName(calname));
    return it != overrideWindows_.end() ? it->second : std::make_pair(Date(), Date());
}

const set<Date>& CalendarAdjustmentConfig::getHolidays(const string& calname) const {
    auto holidays = additionalHolidays_.find(normalisedName(calname));
    if (holidays != additionalHolidays_.end()) {
        return holidays->second;
    } else {
        static set<Date> empty;
        return empty;
    }
}

const set<Date>& CalendarAdjustmentConfig::getBusinessDays(const string& calname) const {
    auto businessDays = additionalBusinessDays_.find(normalisedName(calname));
    if (businessDays != additionalBusinessDays_.end()) {
        return businessDays->second;
    } else {
        static set<Date> empty;
        return empty;
    }
}

set<string> CalendarAdjustmentConfig::getCalendars() const {
    set<string> cals;
    for (auto it : additionalHolidays_) {
        cals.insert(it.first);
    }
    for (auto it : additionalBusinessDays_) {
        cals.insert(it.first);
    }
    for (auto it : overrideWindows_) {
        cals.insert(it.first);
    }
    return cals;
}


const string& CalendarAdjustmentConfig::getBaseCalendar(const string& calname) const {
    auto bc = baseCalendars_.find(normalisedName(calname));

    if (bc != baseCalendars_.end()) {
        return bc->second;
    } else {
        static const string empty = "";
        return empty;
    }
}
string CalendarAdjustmentConfig::normalisedName(const string& c) const { return parseCalendar(c).name(); }

void CalendarAdjustmentConfig::append(const CalendarAdjustmentConfig& c) {
    for (auto it : c.getCalendars()) {
        for (auto h : c.getHolidays(it)) {
            addHolidays(it, h);
        }
        for (auto b : c.getBusinessDays(it)) {
            addBusinessDays(it, b);
        }
        if (c.isOverride(it)) {
            auto w = c.getOverrideWindow(it);
            addOverride(it, w.first, w.second);
        }
    }
};

void CalendarAdjustmentConfig::addCalendars() {
    for (auto cal : baseCalendars_) {
        string calname = cal.first;
        CalendarParser::instance().addCalendar(cal.second, calname);
    }
}

void CalendarAdjustmentConfig::applyAdjustments(XMLNode* calnode, const string& calname, Calendar cal) {
    set<Date> holidays, businessDays;
    vector<string> holidayDates = XMLUtils::getChildrenValues(calnode, "AdditionalHolidays", "Date");
    for (auto holiday : holidayDates) {
        try {
            Date h = parseDate(holiday);
            addHolidays(calname, h);
            holidays.insert(h);
        } catch (std::exception&) {
            ALOG("error parsing holiday " << holiday << " for calendar " << calname);
        }
    }
    vector<string> businessDates = XMLUtils::getChildrenValues(calnode, "AdditionalBusinessDays", "Date");
    for (auto businessDay : businessDates) {
        try {
            Date b = parseDate(businessDay);
            addBusinessDays(calname, b);
            businessDays.insert(b);
        } catch (std::exception&) {
            ALOG("error parsing business day " << businessDay << " for calendar " << calname);
        }
    }

    bool overrideHolidays = XMLUtils::getChildValueAsBool(calnode, "Override", false, false);

    if (!overrideHolidays) {
        for (auto const& h : holidays)
            cal.addHoliday(h);
        for (auto const& b : businessDays)
            cal.removeHoliday(b);
        return;
    }

    // Override: within [start, end] the weekday holidays of the calendar are replaced by the given holidays, the
    // weekends of the calendar are kept unless given as business days. Outside the window the calendar is unchanged.
    Date start, end;
    string startStr = XMLUtils::getChildValue(calnode, "OverrideStart", false);
    string endStr = XMLUtils::getChildValue(calnode, "OverrideEnd", false);
    if (!startStr.empty())
        start = parseDate(startStr);
    if (!endStr.empty())
        end = parseDate(endStr);
    if (start == Date() || end == Date()) {
        set<Date> all(holidays);
        all.insert(businessDays.begin(), businessDays.end());
        QL_REQUIRE(!all.empty(), "CalendarAdjustmentConfig: Override for calendar "
                                     << calname << " requires OverrideStart and OverrideEnd if no dates are given");
        if (start == Date())
            start = *all.begin();
        if (end == Date())
            end = *all.rbegin();
    }
    QL_REQUIRE(start <= end, "CalendarAdjustmentConfig: invalid override window [" << start << ", " << end
                                                                                    << "] for calendar " << calname);
    // validate before modifying the calendar, so that an invalid config does not leave it partially adjusted
    for (auto const& h : holidays)
        QL_REQUIRE(h >= start && h <= end, "CalendarAdjustmentConfig: holiday "
                                               << h << " for calendar " << calname << " is outside the override window ["
                                               << start << ", " << end << "]");
    for (auto const& b : businessDays)
        QL_REQUIRE(b >= start && b <= end, "CalendarAdjustmentConfig: business day "
                                               << b << " for calendar " << calname << " is outside the override window ["
                                               << start << ", " << end << "]");
    addOverride(calname, start, end);

    for (Date d = start; d <= end; ++d) {
        bool isHoliday;
        if (businessDays.count(d))
            isHoliday = false;
        else if (holidays.count(d))
            isHoliday = true;
        else
            isHoliday = cal.isWeekend(d.weekday());
        if (isHoliday && cal.isBusinessDay(d))
            cal.addHoliday(d);
        else if (!isHoliday && cal.isHoliday(d))
            cal.removeHoliday(d);
    }
}

void CalendarAdjustmentConfig::fromXML(XMLNode* node) {
    XMLUtils::checkNode(node, "CalendarAdjustments");

    //we loop once skipping any new calendars
    for (auto calnode : XMLUtils::getChildrenNodes(node, "Calendar")) {
        string calname = XMLUtils::getAttribute(calnode, "name");
        string baseCalendar = XMLUtils::getChildValue(calnode, "BaseCalendar", false);
        if (baseCalendar != "") {
            // we check here if the baseCalendar is in the map before the new calendars are added
            // this is because we don't want any new calendars to be defined in terms of other new calendars
            parseCalendar(baseCalendar);
            continue;
        }
        applyAdjustments(calnode, calname, parseCalendar(calname));
    }
    //then loop again adding the new calendars
    for (auto calnode : XMLUtils::getChildrenNodes(node, "Calendar")) {
        string calname = XMLUtils::getAttribute(calnode, "name");
        string baseCalendar = XMLUtils::getChildValue(calnode, "BaseCalendar", false);
        if (baseCalendar == "")
            continue;
        Calendar cal = CalendarParser::instance().addCalendar(baseCalendar, calname);
        applyAdjustments(calnode, calname, cal);
        addBaseCalendar(calname, baseCalendar);
    }

}

XMLNode* CalendarAdjustmentConfig::toXML(XMLDocument& doc) const {
    XMLNode* node = doc.allocNode("CalendarAdjustments");
    for (auto cal : getCalendars()) {
        XMLNode* calendarNode = XMLUtils::addChild(doc, node, "Calendar");
        XMLUtils::addAttribute(doc, calendarNode, "name", cal);
        if (getBaseCalendar(cal) != "")
            XMLUtils::addChild(doc, calendarNode, "BaseCalendar", getBaseCalendar(cal));
        if (isOverride(cal)) {
            auto w = getOverrideWindow(cal);
            XMLUtils::addChild(doc, calendarNode, "Override", true);
            XMLUtils::addChild(doc, calendarNode, "OverrideStart", to_string(w.first));
            XMLUtils::addChild(doc, calendarNode, "OverrideEnd", to_string(w.second));
        }
        XMLNode* ahd = XMLUtils::addChild(doc, calendarNode, "AdditionalHolidays");
        for (auto hol : getHolidays(cal)) {
            XMLUtils::addChild(doc, ahd, "Date", to_string(hol));
        }
        XMLNode* abd = XMLUtils::addChild(doc, calendarNode, "AdditionalBusinessDays");
        for (auto bd : getBusinessDays(cal)) {
            XMLUtils::addChild(doc, abd, "Date", to_string(bd));
        }
    }
    return node;
}


} // namespace data
} // namespace ore
