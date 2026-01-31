# � Word Chain Challenge (C++)

An exciting C++ console-based vocabulary game that challenges players to create word chains by entering words that start with the last letter of the previous word.

---

## 🎯 Overview

**Word Chain Challenge** is a Windows-based console application where players test their vocabulary skills in a fast-paced word chain game. Play solo against the computer or challenge a friend!

The project focuses on **vocabulary building**, **quick thinking mechanics**, **console UI engineering**, and **file-based data persistence**.

---

## ✨ Key Features

| Feature | Description |
|---------|-------------|
| 🔐 **User Authentication** | Registration & login system with password masking |
| 🎮 **Multiple Game Modes** | Single Player vs Computer or Two-Player mode |
| 📚 **8 Word Categories** | Animals, Countries, Fruits, Professions, Games, Cities, Digital Devices, Foods |
| ⏱️ **Timed Rounds** | 10 seconds per turn with visual countdown |
| 🏆 **Scoring System** | Points for valid words, penalties for invalid ones |
| 💾 **Persistent High Scores** | High scores saved per user account |
| 📁 **External Dictionary** | Words loaded from external file - easily expandable |
| ➕ **Add Custom Words** | Import words from your own text files |
| 🎨 **Centered Console UI** | Dynamic centering adapts to console size |
| ⌨️ **Arrow Key Navigation** | Smooth menu navigation with visual highlighting |
| 📊 **Progress Tracking** | Visual progress bar showing words used |
| 💡 **Hint System** | Get hints when stuck (with point penalty) |
| 🔊 **Sound Effects** | Audio feedback for actions and timer warnings |
| ❤️ **Lives System** | 3 lives per player - strategic gameplay |

---

## 🛠️ Tech Stack

- **Language:** C++
- **Platform:** Windows API (Console Control)
- **Libraries:** 
  - `windows.h` - Console manipulation, cursor positioning & colors
  - `conio.h` - Real-time keyboard input
  - `fstream` - File handling for user data & dictionary
  - `atomic` - Thread-safe timer implementation
- **Features:** 
  - File-based authentication & data persistence
  - External dictionary file management
  - Dynamic screen centering & responsive layout
  - Password masking for security
  - Multi-threaded timer system

---

## 🎮 Game Mechanics

### Word Categories:
| Category | Word Count | Example Words |
|----------|------------|---------------|
| 🦁 Animals | 180+ | elephant, tiger, zebra |
| 🌍 Countries | 170+ | australia, brazil, canada |
| 🍎 Fruits | 75+ | apple, banana, cherry |
| 👔 Professions | 180+ | doctor, engineer, teacher |
| 🎮 Games | 105+ | minecraft, pokemon, zelda |
| 🏙️ Cities | 200+ | london, paris, tokyo |
| 📱 Digital Devices | 150+ | laptop, smartphone, tablet |
| 🍕 Foods | 400+ | pizza, pasta, sushi |

### Game Rules:
1. Start with a random word from the selected category
2. Enter a word that **starts with the last letter** of the current word
3. Word must be in the dictionary and not previously used
4. Complete as many rounds as possible before running out of lives

### Scoring System:
| Action | Points |
|--------|--------|
| Valid Word | +10 |
| Win Game | +50 |
| Using Hint (after 3) | -5 each |
| Invalid Word | Lose a life |
| Time's Up | Lose a life |

---

## 🚀 How to Run

### Option 1: Compile from Source
1. Open `words-chain-game.cpp` in any C++ IDE (Visual Studio, Code::Blocks, Dev-C++)
2. Build the project
3. Run the compiled executable

### Option 2: Command Line
```bash
g++ words-chain-game.cpp -o WordChain.exe
./WordChain.exe
```

**Note:** Make sure `wordchain_dictionary.txt` is in the same directory as the executable.

---

## 🎛️ Controls

| Key | Action |
|-----|--------|
| ↑ ↓ | Navigate menu options |
| Enter | Select option / Submit word |
| ESC | Return to previous menu / Exit |
| Backspace | Delete character while typing |

---

## 📁 Project Structure

```
word-chain-challenge-cpp-main/
├── words-chain-game.cpp      # Main source code
├── wordchain_dictionary.txt  # External word dictionary file
├── wordchain_login.txt       # User credentials & high scores
├── wordchain_highscores.txt  # Game high scores
└── README.md                 # Project documentation
```

---

## 📖 Dictionary File Format

The dictionary file uses a simple format with category headers:

```
[ANIMALS]
aardvark
albatross
alligator
...

[COUNTRIES]
afghanistan
albania
algeria
...
```

