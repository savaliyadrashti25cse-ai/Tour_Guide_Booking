#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H
#include "Tourist.h"
#include "Guide.h"
#include "Tour.h"
#include "Booking.h"
#include "RecordStore.h"
#include <filesystem>

// Owns every record and the next unused IDs. IDs are not reused after deletion.
struct ProjectData {
    RecordStore<Tourist> tourists;
    RecordStore<Guide> guides;
    RecordStore<Tour> tours;
    RecordStore<Booking> bookings;
    int nextTourist = 1, nextGuide = 1, nextTour = 1, nextBooking = 1;
};
class FileManager {
    std::filesystem::path directory;
public:
    explicit FileManager(const std::filesystem::path& directory = "Data");
    ~FileManager();
    const std::filesystem::path& getDirectory() const;
    ProjectData load() const;
    void save(const ProjectData& data) const;
    void writeReport(const std::string& text) const;
};
#endif
