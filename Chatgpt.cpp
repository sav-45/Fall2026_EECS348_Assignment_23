#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

// ------------------------------------------------------------
// Email class
// ------------------------------------------------------------
class Email {
private:
    string sender;
    string subject;
    string date;

public:
    Email() {}

    Email(string sender, string subject, string date) {
        this->sender = sender;
        this->subject = subject;
        this->date = date;
    }

    string getSender() const {
        return sender;
    }

    string getSubject() const {
        return subject;
    }

    string getDate() const {
        return date;
    }
};

// ------------------------------------------------------------
// MaxHeap class
// ------------------------------------------------------------
class MaxHeap {
private:
    vector<Email> heap;

    // Return the priority number for a sender category.
    // Larger number = higher priority.
    int senderPriority(const string& sender) const {
        if (sender == "Boss")
            return 5;
        else if (sender == "Subordinate")
            return 4;
        else if (sender == "Peer")
            return 3;
        else if (sender == "ImportantPerson")
            return 2;
        else
            return 1;   // OtherPerson
    }

    // Convert MM-DD-YYYY into YYYYMMDD.
    // This makes dates easy to compare numerically.
    int dateValue(const string& date) const {
        int month = stoi(date.substr(0, 2));
        int day = stoi(date.substr(3, 2));
        int year = stoi(date.substr(6, 4));

        return year * 10000 + month * 100 + day;
    }

    // Returns true if email1 has higher priority than email2.
    bool higherPriority(const Email& email1, const Email& email2) const {
        int priority1 = senderPriority(email1.getSender());
        int priority2 = senderPriority(email2.getSender());

        // First compare sender category.
        if (priority1 != priority2) {
            return priority1 > priority2;
        }

        // If sender categories are the same,
        // the newest email has higher priority.
        int date1 = dateValue(email1.getDate());
        int date2 = dateValue(email2.getDate());

        return date1 > date2;
    }

    // Move an element upward in the heap.
    void heapifyUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;

            if (higherPriority(heap[index], heap[parent])) {
                swap(heap[index], heap[parent]);
                index = parent;
            }
            else {
                break;
            }
        }
    }

    // Move an element downward in the heap.
    void heapifyDown(int index) {
        int size = heap.size();

        while (true) {
            int leftChild = 2 * index + 1;
            int rightChild = 2 * index + 2;
            int largest = index;

            if (leftChild < size &&
                higherPriority(heap[leftChild], heap[largest])) {
                largest = leftChild;
            }

            if (rightChild < size &&
                higherPriority(heap[rightChild], heap[largest])) {
                largest = rightChild;
            }

            if (largest != index) {
                swap(heap[index], heap[largest]);
                index = largest;
            }
            else {
                break;
            }
        }
    }

public:

    // Add an email to the MaxHeap.
    void insert(const Email& email) {
        heap.push_back(email);

        // Restore heap property.
        heapifyUp(heap.size() - 1);
    }

    // Return the highest-priority email without removing it.
    Email getMax() const {
        return heap[0];
    }

    // Remove the highest-priority email.
    void removeMax() {
        if (heap.empty()) {
            return;
        }

        // Move last element to root.
        heap[0] = heap.back();
        heap.pop_back();

        // Restore heap property.
        if (!heap.empty()) {
            heapifyDown(0);
        }
    }

    // Check whether heap is empty.
    bool isEmpty() const {
        return heap.empty();
    }

    // Return number of emails.
    int size() const {
        return heap.size();
    }
};

// ------------------------------------------------------------
// Print the highest-priority email
// ------------------------------------------------------------
void printNextEmail(const MaxHeap& emailQueue) {
    if (emailQueue.isEmpty()) {
        return;
    }

    Email email = emailQueue.getMax();

    cout << "Next email:" << endl;
    cout << "\tSender: " << email.getSender() << endl;
    cout << "\tSubject: " << email.getSubject() << endl;
    cout << "\tDate: " << email.getDate() << endl;
    cout << endl;
}

// ------------------------------------------------------------
// Main
// ------------------------------------------------------------
int main(int argc, char* argv[]) {

    // The test file is supplied as a command-line argument.
    if (argc < 2) {
        cerr << "Error: Please provide a test file." << endl;
        return 1;
    }

    ifstream inputFile(argv[1]);

    if (!inputFile) {
        cerr << "Error: Could not open file." << endl;
        return 1;
    }

    MaxHeap emailQueue;

    string line;

    while (getline(inputFile, line)) {

        // Skip empty lines.
        if (line.empty()) {
            continue;
        }

        // ----------------------------------------------------
        // EMAIL command
        // ----------------------------------------------------
        if (line.substr(0, 5) == "EMAIL") {

            // Remove "EMAIL " from the beginning.
            string emailData = line.substr(6);

            // Find the commas separating the fields.
            size_t firstComma = emailData.find(',');
            size_t secondComma = emailData.find(',', firstComma + 1);

            string sender =
                emailData.substr(0, firstComma);

            string subject =
                emailData.substr(
                    firstComma + 1,
                    secondComma - firstComma - 1
                );

            string date =
                emailData.substr(secondComma + 1);

            Email newEmail(sender, subject, date);

            emailQueue.insert(newEmail);
        }

        // ----------------------------------------------------
        // NEXT command
        // ----------------------------------------------------
        else if (line == "NEXT") {

            // NEXT does NOT remove the email.
            // Therefore another NEXT displays the same email.
            if (!emailQueue.isEmpty()) {
                printNextEmail(emailQueue);
            }
        }

        // ----------------------------------------------------
        // READ command
        // ----------------------------------------------------
        else if (line == "READ") {

            // READ removes the highest-priority email.
            // It does not display anything.
            if (!emailQueue.isEmpty()) {
                emailQueue.removeMax();
            }
        }

        // ----------------------------------------------------
        // COUNT command
        // ----------------------------------------------------
        else if (line == "COUNT") {

            cout << "There are "
                 << emailQueue.size()
                 << " emails to read.\n"
                 << endl;
        }
    }

    inputFile.close();

    return 0;
}