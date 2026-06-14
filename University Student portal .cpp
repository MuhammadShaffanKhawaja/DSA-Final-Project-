#include <iostream>
#include <string>
#include <sstream> 
#include <cstdlib> 

using namespace std;

// ==========================================
// 0. COMPILER BACKWARD-COMPATIBILITY HELPERS
// ==========================================

template <typename T>
string to_string_fallback(T value) {
    ostringstream oss;
    oss << value;
    return oss.str();
}

// ==========================================
// 1. DATA MODELS & STRUCTURES
// ==========================================

struct Course {
    string courseCode;
    string courseName;
    Course* next;

    Course(string code, string name) {
        courseCode = code;
        courseName = name;
        next = NULL;
    }
};

struct Student {
    int studentId;
    string name;
    double gpa;
    Course* registeredCourses; 

    Student(int id, string n, double g) {
        studentId = id;
        name = n;
        gpa = g;
        registeredCourses = NULL;
    }
};

// Node structure for the Auto-Balancing Student AVL Tree
struct AVLNode {
    Student* student;
    AVLNode* left;
    AVLNode* right;
    int height;

    AVLNode(Student* s) {
        student = s;
        left = NULL;
        right = NULL;
        height = 1;
    }
};

// General Linked List Node structure for Queue and Stack implementations
struct Node {
    string data;
    Node* next;
    Node(string val) {
        data = val;
        next = NULL;
    }
};

// ==========================================
// 2. CUSTOM DATA STRUCTURE IMPLEMENTATIONS
// ==========================================

// Custom Stack for Undo Functional Requirements
class CustomStack {
private:
    Node* topNode;
public:
    CustomStack() { topNode = NULL; }

    void push(string action) {
        Node* newNode = new Node(action);
        newNode->next = topNode;
        topNode = newNode;
    }

    string pop() {
        if (isEmpty()) return "";
        Node* temp = topNode;
        string action = temp->data;
        topNode = topNode->next;
        delete temp;
        return action;
    }

    bool isEmpty() { return topNode == NULL; }
};

// Custom Queue for Student Course Registration Pipelines
class CustomQueue {
private:
    Node* front;
    Node* rear;
public:
    CustomQueue() { front = rear = NULL; }

    void enqueue(string actionDetails) {
        Node* newNode = new Node(actionDetails);
        if (rear == NULL) {
            front = rear = newNode;
            return;
        }
        rear->next = newNode;
        rear = newNode;
    }

    string dequeue() {
        if (front == NULL) return "";
        Node* temp = front;
        string details = temp->data;
        front = front->next;
        if (front == NULL) rear = NULL;
        delete temp;
        return details;
    }

    bool isEmpty() { return front == NULL; }
};

// Custom Max-Heap Structure for Live GPA Ranking Analytics
class StudentMaxHeap {
private:
    Student** heapArray;
    int capacity;
    int heapSize;

    void heapifyUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;
            if (heapArray[index]->gpa > heapArray[parent]->gpa) {
                swap(heapArray[index], heapArray[parent]);
                index = parent;
            } else {
                break;
            }
        }
    }

    void heapifyDown(int index) {
        int highest = index;
        int left = 2 * index + 1;
        int right = 2 * index + 2;

        if (left < heapSize && heapArray[left]->gpa > heapArray[highest]->gpa)
            highest = left;
        if (right < heapSize && heapArray[right]->gpa > heapArray[highest]->gpa)
            highest = right;

        if (highest != index) {
            swap(heapArray[index], heapArray[highest]);
            heapifyDown(highest);
        }
    }

public:
    StudentMaxHeap(int maxCap) {
        capacity = maxCap;
        heapSize = 0;
        heapArray = new Student*[capacity];
    }

    ~StudentMaxHeap() {
        delete[] heapArray;
    }

    void insert(Student* s) {
        if (heapSize == capacity) return;
        heapArray[heapSize] = s;
        heapifyUp(heapSize);
        heapSize++;
    }

    Student* extractMax() {
        if (heapSize <= 0) return NULL;
        if (heapSize == 1) {
            heapSize--;
            return heapArray[0];
        }
        Student* rootNode = heapArray[0];
        heapArray[0] = heapArray[heapSize - 1];
        heapSize--;
        heapifyDown(0);
        return rootNode;
    }

    bool isEmpty() { return heapSize == 0; }
};

// ==========================================
// 3. CORE MANAGEMENT PORTAL CLASS
// ==========================================

