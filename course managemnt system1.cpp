#include <iostream>
#include <unordered_map>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace std;

const string ADMIN_USERNAME = "admin";
const string ADMIN_PASSWORD = "password";
const string DATA_FILE = "courses_data.txt";

const int MAX_PARTICIPANTS = 30;

struct Lecturer {
    string academic_title;
    string firstname;
    string surname;
    string email;
};

struct Student {
    int student_id;
    string firstname;
    string surname;
    string email;
};

struct Course {
    int course_id;
    string course_name;
    Lecturer lecturer;
    unordered_map<int, Student> participants;
};

unordered_map<int, Course> courses;

void printHeader(const string& text) {
    cout << "\n\t\t\t\t\t" << string(50, '=') << endl;
    cout << "\t\t\t\t\t" << text << endl;
    cout << "\t\t\t\t\t" << string(50, '=') << endl;
}

bool adminLogin() {
    string username, password;
    cout << "\t\t\t\t\tEnter username: ";
    cin >> username;
    cout << "\t\t\t\t\tEnter password: ";
    cin >> password;
    return username == ADMIN_USERNAME && password == ADMIN_PASSWORD;
}

void displayCourseHeader() {
    cout << "\t\t\t\t\t" << setw(10) << "Course ID" << " | "
         << setw(20) << "Course Name" << " | "
         << "Lecturer\n";
    cout << "\t\t\t\t\t" << string(50, '-') << "\n";
}

void displayCourseRow(const Course& course) {
    cout << "\t\t\t\t\t" << setw(10) << course.course_id << " | "
         << setw(20) << course.course_name << " | "
         << course.lecturer.academic_title << " " << course.lecturer.firstname
         << " " << course.lecturer.surname << "\n";
}

void displayStudentHeader() {
    cout << "\t\t\t\t\t" << setw(10) << "Student ID" << " | "
         << setw(20) << "Name" << " | "
         << "Email\n";
    cout << "\t\t\t\t\t" << string(50, '-') << "\n";
}

void displayStudentRow(const Student& student) {
    cout << "\t\t\t\t\t" << setw(10) << student.student_id << " | "
         << setw(20) << (student.firstname + " " + student.surname) << " | "
         << student.email << "\n";
}

void addCourse() {
    printHeader("Add New Course");
    int courseID;
    cout << "\t\t\t\t\tEnter a unique Course ID: ";
    cin >> courseID;

    if (courses.find(courseID) != courses.end()) {
        cout << "\t\t\t\t\tCourse ID already exists.\n";
        return;
    }

    Course course;
    course.course_id = courseID;
    cout << "\t\t\t\t\tEnter course name: ";
    cin.ignore();
    getline(cin, course.course_name);
    cout << "\t\t\t\t\tEnter lecturer details:\n\t\t\t\t\tAcademic title: ";
    getline(cin, course.lecturer.academic_title);
    cout << "\t\t\t\t\tFirst name: ";
    getline(cin, course.lecturer.firstname);
    cout << "\t\t\t\t\tLast name: ";
    getline(cin, course.lecturer.surname);
    cout << "\t\t\t\t\tEmail: ";
    getline(cin, course.lecturer.email);

    courses[courseID] = course;
    cout << "\t\t\t\t\tCourse added successfully.\n";
}

void registerStudent() {
    printHeader("Register Student");
    int courseID;
    cout << "\t\t\t\t\tEnter the Course ID: ";
    cin >> courseID;

    auto it = courses.find(courseID);
    if (it == courses.end()) {
        cout << "\t\t\t\t\tInvalid Course ID.\n";
        return;
    }

    Course& course = it->second;
    if (course.participants.size() >= MAX_PARTICIPANTS) {
        cout << "\t\t\t\t\tCourse is fully booked.\n";
        return;
    }

    int studentID;
    cout << "\t\t\t\t\tEnter a unique Student ID: ";
    cin >> studentID;

    if (course.participants.find(studentID) != course.participants.end()) {
        cout << "\t\t\t\t\tStudent ID already registered.\n";
        return;
    }

    Student student;
    student.student_id = studentID;
    cout << "\t\t\t\t\tEnter student's first name: ";
    cin >> student.firstname;
    cout << "\t\t\t\t\tEnter student's last name: ";
    cin >> student.surname;
    cout << "\t\t\t\t\tEnter student's email: ";
    cin >> student.email;

    course.participants[studentID] = student;
    cout << "\t\t\t\t\tStudent registered successfully.\n";
}

