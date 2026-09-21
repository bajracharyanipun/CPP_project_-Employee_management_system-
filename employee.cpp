#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <regex>
#include <limits>
#include <vector>
#include <cctype>

using namespace std;

// Constants
const string FILENAME = "employeeRecord.csv";
const string TEMP_FILENAME = "tempEmployeeRecord.csv";

// Escapes a value so it can safely be stored in a CSV field.
string csvEscape(const string& value) {
    string escaped;
    bool needs_quotes = value.find_first_of(",\"\n") != string::npos;

    for (char character : value) {
        if (character == '"') {
            escaped += "\"\"";
        } else {
            escaped += character;
        }
    }

    return needs_quotes ? '"' + escaped + '"' : escaped;
}

// Splits one CSV line and handles quoted fields and escaped quotes.
vector<string> parseCsvLine(const string& line) {
    vector<string> fields;
    string field;
    bool inside_quotes = false;

    for (size_t index = 0; index < line.length(); ++index) {
        char character = line[index];

        if (character == '"') {
            if (inside_quotes && index + 1 < line.length() && line[index + 1] == '"') {
                field += '"';
                ++index;
            } else {
                inside_quotes = !inside_quotes;
            }
        } else if (character == ',' && !inside_quotes) {
            fields.push_back(field);
            field.clear();
        } else {
            field += character;
        }
    }

    fields.push_back(field);
    return fields;
}

// Writes one employee as a CSV row.
void writeEmployeeCsv(ofstream& file, const string& name, const string& employee_id,
                      const string& department, const string& email_id,
                      long long int contact_no, const string& address) {
    file << csvEscape(name) << ','
         << csvEscape(employee_id) << ','
         << csvEscape(department) << ','
         << csvEscape(email_id) << ','
         << contact_no << ','
         << csvEscape(address) << '\n';
}

// Email Validation Function
bool isValidEmail(const string& email) {
    const regex pattern(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    return regex_match(email, pattern);
}

// Employee ID Validation Function
bool isValidEmployeeId(const string& employee_id) {
    const regex pattern(R"([0-9]+)");
    return regex_match(employee_id, pattern);
}

bool isValidName(const string& employee_name) {
    if (employee_name.empty()) {
        return false;
    }

    bool has_letter = false;
    for (unsigned char character : employee_name) {
        if (isalpha(character)) {
            has_letter = true;
        } else if (character != ' ') {
            return false;
        }
    }

    return has_letter;
}

// Contact Number Validation Function
bool isValidContactNo(long long int contact_no) {
    return (contact_no >= 1000000000 && contact_no <= 9999999999);
}

bool employeeIdExists(const string& employee_id, const string& ignored_employee_id = "") {
    ifstream file(FILENAME);
    string line;

    while (getline(file, line)) {
        vector<string> fields = parseCsvLine(line);
        if (fields.size() == 6 && fields[1] == employee_id && fields[1] != ignored_employee_id) {
            return true;
        }
    }

    return false;
}

bool contactNoExists(long long int contact_no, const string& ignored_employee_id = "") {
    ifstream file(FILENAME);
    string line;

    while (getline(file, line)) {
        vector<string> fields = parseCsvLine(line);
        if (fields.size() == 6 && fields[1] != ignored_employee_id &&
            stoll(fields[4]) == contact_no) {
            return true;
        }
    }

    return false;
}

class Employee {
private:
    string name, employee_id, department, address, email_id;
    long long int contact_no;

    void getValidContactNo() {
        while (true) {
            cout << "\tEnter Contact No (10 digits): ";
            cin >> contact_no;
            
            if (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "\tInvalid input. Please enter numbers only.\n";
                continue;
            }
            
            if (isValidContactNo(contact_no)) {
                break;
            }
            cout << "\tInvalid Contact Number! Please enter a valid 10-digit number.\n";
        }
    }

public:
    void menu();
    void insert();
    void display();
    void modify();
    void search();
    void deleteEmployee();
};

// Menu Function
void Employee::menu() {
    int choice;
    char x;

    while (true) {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
        
        cout << "\t\t\t-----------------------------\n";
        cout << "\t\t\t| EMPLOYEE MANAGEMENT SYSTEM |\n";
        cout << "\t\t\t-----------------------------\n";
        cout << "\t\t\t 1. Enter New Record\n";
        cout << "\t\t\t 2. Display Record\n";
        cout << "\t\t\t 3. Modify Record\n";
        cout << "\t\t\t 4. Search Record\n";
        cout << "\t\t\t 5. Delete Record\n";
        cout << "\t\t\t 6. Exit\n";
        cout << "\t\t\tChoose an Option: ";
        
        cin >> choice;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            choice = 0; // Force default case
        }

        switch (choice) {
            case 1:
                do {
                    insert();
                    cout << "\n\n\t\t\t Add Another Employee Record (Y/N): ";
                    cin >> x;
                } while (x == 'y' || x == 'Y');
                break;
            case 2: 
                display(); 
                cout << "\n\t\t\tPress Enter to continue...";
                cin.ignore();
                cin.get();
                break;
            case 3: modify(); break;
            case 4:
                search();
                cout << "\n\t\t\tPress Enter to continue...";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cin.get();
                break;
            case 5: deleteEmployee(); break;
            case 6:
                cout << "\n\t\t\t Exiting Program...\n";
                exit(0);
            default:
                cout << "\n\t\t\t Invalid Choice... Try Again.\n";
                cin.ignore();
                cin.get();
        }
    }
}