class UniversityPortal {
private:
    AVLNode* root;
    Course* courseHead; 
    CustomStack undoStack;
    CustomQueue registrationQueue;

    // --- AVL Tree Logic Balance Helper Mechanisms ---
    int getHeight(AVLNode* n) { return n ? n->height : 0; }

    int getBalanceFactor(AVLNode* n) { return n ? getHeight(n->left) - getHeight(n->right) : 0; }

    void updateHeight(AVLNode* n) {
        if (n) n->height = 1 + max(getHeight(n->left), getHeight(n->right));
    }

    AVLNode* rotateRight(AVLNode* y) {
        AVLNode* x = y->left;
        AVLNode* T2 = x->right;
        x->right = y;
        y->left = T2;
        updateHeight(y);
        updateHeight(x);
        return x;
    }

    AVLNode* rotateLeft(AVLNode* x) {
        AVLNode* y = x->right;
        AVLNode* T2 = y->left;
        y->left = x;
        x->right = T2;
        updateHeight(x);
        updateHeight(y);
        return y;
    }

    // --- Internal AVL Recursive Operations ---
    AVLNode* insertAVL(AVLNode* node, Student* s) {
        if (node == NULL) return new AVLNode(s);

        if (s->studentId < node->student->studentId)
            node->left = insertAVL(node->left, s);
        else if (s->studentId > node->student->studentId)
            node->right = insertAVL(node->right, s);
        else
            return node;

        updateHeight(node);
        int balance = getBalanceFactor(node);

        // Left Left Case
        if (balance > 1 && s->studentId < node->left->student->studentId)
            return rotateRight(node);
        // Right Right Case
        if (balance < -1 && s->studentId > node->right->student->studentId)
            return rotateLeft(node);
        // Left Right Case
        if (balance > 1 && s->studentId > node->left->student->studentId) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
        // Right Left Case
        if (balance < -1 && s->studentId < node->right->student->studentId) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        return node;
    }

    AVLNode* findMinValueNode(AVLNode* node) {
        AVLNode* current = node;
        while (current && current->left != NULL) current = current->left;
        return current;
    }

    AVLNode* deleteAVL(AVLNode* node, int id) {
        if (node == NULL) return node;

        if (id < node->student->studentId)
            node->left = deleteAVL(node->left, id);
        else if (id > node->student->studentId)
            node->right = deleteAVL(node->right, id);
        else {
            if ((node->left == NULL) || (node->right == NULL)) {
                AVLNode* temp = node->left ? node->left : node->right;
                if (temp == NULL) {
                    temp = node;
                    node = NULL;
                } else {
                    *node = *temp; 
                }
                delete temp->student;
                delete temp;
            } else {
                AVLNode* temp = findMinValueNode(node->right);
                node->student->studentId = temp->student->studentId;
                node->student->name = temp->student->name;
                node->student->gpa = temp->student->gpa;
                node->student->registeredCourses = temp->student->registeredCourses;
                node->right = deleteAVL(node->right, temp->student->studentId);
            }
        }

        if (node == NULL) return node;

        updateHeight(node);
        int balance = getBalanceFactor(node);

        // Left Left Case
        if (balance > 1 && getBalanceFactor(node->left) >= 0)
            return rotateRight(node);
        // Left Right Case
        if (balance > 1 && getBalanceFactor(node->left) < 0) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
        // Right Right Case
        if (balance < -1 && getBalanceFactor(node->right) <= 0)
            return rotateLeft(node);
        // Right Left Case
        if (balance < -1 && getBalanceFactor(node->right) > 0) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        return node;
    }

    AVLNode* searchAVL(AVLNode* node, int id) {
        if (node == NULL || node->student->studentId == id) return node;
        if (id < node->student->studentId) return searchAVL(node->left, id);
        return searchAVL(node->right, id);
    }

    void inorderTraversal(AVLNode* node) {
        if (node == NULL) return;
        inorderTraversal(node->left);
        cout << "ID: " << node->student->studentId 
             << " | Name: " << node->student->name 
             << " | GPA: " << node->student->gpa << "\n";
        inorderTraversal(node->right);
    }

    // Dynamic Heap construction traversal accumulation functions
    void populateHeap(AVLNode* node, StudentMaxHeap& heap) {
        if (node == NULL) return;
        populateHeap(node->left, heap);
        heap.insert(node->student);
        populateHeap(node->right, heap);
    }

    void collectStudentsArray(AVLNode* node, Student** arr, int& index) {
        if (node == NULL) return;
        collectStudentsArray(node->left, arr, index);
        arr[index++] = node->student;
        collectStudentsArray(node->right, arr, index);
    }

