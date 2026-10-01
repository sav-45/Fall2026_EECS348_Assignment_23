#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>

// Represents an Email record
struct Email {
    std::string senderCategory;
    std::string subject;
    std::string dateStr; // Original MM-DD-YYYY
    
    // Converted fields for priority comparison
    int categoryPriority; 
    int year;
    int month;
    int day;

    // Determine category priority weight (Higher number = Higher priority)
    static int getCategoryPriority(const std::string& cat) {
        if (cat == "Boss") return 5;
        if (cat == "Subordinate") return 4;
        if (cat == "Peer") return 3;
        if (cat == "ImportantPerson") return 2;
        return 1; // OtherPerson
    }

    // Parse date string MM-DD-YYYY into integers
    void parseDate(const std::string& d) {
        dateStr = d;
        std::stringstream ss(d);
        char dash1, dash2;
        ss >> month >> dash1 >> day >> dash2 >> year;
    }

    // Custom comparison for MaxHeap (operator> defines higher priority)
    bool operator>(const Email& other) const {
        // Primary priority: Sender Category
        if (categoryPriority != other.categoryPriority) {
            return categoryPriority > other.categoryPriority;
        }

        // Secondary priority: Newest Email First (Year -> Month -> Day)
        if (year != other.year) {
            return year > other.year;
        }
        if (month != other.month) {
            return month > other.month;
        }
        return day > other.day;
    }

    bool operator<(const Email& other) const {
        return !(*this > other || (categoryPriority == other.categoryPriority && 
                                   year == other.year && month == other.month && day == other.day));
    }
};

// Custom dynamic array/list implementation of MaxHeap built from scratch
class MaxHeap {
private:
    std::vector<Email> heap;

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

    void heapifyDown(int index) {
        int size = heap.size();
        while (true) {
            int maxIndex = index;
            int leftChild = 2 * index + 1;
            int rightChild = 2 * index + 2;

            if (leftChild < size && heap[leftChild] > heap[maxIndex]) {
                maxIndex = leftChild;
            }
            if (rightChild < size && heap[rightChild] > heap[maxIndex]) {
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
    void insert(const Email& email) {
        heap.push_back(email);
        heapifyUp(heap.size() - 1);
    }

    void removeMax() {
        if (heap.empty()) return;
        heap[0] = heap.back();
        heap.pop_back();
        heapifyDown(0);
    }

    Email getMax() const {
        if (!heap.empty()) {
            return heap[0];
        }
        return Email();
    }

    bool isEmpty() const {
        return heap.empty();
    }

    int size() const {
        return heap.size();
    }
};

// Trim helper functions to clean up space padding from split strings
std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t");
    return str.substr(start, end - start + 1);
}

int main() {
    MaxHeap emailQueue;
    std::string line;

    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;

        if (line.rfind("EMAIL ", 0) == 0) {
            // Command format: EMAIL <sender category>, <subject line>, <date>
            std::string data = line.substr(6);
            std::stringstream ss(data);
            
            std::string category, subject, dateStr;
            std::getline(ss, category, ',');
            std::getline(ss, subject, ',');
            std::getline(ss, dateStr, ',');

            Email email;
            email.senderCategory = trim(category);
            email.subject = trim(subject);
            email.categoryPriority = Email::getCategoryPriority(email.senderCategory);
            email.parseDate(trim(dateStr));

            emailQueue.insert(email);
        } 
        else if (line == "COUNT") {
            std::cout << "There are " << emailQueue.size() << " emails to read." << std::endl;
            std::cout << std::endl;
        } 
        else if (line == "NEXT") {
            if (!emailQueue.isEmpty()) {
                Email top = emailQueue.getMax();
                std::cout << "Next email:" << std::endl;
                std::cout << "\tSender: " << top.senderCategory << std::endl;
                std::cout << "\tSubject: " << top.subject << std::endl;
                std::cout << "\tDate: " << top.dateStr << std::endl;
                std::cout << std::endl;
            }
        } 
        else if (line == "READ") {
            if (!emailQueue.isEmpty()) {
                emailQueue.removeMax();
            }
        }
    }

    return 0;
}