// Insert New Employee Record
void Employee::insert() {
    ofstream file;
    
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    do {
        cout << "\n\tEnter Name: ";
        getline(cin, name);
        if (!isValidName(name)) {
            cout << "\tInvalid name! Use letters and spaces only.\n";
        }
    } while (!isValidName(name));
    
    do {
        cout << "\tEnter Employee ID: ";
        getline(cin, employee_id);
        if (!isValidEmployeeId(employee_id)) {
            cout << "\tInvalid Employee ID! Try Again.\n";
        } else if (employeeIdExists(employee_id)) {
            cout << "\tEmployee ID already exists! Try Again.\n";
        }
    } while (!isValidEmployeeId(employee_id) || employeeIdExists(employee_id));

    cout << "\tEnter Department: ";
    getline(cin, department);

    do {
        cout << "\tEnter Email Id (name@gmail.com): ";
        getline(cin, email_id);
        if (!isValidEmail(email_id)) {
            cout << "\tInvalid Email! Try Again.\n";
        }
    } while (!isValidEmail(email_id));

    do {
        getValidContactNo();
        if (contactNoExists(contact_no)) {
            cout << "\tContact number already exists! Try Again.\n";
        }
    } while (contactNoExists(contact_no));

    cout << "\tEnter Address: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, address);

    file.open(FILENAME, ios::app);
    if (!file) {
        cout << "\n\tError opening file for writing!\n";
        return;
    }
    
    writeEmployeeCsv(file, name, employee_id, department, email_id, contact_no, address);
    file.close();
    
    cout << "\n\t\t\tRecord Added Successfully!\n";
}

// Display All Employee Records
void Employee::display() {
    fstream file;
    file.open(FILENAME, ios::in);
    
    if (!file) {
        cout << "\n\t\tNo Data is Present...\n";
        return;
    }

    int total = 0;
    string line;
    while (getline(file, line)) {
        vector<string> fields = parseCsvLine(line);
        if (fields.size() != 6) {
            continue;
        }

        name = fields[0];
        employee_id = fields[1];
        department = fields[2];
        email_id = fields[3];
        contact_no = stoll(fields[4]);
        address = fields[5];

        cout << "\n\tEmployee No.: " << ++total;
        cout << "\n\tName: " << name;
        cout << "\n\tEmployee ID: " << employee_id;
        cout << "\n\tDepartment: " << department;
        cout << "\n\tEmail Id: " << email_id;
        cout << "\n\tContact No.: " << contact_no;
        cout << "\n\tAddress: " << address << "\n";

        if (total % 5 == 0) {
            cout << "\n\tPress Enter to continue...";
            cin.ignore();
            cin.get();
        }
    }

    if (total == 0) {
        cout << "\n\t\tNo Data Found...\n";
    }

    file.close();
}

