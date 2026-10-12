#include "Guide.h"
#include "Validation.h"
#include <cmath>
#include <iomanip>
Guide::Guide() : dailyRate(0), available(true) {}
Guide::Guide(int value, const std::string& personName, const std::string& number,
             const std::string& spoken, double rate, bool enabled)
    : Person(value,personName,number), language(Validation::trim(spoken)),
      dailyRate(std::round(rate * 100.0) / 100.0), available(enabled) {
    Validation::text(language,"Language"); Validation::money(rate,"Daily rate");
    Validation::money(dailyRate,"Rounded daily rate");
}
Guide::~Guide() = default;
const std::string& Guide::getLanguage() const { return language; }
double Guide::getDailyRate() const { return dailyRate; }
bool Guide::isAvailable() const { return available; }
std::string Guide::role() const { return "Guide"; }
void Guide::display(std::ostream& out) const {
    out << "Guide #" << id << " | " << name << " | Phone: " << phone << "\n"
        << "  Language: " << language << " | Rate/day: INR " << std::fixed << std::setprecision(2)
        << dailyRate << " | Accepting bookings: " << (available ? "Yes" : "No") << '\n';
}
