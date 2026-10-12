#ifndef PERSON_H
#define PERSON_H
#include <iostream>
#include <string>
class Person {
protected:
    int id;
    std::string name, phone;
public:
    Person();
    Person(int id, const std::string& name, const std::string& phone);
    virtual ~Person();
    int getId() const;
    const std::string& getName() const;
    const std::string& getPhone() const;
    virtual std::string role() const = 0;
    virtual void display(std::ostream& out = std::cout) const = 0;
};
#endif
