# cpp files, this list is maintained manually

set(QuantExt_SRC calendars/austria.cpp
calendars/belgium.cpp
calendars/chile.cpp
calendars/cme.cpp
calendars/colombia.cpp
calendars/france.cpp
calendars/ice.cpp
calendars/israel.cpp
calendars/largejointcalendar.cpp
calendars/luxembourg.cpp
calendars/malaysia.cpp
calendars/netherlands.cpp
calendars/peru.cpp
calendars/philippines.cpp
calendars/spain.cpp
calendars/switzerland.cpp
calendars/thailand.cpp
calendars/wmr.cpp
cashflows/averageonindexedcoupon.cpp
cashflows/averageonindexedcouponpricer.cpp
cashflows/blackovernightindexedcouponpricer.cpp
cashflows/brlcdicouponpricer.cpp
cashflows/commodityindexedaveragecashflow.cpp
cashflows/commodityindexedcashflow.cpp
cashflows/couponpricer.cpp
cashflows/cpicoupon.cpp
cashflows/cpicouponpricer.cpp
cashflows/equitycoupon.cpp
cashflows/equitycouponpricer.cpp
cashflows/floatingannuitycoupon.cpp
cashflows/floatingannuitynominal.cpp
cashflows/fxlinkedcashflow.cpp
cashflows/indexedcoupon.cpp
cashflows/lognormalcmsspreadpricer.cpp
cashflows/overnightindexedcoupon.cpp
cashflows/quantocouponpricer.cpp
cashflows/strippedcapflooredcpicoupon.cpp
cashflows/strippedcapflooredyoyinflationcoupon.cpp
cashflows/subperiodscoupon.cpp
cashflows/subperiodscouponpricer.cpp
currencies/africa.cpp
currencies/america.cpp
currencies/asia.cpp
currencies/europe.cpp
currencies/metals.cpp
indexes/bondindex.cpp
indexes/commodityindex.cpp
indexes/equityindex.cpp
indexes/fxindex.cpp
indexes/ibor/brlcdi.cpp
indexes/ibor/ester.cpp
indexes/ibor/primeindex.cpp
indexes/ibor/sofr.cpp
indexes/inflationindexwrapper.cpp
indexes/region.cpp
instruments/averageois.cpp
instruments/brlcdiswap.cpp
instruments/cashsettledeuropeanoption.cpp
instruments/cdsoption.cpp
instruments/commodityapo.cpp
instruments/commodityforward.cpp
instruments/creditdefaultswap.cpp
instruments/crossccybasismtmresetswap.cpp
instruments/crossccybasisswap.cpp
instruments/crossccyfixfloatswap.cpp
instruments/crossccyswap.cpp
instruments/currencyswap.cpp
instruments/deposit.cpp
instruments/equityforward.cpp
instruments/fixedbmaswap.cpp
instruments/forwardbond.cpp
instruments/fxforward.cpp
instruments/impliedbondspread.cpp
instruments/makeaverageois.cpp
instruments/makecds.cpp
instruments/oibasisswap.cpp
instruments/oiccbasisswap.cpp
instruments/overnightindexfuture.cpp
instruments/payment.cpp
instruments/rebatedexercise.cpp
instruments/subperiodsswap.cpp
instruments/tenorbasisswap.cpp
math/deltagammavar.cpp
math/fillemptymatrix.cpp
methods/multipathgeneratorbase.cpp
models/cdsoptionhelper.cpp
models/cmscaphelper.cpp
models/cpicapfloorhelper.cpp
models/crossassetanalytics.cpp
models/crossassetmodel.cpp
models/crossassetmodelimpliedeqvoltermstructure.cpp
models/crossassetmodelimpliedfxvoltermstructure.cpp
models/dkimpliedyoyinflationtermstructure.cpp
models/dkimpliedzeroinflationtermstructure.cpp
models/eqbsconstantparametrization.cpp
models/eqbsparametrization.cpp
models/eqbspiecewiseconstantparametrization.cpp
models/fxbsconstantparametrization.cpp
models/fxbsparametrization.cpp
models/fxbspiecewiseconstantparametrization.cpp
models/fxeqoptionhelper.cpp
models/gaussian1dcrossassetadaptor.cpp
models/lgm.cpp
models/lgmimplieddefaulttermstructure.cpp
models/lgmimpliedyieldtermstructure.cpp
models/linkablecalibratedmodel.cpp
models/parametrization.cpp
models/piecewiseconstanthelper.cpp
pricingengines/analyticcashsettledeuropeanengine.cpp
pricingengines/analyticcclgmfxoptionengine.cpp
pricingengines/analyticdkcpicapfloorengine.cpp
pricingengines/analyticlgmcdsoptionengine.cpp
pricingengines/analyticlgmswaptionengine.cpp
pricingengines/analyticxassetlgmeqoptionengine.cpp
pricingengines/baroneadesiwhaleyengine.cpp
pricingengines/blackcdsoptionengine.cpp
pricingengines/commodityapoengine.cpp
pricingengines/cpibacheliercapfloorengine.cpp
pricingengines/cpiblackcapfloorengine.cpp
pricingengines/cpicapfloorengines.cpp
pricingengines/crossccyswapengine.cpp
pricingengines/depositengine.cpp
pricingengines/discountingcommodityforwardengine.cpp
pricingengines/discountingcurrencyswapengine.cpp
pricingengines/discountingequityforwardengine.cpp
pricingengines/discountingforwardbondengine.cpp
pricingengines/discountingfxforwardengine.cpp
pricingengines/discountingriskybondengine.cpp
pricingengines/discountingswapenginemulticurve.cpp
pricingengines/inflationcapfloorengines.cpp
pricingengines/lgmconvolutionsolver.cpp
pricingengines/midpointcdsengine.cpp
pricingengines/numericlgmswaptionengine.cpp
pricingengines/oiccbasisswapengine.cpp
pricingengines/paymentdiscountingengine.cpp
processes/crossassetstateprocess.cpp
processes/irlgm1fstateprocess.cpp
quotes/logquote.cpp
termstructures/aposurface.cpp
termstructures/averageoisratehelper.cpp
termstructures/basistwoswaphelper.cpp
termstructures/blackvariancecurve3.cpp
termstructures/blackvariancesurfacemoneyness.cpp
termstructures/blackvariancesurfacesparse.cpp
termstructures/blackvariancesurfacestddevs.cpp
termstructures/blackvolatilitysurfacemoneyness.cpp
termstructures/blackvolsurfacedelta.cpp
termstructures/blackvolsurfacewithatm.cpp
termstructures/brlcdiratehelper.cpp
termstructures/capfloorhelper.cpp
termstructures/capfloortermvolsurface.cpp
termstructures/constantyoyinflationoptionletvolatility.cpp
termstructures/correlationtermstructure.cpp
termstructures/crossccybasismtmresetswaphelper.cpp
termstructures/crossccybasisswaphelper.cpp
termstructures/crossccyfixfloatswaphelper.cpp
termstructures/crosscurrencypricetermstructure.cpp
termstructures/datedstrippedoptionlet.cpp
termstructures/datedstrippedoptionletadapter.cpp
termstructures/defaultprobabilityhelpers.cpp
termstructures/discountratiomodifiedcurve.cpp
termstructures/dynamiccpivolatilitystructure.cpp
termstructures/dynamicoptionletvolatilitystructure.cpp
termstructures/dynamicswaptionvolmatrix.cpp
termstructures/dynamicyoyoptionletvolatilitystructure.cpp
termstructures/equityblackvolsurfaceproxy.cpp
termstructures/equityforwardcurvestripper.cpp
termstructures/equityoptionsurfacestripper.cpp
termstructures/flatcorrelation.cpp
termstructures/fxblackvolsurface.cpp
termstructures/fxvannavolgasmilesection.cpp
termstructures/hazardspreadeddefaulttermstructure.cpp
termstructures/immfraratehelper.cpp
termstructures/oibasisswaphelper.cpp
termstructures/oiccbasisswaphelper.cpp
termstructures/oisratehelper.cpp
termstructures/optionletstripper.cpp
termstructures/optionletstripper1.cpp
termstructures/optionletstripper2.cpp
termstructures/overnightindexfutureratehelper.cpp
termstructures/pricetermstructure.cpp
termstructures/pricetermstructureadapter.cpp
termstructures/spreadedoptionletvolatility.cpp
termstructures/spreadedsmilesection.cpp
termstructures/spreadedswaptionvolatility.cpp
termstructures/strippedoptionletadapter2.cpp
termstructures/strippedyoyinflationoptionletvol.cpp
termstructures/subperiodsswaphelper.cpp
termstructures/swaptionvolatilityconverter.cpp
termstructures/swaptionvolcube2.cpp
termstructures/tenorbasisswaphelper.cpp
termstructures/yoyinflationoptionletvolstripper.cpp
time/actual364.cpp
time/yearcounter.cpp)

