#include "GuideSystem.h"
#include "Validation.h"
#include <cmath>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace {
template<class T> const T& required(const RecordStore<T>& records, int id, const char* label) {
    const auto* record = records.find(id);
    if (!record) throw std::runtime_error(std::string(label) + " ID was not found.");
    return *record;
}
template<class T> void validateCounter(const RecordStore<T>& records, int next) {
    Validation::integer(next,1,1000000000,"Next ID");
    for (std::size_t i = 0; i < records.size(); ++i)
        if (records[i].getId() >= next) throw std::runtime_error("Saved ID counter is invalid.");
}
template<class T> void displayStore(const RecordStore<T>& records, const std::string& heading) {
    std::cout << "\n" << heading << " (" << records.size() << ")\n";
    if (!records.size()) std::cout << "No records available.\n";
    for (std::size_t i = 0; i < records.size(); ++i) records[i].display();
}
template<class T> void searchStore(const RecordStore<T>& records, const std::string& term) {
    bool found = false;
    for (std::size_t i = 0; i < records.size(); ++i) {
        std::ostringstream rendered; records[i].display(rendered);
        if (rendered.str().find(term) != std::string::npos) { std::cout << rendered.str(); found = true; }
    }
    if (!found) std::cout << "No matching record found.\n";
}
int entityMenu(bool all = false) {
    std::cout << "1. Tourist  2. Guide  3. Tour  4. Booking";
    if (all) std::cout << "  5. All records";
    std::cout << '\n';
    return Validation::readInt("Select record type: ",1,all ? 5 : 4);
}
}

