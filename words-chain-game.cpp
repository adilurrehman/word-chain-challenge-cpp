#include <iostream>
#include <fstream>
#include <conio.h>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <windows.h>
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <atomic>
#include <random>
#include <sstream>
#include <map>

using namespace std;

// Constants
const int SCREEN_WIDTH = 120;
const int SCREEN_HEIGHT = 30;
const int TIMER_SECONDS = 10;

// Arrow key codes
const int KEY_UP = 72;
const int KEY_DOWN = 80;
const int KEY_ENTER = 13;
const int KEY_ESC = 27;

// Global variables
atomic<bool> timerExpired(false);
atomic<bool> timerRunning(false);
atomic<int> remainingTime(TIMER_SECONDS);
HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
CONSOLE_SCREEN_BUFFER_INFO csbi;
HANDLE timerThreadHandle = NULL;
string currentUsername = "";
string player2Name = "";
int currentConsoleWidth = 120;
int currentConsoleHeight = 30;
int centerX = 60;
int centerY = 15;
int high_score = 0;
int hints_used = 0;

// Word dictionaries - loaded from file
map<string, vector<string>> wordDictionaries;
vector<string> animals;
vector<string> countries;
vector<string> fruits;
vector<string> professions;
vector<string> games;
vector<string> cities;
vector<string> digitaldevices;
vector<string> foods;

// Function prototypes
void gotoxy(int x, int y);
void hideConsoleCursor();
void showConsoleCursor();
void setConsoleColor(int color);
void getScreenDimensions();
void setConsoleSize();
void clearScreen();
void printCentered(string text, int y);
void printBox(int x, int y, int width, int height);
void playSound(int frequency, int duration);
void printDeveloperHeader();
void printHeaderMenu();
void printLoginMenu();
void login();
void registerUser();
void showIntroduction();
void showMainMenu();
void showInstructions();
void showHighScores();
void singlePlayerGame();
void twoPlayerGame();
void categorySelectionMenu(string& selectedCategory, vector<string>& selectedDictionary);
void startWordChainGame(bool isTwoPlayer, string category, vector<string> dictionary);
bool validateWord(const string& word, char lastChar, const vector<string>& dictionary, vector<string>& usedWords);
DWORD WINAPI timerThreadFunction(LPVOID lpParam);
void startTimer(int width);
void stopTimer();
void resetTimer();
void updateTimerDisplay(int timeLeft, int width);
void clearScreenArea(int x1, int y1, int x2, int y2);
int getConsoleWidth();
int getConsoleHeight();
void getPlayer2Name();
void saveHighScore(string playerName, int score, string category);
string getInputAtPosition(int x, int y, int maxLength);
void showCursor(bool visible);
string getAdvancedInput(int startX, int y, int fieldWidth, int maxLength, atomic<bool>& timerExpiredFlag);
int getCenteredX(int contentWidth);
int navigateMenu(string options[], int numOptions, string title);
void loadDictionaryFromFile();
void saveDictionaryToFile();
void addWordsFromUserFile();
void showProgressBar();
void showHint(string category, vector<string>& usedWords, char lastChar, vector<string>& dictionary);
void gameOver(int score, string playerName, bool won);
void displayScore(int score);
void addWordManually();

// ==================== UTILITY FUNCTIONS ====================

