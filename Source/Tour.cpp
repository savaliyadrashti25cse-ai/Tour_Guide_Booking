#include "Tour.h"
#include "Validation.h"
#include <cmath>
#include <iomanip>
Tour::Tour() : id(0), durationDays(0), capacity(0), pricePerPerson(0) {}
Tour::Tour(int value, const std::string& label, const std::string& place,
           const std::string& date, int days, int limit, double price)
    : id(value), title(Validation::trim(label)), destination(Validation::trim(place)),
      startDate(date), durationDays(days), capacity(limit), pricePerPerson(std::round(price * 100.0) / 100.0) {
    Validation::integer(id,1,1000000000,"ID"); Validation::text(title,"Tour title");
    Validation::text(destination,"Destination"); Validation::date(startDate);
    Validation::integer(days,1,30,"Duration"); Validation::integer(limit,1,1000,"Capacity");
    Validation::money(price,"Price per person"); Validation::money(pricePerPerson,"Rounded price");
}
Tour::~Tour() = default;
int Tour::getId() const { return id; }
const std::string& Tour::getTitle() const { return title; }
const std::string& Tour::getDestination() const { return destination; }
const std::string& Tour::getStartDate() const { return startDate; }
int Tour::getDurationDays() const { return durationDays; }
int Tour::getCapacity() const { return capacity; }
double Tour::getPricePerPerson() const { return pricePerPerson; }
void Tour::display(std::ostream& out) const {
    out << "Tour #" << id << " | " << title << " | Destination: " << destination << "\n"
        << "  Start: " << startDate << " | Days: " << durationDays << " | Capacity: " << capacity
        << " | Price/person: INR " << std::fixed << std::setprecision(2) << pricePerPerson << '\n';
}
