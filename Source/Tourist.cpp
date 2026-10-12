#include "Tourist.h"
#include "Validation.h"
Tourist::Tourist() = default;
Tourist::Tourist(int value, const std::string& personName, const std::string& number, const std::string& home)
    : Person(value, personName, number), city(Validation::trim(home)) { Validation::text(city,"City"); }
Tourist::~Tourist() = default;
const std::string& Tourist::getCity() const { return city; }
std::string Tourist::role() const { return "Tourist"; }
void Tourist::display(std::ostream& out) const {
    out << "Tourist #" << id << " | " << name << " | Phone: " << phone << " | City: " << city << '\n';
}