# hpp files, this list is maintained manually

set(QuantExt_HDR auto_link.hpp
calendars/austria.hpp
calendars/belgium.hpp
calendars/chile.hpp
calendars/cme.hpp
calendars/colombia.hpp
calendars/france.hpp
calendars/ice.hpp
calendars/israel.hpp
calendars/largejointcalendar.hpp
calendars/luxembourg.hpp
calendars/malaysia.hpp
calendars/netherlands.hpp
calendars/peru.hpp
calendars/philippines.hpp
calendars/spain.hpp
calendars/switzerland.hpp
calendars/thailand.hpp
calendars/wmr.hpp
cashflows/averageonindexedcoupon.hpp
cashflows/averageonindexedcouponpricer.hpp
cashflows/blackovernightindexedcouponpricer.hpp
cashflows/brlcdicouponpricer.hpp
cashflows/commodityindexedaveragecashflow.hpp
cashflows/commodityindexedcashflow.hpp
cashflows/couponpricer.hpp
cashflows/cpicoupon.hpp
cashflows/cpicouponpricer.hpp
cashflows/equitycoupon.hpp
cashflows/equitycouponpricer.hpp
cashflows/floatingannuitycoupon.hpp
cashflows/floatingannuitynominal.hpp
cashflows/floatingratefxlinkednotionalcoupon.hpp
cashflows/fxlinkedcashflow.hpp
cashflows/indexedcoupon.hpp
cashflows/lognormalcmsspreadpricer.hpp
cashflows/overnightindexedcoupon.hpp
cashflows/quantocouponpricer.hpp
cashflows/strippedcapflooredcpicoupon.hpp
cashflows/strippedcapflooredyoyinflationcoupon.hpp
cashflows/subperiodscoupon.hpp
cashflows/subperiodscouponpricer.hpp
currencies/africa.hpp
currencies/america.hpp
currencies/asia.hpp
currencies/europe.hpp
currencies/metals.hpp
indexes/bmaindexwrapper.hpp
indexes/bondindex.hpp
indexes/cacpi.hpp
indexes/commodityindex.hpp
indexes/dkcpi.hpp
indexes/equityindex.hpp
indexes/fxindex.hpp
indexes/genericiborindex.hpp
indexes/ibor/audbbsw.hpp
indexes/ibor/brlcdi.hpp
indexes/ibor/chfsaron.hpp
indexes/ibor/chftois.hpp
indexes/ibor/clpcamara.hpp
indexes/ibor/copibr.hpp
indexes/ibor/corra.hpp
indexes/ibor/czkpribor.hpp
indexes/ibor/demlibor.hpp
indexes/ibor/dkkcibor.hpp
indexes/ibor/dkkcita.hpp
indexes/ibor/dkkois.hpp
indexes/ibor/ester.hpp
indexes/ibor/hkdhibor.hpp
indexes/ibor/hufbubor.hpp
indexes/ibor/idridrfix.hpp
indexes/ibor/idrjibor.hpp
indexes/ibor/ilstelbor.hpp
indexes/ibor/inrmiborois.hpp
indexes/ibor/inrmifor.hpp
indexes/ibor/krwcd.hpp
indexes/ibor/krwkoribor.hpp
indexes/ibor/mxntiie.hpp
indexes/ibor/myrklibor.hpp
indexes/ibor/noknibor.hpp
indexes/ibor/nowa.hpp
indexes/ibor/nzdbkbm.hpp
indexes/ibor/phpphiref.hpp
indexes/ibor/plnpolonia.hpp
indexes/ibor/plnwibor.hpp
indexes/ibor/primeindex.hpp
indexes/ibor/rubmosprime.hpp
indexes/ibor/saibor.hpp
indexes/ibor/seksior.hpp
indexes/ibor/sekstina.hpp
indexes/ibor/sekstibor.hpp
indexes/ibor/sekstina.hpp
indexes/ibor/sgdsibor.hpp
indexes/ibor/sgdsor.hpp
indexes/ibor/skkbribor.hpp
indexes/ibor/sofr.hpp
indexes/ibor/thbbibor.hpp
indexes/ibor/tonar.hpp
indexes/ibor/twdtaibor.hpp
indexes/inflationindexobserver.hpp
indexes/inflationindexwrapper.hpp
indexes/region.hpp
indexes/secpi.hpp
instruments/averageois.hpp
instruments/brlcdiswap.hpp
instruments/cashsettledeuropeanoption.hpp
instruments/cdsoption.hpp
instruments/commodityapo.hpp
instruments/commodityforward.hpp
instruments/creditdefaultswap.hpp
instruments/crossccybasismtmresetswap.hpp
instruments/crossccybasisswap.hpp
instruments/crossccyfixfloatswap.hpp
instruments/crossccyswap.hpp
instruments/currencyswap.hpp
instruments/deposit.hpp
instruments/equityforward.hpp
instruments/fixedbmaswap.hpp
instruments/forwardbond.hpp
instruments/fxforward.hpp
instruments/impliedbondspread.hpp
instruments/makeaverageois.hpp
instruments/makecds.hpp
instruments/oibasisswap.hpp
instruments/oiccbasisswap.hpp
instruments/overnightindexfuture.hpp
instruments/payment.hpp
instruments/rebatedexercise.hpp
instruments/subperiodsswap.hpp
instruments/tenorbasisswap.hpp
interpolators/optioninterpolator2d.hpp
math/covariancesalvage.hpp
math/deltagammavar.hpp
math/fillemptymatrix.hpp
math/flatextrapolation.hpp
math/logquadraticinterpolation.hpp
math/nadarayawatson.hpp
math/quadraticinterpolation.hpp
math/stabilisedglls.hpp
math/trace.hpp
methods/multipathgeneratorbase.hpp
models/cdsoptionhelper.hpp
models/cmscaphelper.hpp
models/cpicapfloorhelper.hpp
models/crlgm1fparametrization.hpp
models/crossassetanalytics.hpp
models/crossassetanalyticsbase.hpp
models/crossassetmodel.hpp
models/crossassetmodelimpliedeqvoltermstructure.hpp
models/crossassetmodelimpliedfxvoltermstructure.hpp
models/dkimpliedyoyinflationtermstructure.hpp
models/dkimpliedzeroinflationtermstructure.hpp
models/eqbsconstantparametrization.hpp
models/eqbsparametrization.hpp
models/eqbspiecewiseconstantparametrization.hpp
models/fxbsconstantparametrization.hpp
models/fxbsparametrization.hpp
models/fxbspiecewiseconstantparametrization.hpp
models/fxeqoptionhelper.hpp
models/gaussian1dcrossassetadaptor.hpp
models/infdkparametrization.hpp
models/irlgm1fconstantparametrization.hpp
models/irlgm1fparametrization.hpp
models/irlgm1fpiecewiseconstanthullwhiteadaptor.hpp
models/irlgm1fpiecewiseconstantparametrization.hpp
models/irlgm1fpiecewiselinearparametrization.hpp
models/lgm.hpp
models/lgmimplieddefaulttermstructure.hpp
models/lgmimpliedyieldtermstructure.hpp
models/linkablecalibratedmodel.hpp
models/parametrization.hpp
models/piecewiseconstanthelper.hpp
models/pseudoparameter.hpp
pricingengines/analyticcashsettledeuropeanengine.hpp
pricingengines/analyticcclgmfxoptionengine.hpp
pricingengines/analyticdkcpicapfloorengine.hpp
pricingengines/analyticlgmcdsoptionengine.hpp
pricingengines/analyticlgmswaptionengine.hpp
pricingengines/analyticxassetlgmeqoptionengine.hpp
pricingengines/baroneadesiwhaleyengine.hpp
pricingengines/blackcdsoptionengine.hpp
pricingengines/commodityapoengine.hpp
pricingengines/cpibacheliercapfloorengine.hpp
pricingengines/cpiblackcapfloorengine.hpp
pricingengines/cpicapfloorengines.hpp
pricingengines/crossccyswapengine.hpp
pricingengines/depositengine.hpp
pricingengines/discountingcommodityforwardengine.hpp
pricingengines/discountingcurrencyswapengine.hpp
pricingengines/discountingequityforwardengine.hpp
pricingengines/discountingforwardbondengine.hpp
pricingengines/discountingfxforwardengine.hpp
pricingengines/discountingriskybondengine.hpp
pricingengines/discountingswapenginemulticurve.hpp
pricingengines/inflationcapfloorengines.hpp
pricingengines/lgmconvolutionsolver.hpp
pricingengines/midpointcdsengine.hpp
pricingengines/numericlgmswaptionengine.hpp
pricingengines/oiccbasisswapengine.hpp
pricingengines/paymentdiscountingengine.hpp
processes/crossassetstateprocess.hpp
processes/irlgm1fstateprocess.hpp
quotes/exceptionquote.hpp
quotes/logquote.hpp
termstructures/aposurface.hpp
termstructures/averageoisratehelper.hpp
termstructures/basistwoswaphelper.hpp
termstructures/blackinvertedvoltermstructure.hpp
termstructures/blackmonotonevarvoltermstructure.hpp
termstructures/blackvariancecurve3.hpp
termstructures/blackvariancesurfacemoneyness.hpp
termstructures/blackvariancesurfacesparse.hpp
termstructures/blackvariancesurfacestddevs.hpp
termstructures/blackvolsurfacedelta.hpp
termstructures/blackvolsurfacewithatm.hpp
termstructures/brlcdiratehelper.hpp
termstructures/capfloorhelper.hpp
termstructures/capfloortermvolcurve.hpp
termstructures/capfloortermvolsurface.hpp
termstructures/commodityaveragebasispricecurve.hpp
termstructures/commoditybasispricecurve.hpp
termstructures/constantyoyinflationoptionletvolatility.hpp
termstructures/correlationtermstructure.hpp
termstructures/crossccybasismtmresetswaphelper.hpp
termstructures/crossccybasisswaphelper.hpp
termstructures/crossccyfixfloatswaphelper.hpp
termstructures/crosscurrencypricetermstructure.hpp
termstructures/datedstrippedoptionlet.hpp
termstructures/datedstrippedoptionletadapter.hpp
termstructures/datedstrippedoptionletbase.hpp
termstructures/defaultprobabilityhelpers.hpp
termstructures/discountratiomodifiedcurve.hpp
termstructures/dynamicblackvoltermstructure.hpp
termstructures/dynamiccpivolatilitystructure.hpp
termstructures/dynamicoptionletvolatilitystructure.hpp
termstructures/dynamicstype.hpp
termstructures/dynamicswaptionvolmatrix.hpp
termstructures/dynamicyoyoptionletvolatilitystructure.hpp
termstructures/equityblackvolsurfaceproxy.hpp
termstructures/equityforwardcurvestripper.hpp
termstructures/equityoptionsurfacestripper.hpp
termstructures/equityvolconstantspread.hpp
termstructures/flatcorrelation.hpp
termstructures/fxblackvolsurface.hpp
termstructures/fxsmilesection.hpp
termstructures/fxvannavolgasmilesection.hpp
termstructures/hazardspreadeddefaulttermstructure.hpp
termstructures/immfraratehelper.hpp
termstructures/interpolatedcorrelationcurve.hpp
termstructures/interpolatedcpivolatilitysurface.hpp
termstructures/interpolateddiscountcurve.hpp
termstructures/interpolateddiscountcurve2.hpp
termstructures/interpolatedyoycapfloortermpricesurface.hpp
termstructures/iterativebootstrap.hpp
termstructures/kinterpolatedyoyoptionletvolatilitysurface.hpp
termstructures/oibasisswaphelper.hpp
termstructures/oiccbasisswaphelper.hpp
termstructures/oisratehelper.hpp
termstructures/optionletcurve.hpp
termstructures/optionletstripper.hpp
termstructures/optionletstripper1.hpp
termstructures/optionletstripper2.hpp
termstructures/optionletstripperwithatm.hpp
termstructures/optionpricesurface.hpp
termstructures/overnightindexfutureratehelper.hpp
termstructures/piecewiseatmoptionletcurve.hpp
termstructures/piecewiseoptionletcurve.hpp
termstructures/piecewiseoptionletstripper.hpp
termstructures/pricecurve.hpp
termstructures/pricetermstructure.hpp
termstructures/pricetermstructureadapter.hpp
termstructures/probabilitytraits.hpp
termstructures/spreadedoptionletvolatility.hpp
termstructures/spreadedsmilesection.hpp
termstructures/spreadedswaptionvolatility.hpp
termstructures/staticallycorrectedyieldtermstructure.hpp
termstructures/strippedcpivolatilitystructure.hpp
termstructures/strippedoptionletadapter.hpp
termstructures/strippedoptionletadapter2.hpp
termstructures/strippedyoyinflationoptionletvol.hpp
termstructures/subperiodsswaphelper.hpp
termstructures/survivalprobabilitycurve.hpp
termstructures/swaptionvolatilityconverter.hpp
termstructures/swaptionvolconstantspread.hpp
termstructures/swaptionvolcube2.hpp
termstructures/swaptionvolcubewithatm.hpp
termstructures/tenorbasisswaphelper.hpp
termstructures/yoyinflationcurveobservermoving.hpp
termstructures/yoyinflationcurveobserverstatic.hpp
termstructures/yoyinflationoptionletvolstripper.hpp
termstructures/yoyoptionletvolatilitysurface.hpp
termstructures/zeroinflationcurveobservermoving.hpp
termstructures/zeroinflationcurveobserverstatic.hpp
time/actual364.hpp
time/futureexpirycalculator.hpp
time/yearcounter.hpp
version.hpp)

writeAll("qle" "quantext.hpp" "auto_link.hpp" "${QuantExt_HDR}")
add_library(${QLE_LIB_NAME} ${QuantExt_SRC})
target_link_libraries(${QLE_LIB_NAME} ${QL_LIB_NAME} ${Boost_LIBRARIES})


install(DIRECTORY . DESTINATION include/qle
        FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")

install(TARGETS ${QLE_LIB_NAME}
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
        )
