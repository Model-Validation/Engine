# cpp files, this list is maintained manually

set(QuantExt-Test_SRC analyticcashsettledeuropeanengine.cpp
analyticlgmswaptionengine.cpp
blackvariancecurve.cpp
blackvariancesurfacesparse.cpp
blackvariancesurfacestddevs.cpp
blackvolatilitysurfacemoneyness.cpp
blackvolsurfacedelta.cpp
bonds.cpp
capfloortermvolcurve.cpp
cashflow.cpp
commodityforward.cpp
correlationtermstructure.cpp
cpicapfloor.cpp
crossassetmodel.cpp
crossassetmodel2.cpp
crossassetmodelparametrizations.cpp
crossccybasismtmresetswap.cpp
crossccybasismtmresetswaphelper.cpp
crossccyfixfloatswap.cpp
crossccyfixfloatswaphelper.cpp
currency.cpp
deltagammavar.cpp
deposit.cpp
discountcurve.cpp
discountingcommodityforwardengine.cpp
discountratiomodifiedcurve.cpp
dynamicblackvoltermstructure.cpp
dynamicswaptionvolmatrix.cpp
equityblackvolsurfaceproxy.cpp
equityforwardcurvestripper.cpp
fillemptymatrix.cpp
forwardbond.cpp
fxvolsmile.cpp
index.cpp
interpolatedyoycapfloortermpricesurface.cpp
logquote.cpp
optionletstripper.cpp
payment.cpp
piecewiseatmoptionletcurve.cpp
piecewiseoptionletcurve.cpp
piecewiseoptionletstripper.cpp
pricecurve.cpp
pricetermstructureadapter.cpp
qle_calendars.cpp
quadraticinterpolation.cpp
ratehelpers.cpp
stabilisedglls.cpp
staticallycorrectedyieldtermstructure.cpp
strippedoptionletadapter.cpp
strippedoptionletadapter2.cpp
survivalprobabilitycurve.cpp
swaptionvolatilityconverter.cpp
swaptionvolconstantspread.cpp
testsuite.cpp)

add_executable(quantext-test-suite ${QuantExt-Test_SRC})
target_link_libraries(quantext-test-suite ${QL_LIB_NAME})
target_link_libraries(quantext-test-suite ${QLE_LIB_NAME})
target_link_libraries(quantext-test-suite ${Boost_LIBRARIES})

add_test(NAME quantext-test-suite COMMAND quantext-test-suite WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
