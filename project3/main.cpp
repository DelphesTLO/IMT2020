
#include "binomialtree.hpp"
#include "binomialengine.hpp"
#include <ql/methods/lattices/binomialtree.hpp>
#include <ql/pricingengines/vanilla/binomialengine.hpp>
#include <iostream>

//Added
#include <ql/pricingengines/vanilla/all.hpp>
#include <ql/instruments/vanillaoption.hpp>
#include <ql/stochasticprocess.hpp>
#include <ql/methods/lattices/binomialtree.hpp>
#include <ql/time/calendars/target.hpp>
#include <stdlib.h>


using namespace QuantLib;

int main() {

    try {

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
        Size timeSteps=100;

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
        std::cout << "Risk-free yield :                       " << riskFreeRate <<std::endl;
        std::cout << "Div yield :                             " << dividendYield << std::endl;
        std::cout << "Vol :                                   " << volatility <<std::endl;
        std::cout << "Time steps :                            " << timeSteps <<std::endl;
		std::cout << std::endl << std::endl;


        //List of handler (European exercise, underlying, yield term, volatility, payoff, BS process)
        boost::shared_ptr<Exercise> europeanExercise(new EuropeanExercise(maturity));
        Handle<Quote> underlying_(boost::shared_ptr<Quote>(new SimpleQuote(underlying)));
        Handle<YieldTermStructure> flatTermStructure(boost::shared_ptr<YieldTermStructure>(new FlatForward(settlementDate, riskFreeRate, dayCounter)));
        Handle<YieldTermStructure> flatDividendTermStructure(boost::shared_ptr<YieldTermStructure>(new FlatForward(settlementDate, dividendYield, dayCounter)));
        Handle<BlackVolTermStructure> flatVolTermStructure(boost::shared_ptr<BlackVolTermStructure>(new BlackConstantVol(settlementDate, calendar, volatility,dayCounter)));
        boost::shared_ptr<StrikedTypePayoff> payoff(new PlainVanillaPayoff(type, strike));
        boost::shared_ptr<BlackScholesMertonProcess> bsmProcess(new BlackScholesMertonProcess(underlying_, flatDividendTermStructure,flatTermStructure, flatVolTermStructure));

        VanillaOption europeanOption(payoff, europeanExercise);


///////////////////////////////////////////////////////////////// Black Scholes Method ////////////////////////////////////////////////////////////////
        std::cout << "--------------Black-Scholes Method--------------" << std::endl;
        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new AnalyticEuropeanEngine(bsmProcess)));
        Real deltaM1=europeanOption.delta();
        Real gammaM1=europeanOption.gamma();
        std::cout << "Price :                          "  << europeanOption.NPV() << std::endl; //NPV = Net Present Value
        std::cout << "Delta :                          "  << deltaM1 << std::endl;
        std::cout << "Gamma :                          "  << gammaM1 << std::endl << std::endl;


///////////////////////////////////////////////////////////////// JarrowRudd Method ////////////////////////////////////////////////////////////////
		std::cout << "--------------JarrowRudd Method---------------" << std::endl;
        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine_2<JarrowRudd_2>(bsmProcess, timeSteps)));
        Real deltaM2=europeanOption.delta();
        Real gammaM2=europeanOption.gamma();
        std::cout << "Price :                          "  << europeanOption.NPV() << std::endl;
        std::cout << "Delta :                          "  << deltaM2 << std::endl;
        std::cout << "Gamma :                          "  << gammaM2 << std::endl;
        std::cout << "Delta diff (JR-BS) :             "  << deltaM2-deltaM1 << std::endl;
        std::cout << "Gamma diff (JR-BS) :             "  << gammaM2-gammaM1 << std::endl << std::endl;


        return 0;

    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "unknown error" << std::endl;
        return 1;
    }
}

