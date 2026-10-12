#include "FileManager.h"
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

FileManager::FileManager(const std::filesystem::path& path) : directory(path) {}
FileManager::~FileManager() = default;
const std::filesystem::path& FileManager::getDirectory() const { return directory; }

ProjectData FileManager::load() const {
    ProjectData staged;
    auto path = directory / "records.txt";
    // If a previous save stopped between renames, recover the complete backup.
    if (!std::filesystem::exists(path) && std::filesystem::exists(directory / "records.bak"))
        path = directory / "records.bak";
    if (!std::filesystem::exists(path)) return staged;
    std::ifstream file(path);
    if (!file) throw std::runtime_error("Cannot open saved data: " + path.string());
    std::string line;
    if (!std::getline(file,line)) throw std::runtime_error("Saved data file is empty.");
    if (!line.empty() && line.back() == '\r') line.pop_back();
    if (line != "TOUR_GUIDE_BOOKING 1")
        throw std::runtime_error("Data header is invalid. Original file has not been changed.");
    bool countersFound = false;
    int lineNumber = 1;
    while (std::getline(file,line)) {
        ++lineNumber;
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) continue;
        std::istringstream row(line);
        std::string type; row >> type;
        try {
            int id, tourist, guide, tour, people, days, capacity, active;
            double rate, price, amount;
            std::string name, phone, city, language, title, destination, date;
            if (type == "TOURIST") {
                if (!(row >> id >> std::quoted(name) >> std::quoted(phone) >> std::quoted(city)))
                    throw std::runtime_error("Incomplete tourist row.");
                staged.tourists.add(Tourist(id,name,phone,city));
            } else if (type == "GUIDE") {
                if (!(row >> id >> std::quoted(name) >> std::quoted(phone) >> std::quoted(language) >> rate >> active) || (active != 0 && active != 1))
                    throw std::runtime_error("Incomplete guide row or invalid availability.");
                staged.guides.add(Guide(id,name,phone,language,rate,active == 1));
            } else if (type == "TOUR") {
                if (!(row >> id >> std::quoted(title) >> std::quoted(destination) >> std::quoted(date) >> days >> capacity >> price))
                    throw std::runtime_error("Incomplete tour row.");
                staged.tours.add(Tour(id,title,destination,date,days,capacity,price));
            } else if (type == "BOOKING") {
                if (!(row >> id >> tourist >> guide >> tour >> people >> amount >> active) || (active != 0 && active != 1))
                    throw std::runtime_error("Incomplete booking row or invalid status.");
                staged.bookings.add(Booking(id,tourist,guide,tour,people,amount,active == 1));
            } else if (type == "COUNTERS" && !countersFound) {
                if (!(row >> staged.nextTourist >> staged.nextGuide >> staged.nextTour >> staged.nextBooking))
                    throw std::runtime_error("Incomplete ID counters.");
                countersFound = true;
            } else throw std::runtime_error("Unknown or repeated record type.");
            std::string extra;
            if (row >> extra) throw std::runtime_error("Unexpected extra fields.");
        } catch (const std::exception& error) {
            throw std::runtime_error("Data line " + std::to_string(lineNumber) + ": " + error.what());
        }
    }
    if (!file.eof()) throw std::runtime_error("Error while reading data.");
    if (!countersFound) throw std::runtime_error("Missing ID counters; data not loaded.");
    return staged;
}

void FileManager::save(const ProjectData& data) const {
    std::filesystem::create_directories(directory);
    auto destination = directory / "records.txt";
    auto temporary = directory / "records.tmp";
    auto backup = directory / "records.bak";
    {
        std::ofstream file(temporary, std::ios::trunc);
        if (!file) throw std::runtime_error("Cannot write data. Check folder permissions.");
        file << "TOUR_GUIDE_BOOKING 1\n" << std::fixed << std::setprecision(2);
        for (std::size_t i = 0; i < data.tourists.size(); ++i) {
            const auto& t = data.tourists[i];
            file << "TOURIST " << t.getId() << ' ' << std::quoted(t.getName()) << ' '
                 << std::quoted(t.getPhone()) << ' ' << std::quoted(t.getCity()) << '\n';
        }
        for (std::size_t i = 0; i < data.guides.size(); ++i) {
            const auto& g = data.guides[i];
            file << "GUIDE " << g.getId() << ' ' << std::quoted(g.getName()) << ' ' << std::quoted(g.getPhone())
                 << ' ' << std::quoted(g.getLanguage()) << ' ' << g.getDailyRate() << ' ' << g.isAvailable() << '\n';
        }
        for (std::size_t i = 0; i < data.tours.size(); ++i) {
            const auto& t = data.tours[i];
            file << "TOUR " << t.getId() << ' ' << std::quoted(t.getTitle()) << ' ' << std::quoted(t.getDestination())
                 << ' ' << std::quoted(t.getStartDate()) << ' ' << t.getDurationDays() << ' ' << t.getCapacity()
                 << ' ' << t.getPricePerPerson() << '\n';
        }
        for (std::size_t i = 0; i < data.bookings.size(); ++i) {
            const auto& b = data.bookings[i];
            file << "BOOKING " << b.getId() << ' ' << b.getTouristId() << ' ' << b.getGuideId() << ' ' << b.getTourId()
                 << ' ' << b.getParticipants() << ' ' << b.getAmount() << ' ' << b.isConfirmed() << '\n';
        }
        file << "COUNTERS " << data.nextTourist << ' ' << data.nextGuide << ' ' << data.nextTour << ' ' << data.nextBooking << '\n';
        file.flush();
        if (!file) throw std::runtime_error("Data could not be completely written.");
        file.close();
        if (!file) throw std::runtime_error("Data file could not be closed correctly.");
    }
    bool originalMoved = false;
    try {
        if (std::filesystem::exists(destination)) {
            std::filesystem::remove(backup);
            std::filesystem::rename(destination,backup);
            originalMoved = true;
        }
        std::filesystem::rename(temporary,destination);
    } catch (...) {
        if (originalMoved && !std::filesystem::exists(destination)) {
            std::error_code ignored;
            std::filesystem::rename(backup,destination,ignored);
        }
        throw std::runtime_error("Could not replace data file. Existing data was preserved; retry saving.");
    }
}
void FileManager::writeReport(const std::string& text) const {
    std::filesystem::create_directories(directory);
    std::ofstream file(directory / "report.txt");
    if (!file) throw std::runtime_error("Cannot write report.txt.");
    file << text; file.close();
    if (!file) throw std::runtime_error("Report could not be completely written.");
}
