#include <iostream>
#include <Windows.h>
#include <fstream>
#include <vector>
#include <string>
#include <cctype>
using namespace std;

//function to move the cursor to a certain x,y coordinate
void gotoxy(int x, int y)
{
    COORD c = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

//node structure 
struct Node {
    int x = 0; //x coordinate 
    int y = 0; //y coordinate 
    char data;
    Node* next; // right
    Node* back; //left
    Node* up;   //up
    Node* down; //down
};

//cursor class to handle the cursor movements
class Cursor {
public:
    Node* cursor; //cursor is a node pointer that points to a certain node in the notepad
    Node* getcursor()  //getter
    {
        return cursor;
    }
    void setcursor(Node* C) //setter
    { 
        cursor = C;
    }
    void  cursorback() {    //moving the cursor one character/node back if the cursor is not null
        if (cursor != NULL) {
            if (cursor->back != NULL) {
                cursor = cursor->back;
            }

        }
    }
    void  cursornext() { //moving the cursor one character/node next if the cursor is not null
        if (cursor != NULL) {
            if (cursor->next != NULL) {
                cursor = cursor->next;
            }
        }
    }
    void  cursorup() { //moving the cursor one character/node up if the cursor is not null
        if (cursor != NULL) {
            if (cursor->up != NULL) {
                cursor = cursor->up;
            }
        }
    }
    void  cursordown()  { //moving the cursor one character/node down if the cursor is not null
        if (cursor != NULL) {
            if (cursor->down != NULL) {
                cursor = cursor->down;
            }
        }
    }
    void deleteatcursor() { //deleting the node pointed by the cursor
        if (cursor != NULL) {
            if (cursor->back != NULL) {
                cursor->back->next = cursor->next;
                Node* prev = cursor->back;
                free(cursor);  //freeing memory when the node is deleted 
                cursor = prev;
            }
            else {
                cursor->data = '\0';
            }
        }
    }
};

//link class for notepad 
class Link {
private:
    Node* start;
    Cursor at; //cursor class object 
public:
    Link() {
        start = nullptr;
    }
    Node* getstart() { //getter
        return start;
    }
    void insertinline(char a) { // inserting character in the notepad

        if (start == NULL) { // first node/char in the notepad
            start = new Node;
            start->next = NULL;
            start->back = NULL;
            start->up = NULL;
            start->back = NULL;
            start->data = a;
            at.setcursor(start);
        }
        else { //char other than the first one
            Node* temp = at.getcursor();
            Node* insert = new Node;
            insert->next = temp->next;
            insert->up = NULL;
            insert->back = temp;
            insert->down = NULL;
            insert->data = a;
            temp->next = insert;
            at.setcursor(insert);
        }
        upanddownlink(); //fucntion to make up and down link
    }
    void display() { //displaying notepad
        Node* temp = start;
        while (temp) {
            cout << temp->data;
            temp = temp->next;
        }
        cout << "|"; //default cursor,always at the end of the last character input
    }
    void clear_notepad() { //deallocating memory (clearing the entire/whole notepad)
        start = nullptr;
        at.cursor = nullptr;
    }

    void deleteat() { //deleting character
        at.deleteatcursor();
    }
    void left() { //moving left
        at.cursorback();
    }
    void right() { //moving right
        at.cursornext();
    }
    void up() { //moving up
        at.cursorup();
    }
    void down() { //moving down
        at.cursordown();
    }

    void upanddownlink() { //making a link between the up down pointers
        Node* temp = start;
        Node* head = start;
        Node* lastendl = start; //last character of a line

        int x = 0;
        int y = 0;
        while (temp != NULL) {
            
            if (temp->data == '\n') { //new line(moving vertically)
                y++; 
                temp->y = y; 
                x = 0;

            }
            else { //moving horizontally
                x++;
                temp->x = x;
                temp->y = y;

            }
            if (temp->next) {
                temp->up = NULL;
                temp->down = NULL;
            }
            temp = temp->next;
        }
        temp = start;
    
        while (temp->data != '\n' && temp->next != NULL) {
            temp = temp->next;
        }
        lastendl = temp;
        if (temp->next == NULL) {
            return;
        }
        else {
            while (temp->next != NULL) {
                temp = temp->next;
                temp->up = head;
                if (head->next->data != '\n') {
                    head->down = temp;
                    head = head->next;
                }
                else {
                    if (temp->data == '\n') {
                        head = lastendl->next; //head moves to the start character of the next line from the 'start'
                        lastendl = temp; //moving to next character
                    }
                }
            }
        }

    }

    void writetofile(string filename="save.txt") { //saving the notepad to text file (save.txt)
        fstream f;
        f.open(filename, ios::out);
        Node* temp = start;
        while (temp) {
            f << temp->data;
            temp = temp->next;
        }
    }
    bool openfile(string filename="save.txt") { //loading file "sav.txt" into notepad 

        fstream fa;
        fa.open(filename);

        if (fa.is_open()) {
            char xa = '\0';

            while (!fa.eof()) {
                fa.get(xa);
                insertinline(xa); 
            }
            return 1;
        }
        return 0;
    }
    Node* getcursor() //getter
    {
        return at.getcursor();
    }

    ~Link() { //freeing memory/deallocating
        clear_notepad();
    }
};

//dictionary class for spell checking etc
class Dictionary {
private:
    vector<string> dict;
    vector<vector<string>> correctionsCache; //2d vector for storing corrections
    int START_X = 0;
    int START_Y = 0;
    int LINE_LENGTH = 0; // Initialize LINE_LENGTH

public:
    Dictionary(string path="dictionary.txt") {
        // Read words from the text file and populate the dict vector
        ifstream inputFile(path);
        if (inputFile.is_open()) {
            string word;
            while (getline(inputFile, word)) {
                dict.push_back(word);
            }
            inputFile.close();
        }
        else {
            cout << "Error: Unable to open dictionary file " << path << endl << "Exiting...\n";
            exit(-1);
        }
    }

    bool wordExists(vector<string>& words, string& wordToFind) { //passing by reference for speed
        // Remove '\n' characters from the word before checking
        string strippedWord = wordToFind;
        strippedWord.erase(remove(strippedWord.begin(), strippedWord.end(), '\n'), strippedWord.end());
        return find(words.begin(), words.end(), strippedWord) != words.end(); //returning true if word to find exists in the vector "dict"
    }

    vector<string> store_unique_words(Link& notepad) {
        vector<string> uniqueWords; // To keep track of unique words

        Node* temp = notepad.getstart();
        string tempword = "";
        while (temp) {
            if (temp->data != ' ' && temp->data != '\n') { // Check for both space and newline
                tempword += temp->data;
            }
            else { // Space or newline encountered
                // Check if the word is not already in the vector (i.e., unique)
                if (!wordExists(uniqueWords,tempword)) {
                    uniqueWords.push_back(tempword); // Add the word to the vector
                }
                tempword = "";
            }
            temp = temp->next;
        }

        // Handle the last word
        if (!tempword.empty()) {
            if (!wordExists(uniqueWords,tempword)) {
                uniqueWords.push_back(tempword);
            }
        }

        return uniqueWords;
    }

    vector<string> findCorrections(string &wrongWord) {
        // Check if corrections are already calculated for this word, to avoid calculating each time for existing words
        for (int i = 0; i < correctionsCache.size(); i++) {
            if (correctionsCache[i][0] == wrongWord) { //first element of every row is the wrong word, row=i col=0
                vector<string> cachedCorrections;
                for (int j = 1; j < correctionsCache[i].size(); j++) {
                    cachedCorrections.push_back(correctionsCache[i][j]);    //copying corrections to the 'cachedCorrections' vector
                }
                return cachedCorrections;
            }
        }

        string alphabet = "abcdefghijklmnopqrstuvwxyz";

        // Convert the misspelled word to lowercase
        string lowercaseWord = wrongWord;
        for (int i = 0; i < lowercaseWord.length(); i++) {
            if (isupper(lowercaseWord[i])) {
                lowercaseWord[i] = tolower(lowercaseWord[i]);
            }
        }

        vector<string> uniqueCorrections; // To store unique corrections

        // Letter Substitution
        for (int i = 0; i < lowercaseWord.length(); i++) {
            for (int c = 0; c < alphabet.length(); c++) {
                string substituted = lowercaseWord;
                substituted[i] = alphabet[c];
                if (wordExists(this->dict, substituted)) {
                    string strippedWord = substituted;
                    strippedWord.erase(remove(strippedWord.begin(), strippedWord.end(), '\n'), strippedWord.end()); //cleaning word of \n characters just in case
                    uniqueCorrections.push_back(strippedWord); // Add to the vector to ensure uniqueness
                }
            }
        }

        // Letter Omission
        for (int i = 0; i < lowercaseWord.length(); i++) {
            string omitted = lowercaseWord;
            omitted.erase(i, 1);
            if (wordExists(this->dict, omitted)) {
                string strippedWord = omitted;
                strippedWord.erase(remove(strippedWord.begin(), strippedWord.end(), '\n'), strippedWord.end());
                if (!wordExists(uniqueCorrections, strippedWord)) { //appending only when the correction has not been made before 
                    uniqueCorrections.push_back(strippedWord); // Add to the vector to ensure uniqueness
                }   
            }
        }

        // Letter Insertion
        for (int i = 0; i <= lowercaseWord.length(); i++) {
            for (int c = 0; c < alphabet.length(); c++) {
                string inserted = lowercaseWord;
                inserted.insert(i, 1, alphabet[c]);
                if (wordExists(this->dict, inserted)) {
                    string strippedWord = inserted;
                    strippedWord.erase(remove(strippedWord.begin(), strippedWord.end(), '\n'), strippedWord.end());
                    if (!wordExists(uniqueCorrections, strippedWord)) {
                        uniqueCorrections.push_back(strippedWord); // Add to the vector to ensure uniqueness
                    }
                }
            }
        }

        // Letter Reversal
        int wordLength = lowercaseWord.length();
        for (int i = 0; i < wordLength - 1; i++) {
            string reversed = lowercaseWord;
            swap(reversed[i], reversed[i + 1]);
            if (wordExists(this->dict, reversed)) {
                string strippedWord = reversed;
                strippedWord.erase(remove(strippedWord.begin(), strippedWord.end(), '\n'), strippedWord.end());
                if (!wordExists(uniqueCorrections, strippedWord)) {
                    uniqueCorrections.push_back(strippedWord); // Add to the vector to ensure uniqueness
                }
            }
        }


        // Cache the corrections
        correctionsCache.push_back({ wrongWord });
        for (int correction = 0; correction < uniqueCorrections.size(); correction++) {
            correctionsCache.back().push_back(uniqueCorrections[correction]);
        }

        return uniqueCorrections;
    }

    void printCorrections(Link &notepad, int asciiInsertedChar) {
        // Calculate START_Y and LINE_LENGTH based on console window size
        
        //SPLITTING SCREEN (75% FOR NOTEPAD, 25% FOR CORRECTIONS)
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        int consoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        int consoleHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
        START_Y = consoleHeight - (consoleHeight * 25 / 100); // 25% of the bottom screen
        LINE_LENGTH = consoleWidth; // Adjust the line length dynamically
        gotoxy(START_X, START_Y); //moving to the start of the remaining 25% screen 
        for (int i = 0; i < LINE_LENGTH; i++) {
            cout << "-";
        }
        
        if (asciiInsertedChar == 32 || asciiInsertedChar == 13) //only run dict if space or enter inserted, for optimization 
        {
            START_Y += 1;
            vector<string>words = store_unique_words(notepad); // storing all (wrong and correct) unique words in the notepad
            for (int i = 0; i < words.size(); i++) {
                if (words[i].length() > 1) {
                    if (!wordExists(dict, words[i])) { //wrong word encountered
                        cout << words[i] << " => ";
                        vector<string> corrections = findCorrections(words[i]);
                        if (!corrections.empty()) {
                            for (int j = 0; j < corrections.size(); j++) {
                                cout << corrections.at(j); //printing corrections
                                if (j < corrections.size() - 1) { //making sure that ',' is not printed after the last correctrion
                                    cout << ", ";
                                }
                            }
                        }
                        else { //no corrections found
                            cout << "You are a hopeless speller!\n";
                        }
                    }
                    cout << endl;
                }
            }
        }
    }
};

int main(int argc, char* argv[]) {
    Link a;
    Dictionary d;
    system("cls");
    HANDLE  rhnd = GetStdHandle(STD_INPUT_HANDLE);  // handle to read console

    DWORD Events = 0;     // Event count
    DWORD EventsRead = 0; // Events read from console

    bool Running = true;

    int x = 0, y = 0;
    gotoxy(x, y);
    int line = 0;
    //programs main loop
    char menu;
    bool flag = false;
selectoption:
    if (!flag) {
        cout << "Enter a Number in Menu to Select a Option." << endl;
        cout << "       1) Start Notepad" << endl;
        cout << "CTRL + L) Load from file, after starting Notepad" << endl;
        cout << "CTRL + S) Save to file, after starting Notepad" << endl;
        cin >> menu;
    }
    bool ctrlPressed = false;
    DWORD ctrlPressedTime = 0;
    switch (menu) {
        case '1': {
            system("cls");
            a.display();
            while (Running) {
                // gets the systems current "event" count
                GetNumberOfConsoleInputEvents(rhnd, &Events);

                if (Events != 0) { // if something happened we will handle the events we want

                    // create event buffer the size of how many Events
                    INPUT_RECORD eventBuffer[200];

                    // fills the event buffer with the events and saves count in EventsRead
                    ReadConsoleInput(rhnd, eventBuffer, Events, &EventsRead);

                    // loop through the event buffer using the saved count
                    for (DWORD i = 0; i < EventsRead; ++i) {

                        // check if event[i] is a key event && if so is a press not a release
                        if (eventBuffer[i].EventType == KEY_EVENT && eventBuffer[i].Event.KeyEvent.bKeyDown) {


                            // check if the key press was an arrow key
                            switch (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode) {

                            case VK_UP: //up
                                if (a.getstart()) {
                                    if (a.getcursor()->up != NULL) {
                                            a.up();
                                            gotoxy(a.getcursor()->x, a.getcursor()->y);
                                    }
                                }
                                break;
                            case VK_DOWN: //down
                                if (a.getstart()) {
                                    if (a.getcursor()->down != NULL) {
                                            a.down();
                                            gotoxy(a.getcursor()->x, a.getcursor()->y);
                                    }
                                }
                                break;
                            case VK_RIGHT: //right
                                if (a.getstart()) {
                                    if (a.getcursor()->next != NULL) {
                                            a.right();
                                            gotoxy(a.getcursor()->x, a.getcursor()->y);
                                    }
                                }
                                break;
                            case VK_LEFT: //left
                                if (a.getstart()) {
                                    if (a.getcursor()->back != NULL) {
                                            a.left();
                                            gotoxy(a.getcursor()->x, a.getcursor()->y);
                                    }
                                }
                                break;
                            case VK_ESCAPE:
                            {
                                system("cls");
                                cout << "Quitting...\n";
                                exit(0);
                                break;
                            }
                            case VK_CONTROL:
                                ctrlPressed = true;
                                ctrlPressedTime = GetTickCount();  // Record the time when Ctrl was pressed
                                break;

                            case 'S': //save
                                if (!ctrlPressed) {
                                    system("cls");
                                    a.insertinline(eventBuffer->Event.KeyEvent.uChar.AsciiChar);
                                    if (a.getcursor() != NULL) {
                                        a.display();
                                        d.printCorrections(a, eventBuffer->Event.KeyEvent.uChar.AsciiChar);
                                        gotoxy(a.getcursor()->x, a.getcursor()->y);
                                    }
                                }
                                else if (ctrlPressed && GetTickCount() - ctrlPressedTime <= 500) {
                                    // Ctrl + S is pressed within 500 milliseconds
                                    a.writetofile();
                                    system("cls");
                                    cout << "Saved in \"save.txt\"...";
                                    exit(0);
                                }
                                ctrlPressed = false;  // Reset Ctrl state
                                break;


                            case 'L': { //load
                                if (!ctrlPressed) {
                                    system("cls");
                                    a.insertinline(eventBuffer->Event.KeyEvent.uChar.AsciiChar);
                                    if (a.getcursor() != NULL) {
                                        a.display();
                                        d.printCorrections(a, eventBuffer->Event.KeyEvent.uChar.AsciiChar);
                                        gotoxy(a.getcursor()->x, a.getcursor()->y);
                                    }
                                }
                                else if (ctrlPressed && GetTickCount() - ctrlPressedTime <= 500) {
                                    if (a.getstart()) {
                                        system("cls");
                                        a.clear_notepad();
                                    }
                                    // Ctrl + L is pressed within 500 milliseconds
                                    if (!a.openfile()) { //file doesnt exist
                                        cout << "File doesn't exist\nExiting...";
                                        exit(0);
                                    }
                                    system("cls");
                                    a.display();
                                    gotoxy(a.getcursor()->x, a.getcursor()->y);
                                }
                                ctrlPressed = false;  // Reset Ctrl state
                                break;
                            }


                            default:
                                system("cls");
                                if ((int(eventBuffer->Event.KeyEvent.uChar.AsciiChar) > 63 && int(eventBuffer->Event.KeyEvent.uChar.AsciiChar) < 128) || int(eventBuffer->Event.KeyEvent.uChar.AsciiChar) == 32) {
                                    a.insertinline(eventBuffer->Event.KeyEvent.uChar.AsciiChar);

                                }
                                else if (int(eventBuffer->Event.KeyEvent.uChar.AsciiChar) == 8) {
                                    a.deleteat();

                                }
                                else if (int(eventBuffer->Event.KeyEvent.uChar.AsciiChar) == 13) {
                                    a.insertinline('\n');
                                }
                                if (a.getcursor() != NULL) {
                                    a.display();
                                    d.printCorrections(a, eventBuffer->Event.KeyEvent.uChar.AsciiChar);
                                    gotoxy(a.getcursor()->x, a.getcursor()->y);
                                }
                                break;
                            }
                        }

                    } // end EventsRead loop

                }

            } // end program loop
        }
        default: {
            if (!flag)
                cout << "Enter a Valid Option." << endl;
            goto selectoption;
        }
    }


    return 0;
}