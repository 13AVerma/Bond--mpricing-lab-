#include <ql/quantlib.hpp>
#include <iostream>
#include <iomanip>
#include <vector>

using namespace QuantLib;

int main() {
    try {
        std::cout << "Bond Pricing Lab\n\n";

        // 1. Set valuation date
        Date valueDate(8, December, 2022);
        Settings::instance().evaluationDate() = valueDate;

        // 2. Create calendar
        Calendar calendar = TARGET();

        // 3. Create coupon schedule
        Schedule schedule = MakeSchedule()
            .from(Date(26, August, 2020))
            .to(Date(26, May, 2031))
            .withFirstDate(Date(26, May, 2021))
            .withFrequency(Semiannual)
            .withCalendar(calendar)
            .withConvention(Following)
            .backwards();

        // 4. Bond inputs
        Natural settlementDays = 3;
        Real faceAmount = 1000000.0;
        Rate couponRate = 0.02;
        DayCounter bondDayCounter = Thirty360(Thirty360::BondBasis);

        // 5. Create fixed-rate bond
        FixedRateBond bond(
            settlementDays,
            faceAmount,
            schedule,
            std::vector<Rate>{couponRate},
            bondDayCounter
        );

        // 6. Price from one quoted yield
        Rate bondYield = 0.025;

        Real yieldCleanPrice = bond.cleanPrice(
            bondYield,
            bondDayCounter,
            Compounded,
            Annual
        );

        Real accruedInterest = bond.accruedAmount();
        Real yieldDirtyPrice = yieldCleanPrice + accruedInterest;

        // 7. Build a zero yield curve
        DayCounter curveDayCounter = Actual360();

        std::vector<Date> curveDates;
        curveDates.push_back(valueDate);
        curveDates.push_back(calendar.advance(valueDate, Period(1, Years)));
        curveDates.push_back(calendar.advance(valueDate, Period(2, Years)));
        curveDates.push_back(calendar.advance(valueDate, Period(5, Years)));
        curveDates.push_back(calendar.advance(valueDate, Period(10, Years)));
        curveDates.push_back(calendar.advance(valueDate, Period(15, Years)));

        std::vector<Rate> curveRates;
        curveRates.push_back(0.015);
        curveRates.push_back(0.015);
        curveRates.push_back(0.018);
        curveRates.push_back(0.022);
        curveRates.push_back(0.025);
        curveRates.push_back(0.028);

        ext::shared_ptr<YieldTermStructure> yieldCurve =
            ext::make_shared<InterpolatedZeroCurve<Linear>>(
                curveDates,
                curveRates,
                curveDayCounter
            );

        Handle<YieldTermStructure> yieldCurveHandle(yieldCurve);

        // 8. Attach a discounting engine to the bond
        ext::shared_ptr<PricingEngine> bondEngine =
            ext::make_shared<DiscountingBondEngine>(yieldCurveHandle);

        bond.setPricingEngine(bondEngine);

        // 9. Price using the yield curve
        Real curveCleanPrice = bond.cleanPrice();
        Real curveDirtyPrice = bond.dirtyPrice();
        Real curveNPV = bond.NPV();

        // 10. Add credit spread analysis
        ext::shared_ptr<SimpleQuote> creditSpread =
            ext::make_shared<SimpleQuote>(0.0);

        Handle<Quote> creditSpreadHandle(creditSpread);

        ext::shared_ptr<YieldTermStructure> spreadedCurve =
            ext::make_shared<ZeroSpreadedTermStructure>(
                yieldCurveHandle,
                creditSpreadHandle
            );

        Handle<YieldTermStructure> spreadedCurveHandle(spreadedCurve);

        ext::shared_ptr<PricingEngine> spreadedBondEngine =
            ext::make_shared<DiscountingBondEngine>(spreadedCurveHandle);

        bond.setPricingEngine(spreadedBondEngine);

        // Price with 0% credit spread
        creditSpread->setValue(0.0);
        Real zeroSpreadCleanPrice = bond.cleanPrice();
        Real zeroSpreadDirtyPrice = bond.dirtyPrice();
        Real zeroSpreadNPV = bond.NPV();

        // Price with 1% credit spread
        creditSpread->setValue(0.01);
        Real onePercentSpreadCleanPrice = bond.cleanPrice();
        Real onePercentSpreadDirtyPrice = bond.dirtyPrice();
        Real onePercentSpreadNPV = bond.NPV();

        Real spreadCleanPriceChange =
            onePercentSpreadCleanPrice - zeroSpreadCleanPrice;

        Real spreadNPVChange =
            onePercentSpreadNPV - zeroSpreadNPV;

        // 11. Output
        std::cout << std::fixed << std::setprecision(6);

        std::cout << "Valuation date: " << valueDate << "\n\n";

        std::cout << "Bond details\n";
        std::cout << "Face amount: " << faceAmount << "\n";
        std::cout << "Coupon rate: " << io::rate(couponRate) << "\n";
        std::cout << "Single market yield: " << io::rate(bondYield) << "\n\n";

        std::cout << "Coupon schedule\n";
for (const Date& d : schedule) {
    std::cout << d << "\n";
}

std::cout << "\nBond cashflows\n";
for (const auto& cashflow : bond.cashflows()) {
    std::cout << cashflow->date()
              << " : "
              << cashflow->amount();

    if (cashflow->hasOccurred(valueDate)) {
        std::cout << "  (already occurred)";
    }

    std::cout << "\n";
}

std::cout << "\nZero curve inputs\n";
        for (Size i = 0; i < curveDates.size(); ++i) {
            std::cout << curveDates[i] << " : " << io::rate(curveRates[i]) << "\n";
        }

        std::cout << "\nPricing from single yield\n";
        std::cout << "Clean price per 100: " << yieldCleanPrice << "\n";
        std::cout << "Accrued interest per 100: " << accruedInterest << "\n";
        std::cout << "Dirty price per 100: " << yieldDirtyPrice << "\n";

        std::cout << "\nPricing from yield curve\n";
        std::cout << "Clean price per 100: " << curveCleanPrice << "\n";
        std::cout << "Accrued interest per 100: " << accruedInterest << "\n";
        std::cout << "Dirty price per 100: " << curveDirtyPrice << "\n";
        std::cout << "NPV: " << curveNPV << "\n";

        std::cout << "\nCredit spread analysis\n";
        std::cout << "Clean price with 0% spread per 100: "
                  << zeroSpreadCleanPrice << "\n";
        std::cout << "Dirty price with 0% spread per 100: "
                  << zeroSpreadDirtyPrice << "\n";
        std::cout << "NPV with 0% spread: "
                  << zeroSpreadNPV << "\n\n";

        std::cout << "Clean price with 1% spread per 100: "
                  << onePercentSpreadCleanPrice << "\n";
        std::cout << "Dirty price with 1% spread per 100: "
                  << onePercentSpreadDirtyPrice << "\n";
        std::cout << "NPV with 1% spread: "
                  << onePercentSpreadNPV << "\n\n";

        std::cout << "Clean price change from spread: "
                  << spreadCleanPriceChange << "\n";
        std::cout << "NPV change from spread: "
                  << spreadNPVChange << "\n";

        return 0;

    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}