// Modify Employee Record
void Employee::modify() {
    ifstream file;
    ofstream file1;
    string employee_id_to_modify;
    int found = 0;

    cout << "\n-------------------------------------------------------------------------------------------------------" << endl;
    cout << "------------------------------------ Employee Modify Details ------------------------------------------" << endl;
    
    file.open(FILENAME, ios::in);
    if (!file) {
        cout << "\n\t\t\tNo Data is Present..";
        file.close();
        return;
    }
    
    cout << "\nEnter Employee ID of the Employee you want to modify: ";
    cin >> employee_id_to_modify;
    
    file1.open(TEMP_FILENAME, ios::out | ios::trunc);
    
    string line;
    while (getline(file, line)) {
        vector<string> fields = parseCsvLine(line);
        if (fields.size() != 6) {
            continue;
        }

        string current_employee_id = fields[1];

        if (employee_id_to_modify != current_employee_id) {
            file1 << line << "\n";
        } else {
            cout << "\n\t\t\tCurrent Details:\n";
            cout << "\t\t\tName: " << fields[0] << "\n";
            cout << "\t\t\tEmployee ID: " << fields[1] << "\n";
            cout << "\t\t\tDepartment: " << fields[2] << "\n";
            cout << "\t\t\tEmail Id: " << fields[3] << "\n";
            cout << "\t\t\tContact No.: " << fields[4] << "\n";
            cout << "\t\t\tAddress: " << fields[5] << "\n";

            name = fields[0];
            employee_id = fields[1];
            department = fields[2];
            email_id = fields[3];
            contact_no = stoll(fields[4]);
            address = fields[5];

            int field_choice;
            cout << "\n\t\t\tWhat would you like to edit?\n";
            cout << "\t\t\t1. Name\n";
            cout << "\t\t\t2. Employee ID\n";
            cout << "\t\t\t3. Department\n";
            cout << "\t\t\t4. Email\n";
            cout << "\t\t\t5. Contact Number\n";
            cout << "\t\t\t6. Address\n";
            cout << "\t\t\tChoose a field: ";
            cin >> field_choice;
            while (cin.fail() || field_choice < 1 || field_choice > 6) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "\t\t\tInvalid choice. Choose 1-6: ";
                cin >> field_choice;
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            switch (field_choice) {
                case 1:
                    do {
                        cout << "\t\t\tEnter Name: ";
                        getline(cin, name);
                        if (!isValidName(name)) {
                            cout << "\t\t\tInvalid name! Use letters and spaces only.\n";
                        }
                    } while (!isValidName(name));
                    break;
                case 2:
                    do {
                        cout << "\t\t\tEnter Employee ID: ";
                        getline(cin, employee_id);
                        if (!isValidEmployeeId(employee_id)) {
                            cout << "\t\t\tInvalid Employee ID! Try Again.\n";
                        } else if (employeeIdExists(employee_id, employee_id_to_modify)) {
                            cout << "\t\t\tEmployee ID already exists! Try Again.\n";
                        }
                    } while (!isValidEmployeeId(employee_id) ||
                             employeeIdExists(employee_id, employee_id_to_modify));
                    break;
                case 3:
                    cout << "\t\t\tEnter Department: ";
                    getline(cin, department);
                    break;
                case 4:
                    do {
                        cout << "\t\t\tEnter Email Id (name@gmail.com): ";
                        getline(cin, email_id);
                        if (!isValidEmail(email_id)) {
                            cout << "\t\t\tInvalid Email! Try Again.\n";
                        }
                    } while (!isValidEmail(email_id));
                    break;
                case 5:
                    do {
                        getValidContactNo();
                        if (contactNoExists(contact_no, employee_id_to_modify)) {
                            cout << "\t\t\tContact number already exists! Try Again.\n";
                        }
                    } while (contactNoExists(contact_no, employee_id_to_modify));
                    break;
                case 6:
                    cout << "\t\t\tEnter Address: ";
                    getline(cin, address);
                    break;
            }

            writeEmployeeCsv(file1, name, employee_id, department, email_id, contact_no, address);
            found++;
            cout << "\n\t\t\tRecord Updated Successfully!\n";
        }
    }

    if (found == 0) {
        cout << "\n\n\t\t\t Employee ID Not Found....";
    }
    
    file1.close();
    file.close();
    remove(FILENAME.c_str());
    rename(TEMP_FILENAME.c_str(), FILENAME.c_str());
}

