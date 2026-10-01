/*
EECS 348 Assignment 3
Description of the program: given commands, category of email sender, and a date, and organize them into the highest priority emails
Inputs: File
Output: 2 emails that are at the highest priority
All collaborators: Mikayla Pelletier
Other sources for the code: ChatGPT, Gemini
Author: Savena Lin
Creation date: October 1st 2026
Revision date: October 1st 2026
Revisions: made the classes private, no longer allow incorrect dates and date formats, getMax() is now a constant reference
*/


#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>


// ------------------------------------------------------------
// Email class
// ------------------------------------------------------------

// CHANGED: Changed Email from a struct to a class.
// The data members are now private to improve encapsulation.
class Email {
private:

    std::string senderCategory;
    std::string subject;
    std::string dateStr;

    // Converted fields for priority comparison
    int categoryPriority = 1;
    int year = 0;
    int month = 0;
    int day = 0;


public:

    // CHANGED: Added a default constructor.
    Email() = default;


    // CHANGED: Added a constructor to create an Email object.
    Email(const std::string& sender,
          const std::string& emailSubject,
          const std::string& date) {

        senderCategory = sender;
        subject = emailSubject;
        dateStr = date;

        categoryPriority = getCategoryPriority(senderCategory);
    }


    // --------------------------------------------------------
    // Getter functions
    // --------------------------------------------------------

    // CHANGED: Added getter functions because the data members
    // are now private.

    const std::string& getSenderCategory() const {
        return senderCategory;
    }

    const std::string& getSubject() const {
        return subject;
    }

    const std::string& getDate() const {
        return dateStr;
    }


    // CHANGED: Added getter functions for the priority/date
    // fields in case they are needed elsewhere.

    int getCategoryPriority() const {
        return categoryPriority;
    }

    int getYear() const {
        return year;
    }

    int getMonth() const {
        return month;
    }

    int getDay() const {
        return day;
    }


    // --------------------------------------------------------
    // Sender category priority
    // --------------------------------------------------------

    // Determine category priority weight.
    // Higher number = Higher priority.

    // CHANGED: Replaced the original series of if statements
    // with a map. This makes priorities easier to modify.
    static int getCategoryPriority(const std::string& cat) {

        static const std::map<std::string, int> priorities = {
            {"Boss", 5},
            {"Subordinate", 4},
            {"Peer", 3},
            {"ImportantPerson", 2},
            {"OtherPerson", 1}
        };

        auto it = priorities.find(cat);

        if (it != priorities.end()) {
            return it->second;
        }

        // Unknown categories are treated as OtherPerson.
        return 1;
    }


    // --------------------------------------------------------
    // Date validation
    // --------------------------------------------------------

    // CHANGED: Added function to determine whether a year
    // is a leap year.
    static bool isLeapYear(int y) {

        return (y % 400 == 0) ||
               (y % 4 == 0 && y % 100 != 0);
    }


    // CHANGED: Added function to determine how many days
    // are in a particular month.
    static int daysInMonth(int m, int y) {

        switch (m) {

            case 2:
                return isLeapYear(y) ? 29 : 28;

            case 4:
            case 6:
            case 9:
            case 11:
                return 30;

            case 1:
            case 3:
            case 5:
            case 7:
            case 8:
            case 10:
            case 12:
                return 31;

            default:
                return 0;
        }
    }


    // CHANGED: parseDate now returns bool instead of void.
    // This allows the program to determine whether the date
    // is valid.
    bool parseDate(const std::string& d) {

        std::stringstream ss(d);

        char dash1, dash2;

        // Check whether the date can be successfully parsed.
        if (!(ss >> month >> dash1 >> day >> dash2 >> year)) {
            return false;
        }

        // Make sure the separators are '-'.
        if (dash1 != '-' || dash2 != '-') {
            return false;
        }

        // Check that the year is valid.
        if (year < 1) {
            return false;
        }

        // Check that the month is valid.
        if (month < 1 || month > 12) {
            return false;
        }

        // Check that the day is valid for the month.
        if (day < 1 || day > daysInMonth(month, year)) {
            return false;
        }

        dateStr = d;

        return true;
    }


    // --------------------------------------------------------
    // Email comparison
    // --------------------------------------------------------

    // Custom comparison for MaxHeap.
    // operator> defines which email has higher priority.

    bool operator>(const Email& other) const {

        // Primary priority: Sender Category

        if (categoryPriority != other.categoryPriority) {
            return categoryPriority > other.categoryPriority;
        }

        // Secondary priority: Newest Email First
        // Year -> Month -> Day

        if (year != other.year) {
            return year > other.year;
        }

        if (month != other.month) {
            return month > other.month;
        }

        return day > other.day;
    }


    // CHANGED: Removed the original operator<().
    // It was not used anywhere in the program.
};


// ------------------------------------------------------------
// MaxHeap class
// ------------------------------------------------------------

class MaxHeap {

private:

    std::vector<Email> heap;


