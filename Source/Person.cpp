#include "Person.h"
#include "Validation.h"
Person::Person() : id(0) {}
Person::Person(int value, const std::string& personName, const std::string& number)
    : id(value), name(Validation::trim(personName)), phone(Validation::trim(number)) {
    Validation::integer(id,1,1000000000,"ID");
    Validation::text(name,"Name"); Validation::phone(phone);
}
Person::~Person() = default;
int Person::getId() const { return id; }
const std::string& Person::getName() const { return name; }
const std::string& Person::getPhone() const { return phone; }
