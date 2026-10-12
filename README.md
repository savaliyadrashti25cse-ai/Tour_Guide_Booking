# Tour Guide Booking — CPP45

Enrollment: **IU2541230488**

A C++17 console project for managing tourists, guides, tours and group bookings. It supports record management, date and capacity checks, price quotations, cancellation, reports, and persistent file storage.

## Build and run on Windows

Install a C++17-capable MinGW-w64 compiler and add its `bin` folder to PATH.

1. Extract the ZIP into a folder.
2. Run `build_windows.bat` to create `Executable/Project.exe`.
3. Run `Run_Project.bat` to open the application with the sample data.

## Build and run on Linux

Install a C++17-capable g++ compiler. In the extracted folder, run:

```bash
bash build_linux.sh
bash run_linux.sh
```

The application uses only the C++ standard library. The build scripts create the Executable folder.

## Files

| Folder or file | Contents |
| --- | --- |
| `Source/` | All 18 C++ source and header files |
| `Data/` | Sample records and an exported report |
| `Tests/` | Functional tests, console tests and recorded results |
| `CMakeLists.txt` | Optional CMake build configuration |
| `build_windows.bat`, `build_linux.sh` | Compiler commands |
| `Run_Project.bat`, `run_linux.sh` | Application launchers |
| `run_tests_windows.bat`, `run_tests.sh` | Functional test launchers |

## Classes and OOP

The required classes are Tourist, Guide, Tour, Booking, GuideSystem and FileManager. Tourist and Guide inherit from abstract Person. Virtual role/display methods provide runtime polymorphism. RecordStore owns a dynamically allocated array and releases it in its destructor. Validation checks input, prices, dates and file contents; errors use exceptions.

## Menu

1. Add Record
2. Display Records
3. Search Record
4. Update Record
5. Delete Record
6. Main Transaction / Booking
7. Report
8. Save / Load
9. Exit

Record categories are Tourist, Guide, Tour and Booking. Text search is case-sensitive. To reschedule a confirmed booking, cancel it and create a new booking. Cancellation releases guide availability and tour seats. Referenced records are protected from deletion.

## Booking calculation

**Total = participants × tour price per person + tour duration × guide daily rate.**

A guide is assigned exclusively to a group for the entire tour date range. Stored booking amounts remain fixed when a guide rate is edited. The sample data has three tourists, three guides, three tours and three bookings. Two bookings are confirmed, covering five participants with a total quotation of INR 10200.00.

## Saving records

The launchers use `Data/records.txt`. The program loads records at startup, saves successful menu changes and saves on exit. Names containing spaces are supported. Record IDs are never reused. Loading validates the complete file before replacing the current data. Saving keeps the previous version in `records.bak`. Use one running program per data directory.

An alternative directory can be selected:

```bash
./Executable/Project --data-dir MyData
```

## Tests

Linux:

```bash
bash run_tests.sh
bash build_linux.sh
python3 Tests/ui_tests.py
```

Windows: run `run_tests_windows.bat`. After building the application, run `python Tests/ui_tests.py` if Python is installed.

Recorded verification: 45 functional tests and six real console scenarios passed on Linux. The 45 functional tests also passed AddressSanitizer and UndefinedBehaviorSanitizer; leak detection was disabled because the environment restricted thread inspection. Native Windows execution remains to be checked on a Windows PC.

## Upload to GitHub

Extract this ZIP, then upload its files and folders to your repository, for example `tour-guide-booking-cpp45`. Copy the actual repository URL into the project report and submission template. The complete submission package also contains the executable, report, UML diagrams and screenshots.
