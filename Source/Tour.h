#ifndef TOUR_H
#define TOUR_H
#include <iostream>
#include <string>
class Tour {
    int id;
    std::string title, destination, startDate;
    int durationDays, capacity;
    double pricePerPerson;
public:
    Tour();
    Tour(int id, const std::string& title, const std::string& destination,
         const std::string& startDate, int days, int capacity, double price);
    ~Tour();
    int getId() const;
    const std::string& getTitle() const;
    const std::string& getDestination() const;
    const std::string& getStartDate() const;
    int getDurationDays() const;
    int getCapacity() const;
    double getPricePerPerson() const;
    void display(std::ostream& out = std::cout) const;
};
#endif