GuideSystem::GuideSystem(const std::filesystem::path& directory) : fileManager(directory) {}
GuideSystem::~GuideSystem() = default;
const ProjectData& GuideSystem::getData() const { return data; }
int GuideSystem::addTourist(const std::string& name, const std::string& phone, const std::string& city) {
    int id = data.nextTourist;
    Validation::integer(id,1,999999999,"Next tourist ID");
    data.tourists.add(Tourist(id,name,phone,city)); ++data.nextTourist; return id;
}
int GuideSystem::addGuide(const std::string& name, const std::string& phone, const std::string& language, double rate, bool available) {
    int id = data.nextGuide;
    Validation::integer(id,1,999999999,"Next guide ID");
    data.guides.add(Guide(id,name,phone,language,rate,available)); ++data.nextGuide; return id;
}
int GuideSystem::addTour(const std::string& title, const std::string& destination, const std::string& date, int days, int capacity, double price) {
    int id = data.nextTour;
    Validation::integer(id,1,999999999,"Next tour ID");
    data.tours.add(Tour(id,title,destination,date,days,capacity,price)); ++data.nextTour; return id;
}
void GuideSystem::updateTourist(int id, const std::string& name, const std::string& phone, const std::string& city) {
    required(data.tourists,id,"Tourist");
    *data.tourists.find(id) = Tourist(id,name,phone,city);
}
void GuideSystem::updateGuide(int id, const std::string& name, const std::string& phone, const std::string& language, double rate, bool available) {
    required(data.guides,id,"Guide");
    *data.guides.find(id) = Guide(id,name,phone,language,rate,available);
}
void GuideSystem::updateTour(int id, const std::string& title, const std::string& destination, const std::string& date, int days, int capacity, double price) {
    const auto& previous = required(data.tours,id,"Tour");
    Tour candidate(id,title,destination,date,days,capacity,price);
    if (capacity < bookedSeats(id)) throw std::runtime_error("Capacity cannot be lower than confirmed participants.");
    if (referenced(3,id) && (previous.getStartDate() != date || previous.getDurationDays() != days || previous.getDestination() != destination))
        throw std::runtime_error("A booked tour's date, duration and destination cannot be changed. Add a new tour instead.");
    *data.tours.find(id) = candidate;
}
bool GuideSystem::referenced(int type, int id) const {
    for (std::size_t i = 0; i < data.bookings.size(); ++i) {
        const auto& b = data.bookings[i];
        if ((type == 1 && b.getTouristId() == id) || (type == 2 && b.getGuideId() == id) || (type == 3 && b.getTourId() == id)) return true;
    }
    return false;
}
void GuideSystem::removeEntity(int type, int id) {
    Validation::integer(type,1,4,"Record type");
    if (type == 4) {
        const auto& b = required(data.bookings,id,"Booking");
        if (b.isConfirmed()) throw std::runtime_error("Cancel a confirmed booking before deleting it.");
        data.bookings.remove(id); return;
    }
    if (referenced(type,id)) throw std::runtime_error("Record is referenced by a booking. Remove cancelled booking records first.");
    bool removed = type == 1 ? data.tourists.remove(id) : type == 2 ? data.guides.remove(id) : data.tours.remove(id);
    if (!removed) throw std::runtime_error("Record ID was not found.");
}
int GuideSystem::bookedSeats(int id) const {
    int seats = 0;
    for (std::size_t i = 0; i < data.bookings.size(); ++i)
        if (data.bookings[i].getTourId() == id && data.bookings[i].isConfirmed()) seats += data.bookings[i].getParticipants();
    return seats;
}
double GuideSystem::quote(int tourId, int guideId, int participants) const {
    const auto& tour = required(data.tours,tourId,"Tour");
    const auto& guide = required(data.guides,guideId,"Guide");
    Validation::integer(participants,1,1000,"Participants");
    return std::round((tour.getPricePerPerson() * participants + guide.getDailyRate() * tour.getDurationDays()) * 100.0) / 100.0;
}
Booking GuideSystem::createBooking(int touristId, int guideId, int tourId, int participants) {
    required(data.tourists,touristId,"Tourist");
    const auto& guide = required(data.guides,guideId,"Guide");
    const auto& tour = required(data.tours,tourId,"Tour");
    Validation::integer(participants,1,1000,"Participants");
    if (!guide.isAvailable()) throw std::runtime_error("Guide is not accepting new bookings.");
    if (participants + bookedSeats(tourId) > tour.getCapacity()) throw std::runtime_error("Tour capacity exceeded.");
    for (std::size_t i = 0; i < data.bookings.size(); ++i) {
        const auto& b = data.bookings[i];
        if (!b.isConfirmed()) continue;
        const auto& existing = required(data.tours,b.getTourId(),"Existing tour");
        if (Validation::overlaps(tour.getStartDate(),tour.getDurationDays(),existing.getStartDate(),existing.getDurationDays())) {
            if (b.getGuideId() == guideId) throw std::runtime_error("Guide already has a booking during these dates.");
            if (b.getTouristId() == touristId) throw std::runtime_error("Tourist already has a booking during these dates.");
        }
    }
    Validation::integer(data.nextBooking,1,999999999,"Next booking ID");
    Booking result(data.nextBooking,touristId,guideId,tourId,participants,quote(tourId,guideId,participants));
    data.bookings.add(result); ++data.nextBooking; return result;
}
void GuideSystem::cancelBooking(int id) {
    required(data.bookings,id,"Booking"); data.bookings.find(id)->cancel();
}
void GuideSystem::validateData(const ProjectData& candidate) {
    validateCounter(candidate.tourists,candidate.nextTourist);
    validateCounter(candidate.guides,candidate.nextGuide);
    validateCounter(candidate.tours,candidate.nextTour);
    validateCounter(candidate.bookings,candidate.nextBooking);
    for (std::size_t i = 0; i < candidate.bookings.size(); ++i) {
        const auto& b = candidate.bookings[i];
        required(candidate.tourists,b.getTouristId(),"Saved tourist");
        required(candidate.guides,b.getGuideId(),"Saved guide");
        const auto& t = required(candidate.tours,b.getTourId(),"Saved tour");
        if (!b.isConfirmed()) continue;
        int participants = 0;
        for (std::size_t j = 0; j < candidate.bookings.size(); ++j) {
            const auto& other = candidate.bookings[j];
            if (!other.isConfirmed()) continue;
            if (other.getTourId() == b.getTourId()) participants += other.getParticipants();
            if (i >= j) continue;
            const auto& second = required(candidate.tours,other.getTourId(),"Saved tour");
            if (Validation::overlaps(t.getStartDate(),t.getDurationDays(),second.getStartDate(),second.getDurationDays()) &&
                (b.getGuideId() == other.getGuideId() || b.getTouristId() == other.getTouristId()))
                throw std::runtime_error("Saved bookings contain a scheduling conflict.");
        }
        if (participants > t.getCapacity()) throw std::runtime_error("Saved bookings exceed tour capacity.");
    }
}
void GuideSystem::save() const { fileManager.save(data); }
void GuideSystem::load() {
    ProjectData candidate = fileManager.load();
    validateData(candidate);
    data = std::move(candidate); // Replace live records only after the whole file has passed validation.
}
std::string GuideSystem::report() const {
    std::ostringstream out;
    int confirmed = 0, cancelled = 0, participants = 0; double confirmedValue = 0, cancelledValue = 0;
    for (std::size_t i = 0; i < data.bookings.size(); ++i) {
        const auto& b = data.bookings[i];
        if (b.isConfirmed()) { ++confirmed; participants += b.getParticipants(); confirmedValue += b.getAmount(); }
        else { ++cancelled; cancelledValue += b.getAmount(); }
    }
    out << "TOUR GUIDE BOOKING - SUMMARY REPORT\n"
        << "Tourists: " << data.tourists.size() << " | Guides: " << data.guides.size() << " | Tours: " << data.tours.size() << '\n'
        << "Confirmed bookings: " << confirmed << " | Cancelled bookings: " << cancelled << '\n'
        << "Confirmed participants: " << participants << '\n' << std::fixed << std::setprecision(2)
        << "Confirmed booking value: INR " << confirmedValue << '\n'
        << "Cancelled booking value: INR " << cancelledValue << " (excluded from confirmed value)\n"
        << "Amounts are booking quotations, not recorded payments.\n\nTOUR OCCUPANCY\n";
    for (std::size_t i = 0; i < data.tours.size(); ++i) {
        const auto& t = data.tours[i]; int seats = bookedSeats(t.getId());
        out << "Tour #" << t.getId() << " - " << t.getTitle() << ": " << seats << '/' << t.getCapacity()
            << " booked; " << t.getCapacity() - seats << " remaining\n";
    }
    return out.str();
}
void GuideSystem::showMainMenu() {
    std::cout << "\n==============================================\n"
        << "            TOUR GUIDE BOOKING\n"
        << "==============================================\n"
        << "1. Add Record\n2. Display Records\n3. Search Record\n4. Update Record\n5. Delete Record\n"
        << "6. Main Transaction / Booking\n7. Report\n8. Save / Load\n9. Exit\n";
}
void GuideSystem::addRecord() {
    int type = entityMenu();
    if (type == 4) { bookingMenu(); return; }
    int id;
    if (type == 1) {
        std::string name = Validation::readText("Tourist name: ");
        std::string phone = Validation::readText("Phone (7-15 digits): ");
        std::string city = Validation::readText("City: ");
        id = addTourist(name,phone,city);
    } else if (type == 2) {
        std::string name = Validation::readText("Guide name: ");
        std::string phone = Validation::readText("Phone (7-15 digits): ");
        std::string language = Validation::readText("Languages: ");
        double rate = Validation::readMoney("Daily rate (INR): ");
        bool available = Validation::readInt("Accepting bookings? 1=Yes 0=No: ",0,1) == 1;
        id = addGuide(name,phone,language,rate,available);
    } else {
        std::string title = Validation::readText("Tour title: ");
        std::string destination = Validation::readText("Destination: ");
        std::string date = Validation::readText("Start date (YYYY-MM-DD): ");
        int days = Validation::readInt("Duration in days: ",1,30);
        int capacity = Validation::readInt("Maximum participants: ",1,1000);
        double price = Validation::readMoney("Tour price per person (INR): ");
        id = addTour(title,destination,date,days,capacity,price);
    }
    save(); std::cout << "Record added successfully. ID: " << id << "\nData saved.\n";
}
void GuideSystem::displayRecords() const {
    int type = entityMenu(true);
    if (type == 1 || type == 5) {
        std::cout << "\nTOURISTS (" << data.tourists.size() << ")\n";
        if (!data.tourists.size()) std::cout << "No records available.\n";
        for (std::size_t i = 0; i < data.tourists.size(); ++i) {
            const Person* person = &data.tourists[i]; person->display(); // Runtime polymorphism.
        }
    }
    if (type == 2 || type == 5) {
        std::cout << "\nGUIDES (" << data.guides.size() << ")\n";
        if (!data.guides.size()) std::cout << "No records available.\n";
        for (std::size_t i = 0; i < data.guides.size(); ++i) {
            const Person* person = &data.guides[i]; person->display();
        }
    }
    if (type == 3 || type == 5) {
        displayStore(data.tours,"TOURS");
        for (std::size_t i = 0; i < data.tours.size(); ++i)
            std::cout << "  Tour #" << data.tours[i].getId() << " seats remaining: " << data.tours[i].getCapacity() - bookedSeats(data.tours[i].getId()) << '\n';
    }
    if (type == 4 || type == 5) displayStore(data.bookings,"BOOKINGS");
}
void GuideSystem::searchRecord() const {
    int type = entityMenu();
    int mode = Validation::readInt("1. Search by ID  2. Search by text: ",1,2);
    if (mode == 1) {
        int id = Validation::readInt("Record ID: ",1,1000000000);
        if (type == 1) required(data.tourists,id,"Tourist").display();
        else if (type == 2) required(data.guides,id,"Guide").display();
        else if (type == 3) required(data.tours,id,"Tour").display();
        else required(data.bookings,id,"Booking").display();
    } else {
        std::string term = Validation::readText("Search text (case-sensitive): ");
        Validation::text(term,"Search text");
        if (type == 1) searchStore(data.tourists,term);
        else if (type == 2) searchStore(data.guides,term);
        else if (type == 3) searchStore(data.tours,term);
        else searchStore(data.bookings,term);
    }
}
void GuideSystem::updateRecord() {
    int type = entityMenu();
    int id = Validation::readInt("Record ID: ",1,1000000000);
    if (type == 4) {
        required(data.bookings,id,"Booking").display();
        if (Validation::confirm("Cancel this booking?")) { cancelBooking(id); save(); std::cout << "Booking cancelled and saved.\n"; }
        return;
    }
    std::cout << "Enter the complete replacement details.\n";
    if (type == 1) {
        required(data.tourists,id,"Tourist").display();
        std::string name = Validation::readText("Name: "), phone = Validation::readText("Phone: "), city = Validation::readText("City: ");
        updateTourist(id,name,phone,city);
    } else if (type == 2) {
        required(data.guides,id,"Guide").display();
        std::string name = Validation::readText("Name: "), phone = Validation::readText("Phone: "), language = Validation::readText("Languages: ");
        double rate = Validation::readMoney("Daily rate (INR): ");
        bool available = Validation::readInt("Accepting bookings? 1=Yes 0=No: ",0,1) == 1;
        updateGuide(id,name,phone,language,rate,available);
    } else {
        required(data.tours,id,"Tour").display();
        std::string title = Validation::readText("Title: "), destination = Validation::readText("Destination: "), date = Validation::readText("Start date: ");
        int days = Validation::readInt("Duration in days: ",1,30), capacity = Validation::readInt("Capacity: ",1,1000);
        double price = Validation::readMoney("Price per person (INR): ");
        updateTour(id,title,destination,date,days,capacity,price);
    }
    save(); std::cout << "Record updated and saved.\n";
}
void GuideSystem::deleteRecord() {
    int type = entityMenu();
    int id = Validation::readInt("Record ID to delete: ",1,1000000000);
    if (!Validation::confirm("Delete this record permanently?")) { std::cout << "Deletion aborted.\n"; return; }
    removeEntity(type,id); save(); std::cout << "Record deleted and saved.\n";
}
void GuideSystem::bookingMenu() {
    std::cout << "\n1. Create booking  2. Cancel booking  3. View bookings  4. Back\n";
    int action = Validation::readInt("Choose booking action: ",1,4);
    if (action == 4) return;
    if (action == 3) { displayStore(data.bookings,"BOOKINGS"); return; }
    if (action == 2) {
        int id = Validation::readInt("Booking ID: ",1,1000000000);
        required(data.bookings,id,"Booking").display();
        if (Validation::confirm("Cancel this booking?")) { cancelBooking(id); save(); std::cout << "Booking cancelled and saved. Seats and schedule released.\n"; }
        return;
    }
    if (!data.tourists.size() || !data.guides.size() || !data.tours.size())
        throw std::runtime_error("Add at least one tourist, guide and tour before booking.");
    displayStore(data.tourists,"TOURISTS"); displayStore(data.guides,"GUIDES"); displayStore(data.tours,"TOURS");
    int tourist = Validation::readInt("Tourist ID: ",1,1000000000);
    int tour = Validation::readInt("Tour ID: ",1,1000000000);
    int guide = Validation::readInt("Guide ID: ",1,1000000000);
    int people = Validation::readInt("Number of participants: ",1,1000);
    required(data.tourists,tourist,"Tourist");
    std::cout << "Quotation: INR " << std::fixed << std::setprecision(2) << quote(tour,guide,people) << '\n'
        << "Formula: participants x tour price + duration x guide daily rate.\n";
    if (!Validation::confirm("Confirm booking?")) { std::cout << "Booking aborted.\n"; return; }
    Booking result = createBooking(tourist,guide,tour,people);
    save(); std::cout << "Booking created successfully.\n"; result.display(); std::cout << "Data saved.\n";
}
void GuideSystem::showReport() const {
    std::string text = report(); std::cout << '\n' << text;
    fileManager.writeReport(text);
    std::cout << "Report saved to " << (fileManager.getDirectory() / "report.txt").string() << '\n';
}
void GuideSystem::saveLoadMenu() {
    int action = Validation::readInt("1. Save data  2. Reload data: ",1,2);
    if (action == 1) { save(); std::cout << "Data saved successfully.\n"; }
    else if (Validation::confirm("Reload the last saved data?")) { load(); std::cout << "Data loaded successfully.\n"; }
}