    int countTotalStudents(AVLNode* node) {
        if (node == NULL) return 0;
        return 1 + countTotalStudents(node->left) + countTotalStudents(node->right);
    }

public:
    UniversityPortal() {
        root = NULL;
        courseHead = NULL;
    }
// --- Student Management Functions ---
    void addStudent(int id, string name, double gpa, bool trackUndo = true) {
        if (searchAVL(root, id) != NULL) {
            cout << "\n[!] Error: Student with ID " << id << " already exists!\n";
            return;
        }
        Student* s = new Student(id, name, gpa);
        root = insertAVL(root, s);
        if (trackUndo) {
            undoStack.push("ADD:" + to_string_fallback(id));
        }
        cout << "\n[v] Student added successfully.\n";
    }

    void deleteStudent(int id, bool trackUndo = true) {
        AVLNode* target = searchAVL(root, id);
        if (target == NULL) {
            cout << "\n[!] Error: Student record not found.\n";
            return;
        }
        if (trackUndo) {
            undoStack.push("DEL:" + to_string_fallback(id) + "," + target->student->name + "," + to_string_fallback(target->student->gpa));
        }
        root = deleteAVL(root, id);
        cout << "\n[x] Student record deleted successfully.\n";
    }

    void updateStudent(int id, string newName, double newGpa) {
        AVLNode* target = searchAVL(root, id);
        if (target != NULL) {
            target->student->name = newName;
            target->student->gpa = newGpa;
            cout << "\n[v] Student records updated successfully.\n";
        } else {
            cout << "\n[!] Error: Student ID not found.\n";
        }
    }

    void searchStudent(int id) {
        AVLNode* target = searchAVL(root, id);
        if (target != NULL) {
            cout << "\n===================================\n";
            cout << "          STUDENT PROFILE          \n";
            cout << "===================================\n";
            cout << " ID                : " << target->student->studentId << "\n";
            cout << " Name              : " << target->student->name << "\n";
            cout << " CGPA              : " << target->student->gpa << "\n";
            cout << " Registered Courses: ";
            Course* curr = target->student->registeredCourses;
            if (!curr) cout << "None";
            while (curr) {
                cout << "[" << curr->courseCode << " - " << curr->courseName << "] ";
                curr = curr->next;
            }
            cout << "\n===================================\n";
        } else {
            cout << "\n[!] Error: No student found with that ID.\n";
        }
    }

    // --- Course Management Functions ---
    void addCourse(string code, string name) {
        Course* newCourse = new Course(code, name);
        newCourse->next = courseHead;
        courseHead = newCourse;
        cout << "\n[v] Course added to the catalog successfully.\n";
    }

    void deleteCourse(string code) {
        Course* curr = courseHead;
        Course* prev = NULL;
        while (curr != NULL && curr->courseCode != code) {
            prev = curr;
            curr = curr->next;
        }
        if (curr == NULL) {
            cout << "\n[!] Error: Course code not found in the catalog.\n";
            return;
        }
        if (prev == NULL) courseHead = curr->next;
        else prev->next = curr->next;
        delete curr;
        cout << "\n[x] Course removed from the catalog successfully.\n";
    }

    void displayCourses() {
        cout << "\n===================================\n";
        cout << "       MASTER COURSE CATALOG       \n";
        cout << "===================================\n";
        Course* curr = courseHead;
        if (!curr) cout << "  The course catalog is empty.\n";
        while (curr) {
            cout << "  Code: " << curr->courseCode << " | Title: " << curr->courseName << "\n";
            curr = curr->next;
        }
        cout << "===================================\n";
    }

    // --- Registration Management System ---
    void queueCourseRegistration(int studentId, string courseCode) {
        AVLNode* sTarget = searchAVL(root, studentId);
        if (!sTarget) {
            cout << "\n[!] Error: Invalid Student ID.\n";
            return;
        }
        Course* cCurr = courseHead;
        bool trackingFound = false;
        while (cCurr) {
            if (cCurr->courseCode == courseCode) {
                trackingFound = true;
                break;
            }
            cCurr = cCurr->next;
        }
        if (!trackingFound) {
            cout << "\n[!] Error: Course code does not exist.\n";
            return;
        }
        registrationQueue.enqueue(to_string_fallback(studentId) + "," + courseCode);
        cout << "\n[v] Registration request added to the processing queue.\n";
    }