// Search Employee Record
void Employee::search() {
    fstream file;
    int found = 0;

    file.open(FILENAME, ios::in);
    if (!file) {
        cout << "\n-------------------------------------------------------------------------------------------------------" << endl;
        cout << "------------------------------------ Employee Search Data --------------------------------------------" << endl;
        cout << "\n\t\t\tNo Data is Present... " << endl;
        return;
    }

    string employee_id_to_search;
    cout << "\n-------------------------------------------------------------------------------------------------------" << endl;
    cout << "------------------------------------ Employee Search Table --------------------------------------------" << endl;
    cout << "\nEnter Employee ID of the Employee you want to search: ";
    cin >> employee_id_to_search;

    string line;
    while (getline(file, line)) {
        vector<string> fields = parseCsvLine(line);
        if (fields.size() == 6 && employee_id_to_search == fields[1]) {
            cout << "\n\n\t\t\tName: " << fields[0] << "\n";
            cout << "\t\t\tEmployee ID: " << fields[1] << "\n";
            cout << "\t\t\tDepartment: " << fields[2] << "\n";
            cout << "\t\t\tEmail Id: " << fields[3] << "\n";
            cout << "\t\t\tContact No.: " << fields[4] << "\n";
            cout << "\t\t\tAddress: " << fields[5] << "\n";
            found++;
            break;
        }
    }
    
    if (found == 0) {
        cout << "\n\t\t\t Employee ID Not Found....";
    }

    file.close();
}

// Delete Employee Record
void Employee::deleteEmployee() {
    ifstream file;
    ofstream file1;
    string employee_id_to_delete;
    int found = 0;

    cout << "\n-------------------------------------------------------------------------------------------------------" << endl;
    cout << "------------------------------------ Delete Employee Details ------------------------------------------" << endl;
    file.open(FILENAME, ios::in);
    if (!file) {
        cout << "\n\t\t\tNo Data is Present..";
        file.close();
        return;
    }

    cout << "\nEnter Employee ID of the Employee you want to delete: ";
    cin >> employee_id_to_delete;
    
    char confirm;
    cout << "\n\t\tAre you sure you want to delete this record? (Y/N): ";
    cin >> confirm;
    if (confirm != 'Y' && confirm != 'y') {
        cout << "\n\t\tDeletion cancelled.\n";
        return;
    }

    file1.open(TEMP_FILENAME, ios::out | ios::trunc);

    string line;
    while (getline(file, line)) {
        vector<string> fields = parseCsvLine(line);
        if (fields.size() == 6) {
            if (employee_id_to_delete != fields[1]) {
                file1 << line << "\n";
            } else {
                found++;
            }
        }
    }

    if (found == 0) {
        cout << "\n\t\t\t Employee ID Not Found....";
    } else {
        cout << "\n\t\t\tSuccessfully Deleted Data\n";
    }

    file1.close();
    file.close();
    remove(FILENAME.c_str());
    rename(TEMP_FILENAME.c_str(), FILENAME.c_str());
}

// Main Function
int main() {
    Employee project;
    project.menu();
    return 0;
}