#include <ql/quantlib.hpp>
#include <iostream>
#include <iomanip>

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
        DayCounter dayCounter = Thirty360(Thirty360::BondBasis);

        // 5. Create fixed-rate bond
        FixedRateBond bond(
            settlementDays,
            faceAmount,
            schedule,
            std::vector<Rate>{couponRate},
            dayCounter
        );

        // 6. Price from quoted yield
        Rate bondYield = 0.025;

        Real cleanPrice = bond.cleanPrice(
            bondYield,
            dayCounter,
            Compounded,
            Annual
        );

        Real accruedInterest = bond.accruedAmount();
        Real dirtyPrice = cleanPrice + accruedInterest;

        // 7. Output
        std::cout << "Valuation date: " << valueDate << "\n\n";

        std::cout << "Bond details\n";
        std::cout << "Face amount: " << faceAmount << "\n";
        std::cout << "Coupon rate: " << io::rate(couponRate) << "\n";
        std::cout << "Market yield: " << io::rate(bondYield) << "\n\n";

        std::cout << "Coupon schedule\n";
        for (const Date& d : schedule) {
            std::cout << d << "\n";
        }

        std::cout << "\nPricing from yield\n";
        std::cout << std::fixed << std::setprecision(6);
        std::cout << "Clean price: " << cleanPrice << "\n";
        std::cout << "Accrued interest: " << accruedInterest << "\n";
        std::cout << "Dirty price: " << dirtyPrice << "\n";

        return 0;

    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}