void gotoxy(int x, int y) {
    COORD coord;
    coord.X = static_cast<SHORT>(x);
    coord.Y = static_cast<SHORT>(y);
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void hideConsoleCursor() {
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(console, &info);
}

void showConsoleCursor() {
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = TRUE;
    SetConsoleCursorInfo(console, &info);
}

void showCursor(bool visible) {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(console, &cursorInfo);
    cursorInfo.bVisible = visible;
    cursorInfo.dwSize = visible ? 25 : 1;
    SetConsoleCursorInfo(console, &cursorInfo);
}

void setConsoleColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void getScreenDimensions() {
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    currentConsoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    currentConsoleHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    centerX = currentConsoleWidth / 2;
    centerY = currentConsoleHeight / 2;
}

int getConsoleWidth() {
    GetConsoleScreenBufferInfo(console, &csbi);
    currentConsoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    return currentConsoleWidth;
}

int getConsoleHeight() {
    GetConsoleScreenBufferInfo(console, &csbi);
    return csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

int getCenteredX(int contentWidth) {
    int width = getConsoleWidth();
    return (width - contentWidth) / 2;
}

void setConsoleSize() {
    HWND console = GetConsoleWindow();
    ShowWindow(console, SW_MAXIMIZE);
    Sleep(100);
    getScreenDimensions();
}

void clearScreen() {
    system("CLS");
    getScreenDimensions();
}

void printCentered(string text, int y) {
    int x = centerX - (text.length() / 2);
    if (x < 0) x = 0;
    gotoxy(x, y);
    cout << text;
}

void printBox(int x, int y, int width, int height) {
    setConsoleColor(11); // Cyan
    gotoxy(x, y);
    cout << char(201);
    for (int i = 0; i < width - 2; i++) cout << char(205);
    cout << char(187);
    
    for (int i = 1; i < height - 1; i++) {
        gotoxy(x, y + i);
        cout << char(186);
        gotoxy(x + width - 1, y + i);
        cout << char(186);
    }
    
    gotoxy(x, y + height - 1);
    cout << char(200);
    for (int i = 0; i < width - 2; i++) cout << char(205);
    cout << char(188);
    setConsoleColor(7);
}

void playSound(int frequency, int duration) {
    Beep(frequency, duration);
}

void clearScreenArea(int x1, int y1, int x2, int y2) {
    for (int y = y1; y <= y2; y++) {
        gotoxy(x1, y);
        for (int x = x1; x <= x2; x++) {
            cout << " ";
        }
    }
}

// ==================== DICTIONARY FILE FUNCTIONS ====================

void loadDictionaryFromFile() {
    ifstream file("wordchain_dictionary.txt");
    if (!file.is_open()) {
        // Create default file if not exists
        setConsoleColor(12);
        printCentered("Dictionary file not found! Creating default...", centerY);
        setConsoleColor(7);
        Sleep(1500);
        return;
    }
    
    string line;
    string currentCategory = "";
    
    // Clear existing vectors
    animals.clear();
    countries.clear();
    fruits.clear();
    professions.clear();
    games.clear();
    cities.clear();
    digitaldevices.clear();
    foods.clear();
    wordDictionaries.clear();
    
    while (getline(file, line)) {
        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        if (line.empty()) continue;
        
        // Check if it's a category header
        if (line[0] == '[' && line[line.length() - 1] == ']') {
            currentCategory = line.substr(1, line.length() - 2);
            // Convert to uppercase
            transform(currentCategory.begin(), currentCategory.end(), currentCategory.begin(), ::toupper);
            continue;
        }
        
        // Convert word to lowercase
        transform(line.begin(), line.end(), line.begin(), ::tolower);
        
        // Add word to appropriate category
        if (currentCategory == "ANIMALS") {
            animals.push_back(line);
        } else if (currentCategory == "COUNTRIES") {
            countries.push_back(line);
        } else if (currentCategory == "FRUITS") {
            fruits.push_back(line);
        } else if (currentCategory == "PROFESSIONS") {
            professions.push_back(line);
        } else if (currentCategory == "GAMES") {
            games.push_back(line);
        } else if (currentCategory == "CITIES") {
            cities.push_back(line);
        } else if (currentCategory == "DIGITALDEVICES") {
            digitaldevices.push_back(line);
        } else if (currentCategory == "FOODS") {
            foods.push_back(line);
        }
        
        // Also add to map
        wordDictionaries[currentCategory].push_back(line);
    }
    
    file.close();
}

void saveDictionaryToFile() {
    ofstream file("wordchain_dictionary.txt");
    if (!file.is_open()) {
        setConsoleColor(12);
        printCentered("Error saving dictionary!", centerY);
        setConsoleColor(7);
        return;
    }
    
    // Save Animals
    file << "[ANIMALS]" << endl;
    for (const auto& word : animals) {
        file << word << endl;
    }
    file << endl;
    
    // Save Countries
    file << "[COUNTRIES]" << endl;
    for (const auto& word : countries) {
        file << word << endl;
    }
    file << endl;
    
    // Save Fruits
    file << "[FRUITS]" << endl;
    for (const auto& word : fruits) {
        file << word << endl;
    }
    file << endl;
    
    // Save Professions
    file << "[PROFESSIONS]" << endl;
    for (const auto& word : professions) {
        file << word << endl;
    }
    file << endl;
    
    // Save Games
    file << "[GAMES]" << endl;
    for (const auto& word : games) {
        file << word << endl;
    }
    file << endl;
    
    // Save Cities
    file << "[CITIES]" << endl;
    for (const auto& word : cities) {
        file << word << endl;
    }
    file << endl;
    
    // Save Digital Devices
    file << "[DIGITALDEVICES]" << endl;
    for (const auto& word : digitaldevices) {
        file << word << endl;
    }
    file << endl;
    
    // Save Foods
    file << "[FOODS]" << endl;
    for (const auto& word : foods) {
        file << word << endl;
    }
    
    file.close();
}

void addWordsFromUserFile() {
    clearScreen();
    printDeveloperHeader();
    
    int startY = 6;
    int boxWidth = 70;
    int startX = centerX - boxWidth / 2;
    
    setConsoleColor(14);
    printBox(startX, startY, boxWidth, 20);
    
    setConsoleColor(15);
    printCentered("=== ADD WORDS FROM FILE ===", startY + 1);
    
    setConsoleColor(7);
    printCentered("Select category to add words to:", startY + 3);
    
    string categories[] = {
        "  ANIMALS          ",
        "  COUNTRIES        ",
        "  FRUITS           ",
        "  PROFESSIONS      ",
        "  GAMES            ",
        "  CITIES           ",
        "  DIGITAL DEVICES  ",
        "  FOODS            ",
        "  BACK TO MENU     "
    };
    
    int choice = navigateMenu(categories, 9, "Select Category:");
    
    if (choice == 8) return;
    
    clearScreen();
    printDeveloperHeader();
    
    setConsoleColor(14);
    printBox(startX, startY, boxWidth, 14);
    
    setConsoleColor(15);
    printCentered("=== ENTER FILE PATH ===", startY + 1);
    
    setConsoleColor(7);
    printCentered("Enter the full path to your .txt file:", startY + 3);
    printCentered("(Words should be one per line)", startY + 4);
    
    setConsoleColor(11);
    printCentered("Example: C:\\Users\\Name\\words.txt", startY + 6);
    
    setConsoleColor(7);
    gotoxy(startX + 5, startY + 8);
    cout << "Path: ";
    
    showConsoleCursor();
    string filePath;
    getline(cin, filePath);
    hideConsoleCursor();
    
    // Try to open the file
    ifstream userFile(filePath);
    if (!userFile.is_open()) {
        setConsoleColor(12);
        printCentered("Error: Could not open file!", startY + 10);
        printCentered("Please check the path and try again.", startY + 11);
        playSound(300, 300);
        Sleep(2000);
        return;
    }
    
    // Read words from file
    vector<string> newWords;
    string word;
    while (getline(userFile, word)) {
        // Trim whitespace
        word.erase(0, word.find_first_not_of(" \t\r\n"));
        word.erase(word.find_last_not_of(" \t\r\n") + 1);
        
        if (!word.empty()) {
            // Convert to lowercase
            transform(word.begin(), word.end(), word.begin(), ::tolower);
            newWords.push_back(word);
        }
    }
    userFile.close();
    
    if (newWords.empty()) {
        setConsoleColor(12);
        printCentered("No valid words found in file!", startY + 10);
        Sleep(2000);
        return;
    }
    
    // Add words to selected category
    int addedCount = 0;
    vector<string>* targetVector = nullptr;
    string categoryName;
    
    switch (choice) {
        case 0: targetVector = &animals; categoryName = "ANIMALS"; break;
        case 1: targetVector = &countries; categoryName = "COUNTRIES"; break;
        case 2: targetVector = &fruits; categoryName = "FRUITS"; break;
        case 3: targetVector = &professions; categoryName = "PROFESSIONS"; break;
        case 4: targetVector = &games; categoryName = "GAMES"; break;
        case 5: targetVector = &cities; categoryName = "CITIES"; break;
        case 6: targetVector = &digitaldevices; categoryName = "DIGITALDEVICES"; break;
        case 7: targetVector = &foods; categoryName = "FOODS"; break;
    }
    
    for (const auto& newWord : newWords) {
        // Check if word already exists
        if (find(targetVector->begin(), targetVector->end(), newWord) == targetVector->end()) {
            targetVector->push_back(newWord);
            addedCount++;
        }
    }
    
    // Save to file
    saveDictionaryToFile();
    
    clearScreen();
    printDeveloperHeader();
    
    setConsoleColor(10);
    printCentered("*** WORDS ADDED SUCCESSFULLY! ***", centerY - 2);
    
    setConsoleColor(15);
    printCentered("Category: " + categoryName, centerY);
    printCentered("Words Added: " + to_string(addedCount), centerY + 1);
    printCentered("Duplicates Skipped: " + to_string(newWords.size() - addedCount), centerY + 2);
    
    playSound(800, 200);
    playSound(1000, 200);
    
    setConsoleColor(8);
    printCentered("Press any key to continue...", centerY + 5);
    _getch();
}

void addWordManually() {
    clearScreen();
    printDeveloperHeader();
    
    int startY = 6;
    int boxWidth = 60;
    int startX = centerX - boxWidth / 2;
    
    setConsoleColor(14);
    printBox(startX, startY, boxWidth, 18);
    
    setConsoleColor(15);
    printCentered("=== ADD WORD MANUALLY ===", startY + 1);
    
    setConsoleColor(7);
    printCentered("Select category:", startY + 3);
    
    string categories[] = {
        "  ANIMALS          ",
        "  COUNTRIES        ",
        "  FRUITS           ",
        "  PROFESSIONS      ",
        "  GAMES            ",
        "  CITIES           ",
        "  DIGITAL DEVICES  ",
        "  FOODS            ",
        "  BACK TO MENU     "
    };
    
    int choice = navigateMenu(categories, 9, "Select Category:");
    
    if (choice == 8) return;
    
    clearScreen();
    printDeveloperHeader();
    
    setConsoleColor(14);
    printBox(startX, startY, boxWidth, 10);
    
    setConsoleColor(15);
    printCentered("=== ENTER NEW WORD ===", startY + 1);
    
    setConsoleColor(7);
    gotoxy(startX + 5, startY + 4);
    cout << "Enter word: ";
    
    showConsoleCursor();
    string newWord;
    cin >> newWord;
    hideConsoleCursor();
    
    // Convert to lowercase
    transform(newWord.begin(), newWord.end(), newWord.begin(), ::tolower);
    
    // Add to appropriate category
    vector<string>* targetVector = nullptr;
    string categoryName;
    
    switch (choice) {
        case 0: targetVector = &animals; categoryName = "ANIMALS"; break;
        case 1: targetVector = &countries; categoryName = "COUNTRIES"; break;
        case 2: targetVector = &fruits; categoryName = "FRUITS"; break;
        case 3: targetVector = &professions; categoryName = "PROFESSIONS"; break;
        case 4: targetVector = &games; categoryName = "GAMES"; break;
        case 5: targetVector = &cities; categoryName = "CITIES"; break;
        case 6: targetVector = &digitaldevices; categoryName = "DIGITALDEVICES"; break;
        case 7: targetVector = &foods; categoryName = "FOODS"; break;
    }
    
    // Check if word already exists
    if (find(targetVector->begin(), targetVector->end(), newWord) != targetVector->end()) {
        setConsoleColor(12);
        printCentered("Word already exists in this category!", startY + 7);
        playSound(300, 300);
    } else {
        targetVector->push_back(newWord);
        saveDictionaryToFile();
        
        setConsoleColor(10);
        printCentered("Word '" + newWord + "' added to " + categoryName + "!", startY + 7);
        playSound(800, 200);
    }
    
    setConsoleColor(8);
    printCentered("Press any key to continue...", startY + 9);
    _getch();
}

// ==================== UI FUNCTIONS ====================

void printDeveloperHeader() {
    getScreenDimensions();
    int boxWidth = 60;
    int startX = centerX - boxWidth / 2;
    
    setConsoleColor(14);
    printBox(startX, 0, boxWidth, 5);
    setConsoleColor(15);
    printCentered("W O R D   C H A I N   C H A L L E N G E", 1);
    setConsoleColor(13);
    printCentered("ULTIMATE VOCABULARY SHOWDOWN", 2);
    setConsoleColor(10);
    printCentered("Developed by: Adil ur Rehman | Roll: 24-CS-760", 3);
    setConsoleColor(7);
}

void printHeaderMenu() {
    clearScreen();
    getScreenDimensions();
    
    int boxWidth = 60;
    int startX = centerX - boxWidth / 2;
    int startY = centerY - 8;
    
    setConsoleColor(14);
    printBox(startX, startY, boxWidth, 10);
    
    setConsoleColor(15);
    printCentered("**************************************************", startY + 1);
    setConsoleColor(11);
    printCentered("*        W O R D   C H A I N   C H A L L E N G E  *", startY + 2);
    setConsoleColor(13);
    printCentered("*          ULTIMATE VOCABULARY SHOWDOWN          *", startY + 3);
    setConsoleColor(10);
    printCentered("*           Developed by: Adil ur Rehman         *", startY + 4);
    printCentered("*               Roll No: 24-CS-760               *", startY + 5);
    setConsoleColor(15);
    printCentered("**************************************************", startY + 6);
    
    setConsoleColor(12);
    printCentered("Loading Game", startY + 9);
    
    // Animated loading bar
    int barWidth = 40;
    int barX = centerX - barWidth / 2;
    gotoxy(barX, startY + 10);
    setConsoleColor(8);
    cout << "[";
    for (int i = 0; i < barWidth - 2; i++) cout << " ";
    cout << "]";
    
    for (int i = 0; i < barWidth - 2; i++) {
        gotoxy(barX + 1 + i, startY + 10);
        setConsoleColor(10);
        cout << char(219);
        cout.flush();  // Force immediate display before sound
        playSound(200 + i * 15, 15);
        Sleep(15);
    }
    
    setConsoleColor(10);
    printCentered("PRESS ANY KEY TO CONTINUE...", startY + 13);
    setConsoleColor(7);
    _getch();
}

void printLoginMenu() {
    clearScreen();
    printDeveloperHeader();
    
    int boxWidth = 50;
    int startX = centerX - boxWidth / 2;
    int startY = 7;
    
    setConsoleColor(14);
    printBox(startX, startY, boxWidth, 6);
    
    setConsoleColor(15);
    printCentered("Welcome To Login Page", startY + 2);
    setConsoleColor(11);
    printCentered("Enter Username and Password", startY + 3);
    setConsoleColor(7);
    Sleep(500);
}

int navigateMenu(string options[], int numOptions, string title) {
    int selected = 0;
    int key;
    
    int boxWidth = 40;
    int startX = centerX - boxWidth / 2;
    int startY = 14;
    
    while (true) {
        setConsoleColor(14);
        printCentered(title, startY - 2);
        setConsoleColor(7);
        
        printBox(startX - 2, startY - 1, boxWidth + 4, numOptions + 2);
        
        for (int i = 0; i < numOptions; i++) {
            gotoxy(startX, startY + i);
            
            if (i == selected) {
                setConsoleColor(0 | (14 << 4)); // Black on yellow
                cout << ">" << options[i] << "<";
                setConsoleColor(7);
            } else {
                setConsoleColor(7);
                cout << " " << options[i] << " ";
            }
        }
        
        setConsoleColor(8);
        printCentered("[UP/DOWN: Navigate | ENTER: Select]", startY + numOptions + 2);
        setConsoleColor(7);
        
        key = _getch();
        
        if (key == 0 || key == 224) {
            key = _getch();
            
            if (key == KEY_UP) {
                selected = (selected - 1 + numOptions) % numOptions;
                playSound(700, 20);
            }
            else if (key == KEY_DOWN) {
                selected = (selected + 1) % numOptions;
                playSound(700, 20);
            }
        }
        else if (key == KEY_ENTER) {
            playSound(1000, 50);
            return selected;
        }
        else if (key == KEY_ESC) {
            return numOptions - 1; // Return last option (usually exit/back)
        }
    }
}

void displayScore(int score) {
    int scoreX = currentConsoleWidth - 25;
    
    setConsoleColor(14);
    gotoxy(scoreX, 0);
    cout << "Score: " << score;
    gotoxy(scoreX, 1);
    setConsoleColor(11);
    cout << "High Score: " << high_score;
    setConsoleColor(7);
}

void showProgressBar(int wordsUsed, int totalWords) {
    int barWidth = 20;
    int startX = 2;
    int startY = 0;
    
    setConsoleColor(14);
    gotoxy(startX, startY);
    cout << "Words: [";
    
    int maxDisplay = min(totalWords, 100);
    int filled = (wordsUsed * barWidth) / maxDisplay;
    if (filled > barWidth) filled = barWidth;
    
    setConsoleColor(10);
    for (int i = 0; i < filled; i++) cout << char(219);
    setConsoleColor(8);
    for (int i = filled; i < barWidth; i++) cout << char(176);
    
    setConsoleColor(14);
    cout << "] " << wordsUsed;
    setConsoleColor(7);
}

void showHint(string category, vector<string>& usedWords, char lastChar, vector<string>& dictionary) {
    clearScreen();
    printDeveloperHeader();
    
    int startY = 6;
    int boxWidth = 60;
    int startX = centerX - boxWidth / 2;
    
    setConsoleColor(14);
    printBox(startX, startY, boxWidth, 16);
    
    setConsoleColor(15);
    printCentered("=== HINT SYSTEM ===", startY + 1);
    
    setConsoleColor(7);
    gotoxy(startX + 3, startY + 3);
    cout << "Category: " << category;
    
    gotoxy(startX + 3, startY + 4);
    cout << "Looking for words starting with: " << char(toupper(lastChar));
    
    // Find some valid words as hints
    vector<string> hints;
    for (const auto& word : dictionary) {
        if (tolower(word[0]) == tolower(lastChar)) {
            if (find(usedWords.begin(), usedWords.end(), word) == usedWords.end()) {
                hints.push_back(word);
                if (hints.size() >= 5) break;
            }
        }
    }
    
    setConsoleColor(11);
    gotoxy(startX + 3, startY + 6);
    cout << "Available words starting with '" << char(toupper(lastChar)) << "':";
    
    if (hints.empty()) {
        setConsoleColor(12);
        gotoxy(startX + 5, startY + 8);
        cout << "No more words available with this letter!";
    } else {
        for (int i = 0; i < hints.size(); i++) {
            setConsoleColor(10);
            gotoxy(startX + 5, startY + 8 + i);
            // Show partial hint (first 3 letters + ...)
            if (hints[i].length() > 3) {
                cout << (i + 1) << ". " << hints[i].substr(0, 3) << "...";
            } else {
                cout << (i + 1) << ". " << hints[i];
            }
        }
    }
    
    hints_used++;
    if (hints_used > 3) {
        setConsoleColor(12);
        gotoxy(startX + 3, startY + 14);
        cout << "(-5 points penalty for hint)";
    }
    
    setConsoleColor(8);
    printCentered("Press any key to continue...", startY + 16);
    setConsoleColor(7);
    _getch();
}

// ==================== AUTH FUNCTIONS ====================

void login() {
    // Show login/register choice first
    clearScreen();
    printDeveloperHeader();
    
    int boxWidth = 50;
    int startX = centerX - boxWidth / 2;
    int startY = 7;
    
    setConsoleColor(14);
    printBox(startX, startY, boxWidth, 6);
    
    setConsoleColor(15);
    printCentered("Welcome To Word Chain Challenge", startY + 2);
    setConsoleColor(11);
    printCentered("Login or Create New Account", startY + 3);
    setConsoleColor(7);
    
    string authOptions[] = {
        "  Login (Existing User)   ",
        "  Sign Up (New User)      "
    };
    
    int authChoice = navigateMenu(authOptions, 2, "Select Option:");
    
    if (authChoice == 1) {
        // User chose to register
        registerUser();
        return;
    }
    
    // User chose to login
    int attempts = 3;
    while (attempts > 0) {
        printLoginMenu();
        
        string username, password, id, pass, score_str;
        bool found = false;
        
        int inputY = 14;
        startX = centerX - 20;
        
        setConsoleColor(14);
        gotoxy(startX, inputY);
        cout << "=== LOGIN ===";
        setConsoleColor(7);
        
        gotoxy(startX, inputY + 2);
        cout << "Enter Username: ";
        showConsoleCursor();
        cin >> username;
        
        gotoxy(startX, inputY + 3);
        cout << "Enter Password: ";
        
        // Password masking
        password = "";
        char ch;
        while ((ch = _getch()) != 13) {
            if (ch == 8 && password.length() > 0) {
                password.pop_back();
                cout << "\b \b";
            } else if (ch != 8) {
                password.push_back(ch);
                cout << "*";
            }
        }
        cout << endl;
        hideConsoleCursor();
        
        ifstream loginFile("wordchain_login.txt");
        while (loginFile >> id >> pass >> score_str) {
            if (id == username && pass == password) {
                found = true;
                currentUsername = username;
                try {
                    high_score = stoi(score_str);
                } catch (...) {
                    high_score = 0;  // Default to 0 if score is invalid
                }
                break;
            }
        }
        loginFile.close();
        
        if (found) {
            setConsoleColor(10);
            gotoxy(startX, inputY + 5);
            cout << "Welcome back, " << username << "!";
            gotoxy(startX, inputY + 6);
            cout << "Your High Score: " << high_score;
            playSound(800, 200);
            playSound(1000, 200);
            Sleep(1500);
            return;
        } else {
            attempts--;
            setConsoleColor(12);
            gotoxy(startX, inputY + 5);
            cout << "Invalid credentials! Attempts left: " << attempts;
            playSound(300, 300);
            
            if (attempts > 0) {
                gotoxy(startX, inputY + 7);
                cout << "Press any key to try again...";
                _getch();
            }
        }
    }
    
    setConsoleColor(14);
    printCentered("Maximum attempts reached! Please register.", centerY + 2);
    printCentered("Press any key to register...", centerY + 4);
    _getch();
    registerUser();
}

void registerUser() {
    clearScreen();
    printDeveloperHeader();
    
    string username, password, id, pass, score_str;
    bool usernameExists = false;
    
    int inputY = 10;
    int startX = centerX - 20;
    
    setConsoleColor(14);
    gotoxy(startX, inputY);
    cout << "=== REGISTER ===";
    setConsoleColor(7);
    
    do {
        usernameExists = false;
        gotoxy(startX, inputY + 2);
        cout << "Choose Username: ";
        showConsoleCursor();
        cin >> username;
        
        ifstream loginFile("wordchain_login.txt");
        while (loginFile >> id >> pass >> score_str) {
            if (id == username) {
                usernameExists = true;
                setConsoleColor(12);
                gotoxy(startX, inputY + 4);
                cout << "Username already taken! Try another.";
                playSound(300, 200);
                Sleep(1000);
                // Clear the message
                gotoxy(startX, inputY + 4);
                cout << "                                      ";
                gotoxy(startX, inputY + 2);
                cout << "Choose Username:                       ";
                break;
            }
        }
        loginFile.close();
    } while (usernameExists);
    
    gotoxy(startX, inputY + 3);
    cout << "Choose Password: ";
    
    password = "";
    char ch;
    while ((ch = _getch()) != 13) {
        if (ch == 8 && password.length() > 0) {
            password.pop_back();
            cout << "\b \b";
        } else if (ch != 8) {
            password.push_back(ch);
            cout << "*";
        }
    }
    cout << endl;
    hideConsoleCursor();
    
    ofstream loginFile("wordchain_login.txt", ios::app);
    loginFile << username << " " << password << " " << 0 << endl;
    loginFile.close();
    
    currentUsername = username;
    high_score = 0;
    
    setConsoleColor(10);
    gotoxy(startX, inputY + 5);
    cout << "Registration Successful!";
    playSound(600, 150);
    playSound(800, 150);
    playSound(1000, 200);
    
    gotoxy(startX, inputY + 6);
    cout << "Redirecting to game...";
    Sleep(1500);
}

// ==================== GAME INTRO & MENUS ====================

void showIntroduction() {
    clearScreen();
    printDeveloperHeader();
    
    int startY = 6;
    int boxWidth = 70;
    int startX = centerX - boxWidth / 2;
    
    setConsoleColor(14);
    printBox(startX, startY, boxWidth, 16);
    
    setConsoleColor(15);
    printCentered("=== WELCOME TO WORD CHAIN CHALLENGE ===", startY + 2);
    
    setConsoleColor(7);
    string story[] = {
        "Test your vocabulary in this exciting word chain game!",
        "",
        "You will be given a starting word, and you must",
        "enter a word that starts with the last letter",
        "of the previous word.",
        "",
        "Game Features:",
        "  > Multiple word categories to choose from",
        "  > Single player and two-player modes",
        "  > Timed rounds - think fast!",
        "  > Add your own words to the dictionary!"
    };
    
    for (int i = 0; i < 11; i++) {
        printCentered(story[i], startY + 3 + i);
        Sleep(80);
    }
    
    setConsoleColor(10);
    printCentered("Use ARROW KEYS to navigate, ENTER to select", startY + 16);
    setConsoleColor(14);
    printCentered("Press any key to continue...", startY + 18);
    setConsoleColor(7);
    
    playSound(600, 200);
    _getch();
}

void showInstructions() {
    clearScreen();
    printDeveloperHeader();
    
    int startY = 6;
    int boxWidth = 70;
    int startX = centerX - boxWidth / 2;
    
    setConsoleColor(14);
    printBox(startX, startY, boxWidth, 20);
    
    setConsoleColor(15);
    printCentered("=== HOW TO PLAY ===", startY + 1);
    
    setConsoleColor(7);
    printCentered("1. Choose game mode (Single or Two Player)", startY + 3);
    printCentered("2. Select a word category", startY + 4);
    printCentered("3. Start with a given word", startY + 5);
    printCentered("4. Enter a word starting with the last letter", startY + 6);
    printCentered("5. Each player has 10 seconds per turn", startY + 7);
    
    setConsoleColor(11);
    printCentered("=== RULES ===", startY + 9);
    
    setConsoleColor(7);
    printCentered("* Words must be from selected category", startY + 10);
    printCentered("* No word repetitions allowed", startY + 11);
    printCentered("* Each valid word = 10 points", startY + 12);
    printCentered("* Invalid word = lose a life", startY + 13);
    printCentered("* Time's up = lose a life", startY + 14);
    printCentered("* Each player starts with 3 lives", startY + 15);
    
    setConsoleColor(10);
    printCentered("=== SCORING ===", startY + 17);
    setConsoleColor(7);
    printCentered("Valid Word: +10 | Win Bonus: +50 | Hint Penalty: -5", startY + 18);
    
    setConsoleColor(8);
    printCentered("Press any key to continue...", startY + 20);
    setConsoleColor(7);
    _getch();
}

void showMainMenu() {
    while (true) {
        clearScreen();
        printDeveloperHeader();
        displayScore(0);
        
        setConsoleColor(14);
        int boxWidth = 50;
        int startX = centerX - boxWidth / 2;
        printBox(startX, 8, boxWidth, 6);
        
        setConsoleColor(15);
        printCentered("=== MAIN MENU ===", 10);
        setConsoleColor(11);
        printCentered("Welcome, " + currentUsername + "!", 12);
        setConsoleColor(7);
        
        string menuOptions[] = {
            "  Single Player (vs Computer)  ",
            "  Two Players                  ",
            "  View Instructions            ",
            "  View High Scores             ",
            "  Add Words from File          ",
            "  Add Word Manually            ",
            "  Exit Game                    "
        };
        
        int choice = navigateMenu(menuOptions, 7, "Select Option:");
        
        switch (choice) {
            case 0:
                singlePlayerGame();
                break;
            case 1:
                twoPlayerGame();
                break;
            case 2:
                showInstructions();
                break;
            case 3:
                showHighScores();
                break;
            case 4:
                addWordsFromUserFile();
                break;
            case 5:
                addWordManually();
                break;
            case 6:
                clearScreen();
                printDeveloperHeader();
                setConsoleColor(10);
                printCentered("T H A N K S   F O R   P L A Y I N G !", centerY);
                setConsoleColor(14);
                printCentered("See you next time!", centerY + 2);
                playSound(523, 200);
                playSound(659, 200);
                playSound(784, 400);
                Sleep(2000);
                exit(0);
        }
    }
}

void showHighScores() {
    clearScreen();
    printDeveloperHeader();
    
    int startY = 6;
    int boxWidth = 70;
    int startX = centerX - boxWidth / 2;
    
    setConsoleColor(14);
    printBox(startX, startY, boxWidth, 18);
    
    setConsoleColor(15);
    printCentered("=== HIGH SCORES ===", startY + 1);
    
    ifstream highscore("wordchain_highscores.txt");
    if (highscore.is_open()) {
        string line;
        int y = startY + 3;
        int count = 0;
        while (getline(highscore, line) && count < 12) {
            setConsoleColor(count < 3 ? 14 : 7); // Top 3 in yellow
            printCentered(line, y++);
            count++;
        }
        highscore.close();
        
        if (count == 0) {
            setConsoleColor(8);
            printCentered("No high scores yet! Be the first!", startY + 8);
        }
    } else {
        setConsoleColor(8);
        printCentered("No high scores yet! Be the first!", startY + 8);
    }
    
    setConsoleColor(8);
    printCentered("Press any key to continue...", startY + 17);
    setConsoleColor(7);
    _getch();
}

// ==================== GAME FUNCTIONS ====================

void categorySelectionMenu(string& selectedCategory, vector<string>& selectedDictionary) {
    clearScreen();
    printDeveloperHeader();
    
    setConsoleColor(14);
    int boxWidth = 50;
    int startX = centerX - boxWidth / 2;
    printBox(startX, 6, boxWidth, 6);
    
    setConsoleColor(15);
    printCentered("=== SELECT WORD CATEGORY ===", 8);
    setConsoleColor(11);
    printCentered("Choose your challenge!", 10);
    setConsoleColor(7);
    
    string categories[] = {
        "  ANIMALS          ",
        "  COUNTRIES        ",
        "  FRUITS           ",
        "  PROFESSIONS      ",
        "  GAMES            ",
        "  CITIES           ",
        "  DIGITAL DEVICES  ",
        "  FOODS            ",
        "  MIXED (All)      "
    };
    
    int choice = navigateMenu(categories, 9, "Select Category:");
    
    switch (choice) {
        case 0:
            selectedCategory = "ANIMALS";
            selectedDictionary = animals;
            break;
        case 1:
            selectedCategory = "COUNTRIES";
            selectedDictionary = countries;
            break;
        case 2:
            selectedCategory = "FRUITS";
            selectedDictionary = fruits;
            break;
        case 3:
            selectedCategory = "PROFESSIONS";
            selectedDictionary = professions;
            break;
        case 4:
            selectedCategory = "GAMES";
            selectedDictionary = games;
            break;
        case 5:
            selectedCategory = "CITIES";
            selectedDictionary = cities;
            break;
        case 6:
            selectedCategory = "DIGITAL DEVICES";
            selectedDictionary = digitaldevices;
            break;
        case 7:
            selectedCategory = "FOODS";
            selectedDictionary = foods;
            break;
        case 8:
            selectedCategory = "MIXED";
            selectedDictionary.clear();
            selectedDictionary.insert(selectedDictionary.end(), animals.begin(), animals.end());
            selectedDictionary.insert(selectedDictionary.end(), countries.begin(), countries.end());
            selectedDictionary.insert(selectedDictionary.end(), fruits.begin(), fruits.end());
            selectedDictionary.insert(selectedDictionary.end(), professions.begin(), professions.end());
            selectedDictionary.insert(selectedDictionary.end(), games.begin(), games.end());
            selectedDictionary.insert(selectedDictionary.end(), cities.begin(), cities.end());
            selectedDictionary.insert(selectedDictionary.end(), digitaldevices.begin(), digitaldevices.end());
            selectedDictionary.insert(selectedDictionary.end(), foods.begin(), foods.end());
            break;
    }
    
    clearScreen();
    printDeveloperHeader();
    setConsoleColor(10);
    printCentered("Category Selected: " + selectedCategory, centerY);
    printCentered("Total Words: " + to_string(selectedDictionary.size()), centerY + 1);
    playSound(800, 100);
    Sleep(1500);
}

bool validateWord(const string& word, char lastChar, const vector<string>& dictionary, vector<string>& usedWords) {
    if (word.empty()) return false;
    
    // Check if word starts with the correct letter
    if (tolower(word[0]) != tolower(lastChar)) return false;
    
    // Check if word is in dictionary
    string lowerWord = word;
    transform(lowerWord.begin(), lowerWord.end(), lowerWord.begin(), ::tolower);
    
    if (find(dictionary.begin(), dictionary.end(), lowerWord) == dictionary.end()) return false;
    
    // Check if word has already been used
    if (find(usedWords.begin(), usedWords.end(), lowerWord) != usedWords.end()) return false;
    
    return true;
}

void getPlayer2Name() {
    clearScreen();
    printDeveloperHeader();
    
    int startY = 10;
    int boxWidth = 50;
    int startX = centerX - boxWidth / 2;
    
    setConsoleColor(14);
    printBox(startX, startY, boxWidth, 8);
    
    setConsoleColor(15);
    printCentered("=== ENTER PLAYER 2 NAME ===", startY + 1);
    
    setConsoleColor(7);
    printCentered("Player 1: " + currentUsername, startY + 3);
    
    gotoxy(startX + 5, startY + 5);
    cout << "Player 2 Name: ";
    
    showConsoleCursor();
    cin >> player2Name;
    hideConsoleCursor();
    
    if (player2Name.empty()) {
        player2Name = "Player 2";
    }
    
    setConsoleColor(10);
    printCentered("Welcome " + player2Name + "!", startY + 7);
    playSound(800, 200);
    Sleep(1500);
}

// Timer functions
DWORD WINAPI timerThreadFunction(LPVOID lpParam) {
    int width = *((int*)lpParam);
    remainingTime = TIMER_SECONDS;
    timerExpired = false;
    timerRunning = true;
    
    updateTimerDisplay(remainingTime, width);
    
    while (remainingTime > 0 && timerRunning) {
        Sleep(1000);
        
        if (timerRunning) {
            remainingTime--;
            updateTimerDisplay(remainingTime, width);
        }
    }
    
    if (remainingTime == 0 && timerRunning) {
        timerExpired = true;
        setConsoleColor(12);
        gotoxy(width / 2 - 5, 21);
        cout << "TIME'S UP!";
        playSound(300, 300);
        setConsoleColor(7);
    }
    
    timerRunning = false;
    return 0;
}

void startTimer(int width) {
    stopTimer();
    resetTimer();
    
    int* widthPtr = new int(width);
    timerThreadHandle = CreateThread(NULL, 0, timerThreadFunction, widthPtr, 0, NULL);
}

void stopTimer() {
    timerRunning = false;
    if (timerThreadHandle != NULL) {
        WaitForSingleObject(timerThreadHandle, 100);
        CloseHandle(timerThreadHandle);
        timerThreadHandle = NULL;
    }
}

void resetTimer() {
    stopTimer();
    timerExpired = false;
    remainingTime = TIMER_SECONDS;
}

void updateTimerDisplay(int timeLeft, int width) {
    if (timeLeft <= 3) {
        setConsoleColor(12); // Red
        playSound(400, 100);
    } else if (timeLeft <= 5) {
        setConsoleColor(14); // Yellow
    } else {
        setConsoleColor(10); // Green
    }
    
    gotoxy(width - 20, 2);
    cout << "Time: " << setw(2) << setfill('0') << timeLeft << "s ";
    setConsoleColor(7);
}

string getAdvancedInput(int startX, int y, int fieldWidth, int maxLength, atomic<bool>& timerExpiredFlag) {
    vector<char> buffer;
    int scrollOffset = 0;
    int cursor = 0;
    
    for (int i = 0; i < fieldWidth; i++) {
        gotoxy(startX + i, y);
        cout << " ";
    }
    
    CONSOLE_CURSOR_INFO ci;
    GetConsoleCursorInfo(console, &ci);
    ci.bVisible = true;
    ci.dwSize = 20;
    SetConsoleCursorInfo(console, &ci);
    
    while (!timerExpiredFlag) {
        if (_kbhit()) {
            char c = _getch();
            
            if (c == '\r') break;
            
            if (c == '\b') {
                if (cursor > 0) {
                    cursor--;
                    buffer.erase(buffer.begin() + cursor);
                    if (scrollOffset > 0 && cursor < scrollOffset)
                        scrollOffset--;
                    playSound(600, 20);
                }
            }
            else if (isalpha(c) && buffer.size() < maxLength) {
                buffer.insert(buffer.begin() + cursor, tolower(c));
                cursor++;
                if (cursor - scrollOffset > fieldWidth - 1)
                    scrollOffset++;
                playSound(800, 15);
            }
            
            for (int i = 0; i < fieldWidth; i++) {
                gotoxy(startX + i, y);
                if (i + scrollOffset < buffer.size())
                    cout << buffer[i + scrollOffset];
                else
                    cout << " ";
            }
            
            int cursorX = startX + (cursor - scrollOffset);
            if (cursorX < startX) cursorX = startX;
            if (cursorX > startX + fieldWidth - 1)
                cursorX = startX + fieldWidth - 1;
            
            gotoxy(cursorX, y);
            
            gotoxy(startX + fieldWidth + 2, y);
            setConsoleColor(buffer.size() >= maxLength ? 12 : 8);
            cout << buffer.size() << "/" << maxLength;
            setConsoleColor(7);
        }
    }
    
    ci.bVisible = false;
    SetConsoleCursorInfo(console, &ci);
    
    return string(buffer.begin(), buffer.end());
}

void saveHighScore(string playerName, int score, string category) {
    // Update login file if high score
    if (score > high_score) {
        high_score = score;
        
        string content = "";
        string id, pass, scoreStr;
        
        ifstream inFile("wordchain_login.txt");
        while (inFile >> id >> pass >> scoreStr) {
            if (id == currentUsername) {
                content += id + " " + pass + " " + to_string(high_score) + "\n";
            } else {
                content += id + " " + pass + " " + scoreStr + "\n";
            }
        }
        inFile.close();
        
        ofstream outFile("wordchain_login.txt");
        outFile << content;
        outFile.close();
    }
    
    // Save to highscores file
    ofstream highscore("wordchain_highscores.txt", ios::app);
    time_t now = time(nullptr);
    char* buffer = ctime(&now);
    string dateStr = buffer ? string(buffer) : "";
    if (!dateStr.empty() && dateStr.back() == '\n') dateStr.pop_back();
    highscore << playerName << " | Score: " << score << " | " << category << " | " << dateStr << endl;
    highscore.close();
}

void gameOver(int score, string playerName, bool won) {
    clearScreen();
    getScreenDimensions();
    
    int startY = centerY - 10;
    
    setConsoleColor(won ? 10 : 12);
    printCentered("  ________                        ________                     ", startY);
    printCentered(" /  _____/_____    _____   ____   \\_____  \\___  __ ___________ ", startY + 1);
    printCentered("/   \\  ___\\__  \\  /     \\_/ __ \\   /  ____/\\  \\/ // __ \\_  __ \\", startY + 2);
    printCentered("\\    \\_\\  \\/ __ \\|  Y Y  \\  ___/  /       \\ \\   /\\  ___/|  | \\/", startY + 3);
    printCentered(" \\______  (____  /__|_|  /\\___  > \\_______ \\ \\_/  \\___  >__|   ", startY + 4);
    printCentered("        \\/     \\/      \\/     \\/          \\/         \\/        ", startY + 5);
    
    setConsoleColor(14);
    printCentered("Player: " + playerName, startY + 7);
    printCentered("Final Score: " + to_string(score), startY + 8);
    
    if (score > high_score) {
        setConsoleColor(10);
        printCentered("*** NEW HIGH SCORE! ***", startY + 10);
        playSound(523, 200);
        playSound(659, 200);
        playSound(784, 200);
        playSound(1047, 400);
    } else {
        setConsoleColor(11);
        printCentered("High Score: " + to_string(high_score), startY + 10);
    }
    
    setConsoleColor(8);
    printCentered("Press any key to continue...", startY + 14);
    setConsoleColor(7);
    _getch();
}

void startWordChainGame(bool isTwoPlayer, string category, vector<string> dictionary) {
    int player1Score = 0, player2Score = 0;
    int player1Lives = 3, player2Lives = 3;
    int round = 1;
    int maxRounds = 20;
    vector<string> usedWords;
    string currentWord;
    hints_used = 0;
    
    // Get starting word
    srand(static_cast<unsigned int>(time(nullptr)));
    int randomIndex = rand() % dictionary.size();
    currentWord = dictionary[randomIndex];
    usedWords.push_back(currentWord);
    
    string player1Name = currentUsername;
    string player2NameLocal = isTwoPlayer ? player2Name : "COMPUTER";
    
    while (round <= maxRounds && player1Lives > 0 && (isTwoPlayer ? player2Lives > 0 : true)) {
        clearScreen();
        printDeveloperHeader();
        
        int width = getConsoleWidth();
        
        // Display game info
        showProgressBar(usedWords.size(), dictionary.size());
        displayScore(player1Score);
        
        setConsoleColor(11);
        gotoxy(5, 2);
        cout << player1Name << ": " << player1Score << " pts | Lives: ";
        for (int i = 0; i < player1Lives; i++) cout << char(3) << " ";
        
        setConsoleColor(isTwoPlayer ? 13 : 10);
        gotoxy(width - 40, 2);
        cout << player2NameLocal << ": " << player2Score << " pts";
        if (isTwoPlayer) {
            cout << " | Lives: ";
            for (int i = 0; i < player2Lives; i++) cout << char(3) << " ";
        }
        
        setConsoleColor(14);
        printCentered("ROUND " + to_string(round) + "/" + to_string(maxRounds) + " | " + category, 4);
        
        // Current word display
        setConsoleColor(15);
        printCentered("Current Word: " + currentWord, 8);
        
        char lastChar = currentWord.back();
        setConsoleColor(11);
        printCentered("Next word must start with: [ " + string(1, char(toupper(lastChar))) + " ]", 10);
        
        // Player 1 turn
        setConsoleColor(14);
        printCentered(player1Name + "'s Turn", 13);
        setConsoleColor(7);
        
        // Draw input box
        int boxWidth = 50;
        int boxX = (width - boxWidth) / 2;
        printBox(boxX, 15, boxWidth, 5);
        
        gotoxy(boxX + 3, 17);
        cout << "Enter word: ";
        
        int inputFieldX = boxX + 15;
        
        // Start timer
        startTimer(width);
        
        string player1Word = getAdvancedInput(inputFieldX, 17, 25, 40, timerExpired);
        
        stopTimer();
        
        // Handle result
        gotoxy(boxX + 10, 19);
        if (timerExpired) {
            setConsoleColor(12);
            cout << "TIME'S UP! Lost a life.";
            player1Lives--;
            playSound(300, 300);
            Sleep(1500);
        } else if (validateWord(player1Word, lastChar, dictionary, usedWords)) {
            setConsoleColor(10);
            cout << "VALID! +10 points";
            player1Score += 10;
            currentWord = player1Word;
            usedWords.push_back(player1Word);
            playSound(800, 100);
            Sleep(1000);
        } else {
            setConsoleColor(12);
            cout << "INVALID! Lost a life.";
            player1Lives--;
            playSound(300, 200);
            Sleep(1500);
        }
        
        // Computer or Player 2 turn
        if (player1Lives > 0 && round < maxRounds) {
            if (!isTwoPlayer) {
                // Computer turn
                clearScreen();
                printDeveloperHeader();
                showProgressBar(usedWords.size(), dictionary.size());
                displayScore(player1Score);
                
                setConsoleColor(10);
                printCentered("COMPUTER's Turn", 13);
                setConsoleColor(7);
                
                printCentered("Current Word: " + currentWord, 8);
                lastChar = currentWord.back();
                printCentered("Looking for word starting with: [ " + string(1, char(toupper(lastChar))) + " ]", 10);
                
                printBox(boxX, 15, boxWidth, 5);
                gotoxy(boxX + 3, 17);
                cout << "Computer is thinking...";
                
                Sleep(1500);
                
                string computerWord = "";
                for (const auto& word : dictionary) {
                    if (validateWord(word, lastChar, dictionary, usedWords)) {
                        computerWord = word;
                        break;
                    }
                }
                
                gotoxy(boxX + 3, 18);
                if (!computerWord.empty()) {
                    setConsoleColor(10);
                    cout << "Computer chose: " << computerWord;
                    player2Score += 10;
                    currentWord = computerWord;
                    usedWords.push_back(computerWord);
                    playSound(600, 100);
                } else {
                    setConsoleColor(12);
                    cout << "Computer couldn't find a word!";
                }
                Sleep(2000);
            } else if (player2Lives > 0) {
                // Player 2 turn
                clearScreen();
                printDeveloperHeader();
                
                setConsoleColor(13);
                printCentered(player2NameLocal + "'s Turn", 13);
                setConsoleColor(7);
                
                printCentered("Current Word: " + currentWord, 8);
                lastChar = currentWord.back();
                printCentered("Enter word starting with: [ " + string(1, char(toupper(lastChar))) + " ]", 10);
                
                printBox(boxX, 15, boxWidth, 5);
                gotoxy(boxX + 3, 17);
                cout << "Enter word: ";
                
                resetTimer();
                startTimer(width);
                
                string player2Word = getAdvancedInput(inputFieldX, 17, 25, 40, timerExpired);
                
                stopTimer();
                
                gotoxy(boxX + 10, 19);
                if (timerExpired) {
                    setConsoleColor(12);
                    cout << "TIME'S UP! Lost a life.";
                    player2Lives--;
                    playSound(300, 300);
                    Sleep(1500);
                } else if (validateWord(player2Word, lastChar, dictionary, usedWords)) {
                    setConsoleColor(10);
                    cout << "VALID! +10 points";
                    player2Score += 10;
                    currentWord = player2Word;
                    usedWords.push_back(player2Word);
                    playSound(800, 100);
                    Sleep(1000);
                } else {
                    setConsoleColor(12);
                    cout << "INVALID! Lost a life.";
                    player2Lives--;
                    playSound(300, 200);
                    Sleep(1500);
                }
            }
        }
        
        round++;
        
        if (round <= maxRounds && player1Lives > 0 && (isTwoPlayer ? player2Lives > 0 : true)) {
            setConsoleColor(8);
            for (int i = 3; i > 0; i--) {
                printCentered("Next round in " + to_string(i) + "...", 23);
                playSound(500, 100);
                Sleep(1000);
            }
        }
    }
    
    stopTimer();
    
    // Game over
    clearScreen();
    printDeveloperHeader();
    
    setConsoleColor(14);
    printCentered("+==========================================+", 8);
    printCentered("|             GAME OVER!                   |", 9);
    printCentered("+==========================================+", 10);
    
    setConsoleColor(11);
    printCentered(player1Name + ": " + to_string(player1Score) + " points", 12);
    setConsoleColor(isTwoPlayer ? 13 : 10);
    printCentered(player2NameLocal + ": " + to_string(player2Score) + " points", 13);
    
    // Save high score
    if (!isTwoPlayer) {
        saveHighScore(currentUsername, player1Score, category);
    }
    
    // Determine winner
    setConsoleColor(14);
    if (player1Score > player2Score) {
        printCentered(player1Name + " WINS!", 15);
        if (!isTwoPlayer) {
            player1Score += 50; // Win bonus
            printCentered("+50 Win Bonus!", 16);
        }
        playSound(800, 200);
        playSound(1000, 200);
        playSound(1200, 300);
    } else if (player2Score > player1Score) {
        printCentered(player2NameLocal + " WINS!", 15);
    } else {
        printCentered("IT'S A TIE!", 15);
    }
    
    setConsoleColor(7);
    printCentered("Words Used: " + to_string(usedWords.size()), 18);
    
    setConsoleColor(8);
    printCentered("Press any key to continue...", 22);
    _getch();
    
    gameOver(player1Score, player1Name, player1Score >= player2Score);
}

void singlePlayerGame() {
    string selectedCategory;
    vector<string> selectedDictionary;
    
    categorySelectionMenu(selectedCategory, selectedDictionary);
    startWordChainGame(false, selectedCategory, selectedDictionary);
}

void twoPlayerGame() {
    if (player2Name.empty()) {
        getPlayer2Name();
    }
    
    string selectedCategory;
    vector<string> selectedDictionary;
    
    categorySelectionMenu(selectedCategory, selectedDictionary);
    startWordChainGame(true, selectedCategory, selectedDictionary);
}

// ==================== MAIN FUNCTION ====================

int main() {
    // Set console window properties
    system("title WORD CHAIN CHALLENGE - ULTIMATE VOCABULARY SHOWDOWN");
    system("mode con: cols=120 lines=40");
    
    // Change console font (Windows only, optional, skip for MinGW/g++)
    /*
    #ifdef _WIN32
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(cfi);
    cfi.nFont = 0;
    cfi.dwFontSize.X = 8;
    cfi.dwFontSize.Y = 16;
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;
    wcsncpy(cfi.FaceName, L"Consolas", 32);
    SetCurrentConsoleFontEx(console, FALSE, &cfi);
    #endif
    */
    
    setConsoleSize();
    hideConsoleCursor();
    
    // Load dictionary from file
    loadDictionaryFromFile();
    
    // Show header and intro
    printHeaderMenu();
    
    clearScreen();
    printLoginMenu();
    login();
    
    clearScreen();
    showIntroduction();
    
    // Main game loop
    showMainMenu();
    
    return 0;
}
