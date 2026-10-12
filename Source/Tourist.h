#ifndef TOURIST_H
#define TOURIST_H
#include "Person.h"
class Tourist : public Person {
    std::string city;
public:
    Tourist();
    Tourist(int id, const std::string& name, const std::string& phone, const std::string& city);
    ~Tourist() override;
    const std::string& getCity() const;
    std::string role() const override;
    void display(std::ostream& out = std::cout) const override;
};
#endif
