#ifndef BOOKING_H
#define BOOKING_H
#include <iostream>
#include <string>
class Booking {
    int id, touristId, guideId, tourId, participants;
    double amount;
    bool confirmed;
public:
    Booking();
    Booking(int id, int touristId, int guideId, int tourId, int participants, double amount, bool confirmed = true);
    ~Booking();
    int getId() const;
    int getTouristId() const;
    int getGuideId() const;
    int getTourId() const;
    int getParticipants() const;
    double getAmount() const;
    bool isConfirmed() const;
    void cancel();
    void display(std::ostream& out = std::cout) const;
};
#endif