    void processNextRegistration() {
        if (registrationQueue.isEmpty()) {
            cout << "\n[-] The registration queue is empty. No requests to process.\n";
            return;
        }
        string raw = registrationQueue.dequeue();
        int commaIndex = raw.find(',');
        int sId = atoi(raw.substr(0, commaIndex).c_str());
        string cCode = raw.substr(commaIndex + 1);

        AVLNode* sTarget = searchAVL(root, sId);
        Course* cCurr = courseHead;
        while (cCurr && cCurr->courseCode != cCode) cCurr = cCurr->next;

        if (sTarget && cCurr) {
            Course* enrolled = new Course(cCurr->courseCode, cCurr->courseName);
            enrolled->next = sTarget->student->registeredCourses;
            sTarget->student->registeredCourses = enrolled;
            cout << "\n[v] Successfully registered Student ID " << sId << " into " << cCode << ".\n";
        }
    }

    // --- Max-Heap Live Priority Ranking Engine ---
    void displayHeapGPARanking() {
        int total = countTotalStudents(root);
        if (total == 0) {
            cout << "\n[-] Cannot display rankings. No students found in the system.\n";
            return;
        }
        StudentMaxHeap rankHeap(total);
        populateHeap(root, rankHeap);

        cout << "\n=======================================================\n";
        cout << "             STUDENT RANKINGS BY CGPA (MAX-HEAP)       \n";
        cout << "=======================================================\n";
        int rankingCounter = 1;
        while (!rankHeap.isEmpty()) {
            Student* topVal = rankHeap.extractMax();
            cout << "  Rank " << rankingCounter++ << " -> ID: " << topVal->studentId 
                 << " | Name: " << topVal->name << " | CGPA: " << topVal->gpa << "\n";
        }
        cout << "=======================================================\n";
    }

    // --- Standard Sorting Matrix ---
    void displayAlphabeticalDirectory() {
        int total = countTotalStudents(root);
        if (total == 0) {
            cout << "\n[-] Alphabetical directory is empty. No students found.\n";
            return;
        }
        Student** list = new Student*[total];
        int collectionCounter = 0;
        collectStudentsArray(root, list, collectionCounter);

        // Sorting Execution Block (Stable Bubble Sort Matrix Variant)
        for (int i = 0; i < total - 1; i++) {
            for (int j = 0; j < total - i - 1; j++) {
                if (list[j]->name > list[j + 1]->name) {
                    Student* tempStore = list[j];
                    list[j] = list[j + 1];
                    list[j + 1] = tempStore;
                }
            }
        }

        cout << "\n=======================================================\n";
        cout << "             ALPHABETICAL STUDENT DIRECTORY            \n";
        cout << "=======================================================\n";
        for (int i = 0; i < total; i++) {
            cout << "  ID: " << list[i]->studentId << " | Name: " << list[i]->name << " | CGPA: " << list[i]->gpa << "\n";
        }
        cout << "=======================================================\n";
        delete[] list;
    }

    // --- Undo Operation (Stack Mechanism Core Requirement) ---
    void performSystemUndo() {
        if (undoStack.isEmpty()) {
            cout << "\n[-] No past actions found to undo.\n";
            return;
        }
        string token = undoStack.pop();
        int divisionPoint = token.find(':');
        string typeCode = token.substr(0, divisionPoint);
        string parameterBlock = token.substr(divisionPoint + 1);

        if (typeCode == "ADD") {
            int targetId = atoi(parameterBlock.c_str());
            root = deleteAVL(root, targetId);
            cout << "\n[x] Undo Action: Successfully removed added Student ID: " << targetId << "\n";
        } 
        else if (typeCode == "DEL") {
            int targetComma1 = parameterBlock.find(',');
            int targetComma2 = parameterBlock.find(',', targetComma1 + 1);
            int prevId = atoi(parameterBlock.substr(0, targetComma1).c_str());
            string prevName = parameterBlock.substr(targetComma1 + 1, targetComma2 - (targetComma1 + 1));
            double prevGpa = atof(parameterBlock.substr(targetComma2 + 1).c_str());

            addStudent(prevId, prevName, prevGpa, false);
            cout << "\n[v] Undo Action: Successfully restored deleted student [" << prevName << "].\n";
        }
    }

    void displayAllRecords() {
        cout << "\n=======================================================\n";
        cout << "         ALL STUDENT RECORDS (BST IN-ORDER VIEW)       \n";
        cout << "=======================================================\n";
        if (!root) cout << "  No student records currently available in the system.\n";
        inorderTraversal(root);
        cout << "=======================================================\n";
    }
};