    // Move an element upward in the heap.
    void heapifyUp(int index) {

        while (index > 0) {

            int parentIndex = (index - 1) / 2;

            if (heap[index] > heap[parentIndex]) {

                std::swap(heap[index], heap[parentIndex]);

                index = parentIndex;

            } else {
                break;
            }
        }
    }


    // Move an element downward in the heap.
    void heapifyDown(int index) {

        int size = static_cast<int>(heap.size());

        while (true) {

            int maxIndex = index;

            int leftChild = 2 * index + 1;
            int rightChild = 2 * index + 2;


            if (leftChild < size &&
                heap[leftChild] > heap[maxIndex]) {

                maxIndex = leftChild;
            }


            if (rightChild < size &&
                heap[rightChild] > heap[maxIndex]) {

                maxIndex = rightChild;
            }


            if (maxIndex != index) {

                std::swap(heap[index], heap[maxIndex]);

                index = maxIndex;

            } else {
                break;
            }
        }
    }


public:

    // --------------------------------------------------------
    // Insert
    // --------------------------------------------------------

    void insert(const Email& email) {

        heap.push_back(email);

        heapifyUp(static_cast<int>(heap.size()) - 1);
    }


    // --------------------------------------------------------
    // Remove maximum
    // --------------------------------------------------------

    void removeMax() {

        if (heap.empty()) {
            return;
        }

        // Move the last element to the root.
        heap[0] = heap.back();

        heap.pop_back();


        // CHANGED: Only call heapifyDown if elements remain.
        if (!heap.empty()) {
            heapifyDown(0);
        }
    }


    // --------------------------------------------------------
    // Get maximum
    // --------------------------------------------------------

    // CHANGED: Return a const reference instead of making
    // a copy of the Email object.
    const Email& getMax() const {

        return heap[0];
    }


    // --------------------------------------------------------
    // Check if empty
    // --------------------------------------------------------

    bool isEmpty() const {

        return heap.empty();
    }


    // --------------------------------------------------------
    // Get size
    // --------------------------------------------------------

    int size() const {

        // CHANGED: Explicitly convert size_t to int.
        return static_cast<int>(heap.size());
    }
};


// ------------------------------------------------------------
// Trim helper function
// ------------------------------------------------------------

// Removes extra spaces and tabs from the beginning and end
// of a string.
std::string trim(const std::string& str) {

    size_t start = str.find_first_not_of(" \t");

    if (start == std::string::npos) {
        return "";
    }

    size_t end = str.find_last_not_of(" \t");

    return str.substr(start, end - start + 1);
}


// ------------------------------------------------------------
// Main
// ------------------------------------------------------------

int main() {

    MaxHeap emailQueue;

    std::string line;


    // Read commands from standard input.
    while (std::getline(std::cin, line)) {

        // Ignore empty lines.
        if (line.empty()) {
            continue;
        }


        // ====================================================
        // EMAIL command
        // ====================================================

        if (line.rfind("EMAIL ", 0) == 0) {

            // Command format:
            // EMAIL <sender category>, <subject line>, <date>

            std::string data = line.substr(6);

            std::stringstream ss(data);

            std::string category;
            std::string subject;
            std::string dateStr;


            // CHANGED: Check that all three pieces of the
            // EMAIL command can be extracted.
            if (!std::getline(ss, category, ',') ||
                !std::getline(ss, subject, ',') ||
                !std::getline(ss, dateStr)) {

                std::cerr << "Invalid EMAIL command. "
                          << "Skipping line."
                          << std::endl;

                continue;
            }


            // CHANGED: Trim all input fields before using them.
            category = trim(category);
            subject = trim(subject);
            dateStr = trim(dateStr);


            // CHANGED: Create the Email using the constructor
            // because the Email data members are private.
            Email email(
                category,
                subject,
                dateStr
            );


            // CHANGED: Validate the date before adding the
            // email to the heap.
            if (!email.parseDate(dateStr)) {

                std::cerr << "Invalid date. "
                          << "Skipping email."
                          << std::endl;

                continue;
            }


            // Add the email to the priority queue.
            emailQueue.insert(email);
        }


        // ====================================================
        // COUNT command
        // ====================================================

        else if (line == "COUNT") {

            std::cout << "There are "
                      << emailQueue.size()
                      << " emails to read."
                      << std::endl;

            std::cout << std::endl;
        }


        // ====================================================
        // NEXT command
        // ====================================================

        else if (line == "NEXT") {

            if (!emailQueue.isEmpty()) {

                // CHANGED: Use a const reference instead of
                // copying the entire Email object.
                const Email& top = emailQueue.getMax();


                std::cout << "Next email:" << std::endl;

                std::cout << "\tSender: "
                          << top.getSenderCategory()
                          << std::endl;

                std::cout << "\tSubject: "
                          << top.getSubject()
                          << std::endl;

                std::cout << "\tDate: "
                          << top.getDate()
                          << std::endl;

                std::cout << std::endl;
            }
        }


        // ====================================================
        // READ command
        // ====================================================

        else if (line == "READ") {

            if (!emailQueue.isEmpty()) {

                emailQueue.removeMax();
            }
        }
    }


    return 0;
}