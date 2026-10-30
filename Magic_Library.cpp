#include <iostream>
#include <fstream>
#include <ctime>   // Added for date and time calculation
using namespace std;

// Added function: saves the borrowing date separately in borrow_dates.txt
void saveBorrowDate(int bookID)
{
    ofstream fout("borrow_dates.txt", ios::app);
    fout << bookID << " " << time(0) << "\n";
    fout.close();
}

// Added function: calculates the fine and removes the borrowing date after return
int calculateFineAndRemoveDate(int bookID)
{
    ifstream fin("borrow_dates.txt");
    ofstream temp("temp_dates.txt");
    int storedID;
    time_t borrowedTime;
    int fine = 0;
    bool dateFound = false;

    while (fin >> storedID >> borrowedTime)
    {
        if (storedID == bookID && !dateFound)
        {
            time_t currentTime = time(0);
            double daysPassed = difftime(currentTime, borrowedTime) / (60 * 60 * 24);

            if (daysPassed > 7)
                fine = 500;

            dateFound = true;
        }
        else
        {
            temp << storedID << " " << borrowedTime << "\n";
        }
    }

    fin.close();
    temp.close();

    remove("borrow_dates.txt");
    rename("temp_dates.txt", "borrow_dates.txt");

    return fine;
}

class Book
{
protected:
    int id;
    string title;
    string author;
    bool borrowed;

public:
    // Constructor
    Book(int i = 0, string t = "", string a = "", bool b = false)
    {
        id = i;
        title = t;
        author = a;
        borrowed = b;
    }

    // Inline function to display book details
    inline void display() {
        cout << id << " | " << title << " by " << author;
        cout << (borrowed ? " | Borrowed\n" : " | Available\n");
    }

    // Save book to file
    void saveToFile(ofstream &fout) {
        fout << id << "\n" << title << "\n" << author << "\n" << borrowed << "\n";
    }

    // Load book from file
    bool loadFromFile(ifstream &fin)
    {
        if (! (fin >> id))
            return false;

        fin.ignore();
        getline(fin, title);
        getline(fin, author);
        fin >> borrowed;
        fin.ignore();

        return true;
    }

    int getID()
    {
         return id;
    }
    bool isBorrowed()
    {
         return borrowed;
    }
    void borrowBook()
    {
        borrowed = true;
    }
    void returnBook()
    {
        borrowed = false;
    }
};

void addDefaultBooks()
{
    ofstream fout("books.txt", ios::out);
    Book b1(1, "C++ Basics", "Bjarne Stroustrup");
    Book b2(2, "Python Intro", "Guido Rossum");
    Book b3(3, "DSA Made Easy", "Robert Lafore");
    b1.saveToFile(fout);
    b2.saveToFile(fout);
    b3.saveToFile(fout);
    fout.close();
}

void addBookToFile()
{
    int id;
    string title, author;
    cout << "Enter Book ID: ";
    cin >> id;
    cin.ignore();
    cout << "Enter Book Title: ";
    getline(cin, title);
    cout << "Enter Author Name: ";
    getline(cin, author);

    Book newBook(id, title, author);
    ofstream fout("books.txt", ios::app);
    newBook.saveToFile(fout);
    fout.close();
    cout << "Book added successfully!\n";
}

void showAllBooks()
{
    ifstream fin("books.txt");
    if (!fin) {
        cout << "No books found!\n";
        return;
    }

    Book b;
    cout << "\n--- Book List ---\n";
        {
        Book b;
        while (b.loadFromFile(fin))
        {
          if (b.getID() != 0)
            b.display();
        }

        }
    fin.close();
}

void borrowBookFromFile(int bookID)
{
    ifstream fin("books.txt");
    ofstream temp("temp.txt");
    Book b;
    bool found = false;

    while (b.loadFromFile(fin))
    {
        if(b.getID()== bookID && !b.isBorrowed())
        {
            b.borrowBook();
            found = true;
        }

        if (b.getID() !=0)
            b.saveToFile(temp);
    }


    fin.close();
    temp.close();

    remove("books.txt");
    rename("temp.txt", "books.txt");

    if (found)
    {
        saveBorrowDate(bookID);   // Added: record the date of borrowing
        cout << "Book borrowed successfully.\n";
    }
    else
        cout << "Book not found or already borrowed.\n";
}

void returnBookToFile(int bookID) {
    ifstream fin("books.txt");
    ofstream temp("temp.txt");
    Book b;
    bool found = false;

    while (b.loadFromFile(fin))
    {
        if (b.getID() == bookID && b.isBorrowed())
        {
            b.returnBook();
            found = true;
        }

        if (b.getID() != 0)
            b.saveToFile(temp);
    }

    fin.close();
    temp.close();

    remove("books.txt");
    rename("temp.txt", "books.txt");

    if (found)
    {
        int fine = calculateFineAndRemoveDate(bookID);  // Added: check overdue fine
        cout << "Book returned successfully.\n";

        if (fine > 0)
            cout << "You returned the book after 7 days. Fine: Rs. " << fine << "\n";
        else
            cout << "Book returned within 7 days. No fine.\n";
    }
    else
        cout << "Book not found or already available.\n";
}

int main()
{
    // Check if file exists, if not add default books
    ifstream check("books.txt");
    if (!check) {
        addDefaultBooks();
    }
    check.close();

    cout << "WELCOME TO MAGIC LIBRARY!!!!"<<endl;
    cout<<endl;

    int choice, id;
    do {
        cout << "\n--- Library Menu ---\n";
        cout << "1. Show All Books\n";
        cout << "2. Add Book\n";
        cout << "3. Borrow Book\n";
        cout << "4. Return Book\n";
        cout << "5. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            showAllBooks();
            break;
        case 2:
            addBookToFile();
            break;
        case 3:
            cout << "Enter Book ID to Borrow: ";
            cin >> id;
            borrowBookFromFile(id);
            break;
        case 4:
            cout << "Enter Book ID to Return: ";
            cin >> id;
            returnBookToFile(id);
            break;
        case 5:
            cout << "Exiting Library System.\n";
            break;
        default:
            cout << "Invalid choice! Try again.\n";
        }
    } while (choice != 5);

    return 0;
}
