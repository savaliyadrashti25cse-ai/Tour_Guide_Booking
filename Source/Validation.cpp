#include "Validation.h"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <iostream>
#include <sstream>
#include <stdexcept>

std::string Validation::trim(const std::string& value) {
    const auto first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    return value.substr(first, value.find_last_not_of(" \t\r\n") - first + 1);
}
void Validation::text(const std::string& value, const std::string& field, std::size_t maxLength) {
    if (trim(value).empty() || value.size() > maxLength)
        throw std::invalid_argument(field + " must contain 1 to " + std::to_string(maxLength) + " characters.");
    for (unsigned char ch : value)
        if (ch < 32 || ch == 127) throw std::invalid_argument(field + " contains a control character.");
}
void Validation::phone(const std::string& value) {
    if (value.size() < 7 || value.size() > 15 ||
        !std::all_of(value.begin(), value.end(), [](unsigned char c){ return std::isdigit(c); }))
        throw std::invalid_argument("Phone must contain 7 to 15 digits.");
}
void Validation::money(double value, const std::string& field) {
    if (!std::isfinite(value) || value <= 0 || value > 1000000)
        throw std::invalid_argument(field + " must be greater than 0 and at most 1000000.");
}
void Validation::integer(int value, int minimum, int maximum, const std::string& field) {
    if (value < minimum || value > maximum)
        throw std::invalid_argument(field + " must be between " + std::to_string(minimum) + " and " + std::to_string(maximum) + ".");
}
void Validation::date(const std::string& value) {
    if (value.size() != 10 || value[4] != '-' || value[7] != '-')
        throw std::invalid_argument("Date must use YYYY-MM-DD format.");
    for (std::size_t i = 0; i < value.size(); ++i)
        if (i != 4 && i != 7 && !std::isdigit(static_cast<unsigned char>(value[i])))
            throw std::invalid_argument("Date contains a non-numeric value.");
    int year = std::stoi(value.substr(0,4)), month = std::stoi(value.substr(5,2));
    int day = std::stoi(value.substr(8,2));
    integer(year, 2000, 2100, "Year"); integer(month, 1, 12, "Month");
    int days[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) days[1] = 29;
    integer(day, 1, days[month - 1], "Day");
}
long long Validation::dayNumber(const std::string& value) {
    date(value);
    int year = std::stoi(value.substr(0,4)), month = std::stoi(value.substr(5,2));
    int day = std::stoi(value.substr(8,2));
    long long total = 0;
    for (int y = 2000; y < year; ++y)
        total += (y % 4 == 0 && (y % 100 != 0 || y % 400 == 0)) ? 366 : 365;
    int days[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) days[1] = 29;
    for (int m = 1; m < month; ++m) total += days[m - 1];
    return total + day - 1;
}
bool Validation::overlaps(const std::string& a, int aDays, const std::string& b, int bDays) {
    long long first = dayNumber(a), second = dayNumber(b);
    return first < second + bDays && second < first + aDays;
}
std::string Validation::readText(const std::string& prompt) {
    std::cout << prompt;
    std::string value;
    if (!std::getline(std::cin, value)) throw EndOfInput();
    return trim(value);
}
int Validation::readInt(const std::string& prompt, int minimum, int maximum) {
    while (true) {
        std::istringstream input(readText(prompt));
        int value; char extra;
        if (input >> value && !(input >> extra) && value >= minimum && value <= maximum) return value;
        std::cout << "Invalid input. Enter an integer from " << minimum << " to " << maximum << ".\n";
    }
}
double Validation::readMoney(const std::string& prompt) {
    while (true) {
        std::istringstream input(readText(prompt));
        double value; char extra;
        if (input >> value && !(input >> extra)) {
            try { money(value, "Amount"); return std::round(value * 100.0) / 100.0; }
            catch (const std::exception&) {}
        }
        std::cout << "Invalid input. Enter a positive amount up to 1000000.\n";
    }
}
bool Validation::confirm(const std::string& prompt) {
    std::string value = readText(prompt + " (y/n): ");
    return value == "y" || value == "Y";
}
