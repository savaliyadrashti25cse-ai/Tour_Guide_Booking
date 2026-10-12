#include "GuideSystem.h"
#include "Validation.h"
#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace fs = std::filesystem;
int passed = 0, failed = 0;
void expect(bool condition, const std::string& message) {
    if (!condition) throw std::runtime_error(message);
}
void rejects(const std::function<void()>& operation) {
    try { operation(); } catch (const std::exception&) { return; }
    throw std::runtime_error("Expected the invalid operation to be rejected.");
}
void test(const std::string& name, const std::function<void()>& operation) {
    try { operation(); ++passed; std::cout << "PASS | " << name << '\n'; }
    catch (const std::exception& error) { ++failed; std::cout << "FAIL | " << name << " | " << error.what() << '\n'; }
}
std::string readFile(const fs::path& path) {
    std::ifstream file(path); return {std::istreambuf_iterator<char>(file),std::istreambuf_iterator<char>()};
}
void writeFile(const fs::path& path, const std::string& content) {
    std::ofstream file(path,std::ios::binary); file << content;
}
int main() {
    fs::path root = fs::temp_directory_path() / ("cpp45_tests_" + std::to_string(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
    fs::create_directories(root);
    GuideSystem system(root);
    int tourist = 0, secondTourist = 0, guide = 0, secondGuide = 0, tour = 0, overlap = 0, adjacent = 0;
    Booking first;
    test("01 Missing data starts with empty records", [&]{ system.load(); expect(system.getData().tourists.size() == 0,"Not empty."); });
    test("02 Add tourist and return ID", [&]{ tourist=system.addTourist("Aarav Shah","9000000001","Ahmedabad"); expect(tourist==1,"Wrong ID."); });
    test("03 Add second tourist", [&]{ secondTourist=system.addTourist("Diya Patel","9000000002","Surat"); expect(secondTourist==2,"Wrong ID."); });
    test("04 Add available guides", [&]{ guide=system.addGuide("Raj Patel","9000000003","Hindi",1200); secondGuide=system.addGuide("Meera Shah","9000000004","English",1800); expect(system.getData().guides.size()==2,"Wrong count."); });
    test("05 Add tours including overlapping and adjacent dates", [&]{
        tour=system.addTour("Heritage Tour","Ahmedabad","2026-10-20",2,5,800);
        overlap=system.addTour("Museum Tour","Gandhinagar","2026-10-21",2,5,500);
        adjacent=system.addTour("Unity Tour","Kevadia","2026-10-22",1,5,1000);
        expect(system.getData().tours.size()==3,"Wrong count.");
    });
    test("06 Blank tourist name rejected", [&]{ rejects([&]{system.addTourist(" ","9000000005","Surat");}); });
    test("07 Invalid phone rejected", [&]{ rejects([&]{system.addTourist("Rohan","123abc","Surat");}); });
    test("08 Negative guide rate rejected", [&]{ rejects([&]{system.addGuide("Rohan","9000000005","Hindi",-1);}); });
    test("09 Zero tour capacity rejected", [&]{ rejects([&]{system.addTour("Test","City","2026-10-20",1,0,800);}); });
    test("10 Nonexistent calendar date rejected", [&]{ rejects([&]{Validation::date("2026-02-29");}); });
    test("11 Leap day accepted and day arithmetic correct", [&]{Validation::date("2028-02-29"); expect(Validation::dayNumber("2028-03-01")-Validation::dayNumber("2028-02-28")==2,"Leap calculation.");});
    test("12 Price quotation includes party and guide charges", [&]{ expect(std::fabs(system.quote(tour,guide,3)-4800.0)<0.001,"Wrong quote."); });
    test("13 Confirm booking and allocate seats", [&]{ first=system.createBooking(tourist,guide,tour,3); expect(first.isConfirmed()&&system.bookedSeats(tour)==3,"Booking not active."); });
    test("14 Missing tourist ID rejected", [&]{ rejects([&]{system.createBooking(99,secondGuide,tour,1);}); });
    test("15 Guide date conflict rejected", [&]{ rejects([&]{system.createBooking(secondTourist,guide,overlap,1);}); });
    test("16 Tourist date conflict rejected", [&]{ rejects([&]{system.createBooking(tourist,secondGuide,overlap,1);}); });
    test("17 Tour overcapacity rejected", [&]{ rejects([&]{system.createBooking(secondTourist,secondGuide,tour,3);}); });
    test("18 Zero participants rejected", [&]{ rejects([&]{system.createBooking(secondTourist,secondGuide,tour,0);}); });
    test("19 Unavailable guide rejected", [&]{ int id=system.addGuide("Kiran","9000000005","Gujarati",1000,false); rejects([&]{system.createBooking(secondTourist,id,tour,1);}); });
    test("20 Different guide can fill remaining seats", [&]{ system.createBooking(secondTourist,secondGuide,tour,2); expect(system.bookedSeats(tour)==5,"Not full."); });
    test("21 Adjacent nonoverlapping tour accepted", [&]{system.createBooking(tourist,guide,adjacent,1); expect(system.bookedSeats(adjacent)==1,"Adjacent rejected.");});
    test("22 Update tourist and search by ID", [&]{system.updateTourist(tourist,"Aarav S Shah","9000000011","Vadodara"); expect(system.getData().tourists.find(tourist)->getCity()=="Vadodara","Update missing.");});
    test("23 Updating guide price preserves existing quotation", [&]{ system.updateGuide(guide,"Raj Patel","9000000003","Hindi Gujarati",1500,true); expect(std::fabs(system.getData().bookings.find(first.getId())->getAmount()-4800.0)<0.001,"Historical price changed."); });
    test("24 Referenced record deletion rejected", [&]{ rejects([&]{system.removeEntity(1,tourist);}); });
    test("25 Confirmed booking deletion rejected", [&]{ rejects([&]{system.removeEntity(4,first.getId());}); });
    test("26 Capacity reduction below booked seats rejected", [&]{rejects([&]{system.updateTour(tour,"Heritage Tour","Ahmedabad","2026-10-20",2,4,800);});});
    test("27 Booked tour schedule change rejected", [&]{rejects([&]{system.updateTour(tour,"Heritage Tour","Ahmedabad","2026-10-25",2,5,800);});});
    test("28 Cancel booking releases seats", [&]{system.cancelBooking(first.getId()); expect(system.bookedSeats(tour)==2&&!system.getData().bookings.find(first.getId())->isConfirmed(),"Seats not released.");});
    test("29 Repeated cancellation rejected", [&]{rejects([&]{system.cancelBooking(first.getId());});});
    test("30 Cancelled guide schedule can be booked again", [&]{ auto b=system.createBooking(tourist,guide,tour,3); expect(b.getId()==4,"Failed reuse of released schedule."); });
    test("31 Report excludes cancelled value", [&]{ auto text=system.report(); expect(text.find("Confirmed bookings: 3 | Cancelled bookings: 1")!=std::string::npos,"Wrong report."); expect(text.find("Confirmed booking value: INR 12800.00")!=std::string::npos,"Wrong confirmed value."); });
    test("32 Save and reopen restores all records", [&]{ system.save(); GuideSystem reopened(root); reopened.load(); expect(reopened.getData().bookings.size()==4&&reopened.getData().tourists.find(tourist)->getCity()=="Vadodara","Persistence failed."); });
    test("33 Corrupt file rejected without replacing live records", [&]{ auto content=readFile(root/"records.txt"); writeFile(root/"records.txt",content+"UNKNOWN 5\n"); rejects([&]{system.load();}); expect(system.getData().bookings.size()==4,"Live data lost."); writeFile(root/"records.txt",content); });
    test("34 Quoted names round trip correctly", [&]{int id=system.addTourist("A \"Quoted\" Name","9000000066","New City"); system.save(); GuideSystem reopened(root); reopened.load(); expect(reopened.getData().tourists.find(id)->getName()=="A \"Quoted\" Name","Name changed.");});
    test("35 Dynamic store grows beyond initial capacity", [&]{for(int i=0;i<20;++i) system.addTourist("Visitor "+std::to_string(i),"9000000077","Surat"); expect(system.getData().tourists.size()==23&&system.getData().tourists.allocatedCapacity()>=23,"Growth failed.");});
    test("36 Deletion does not reuse the last issued ID", [&]{ int last=system.addTourist("Temporary","9000000088","City"); system.removeEntity(1,last); int next=system.addTourist("Next","9000000089","City"); expect(next==last+1,"ID reused."); });
    test("37 Cancelled booking can be deleted", [&]{system.removeEntity(4,first.getId()); expect(!system.getData().bookings.find(first.getId()),"Booking still exists.");});
    test("38 Runtime polymorphism displays both derived roles", [&]{Tourist t(100,"Tourist","9000000090","City"); Guide g(100,"Guide","9000000091","Hindi",900); const Person* people[]={&t,&g}; std::ostringstream output; for(auto p:people)p->display(output); expect(output.str().find("Tourist #100")!=std::string::npos&&output.str().find("Guide #100")!=std::string::npos,"Wrong virtual dispatch.");});
    test("39 Windows CRLF data loads", [&]{system.save(); std::string text=readFile(root/"records.txt"), windows; for(char c:text){if(c=='\n')windows+='\r';windows+=c;} writeFile(root/"records.txt",windows); GuideSystem reopened(root); reopened.load(); expect(reopened.getData().bookings.size()==3,"CRLF failed.");});
    test("40 Backup recovery restores data when primary is missing", [&]{system.save(); system.save(); fs::remove(root/"records.txt"); GuideSystem reopened(root); reopened.load(); expect(reopened.getData().bookings.size()==3,"Backup recovery failed.");});
    test("41 Duplicate persisted IDs rejected", [&]{system.save(); auto text=readFile(root/"records.txt"); writeFile(root/"records.txt",text+"TOURIST 1 \"Duplicate\" \"9000000011\" \"City\"\n"); rejects([&]{system.load();}); writeFile(root/"records.txt",text);});
    test("42 Dangling saved booking rejected", [&]{system.save(); auto text=readFile(root/"records.txt"); auto pos=text.find("BOOKING 2 "); expect(pos!=std::string::npos,"Fixture missing."); auto end=text.find('\n',pos); auto corrupt=text; corrupt.replace(pos,end-pos,"BOOKING 2 999 2 1 2 5200.00 1"); writeFile(root/"records.txt",corrupt); rejects([&]{system.load();}); writeFile(root/"records.txt",text);});
    test("43 Data write failure surfaces an error", [&]{auto file=root/"not_a_directory"; writeFile(file,"x"); GuideSystem invalid(file); rejects([&]{invalid.save();});});
    test("44 Unknown record deletion rejected", [&]{rejects([&]{system.removeEntity(3,999);});});
    test("45 Booking amount that rounds to zero rejected", [&]{rejects([&]{Booking invalid(100,1,1,1,1,0.001);});});
    fs::remove_all(root);
    std::cout << "RESULT | " << passed << " passed, " << failed << " failed\n";
    return failed ? 1 : 0;
}
