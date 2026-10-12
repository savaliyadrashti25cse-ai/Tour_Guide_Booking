#include "Booking.h"
#include "Validation.h"
#include <cmath>
#include <iomanip>
#include <stdexcept>
Booking::Booking() : id(0), touristId(0), guideId(0), tourId(0), participants(0), amount(0), confirmed(false) {}
Booking::Booking(int value, int tourist, int guide, int tour, int people, double total, bool active)
    : id(value), touristId(tourist), guideId(guide), tourId(tour), participants(people),
      amount(std::round(total * 100.0) / 100.0), confirmed(active) {
    Validation::integer(id,1,1000000000,"Booking ID");
    Validation::integer(touristId,1,1000000000,"Tourist ID");
    Validation::integer(guideId,1,1000000000,"Guide ID");
    Validation::integer(tourId,1,1000000000,"Tour ID");
    Validation::integer(participants,1,1000,"Participants");
    if (!std::isfinite(total) || total <= 0 || amount <= 0 || total > 2000000000.0)
        throw std::invalid_argument("Booking total is invalid.");
}
Booking::~Booking() = default;
int Booking::getId() const { return id; }
int Booking::getTouristId() const { return touristId; }
int Booking::getGuideId() const { return guideId; }
int Booking::getTourId() const { return tourId; }
int Booking::getParticipants() const { return participants; }
double Booking::getAmount() const { return amount; }
bool Booking::isConfirmed() const { return confirmed; }
void Booking::cancel() {
    if (!confirmed) throw std::runtime_error("Booking is already cancelled.");
    confirmed = false;
}
void Booking::display(std::ostream& out) const {
    out << "Booking #" << id << " | Tourist #" << touristId << " | Guide #" << guideId << " | Tour #" << tourId << "\n"
        << "  Participants: " << participants << " | Total: INR " << std::fixed << std::setprecision(2)
        << amount << " | Status: " << (confirmed ? "CONFIRMED" : "CANCELLED") << '\n';
}
