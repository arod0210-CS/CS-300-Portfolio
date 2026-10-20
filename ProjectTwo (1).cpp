/*
 * ProjectTwo.cpp
 * CS 300 - Project Two: ABCU Advising Assistance Program
 *
 * This console program loads courses from a CSV file, prints an
 * alphanumerically ordered course list, and shows course prerequisites.
 *
 */

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// Store the information for one ABCU course.
struct Course {
    string number;
    string title;
    vector<string> prerequisites;
};

// Remove unnecessary spaces from the beginning and end of a string.
string trim(const string& text) {
    const string spaces = " \t\r\n";
    size_t first = text.find_first_not_of(spaces);
    if (first == string::npos) {
        return "";
    }
    size_t last = text.find_last_not_of(spaces);
    return text.substr(first, last - first + 1);
}

// Convert a course number to uppercase so searches are case-insensitive.
string uppercase(string text) {
    transform(text.begin(), text.end(), text.begin(),
              [](unsigned char letter) { return static_cast<char>(toupper(letter)); });
    return text;
}

// Parse a line containing: course number, title, optional prerequisites.
// The ABCU input format does not use quoted fields with embedded commas.
bool parseCourse(const string& line, Course& course) {
    stringstream input(line);
    string field;
    vector<string> fields;

    while (getline(input, field, ',')) {
        fields.push_back(trim(field));
    }

    if (fields.size() < 2 || fields[0].empty() || fields[1].empty()) {
        return false;
    }

    course.number = uppercase(fields[0]);
    course.title = fields[1];
    course.prerequisites.clear();

    for (size_t i = 2; i < fields.size(); ++i) {
        if (!fields[i].empty()) {
            course.prerequisites.push_back(uppercase(fields[i]));
        }
    }
    return true;
}

// Read courses into a temporary map. Replace the existing data only if
// the file opens successfully and contains at least one valid course.
bool loadCourses(const string& filename, map<string, Course>& courses) {
    ifstream file(filename.c_str());
    if (!file.is_open()) {
        cout << "Error: Could not open file '" << filename << "'." << endl;
        cout << "Please check the file name and location." << endl;
        return false;
    }

    map<string, Course> loadedCourses;
    string line;
    int lineNumber = 0;

    while (getline(file, line)) {
        ++lineNumber;
        if (trim(line).empty()) {
            continue;
        }

        Course course;
        if (!parseCourse(line, course)) {
            cout << "Warning: Skipping invalid data on line " << lineNumber << "." << endl;
            continue;
        }

        if (loadedCourses.count(course.number) > 0) {
            cout << "Warning: Duplicate course " << course.number
                 << " on line " << lineNumber << ". Replacing earlier entry." << endl;
        }
        loadedCourses[course.number] = course;
    }

    if (loadedCourses.empty()) {
        cout << "Error: No valid course information was found." << endl;
        return false;
    }

    // The map keeps course numbers sorted automatically.
    courses.swap(loadedCourses);
    cout << "Successfully loaded " << courses.size() << " courses." << endl;
    return true;
}

// Print all courses in ascending alphanumeric order, including math courses.
void printCourseList(const map<string, Course>& courses) {
    if (courses.empty()) {
        cout << "Please load the course data first using option 1." << endl;
        return;
    }

    cout << "Here is a sample schedule:" << endl << endl;
    for (map<string, Course>::const_iterator it = courses.begin();
         it != courses.end(); ++it) {
        cout << it->second.number << ", " << it->second.title << endl;
    }
}

// Show a course's title and its prerequisite course numbers and titles.
void printCourse(const map<string, Course>& courses) {
    if (courses.empty()) {
        cout << "Please load the course data first using option 1." << endl;
        return;
    }

    cout << "What course do you want to know about? ";
    string courseNumber;
    if (!getline(cin, courseNumber)) {
        return;
    }
    courseNumber = uppercase(trim(courseNumber));

    map<string, Course>::const_iterator found = courses.find(courseNumber);
    if (found == courses.end()) {
        cout << "Error: Course '" << courseNumber << "' was not found." << endl;
        return;
    }

    const Course& course = found->second;
    cout << course.number << ", " << course.title << endl;

    if (course.prerequisites.empty()) {
        cout << "Prerequisites: None" << endl;
        return;
    }

    cout << "Prerequisites: ";
    for (size_t i = 0; i < course.prerequisites.size(); ++i) {
        if (i > 0) {
            cout << ", ";
        }
        cout << course.prerequisites[i];
    }
    cout << endl;

    cout << "Prerequisite course titles:" << endl;
    for (size_t i = 0; i < course.prerequisites.size(); ++i) {
        const string& prerequisiteNumber = course.prerequisites[i];
        map<string, Course>::const_iterator prerequisite = courses.find(prerequisiteNumber);
        if (prerequisite != courses.end()) {
            cout << "  " << prerequisiteNumber << ", " << prerequisite->second.title << endl;
        } else {
            cout << "  " << prerequisiteNumber << ", title not available in loaded data" << endl;
        }
    }
}

// Keep showing the menu until the user selects option 9.
int main() {
    map<string, Course> courses;
    string choice;

    cout << "Welcome to the course planner." << endl;

    while (true) {
        cout << "\n    1. Load Data Structure." << endl;
        cout << "    2. Print Course List." << endl;
        cout << "    3. Print Course." << endl;
        cout << "    9. Exit" << endl << endl;
        cout << "What would you like to do? ";

        if (!getline(cin, choice)) {
            cout << "\nInput closed. Exiting the course planner." << endl;
            break;
        }
        choice = trim(choice);

        if (choice == "1") {
            cout << "Enter the course data file name: ";
            string filename;
            if (!getline(cin, filename)) {
                cout << "\nInput closed. Exiting the course planner." << endl;
                break;
            }
            filename = trim(filename);
            if (filename.empty()) {
                cout << "Error: File name cannot be empty." << endl;
            } else {
                loadCourses(filename, courses);
            }
        } else if (choice == "2") {
            printCourseList(courses);
        } else if (choice == "3") {
            printCourse(courses);
        } else if (choice == "9") {
            cout << "Thank you for using the course planner!" << endl;
            break;
        } else {
            cout << choice << " is not a valid option." << endl;
        }
    }

    return 0;
}
