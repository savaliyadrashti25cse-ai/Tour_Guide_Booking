#ifndef VALIDATION_H
#define VALIDATION_H
#include <string>
namespace Validation {
    std::string trim(const std::string& value);
    void text(const std::string& value, const std::string& field, std::size_t maxLength = 80);
    void phone(const std::string& value);
    void money(double value, const std::string& field);
    void integer(int value, int minimum, int maximum, const std::string& field);
    void date(const std::string& value);
    long long dayNumber(const std::string& value);
    bool overlaps(const std::string& firstDate, int firstDays,
                  const std::string& secondDate, int secondDays);
    int readInt(const std::string& prompt, int minimum, int maximum);
    double readMoney(const std::string& prompt);
    std::string readText(const std::string& prompt);
    bool confirm(const std::string& prompt);
}
class EndOfInput {};
#endif
