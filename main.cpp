
#include <iostream>
#include <mysql.h>
#include <mysqld_error.h>
#include <windows.h>
#include <sstream>
#include <string>
#include <limits>
#include <cctype>

using namespace std;

const char* HOST = "localhost";
const char* USER = "root";
const char* PW = "Samiran@1433";
const char* DB = "student_management";

MYSQL* conn;

// Function to safely use text inside SQL queries
string sqlEscape(string s) {
    char* buffer = new char[s.length() * 2 + 1];

    mysql_real_escape_string(conn, buffer, s.c_str(), s.length());

    string result = buffer;
    delete[] buffer;

    return result;
}

// Caesar cipher encryption: shift by 3
string encrypt(string s) {
    for (int i = 0; i < s.length(); i++) {
        if (s[i] >= 'a' && s[i] <= 'z')
            s[i] = (s[i] - 'a' + 3) % 26 + 'a';
        else if (s[i] >= 'A' && s[i] <= 'Z')
            s[i] = (s[i] - 'A' + 3) % 26 + 'A';
    }

    return s;
}

// Caesar cipher decryption: shift back by 3
string decrypt(string s) {
    for (int i = 0; i < s.length(); i++) {
        if (s[i] >= 'a' && s[i] <= 'z')
            s[i] = (s[i] - 'a' + 23) % 26 + 'a';
        else if (s[i] >= 'A' && s[i] <= 'Z')
            s[i] = (s[i] - 'A' + 23) % 26 + 'A';
    }

    return s;
}

// Read a complete line
string readText(string message) {
    string s;
    cout << message;
    getline(cin, s);
    return s;
}

