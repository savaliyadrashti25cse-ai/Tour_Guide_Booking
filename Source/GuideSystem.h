#ifndef GUIDE_SYSTEM_H
#define GUIDE_SYSTEM_H
#include "FileManager.h"
class GuideSystem {
    ProjectData data;
    FileManager fileManager;
    static void validateData(const ProjectData& data);
    bool referenced(int entityType, int id) const;
public:
    explicit GuideSystem(const std::filesystem::path& directory = "Data");
    ~GuideSystem();
    const ProjectData& getData() const;
    int addTourist(const std::string& name, const std::string& phone, const std::string& city);
    int addGuide(const std::string& name, const std::string& phone, const std::string& language, double rate, bool available = true);
    int addTour(const std::string& title, const std::string& destination, const std::string& startDate, int days, int capacity, double price);
    void updateTourist(int id, const std::string& name, const std::string& phone, const std::string& city);
    void updateGuide(int id, const std::string& name, const std::string& phone, const std::string& language, double rate, bool available);
    void updateTour(int id, const std::string& title, const std::string& destination, const std::string& date, int days, int capacity, double price);
    void removeEntity(int entityType, int id);
    int bookedSeats(int tourId) const;
    double quote(int tourId, int guideId, int participants) const;
    Booking createBooking(int touristId, int guideId, int tourId, int participants);
    void cancelBooking(int id);
    void save() const;
    void load();
    std::string report() const;
    void addRecord();
    void displayRecords() const;
    void searchRecord() const;
    void updateRecord();
    void deleteRecord();
    void bookingMenu();
    void showReport() const;
    void saveLoadMenu();
    static void showMainMenu();
};
#endif
