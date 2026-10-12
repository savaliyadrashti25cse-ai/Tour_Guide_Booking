#ifndef GUIDE_H
#define GUIDE_H
#include "Person.h"
class Guide : public Person {
    std::string language;
    double dailyRate;
    bool available;
public:
    Guide();
    Guide(int id, const std::string& name, const std::string& phone,
          const std::string& language, double dailyRate, bool available = true);
    ~Guide() override;
    const std::string& getLanguage() const;
    double getDailyRate() const;
    bool isAvailable() const;
    std::string role() const override;
    void display(std::ostream& out = std::cout) const override;
};
#endif