// ==========================================
// 4. MAIN INTERFACE DECOUPLED OPERATIONS
// ==========================================

void executeAddStudent(UniversityPortal& portal) {
    int id; string name; double gpa;
    cout << "\nEnter New Student ID: "; cin >> id;
    cout << "Enter Student Full Name: "; cin.ignore(); getline(cin, name);
    cout << "Enter Student CGPA: "; cin >> gpa;
    portal.addStudent(id, name, gpa);
}

void executeDeleteStudent(UniversityPortal& portal) {
    int id; 
    cout << "\nEnter Student ID to remove: "; cin >> id;
    portal.deleteStudent(id);
}

void executeUpdateStudent(UniversityPortal& portal) {
    int id; string name; double gpa;
    cout << "\nEnter Student ID to modify: "; cin >> id;
    cout << "Enter Updated Full Name: "; cin.ignore(); getline(cin, name);
    cout << "Enter Updated CGPA: "; cin >> gpa;
    portal.updateStudent(id, name, gpa);
}

void executeSearchStudent(UniversityPortal& portal) {
    int id; 
    cout << "\nEnter Student ID to search: "; cin >> id;
    portal.searchStudent(id);
}

void executeAddCourse(UniversityPortal& portal) {
    string code, name;
    cout << "\nEnter New Course Code (e.g., CS201): "; cin >> code;
    cout << "Enter Course Title Name: "; cin.ignore(); getline(cin, name);
    portal.addCourse(code, name);
}

void executeDeleteCourse(UniversityPortal& portal) {
    string code; 
    cout << "\nEnter Course Code to remove from catalog: "; cin >> code;
    portal.deleteCourse(code);
}

void executeQueueRegistration(UniversityPortal& portal) {
    int id; string code;
    cout << "\nEnter Student ID for registration: "; cin >> id;
    cout << "Enter Course Code to request: "; cin >> code;
    portal.queueCourseRegistration(id, code);
}

int main() {
    UniversityPortal portal;
    int primaryMenuSelection;

    // Seed mock database contents
    portal.addStudent(102, "Muneeza Tahir", 3.91, false);
    portal.addStudent(101, "Muhammad Shaffan", 3.85, false);
    portal.addCourse("CS201", "Data Structures and Algorithms");
    portal.addCourse("EE104", "Digital Logic Design");

do {
        cout << "\n";
        cout << "=======================================================\n";
        cout << "              UNIVERSITY MANAGEMENT PORTAL             \n";
        cout << "=======================================================\n";
        cout << "  1. Add Student Profile Record                        \n";
        cout << "  2. Delete Existing Student Record                    \n";
        cout << "  3. Modify Student Record Details                     \n";
        cout << "  4. Search Student by ID                              \n";
        cout << "  5. Add New Course to Catalog                         \n";
        cout << "  6. Drop Course from Catalog                          \n";
        cout << "  7. View Master Course Catalog                        \n";
        cout << "  8. Queue Course Registration Request                 \n";
        cout << "  9. Process Next Registration Request                 \n";
        cout << " 10. View Student GPA Rankings                         \n";
        cout << " 11. View Alphabetical Student Directory               \n";
        cout << " 12. Rollback Last Action (Undo)                       \n";
        cout << " 13. Print All Student Records (In-Order)              \n";
        cout << " 14. Exit Portal                                       \n";
        cout << "-------------------------------------------------------\n";
        cout << "Select an option (1-14): ";
        cin >> primaryMenuSelection;

        switch (primaryMenuSelection) {
            case 1:  executeAddStudent(portal); break;
            case 2:  executeDeleteStudent(portal); break;
            case 3:  executeUpdateStudent(portal); break;
            case 4:  executeSearchStudent(portal); break;
            case 5:  executeAddCourse(portal); break;
            case 6:  executeDeleteCourse(portal); break;
            case 7:  portal.displayCourses(); break;
            case 8:  executeQueueRegistration(portal); break;
            case 9:  portal.processNextRegistration(); break;
            case 10: portal.displayHeapGPARanking(); break;
            case 11: portal.displayAlphabeticalDirectory(); break;
            case 12: portal.performSystemUndo(); break;
            case 13: portal.displayAllRecords(); break;
            case 14: cout << "\nExiting portal. Goodbye!\n"; break;
            default: cout << "\n[!] Invalid selection. Please choose an option between 1 and 14.\n";
        }
    } while (primaryMenuSelection != 14);

    return 0;
}