void displayCoursesWithParticipants() {
    printHeader("Courses with Participants");
    displayCourseHeader();
    for (const auto& [id, course] : courses) {
        if (!course.participants.empty()) {
            displayCourseRow(course);
            displayStudentHeader();
            for (const auto& [sid, student] : course.participants) {
                displayStudentRow(student);
            }
        }
    }
}

void displayCoursesWithoutParticipants() {
    printHeader("Courses without Participants");
    displayCourseHeader();
    for (const auto& [id, course] : courses) {
        if (course.participants.empty()) {
            displayCourseRow(course);
        }
    }
}

void checkCourseCancellationWarnings() {
    printHeader("Low Enrollment Warnings");
    for (const auto& [id, course] : courses) {
        if (course.participants.size() < 3) {
            cout << "\t\t\t\t\tCourse ID: " << course.course_id << " - " << course.course_name << "\n";
        }
    }
}

void saveData() {
    ofstream outFile(DATA_FILE);
    if (outFile.is_open()) {
        for (const auto& [id, course] : courses) {
            outFile << course.course_id << "," << course.course_name << ","
                    << course.lecturer.academic_title << "," << course.lecturer.firstname << ","
                    << course.lecturer.surname << "," << course.lecturer.email << "\n";

            for (const auto& [sid, student] : course.participants) {
                outFile << student.student_id << "," << student.firstname << ","
                        << student.surname << "," << student.email << "\n";
            }
            outFile << "END\n";
        }
    }
    cout << "\t\t\t\t\tData saved successfully.\n";
}

void loadData() {
    ifstream inFile(DATA_FILE);
    if (!inFile.is_open()) {
        cout << "\t\t\t\t\tNo data file found.\n";
        return;
    }

    string line;
    while (getline(inFile, line)) {
        if (line == "END") continue;

        Course course;
        stringstream ss(line);
        getline(ss, line, ',');
        course.course_id = stoi(line);
        getline(ss, course.course_name, ',');
        getline(ss, course.lecturer.academic_title, ',');
        getline(ss, course.lecturer.firstname, ',');
        getline(ss, course.lecturer.surname, ',');
        getline(ss, course.lecturer.email);

        while (getline(inFile, line) && line != "END") {
            Student student;
            stringstream sss(line);
            getline(sss, line, ',');
            student.student_id = stoi(line);
            getline(sss, student.firstname, ',');
            getline(sss, student.surname, ',');
            getline(sss, student.email);
            course.participants[student.student_id] = student;
        }

        courses[course.course_id] = course;
    }
    cout << "\t\t\t\t\tData loaded successfully.\n";
}

int main() {
    printHeader("Welcome to Course Management System");

    if (!adminLogin()) {
        cout << "\t\t\t\t\tInvalid login credentials.\n";
        return 0;
    }

    loadData();

    int choice;
    do {
        printHeader("Main Menu");
        cout << "\t\t\t\t\t1. Add Course\n"
             << "\t\t\t\t\t2. Register Student\n"
             << "\t\t\t\t\t3. Display Courses with Participants\n"
             << "\t\t\t\t\t4. Display Courses without Participants\n"
             << "\t\t\t\t\t5. Check Cancellation Warnings\n"
             << "\t\t\t\t\t6. Save Data\n"
             << "\t\t\t\t\t7. Exit\n"
             << "\t\t\t\t\tEnter choice: ";
        cin >> choice;

        switch (choice) {
            case 1: addCourse(); break;
            case 2: registerStudent(); break;
            case 3: displayCoursesWithParticipants(); break;
            case 4: displayCoursesWithoutParticipants(); break;
            case 5: checkCourseCancellationWarnings(); break;
            case 6: saveData(); break;
            case 7: cout << "\t\t\t\t\tExiting program. Goodbye!\n"; break;
            default: cout << "\t\t\t\t\tInvalid choice. Please try again.\n";
        }
    } while (choice != 7);

    return 0;
}
