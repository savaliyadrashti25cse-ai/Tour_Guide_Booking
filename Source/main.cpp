#include "GuideSystem.h"
#include "Validation.h"
#include <filesystem>
#include <iostream>
#include <stdexcept>

int main(int argc, char* argv[]) {
    try {
        std::filesystem::path dataDirectory;
        if (argc == 3 && std::string(argv[1]) == "--data-dir") dataDirectory = argv[2];
        else if (argc != 1) {
            std::cerr << "Usage: Project [--data-dir PATH]\n"; return 1;
        } else {
            auto parent = std::filesystem::absolute(argv[0]).parent_path();
            dataDirectory = parent.filename() == "Executable" ? parent.parent_path() / "Data" : std::filesystem::current_path() / "Data";
        }
        GuideSystem system(dataDirectory);
        system.load();
        std::cout << "Tour Guide Booking | CPP45\nData folder: " << dataDirectory.string() << '\n'
            << "Saved records loaded successfully.\n";
        while (true) {
            try {
                GuideSystem::showMainMenu();
                int choice = Validation::readInt("Enter choice: ",1,9);
                switch (choice) {
                    case 1: system.addRecord(); break;
                    case 2: system.displayRecords(); break;
                    case 3: system.searchRecord(); break;
                    case 4: system.updateRecord(); break;
                    case 5: system.deleteRecord(); break;
                    case 6: system.bookingMenu(); break;
                    case 7: system.showReport(); break;
                    case 8: system.saveLoadMenu(); break;
                    case 9: system.save(); std::cout << "Data saved. Goodbye!\n"; return 0;
                }
            } catch (const EndOfInput&) {
                system.save(); std::cout << "\nInput ended. Data saved. Goodbye!\n"; return 0;
            } catch (const std::exception& error) {
                std::cout << "Error: " << error.what() << '\n';
            }
        }
    } catch (const std::exception& error) {
        std::cerr << "Startup error: " << error.what() << "\nSaved data has not been overwritten.\n"; return 1;
    }
}
