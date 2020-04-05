#include "binomialtree.hpp"
#include "binomialengine.hpp"
#include <ql/methods/lattices/binomialtree.hpp>
#include <ql/pricingengines/vanilla/all.hpp>
#include <ql/instruments/vanillaoption.hpp>
#include <ql/stochasticprocess.hpp>

#include <ql/pricingengines/vanilla/binomialengine.hpp>
#include <ql/methods/lattices/binomialtree.hpp>
#include <ql/time/calendars/target.hpp>


#include <ql/quantlib.hpp>
#include <iostream>
#include <fstream>

using namespace QuantLib;

int main()
{

    std::ofstream out("output.txt");

    auto* coutbuf = std::cout.rdbuf();
    std::cout.rdbuf(out.rdbuf());

    // Calendar SETUP
    
    Calendar calendar = TARGET();


    Date todaysDate(26, February, 2019);


    Date settlementDate(28, February, 2019);


    Settings::instance().evaluationDate() = todaysDate;


    DayCounter dayCounter = Actual365Fixed();




    //Option PARAMETERS


    Option::Type type(Option::Call);


    Real strike = 120;


    Real underlying = 100;


    Date maturity(26, February, 2020);


    Spread dividendYield = 0.00;


    Rate riskFreeRate = 0.04;


    Volatility volatility = 0.25;


    Size timeSteps = 300;



    std::cout << std::endl;

    std::cout << "=================Empirical Finance : Project QuantLib 3=================" << std::endl;

    std::cout << std::endl;



    std::cout << "--------------Option Characteristics--------------" << std::endl;


    std::cout << "Option type :                           " << "Vanilla - " << type << std::endl;


    std::cout << "Exercise :                              " << "European" << std::endl;


    std::cout << "Settlement Date :                       " << settlementDate << std::endl;


    std::cout << "Maturity :                              " << maturity << std::endl;


    std::cout << "Strike :                                " << strike << std::endl;


    std::cout << "Underlying price :                      " << underlying << std::endl;


    std::cout << "Risk-free yield :                       " << 100 * riskFreeRate << " %" << std::endl;


    std::cout << "Div yield :                             " << 100 * dividendYield << " %" << std::endl;


    std::cout << "Vol :                                   " << 100 * volatility << " %" << std::endl;


    std::cout << "Time steps :                            " << timeSteps << std::endl;

    std::cout << std::endl << std::endl;






    //List of handler (European exercise, underlying, yield term, volatility, payoff, BS process)


    boost::shared_ptr<Exercise> europeanExercise(new EuropeanExercise(maturity));


    Handle<Quote> underlying_(boost::shared_ptr<Quote>(new SimpleQuote(underlying)));


    Handle<YieldTermStructure> flatTermStructure(boost::shared_ptr<YieldTermStructure>(new FlatForward(settlementDate, riskFreeRate, dayCounter)));


    Handle<YieldTermStructure> flatDividendTermStructure(boost::shared_ptr<YieldTermStructure>(new FlatForward(settlementDate, dividendYield, dayCounter)));


    Handle<BlackVolTermStructure> flatVolTermStructure(boost::shared_ptr<BlackVolTermStructure>(new BlackConstantVol(settlementDate, calendar, volatility, dayCounter)));


    boost::shared_ptr<StrikedTypePayoff> payoff(new PlainVanillaPayoff(type, strike));


    boost::shared_ptr<BlackScholesMertonProcess> bsmProcess(new BlackScholesMertonProcess(underlying_, flatDividendTermStructure, flatTermStructure, flatVolTermStructure));




    VanillaOption europeanOption(payoff, europeanExercise);





    ///////////////////////////////////////////////////////////////// Black-Scholes Method ////////////////////////////////////////////////////////////////


    std::cout << "--------------Reference : Black-Scholes Method--------------" << std::endl;

    const clock_t BS_start = clock();


    europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new AnalyticEuropeanEngine(bsmProcess)));


    Real deltaM1 = europeanOption.delta();


    Real gammaM1 = europeanOption.gamma();

    Real priceM1 = europeanOption.NPV();


    std::cout << "Computing time :           	  " << float(clock() - BS_start) / CLOCKS_PER_SEC << " sec" << std::endl;


    std::cout << "Price :                          " << priceM1 << std::endl; //NPV = Net Present Value


    std::cout << "Delta :                          " << deltaM1 << std::endl;


    std::cout << "Gamma :                          " << gammaM1 << std::endl << std::endl;


    std::cout << " " << std::endl << std::endl;



    ///////////////////////////////////////////////////////////////// JarrowRudd Method ////////////////////////////////////////////////////////////////

    std::cout << "--------------JarrowRudd Method---------------" << std::endl;

    std::cout << "## BEFORE ##" << std::endl;

    clock_t JR_start = clock();


    europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<JarrowRudd>(bsmProcess, timeSteps)));


    Real deltaM2 = europeanOption.delta();


    Real gammaM2 = europeanOption.gamma();

    Real priceM2 = europeanOption.NPV();


    std::cout << "Computing time :           	  " << float(clock() - JR_start) / CLOCKS_PER_SEC << " sec" << std::endl;


    std::cout << "Price :                          " << priceM2 << std::endl;


    std::cout << "Delta :                          " << deltaM2 << std::endl;


    std::cout << "Gamma :                          " << gammaM2 << std::endl;


    std::cout << "Delta diff (JR-BS) :             " << deltaM2 - deltaM1 << std::endl;


    std::cout << "Gamma diff (JR-BS) :             " << gammaM2 - gammaM1 << std::endl << std::endl;



    std::cout << "## AFTER ##" << std::endl;

    clock_t JRA_start = clock();


    europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine_2<JarrowRudd_2>(bsmProcess, timeSteps)));


    Real deltaM2A = europeanOption.delta();


    Real gammaM2A = europeanOption.gamma();

    Real priceM2A = europeanOption.NPV();


    std::cout << "Computing time :           	  " << float(clock() - JRA_start) / CLOCKS_PER_SEC << " sec" << std::endl;


    std::cout << "Price :                          " << priceM2A << std::endl;


    std::cout << "Delta :                          " << deltaM2A << std::endl;


    std::cout << "Gamma :                          " << gammaM2A << std::endl;


    std::cout << "Delta diff (JR-BS) :             " << deltaM2A - deltaM1 << std::endl;


    std::cout << "Gamma diff (JR-BS) :             " << gammaM2A - gammaM1 << std::endl << std::endl;


    std::cout << " " << std::endl << std::endl;


    ///////////////////////////////////////////////////////////////// AdditiveEQPBinomialTree Method ////////////////////////////////////////////////////////////////

    std::cout << "--------------AdditiveEQPBinomialTree Method---------------" << std::endl;

    std::cout << "## BEFORE ##" << std::endl;

    JR_start = clock();


    europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<AdditiveEQPBinomialTree>(bsmProcess, timeSteps)));


    deltaM2 = europeanOption.delta();


    gammaM2 = europeanOption.gamma();

    priceM2 = europeanOption.NPV();


    std::cout << "Computing time :           	  " << float(clock() - JR_start) / CLOCKS_PER_SEC << " sec" << std::endl;


    std::cout << "Price :                          " << priceM2 << std::endl;


    std::cout << "Delta :                          " << deltaM2 << std::endl;


    std::cout << "Gamma :                          " << gammaM2 << std::endl;


    std::cout << "Delta diff (ADD-BS) :             " << deltaM2 - deltaM1 << std::endl;


    std::cout << "Gamma diff (ADD-BS) :             " << gammaM2 - gammaM1 << std::endl << std::endl;



    std::cout << "## AFTER ##" << std::endl;

    JRA_start = clock();


    europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine_2<AdditiveEQPBinomialTree_2>(bsmProcess, timeSteps)));


    deltaM2A = europeanOption.delta();


    gammaM2A = europeanOption.gamma();

    priceM2A = europeanOption.NPV();


    std::cout << "Computing time :           	  " << float(clock() - JRA_start) / CLOCKS_PER_SEC << " sec" << std::endl;


    std::cout << "Price :                          " << priceM2A << std::endl;


    std::cout << "Delta :                          " << deltaM2A << std::endl;


    std::cout << "Gamma :                          " << gammaM2A << std::endl;


    std::cout << "Delta diff (ADD-BS) :             " << deltaM2A - deltaM1 << std::endl;


    std::cout << "Gamma diff (ADD-BS) :             " << gammaM2A - gammaM1 << std::endl << std::endl;


    std::cout << " " << std::endl << std::endl;



    ///////////////////////////////////////////////////////////////// Cox-Ross-Rubinstein Method ////////////////////////////////////////////////////////////////

    std::cout << "--------------Cox-Ross-Rubinstein Method---------------" << std::endl;

    std::cout << "## BEFORE ##" << std::endl;

    JR_start = clock();


    europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<CoxRossRubinstein>(bsmProcess, timeSteps)));


    deltaM2 = europeanOption.delta();


    gammaM2 = europeanOption.gamma();

    priceM2 = europeanOption.NPV();


    std::cout << "Computing time :           	  " << float(clock() - JR_start) / CLOCKS_PER_SEC << " sec" << std::endl;


    std::cout << "Price :                          " << priceM2 << std::endl;


    std::cout << "Delta :                          " << deltaM2 << std::endl;


    std::cout << "Gamma :                          " << gammaM2 << std::endl;


    std::cout << "Delta diff (CCR-BS) :             " << deltaM2 - deltaM1 << std::endl;


    std::cout << "Gamma diff (CCR-BS) :             " << gammaM2 - gammaM1 << std::endl << std::endl;



    std::cout << "## AFTER ##" << std::endl;

    JRA_start = clock();


    europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine_2<CoxRossRubinstein_2>(bsmProcess, timeSteps)));


    deltaM2A = europeanOption.delta();


    gammaM2A = europeanOption.gamma();

    priceM2A = europeanOption.NPV();


    std::cout << "Computing time :           	  " << float(clock() - JRA_start) / CLOCKS_PER_SEC << " sec" << std::endl;


    std::cout << "Price :                          " << priceM2A << std::endl;


    std::cout << "Delta :                          " << deltaM2A << std::endl;


    std::cout << "Gamma :                          " << gammaM2A << std::endl;


    std::cout << "Delta diff (CCR-BS) :             " << deltaM2A - deltaM1 << std::endl;


    std::cout << "Gamma diff (CCR-BS) :             " << gammaM2A - gammaM1 << std::endl << std::endl;


    std::cout << " " << std::endl << std::endl;



    ///////////////////////////////////////////////////////////////// Trigeorgis Method ////////////////////////////////////////////////////////////////

    std::cout << "--------------Trigeorgis Method---------------" << std::endl;

    std::cout << "## BEFORE ##" << std::endl;

    JR_start = clock();


    europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<Trigeorgis>(bsmProcess, timeSteps)));


    deltaM2 = europeanOption.delta();


    gammaM2 = europeanOption.gamma();

    priceM2 = europeanOption.NPV();


    std::cout << "Computing time :           	  " << float(clock() - JR_start) / CLOCKS_PER_SEC << " sec" << std::endl;


    std::cout << "Price :                          " << priceM2 << std::endl;


    std::cout << "Delta :                          " << deltaM2 << std::endl;


    std::cout << "Gamma :                          " << gammaM2 << std::endl;


    std::cout << "Delta diff (TR-BS) :             " << deltaM2 - deltaM1 << std::endl;


    std::cout << "Gamma diff (TR-BS) :             " << gammaM2 - gammaM1 << std::endl << std::endl;



    std::cout << "## AFTER ##" << std::endl;

    JRA_start = clock();


    europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine_2<Trigeorgis_2>(bsmProcess, timeSteps)));


    deltaM2A = europeanOption.delta();


    gammaM2A = europeanOption.gamma();

    priceM2A = europeanOption.NPV();


    std::cout << "Computing time :           	  " << float(clock() - JRA_start) / CLOCKS_PER_SEC << " sec" << std::endl;


    std::cout << "Price :                          " << priceM2A << std::endl;


    std::cout << "Delta :                          " << deltaM2A << std::endl;


    std::cout << "Gamma :                          " << gammaM2A << std::endl;


    std::cout << "Delta diff (TR-BS) :             " << deltaM2A - deltaM1 << std::endl;


    std::cout << "Gamma diff (TR-BS) :             " << gammaM2A - gammaM1 << std::endl << std::endl;


    std::cout << " " << std::endl << std::endl;




    ///////////////////////////////////////////////////////////////// Joshi4 Method ////////////////////////////////////////////////////////////////

    std::cout << "--------------Joshi4 Method---------------" << std::endl;

    std::cout << "## BEFORE ##" << std::endl;

    JR_start = clock();


    europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<Joshi4>(bsmProcess, timeSteps)));


    deltaM2 = europeanOption.delta();


    gammaM2 = europeanOption.gamma();

    priceM2 = europeanOption.NPV();


    std::cout << "Computing time :           	  " << float(clock() - JR_start) / CLOCKS_PER_SEC << " sec" << std::endl;


    std::cout << "Price :                          " << priceM2 << std::endl;


    std::cout << "Delta :                          " << deltaM2 << std::endl;


    std::cout << "Gamma :                          " << gammaM2 << std::endl;


    std::cout << "Delta diff (J4-BS) :             " << deltaM2 - deltaM1 << std::endl;


    std::cout << "Gamma diff (J4-BS) :             " << gammaM2 - gammaM1 << std::endl << std::endl;



    std::cout << "## AFTER ##" << std::endl;

    JRA_start = clock();


    europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine_2<Joshi4_2>(bsmProcess, timeSteps)));


    deltaM2A = europeanOption.delta();


    gammaM2A = europeanOption.gamma();

    priceM2A = europeanOption.NPV();


    std::cout << "Computing time :           	  " << float(clock() - JRA_start) / CLOCKS_PER_SEC << " sec" << std::endl;


    std::cout << "Price :                          " << priceM2A << std::endl;


    std::cout << "Delta :                          " << deltaM2A << std::endl;


    std::cout << "Gamma :                          " << gammaM2A << std::endl;


    std::cout << "Delta diff (J4-BS) :             " << deltaM2A - deltaM1 << std::endl;


    std::cout << "Gamma diff (J4-BS) :             " << gammaM2A - gammaM1 << std::endl << std::endl;


    std::cout << " " << std::endl << std::endl;



    ///////////////////////////////////////////////////////////////// Tian Method ////////////////////////////////////////////////////////////////

    std::cout << "--------------Tian Method---------------" << std::endl;

    std::cout << "## BEFORE ##" << std::endl;

    const clock_t T_start = clock();


    europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<Tian>(bsmProcess, timeSteps)));


    Real deltaM3 = europeanOption.delta();


    Real gammaM3 = europeanOption.gamma();

    Real priceM3 = europeanOption.NPV();


    std::cout << "Computing time :           	  " << float(clock() - T_start) / CLOCKS_PER_SEC << " sec" << std::endl;


    std::cout << "Price :                          " << priceM3 << std::endl;


    std::cout << "Delta :                          " << deltaM3 << std::endl;


    std::cout << "Gamma :                          " << gammaM3 << std::endl;


    std::cout << "Delta diff (T-BS) :              " << deltaM3 - deltaM1 << std::endl;


    std::cout << "Gamma diff (T-BS) :              " << gammaM3 - gammaM1 << std::endl << std::endl;



    std::cout << "## AFTER ##" << std::endl;

    const clock_t TA_start = clock();


    europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine_2<Tian_2>(bsmProcess, timeSteps)));


    Real deltaM3A = europeanOption.delta();


    Real gammaM3A = europeanOption.gamma();

    Real priceM3A = europeanOption.NPV();


    std::cout << "Computing time :           	  " << float(clock() - TA_start) / CLOCKS_PER_SEC << " sec" << std::endl;


    std::cout << "Price :                          " << priceM3A << std::endl;


    std::cout << "Delta :                          " << deltaM3A << std::endl;


    std::cout << "Gamma :                          " << gammaM3A << std::endl;


    std::cout << "Delta diff (TA-BS) :             " << deltaM3A - deltaM1 << std::endl;


    std::cout << "Gamma diff (TA-BS) :             " << gammaM3A - gammaM1 << std::endl << std::endl;


    std::cout << " " << std::endl << std::endl;





    ///////////////////////////////////////////////////////////////// LeisenReimer Method ////////////////////////////////////////////////////////////////

    std::cout << "--------------LeisenReimer Method---------------" << std::endl;

    std::cout << "## BEFORE ##" << std::endl;

    const clock_t LR_start = clock();


    europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<LeisenReimer>(bsmProcess, timeSteps)));


    Real deltaM4 = europeanOption.delta();


    Real gammaM4 = europeanOption.gamma();

    Real priceM4 = europeanOption.NPV();


    std::cout << "Computing time :           	  " << float(clock() - LR_start) / CLOCKS_PER_SEC << " sec" << std::endl;


    std::cout << "Price :                          " << priceM4 << std::endl;


    std::cout << "Delta :                          " << deltaM4 << std::endl;


    std::cout << "Gamma :                          " << gammaM4 << std::endl;


    std::cout << "Delta diff (T-BS) :              " << deltaM4 - deltaM1 << std::endl;


    std::cout << "Gamma diff (T-BS) :              " << gammaM4 - gammaM1 << std::endl << std::endl;



    std::cout << "## AFTER ##" << std::endl;

    const clock_t LRA_start = clock();


    europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine_2<LeisenReimer_2>(bsmProcess, timeSteps)));


    Real deltaM4A = europeanOption.delta();


    Real gammaM4A = europeanOption.gamma();

    Real priceM4A = europeanOption.NPV();


    std::cout << "Computing time :           	  " << float(clock() - LRA_start) / CLOCKS_PER_SEC << " sec" << std::endl;


    std::cout << "Price :                          " << priceM4A << std::endl;


    std::cout << "Delta :                          " << deltaM4A << std::endl;


    std::cout << "Gamma :                          " << gammaM4A << std::endl;


    std::cout << "Delta diff (LRA-BS) :             " << deltaM4A - deltaM1 << std::endl;


    std::cout << "Gamma diff (LRA-BS) :             " << gammaM4A - gammaM1 << std::endl << std::endl;

    return 0;
}