### Adding Custom Words:
1. **From File:** Use "Add Words from File" option in menu
   - Create a text file with one word per line
   - Select category and provide file path
   
2. **Manually:** Use "Add Word Manually" option
   - Select category and type the word

---

## 📸 Screenshots

```
╔════════════════════════════════════════════════════════════╗
║         W O R D   C H A I N   C H A L L E N G E            ║
║              ULTIMATE VOCABULARY SHOWDOWN                   ║
║        Developed by: Student Name | Roll: XX-CS-XXX        ║
╚════════════════════════════════════════════════════════════╝

Words: [████████████░░░░░░░░] 12         Score: 120
                                          High Score: 150
Player1: 120 pts | ♥ ♥ ♥            COMPUTER: 100 pts

              ROUND 7/20 | ANIMALS

              Current Word: elephant

        Next word must start with: [ T ]

          ╔══════════════════════════════════════════════╗
          ║  Enter word: tiger                           ║
          ║                                              ║
          ║  VALID! +10 points                           ║
          ╚══════════════════════════════════════════════╝

                    Time: 07s
```

---

## 🎯 Game Flow

```
┌─────────────────┐
│  Login/Register │
└────────┬────────┘
         ▼
┌─────────────────┐
│   Introduction  │
└────────┬────────┘
         ▼
┌─────────────────┐
│    Main Menu    │◄──────────────────┐
└────────┬────────┘                   │
         ▼                            │
┌─────────────────┐                   │
│ Select Category │                   │
└────────┬────────┘                   │
         ▼                            │
┌─────────────────┐                   │
│   Game Rounds   │───────────────────┤
│  (Max 20 turns) │   Game Over       │
└────────┬────────┘                   │
         ▼                            │
┌─────────────────┐                   │
│   Determine     │                   │
│    Winner       │                   │
└────────┬────────┘                   │
         ▼                            │
┌─────────────────┐                   │
│  Save Scores    │───────────────────┘
│  Play Again?    │
└─────────────────┘
```

---

## ⚠️ Requirements

- **OS:** Windows only (uses WinAPI)
- **Compiler:** Any C++ compiler supporting Windows headers
- **Console:** Windows Command Prompt or PowerShell
- **Sound:** System speaker for audio feedback
- **Files:** `wordchain_dictionary.txt` must be present

---

## 📚 Learning Outcomes

- ✅ Console UI/UX design with dynamic centering
- ✅ File-based authentication system
- ✅ External data file management
- ✅ Game state management (scores, lives, rounds)
- ✅ Interactive real-time gameplay
- ✅ Multi-threaded timer implementation
- ✅ Arrow key input handling
- ✅ Data persistence with file I/O
- ✅ Modular function design
- ✅ Color-coded console output
- ✅ Sound effect integration
- ✅ User-expandable dictionary system

---

## 🎨 UI Features

| Feature | Implementation |
|---------|----------------|
| Colored Text | 15+ color combinations for different elements |
| Box Drawing | Unicode box characters for menus & input areas |
| Progress Bar | Visual word usage indicator |
| Animated Loading | Loading bar with sound on startup |
| Password Masking | Asterisks shown instead of password |
| Centered Layout | All content dynamically centered |
| Highlighted Selection | Inverted colors for selected menu item |
| Timer Display | Color-coded countdown (green → yellow → red) |
| Lives Display | Heart symbols for remaining lives |

---

## 🆕 Recent Updates

### Version 2.0
- ✅ Moved word dictionary to external file
- ✅ Added ability to import words from user files
- ✅ Added manual word addition feature
- ✅ Enhanced UI similar to Murder Mystery game
- ✅ Improved timer with warning sounds
- ✅ Added lives display with heart symbols
- ✅ Better game over screen with ASCII art
- ✅ High score tracking per user

---

## 🔧 Customization

### Adding New Categories:
1. Edit `wordchain_dictionary.txt`
2. Add new section header: `[CATEGORYNAME]`
3. Add words (one per line)
4. Update the game code to recognize new category

### Adjusting Difficulty:
- **Timer:** Change `TIMER_SECONDS` constant (default: 10)
- **Lives:** Modify starting lives in `startWordChainGame()` (default: 3)
- **Rounds:** Adjust `maxRounds` variable (default: 20)

---

## 👨‍💻 Author

**Student Name**  
Computer Science Undergraduate  
Roll No: XX-CS-XXX

---

## 📄 License

This project is created for educational purposes as part of the **Programming Fundamentals** course final semester project.

---

*Made with ❤️ and C++*

## 👨‍💻 Author
**Adil ur Rehman**  
Computer Science Undergraduate  
GitHub: https://github.com/adilurrehman