// Read an integer
int readInt(string message) {
    int n;

    while (true) {
        cout << message;

        if (cin >> n) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return n;
        }

        cout << "Invalid input. Enter a number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

// Read and validate CGPA
double readCGPA() {
    double cgpa;

    while (true) {
        cout << "Enter CGPA (0 to 10): ";

        if (cin >> cgpa) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (cgpa >= 0 && cgpa <= 10)
                return cgpa;
        } else {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        cout << "Invalid CGPA. Try again.\n";
    }
}

// Student class
class Student {
private:
    string ScholarID, Name, Branch, Email;
    double CGPA;

public:
    Student(string id, string name, string branch,
            double cgpa, string email) {
        ScholarID = id;
        Name = name;
        Branch = branch;
        CGPA = cgpa;
        Email = email;
    }

    string getID() {
        return ScholarID;
    }

    string getName() {
        return Name;
    }

    string getBranch() {
        return Branch;
    }

    string getEmail() {
        return Email;
    }

    double getCGPA() {
        return CGPA;
    }
};

// Create database tables
bool createTables() {
    string q1 =
        "CREATE TABLE IF NOT EXISTS Students ("
        "ScholarID VARCHAR(30) PRIMARY KEY,"
        "Name VARCHAR(100) NOT NULL,"
        "Branch VARCHAR(100) NOT NULL,"
        "CGPA DOUBLE NOT NULL,"
        "Email VARCHAR(150))";

    string q2 =
        "CREATE TABLE IF NOT EXISTS Admins ("
        "username VARCHAR(50) PRIMARY KEY,"
        "password_hash VARCHAR(255) NOT NULL)";

    if (mysql_query(conn, q1.c_str())) {
        cout << "Students table error: " << mysql_error(conn) << endl;
        return false;
    }

    if (mysql_query(conn, q2.c_str())) {
        cout << "Admins table error: " << mysql_error(conn) << endl;
        return false;
    }

    return true;
}

// Admin signup
void adminSignup() {
    cout << "\n--- Admin Signup ---\n";

    string username = readText("Enter username: ");
    string password = readText("Enter password: ");

    if (username.empty() || password.empty()) {
        cout << "Username and password cannot be empty.\n";
        Sleep(2000);
        return;
    }

    string encryptedPassword = encrypt(password);

    string q =
        "INSERT INTO Admins(username, password_hash) VALUES('" +
        sqlEscape(username) + "','" +
        sqlEscape(encryptedPassword) + "')";

    if (mysql_query(conn, q.c_str())) {
        if (mysql_errno(conn) == ER_DUP_ENTRY)
            cout << "Username already exists.\n";
        else
            cout << "Error: " << mysql_error(conn) << endl;
    } else {
        cout << "Admin account created successfully!\n";
    }

    Sleep(2000);
}

// Admin login
bool adminLogin() {
    cout << "\n--- Admin Login ---\n";

    string username = readText("Enter username: ");
    string password = readText("Enter password: ");

    string q =
        "SELECT password_hash FROM Admins WHERE username='" +
        sqlEscape(username) + "'";

    if (mysql_query(conn, q.c_str())) {
        cout << "Error: " << mysql_error(conn) << endl;
        Sleep(2000);
        return false;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    MYSQL_ROW row = mysql_fetch_row(res);

    if (row && row[0] != NULL) {
        string storedPassword = row[0];
        string originalPassword = decrypt(storedPassword);

        mysql_free_result(res);

        if (originalPassword == password) {
            cout << "Login successful!\n";
            Sleep(1000);
            return true;
        }
    } else {
        mysql_free_result(res);
    }

    cout << "Invalid username or password.\n";
    Sleep(2000);
    return false;
}

// Add a student
void addStudent() {
    cout << "\n--- Add Student ---\n";

    string id = readText("Enter Scholar ID: ");
    string name = readText("Enter student name: ");
    string branch = readText("Enter branch: ");
    double cgpa = readCGPA();
    string email = readText("Enter email: ");

    if (id.empty() || name.empty() || branch.empty()) {
        cout << "Scholar ID, name and branch are required.\n";
        Sleep(2000);
        return;
    }

    Student s(id, name, branch, cgpa, email);

    stringstream ss;
    ss << s.getCGPA();

    string cgpaText = ss.str();

    string q =
        "INSERT INTO Students(ScholarID, Name, Branch, CGPA, Email) VALUES('" +
        sqlEscape(s.getID()) + "','" +
        sqlEscape(s.getName()) + "','" +
        sqlEscape(s.getBranch()) + "'," +
        cgpaText + ",'" +
        sqlEscape(s.getEmail()) + "')";

    if (mysql_query(conn, q.c_str())) {
        
    cout << "Query: " << q << endl;
    cout << "MySQL Error: " << mysql_error(conn) << endl;

        if (mysql_errno(conn) == ER_DUP_ENTRY)
            cout << "This Scholar ID already exists.\n";
        else
            cout << "Error: " << mysql_error(conn) << endl;
    } else {
        cout << "Student added successfully!\n";
    }

    Sleep(2000);
}

// Display all students
void viewStudents() {
    cout << "\n--- All Students ---\n";

    string q =
        "SELECT ScholarID, Name, Branch, CGPA, Email "
        "FROM Students ORDER BY ScholarID";

    if (mysql_query(conn, q.c_str())) {
        cout << "Error: " << mysql_error(conn) << endl;
        Sleep(2000);
        return;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    MYSQL_ROW row;

    if (res == NULL) {
        cout << "Unable to retrieve student records.\n";
        Sleep(2000);
        return;
    }

    if (mysql_num_rows(res) == 0) {
        cout << "No student records found.\n";
    }

    while ((row = mysql_fetch_row(res))) {
        cout << "\nScholar ID: " << row[0]
             << "\nName: " << row[1]
             << "\nBranch: " << row[2]
             << "\nCGPA: " << row[3]
             << "\nEmail: " << (row[4] ? row[4] : "")
             << "\n-------------------------\n";
    }

    mysql_free_result(res);
    system("pause");
}

// Search for a student
void searchStudent() {
    cout << "\n--- Search Student ---\n";

    string id = readText("Enter Scholar ID: ");

    string q =
        "SELECT ScholarID, Name, Branch, CGPA, Email "
        "FROM Students WHERE ScholarID='" + sqlEscape(id) + "'";

    if (mysql_query(conn, q.c_str())) {
        cout << "Error: " << mysql_error(conn) << endl;
        Sleep(2000);
        return;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    MYSQL_ROW row = mysql_fetch_row(res);

    if (row) {
        cout << "\nScholar ID: " << row[0]
             << "\nName: " << row[1]
             << "\nBranch: " << row[2]
             << "\nCGPA: " << row[3]
             << "\nEmail: " << (row[4] ? row[4] : "")
             << endl;
    } else {
        cout << "Student not found.\n";
    }

    mysql_free_result(res);
    Sleep(3000);
}

// Update student details
void updateStudent() {
    cout << "\n--- Update Student ---\n";

    string id = readText("Enter Scholar ID to update: ");
    string safeID = sqlEscape(id);

    string check =
        "SELECT ScholarID FROM Students WHERE ScholarID='" +
        safeID + "'";

    if (mysql_query(conn, check.c_str())) {
        cout << "Error: " << mysql_error(conn) << endl;
        Sleep(2000);
        return;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    bool found = mysql_num_rows(res) > 0;
    mysql_free_result(res);

    if (!found) {
        cout << "Student not found.\n";
        Sleep(2000);
        return;
    }

    string name = readText("Enter new name: ");
    string branch = readText("Enter new branch: ");
    double cgpa = readCGPA();
    string email = readText("Enter new email: ");

    stringstream ss;
    ss << cgpa;

    string q =
        "UPDATE Students SET Name='" + sqlEscape(name) +
        "', Branch='" + sqlEscape(branch) +
        "', CGPA=" + ss.str() +
        ", Email='" + sqlEscape(email) +
        "' WHERE ScholarID='" + safeID + "'";

    if (mysql_query(conn, q.c_str())) {
        cout << "Error: " << mysql_error(conn) << endl;
    } else {
        cout << "Student details updated successfully!\n";
    }

    Sleep(2000);
}

// Delete a student
void deleteStudent() {
    cout << "\n--- Delete Student ---\n";

    string id = readText("Enter Scholar ID to delete: ");

    string q =
        "DELETE FROM Students WHERE ScholarID='" +
        sqlEscape(id) + "'";

    if (mysql_query(conn, q.c_str())) {
        cout << "Error: " << mysql_error(conn) << endl;
    } else if (mysql_affected_rows(conn) > 0) {
        cout << "Student deleted successfully!\n";
    } else {
        cout << "Student not found.\n";
    }

    Sleep(2000);
}

// Display ranking for a particular branch
void branchRanking(string branch) {
    cout << "\n--- Branch-wise CGPA Ranking ---\n";
    cout << "Branch: " << branch << endl;

    string q =
        "SELECT ScholarID, Name, CGPA FROM Students "
        "WHERE Branch='" + sqlEscape(branch) +
        "' ORDER BY CGPA DESC, Name ASC";

    if (mysql_query(conn, q.c_str())) {
        cout << "Error: " << mysql_error(conn) << endl;
        Sleep(2000);
        return;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    MYSQL_ROW row;
    int rank = 1;

    if (mysql_num_rows(res) == 0) {
        cout << "No students found in this branch.\n";
    }

    while ((row = mysql_fetch_row(res))) {
        cout << "\nRank: " << rank++
             << "\nScholar ID: " << row[0]
             << "\nName: " << row[1]
             << "\nCGPA: " << row[2]
             << "\n-------------------------\n";
    }

    mysql_free_result(res);
    system("pause");
}

// Admin portal
void adminPortal() {
    bool exit = false;

    while (!exit) {
        system("cls");

        cout << "\n===== ADMIN PORTAL =====\n";
        cout << "1. Add Student\n";
        cout << "2. View All Students\n";
        cout << "3. Search Student\n";
        cout << "4. Update Student\n";
        cout << "5. Delete Student\n";
        cout << "6. Branch-wise CGPA Ranking\n";
        cout << "7. Logout\n";

        int val = readInt("Enter your choice: ");

        if (val == 1)
            addStudent();
        else if (val == 2)
            viewStudents();
        else if (val == 3)
            searchStudent();
        else if (val == 4)
            updateStudent();
        else if (val == 5)
            deleteStudent();
        else if (val == 6) {
            string branch = readText("Enter branch: ");
            branchRanking(branch);
        }
        else if (val == 7) {
            exit = true;
            cout << "Logged out successfully.\n";
            Sleep(1500);
        }
        else {
            cout << "Invalid input.\n";
            Sleep(1500);
        }
    }
}

// Student portal
void studentPortal() {
    cout << "\n--- Student Login ---\n";

    string id = readText("Enter your Scholar ID: ");

    string q =
        "SELECT ScholarID, Name, Branch, CGPA, Email "
        "FROM Students WHERE ScholarID='" + sqlEscape(id) + "'";

    if (mysql_query(conn, q.c_str())) {
        cout << "Error: " << mysql_error(conn) << endl;
        Sleep(2000);
        return;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    MYSQL_ROW row = mysql_fetch_row(res);

    if (!row) {
        cout << "Student not found. Contact the administrator.\n";
        mysql_free_result(res);
        Sleep(2000);
        return;
    }

    string studentID = row[0];
    string name = row[1];
    string branch = row[2];
    string email = row[4] ? row[4] : "";

    mysql_free_result(res);

    bool exit = false;

    while (!exit) {
        system("cls");

        cout << "\n===== STUDENT PORTAL =====\n";
        cout << "Welcome, " << name << "!\n";
        cout << "1. View My Details\n";
        cout << "2. View My Branch Ranking\n";
        cout << "3. Logout\n";

        int val = readInt("Enter your choice: ");

        if (val == 1) {
            string detailQuery =
                "SELECT CGPA FROM Students WHERE ScholarID='" +
                sqlEscape(studentID) + "'";

            if (mysql_query(conn, detailQuery.c_str())) {
                cout << "Error: " << mysql_error(conn) << endl;
            } else {
                MYSQL_RES* details = mysql_store_result(conn);
                MYSQL_ROW detail = mysql_fetch_row(details);

                cout << "\nScholar ID: " << studentID
                     << "\nName: " << name
                     << "\nBranch: " << branch
                     << "\nCGPA: " << (detail ? detail[0] : "N/A")
                     << "\nEmail: " << email << endl;

                mysql_free_result(details);
            }

            system("pause");
        }
        else if (val == 2) {
            branchRanking(branch);
        }
        else if (val == 3) {
            exit = true;
            cout << "Logged out successfully.\n";
            Sleep(1500);
        }
        else {
            cout << "Invalid input.\n";
            Sleep(1500);
        }
    }
}

int main() {
    MYSQL* database = mysql_init(NULL);
    conn = database;

    if (conn == NULL) {
        cout << "MySQL initialization failed.\n";
        return 1;
    }

    if (!mysql_real_connect(conn, HOST, USER, PW, DB, 3306, NULL, 0)) {
        cout << "Database connection error: "
             << mysql_error(conn) << endl;

        mysql_close(conn);
        return 1;
    }

    cout << "Logged In Database Successfully!\n";

    if (!createTables()) {
        mysql_close(conn);
        return 1;
    }

    Sleep(1000);

    bool exit = false;

    while (!exit) {
        system("cls");

        cout << "\n====================================\n";
        cout << "    STUDENT MANAGEMENT SYSTEM\n";
        cout << "====================================\n";
        cout << "1. Admin Signup\n";
        cout << "2. Admin Login\n";
        cout << "3. Student Portal\n";
        cout << "4. Exit\n";

        int val = readInt("Enter your choice: ");

        if (val == 1) {
            adminSignup();
        }
        else if (val == 2) {
            if (adminLogin())
                adminPortal();
        }
        else if (val == 3) {
            studentPortal();
        }
        else if (val == 4) {
            exit = true;
            cout << "Good Luck!\n";
            Sleep(1500);
        }
        else {
            cout << "Invalid input.\n";
            Sleep(1500);
        }
    }

    mysql_close(conn);
    return 0;
}
