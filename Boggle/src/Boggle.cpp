/*
 * File: Boggle.cpp
 */

#include <iostream>
#include "gboggle.h"
#include "grid.h"
#include "gwindow.h"
#include "lexicon.h"
#include "random.h"
#include "simpio.h"
#include <algorithm>
#include "set"
using namespace std;

/* Enums */

enum boardSize { standard = 1, big = 2 };
enum boardSetUp { custom = 1, original = 2 };
enum gameContinual { restart = 1, exitGame = 2 };

/* Constants */

const int BOGGLE_WINDOW_WIDTH = 650;
const int BOGGLE_WINDOW_HEIGHT = 350;
const int STANDARD_BOGGLE_SIZE = 16;
const int BIG_BOGGLE_SIZE = 25;
const int STANDARD_BOARD_SIZE = 4;
const int DELAY = 100;
const string FILENAME = "EnglishWords.dat";

const string STANDARD_CUBES[16] = {
    "AAEEGN", "ABBJOO", "ACHOPS", "AFFKPS",
    "AOOTTW", "CIMOTU", "DEILRX", "DELRVY",
    "DISTTY", "EEGHNW", "EEINSU", "EHRTVW",
    "EIOSST", "ELRTTY", "HIMNQU", "HLNNRZ"
};


const string BIG_BOGGLE_CUBES[25] = {
    "AAAFRS", "AAEEEE", "AAFIRS", "ADENNN", "AEEEEM",
    "AEEGMU", "AEGMNN", "AFIRSY", "BJKQXZ", "CCNSTW",
    "CEIILT", "CEILPT", "CEIPST", "DDLNOR", "DDHNOT",
    "DHHLOR", "DHLNOR", "EIIITT", "EMOTTT", "ENSSSU",
    "FIPRSY", "GORRVW", "HIPRRY", "NOOTUW", "OOOTTU"
};

/* Function prototypes */

void welcome();
void giveInstructions();
void playGame();
int chooseSize();
boardSetUp getChoice();
void fillCombinationOfCubes(boardSetUp&, string[], int&);
void fillCustomCombinations(string[], int&);
void fillOriginalCombinations(string[], int&);
void shuffleCombsOfCube(string[], int&);
int getMinWordSize();
void fillCubesGrid(string[], Grid<char>&);
void humanTakesTurn(Lexicon&, Set<string>&, Grid<char>&, int&);
void checkWord(string&, Lexicon&, Set<string>&, Grid<char>&, int&);
bool validOnBoard(Grid<char>&, Grid<bool>&, string);
bool possible(Grid<char>&, Grid<bool>&, string, int, int, Vector<pair<int, int>>&);
void highlightAnswer(Vector<pair<int, int>>&);
void computerTakesTurn(Lexicon&, Set<string>&, Grid<char>&, int&);
void findWordsFromCell(Lexicon&, Set<string>&, Set<string>&,
    Grid<char>&, string, Grid<bool>&, int, int, Vector<pair<int, int>>&, int&);
bool isValidWordForComputer(string&, Set<string>&, Set<string>&, Lexicon&, int&);
void exploreNeighbours(Lexicon&, Set<string>&, Set<string>&,
    Grid<char>&, string, Grid<bool>&, int r, int c, Vector<pair<int, int>>&, int&);
gameContinual choiceOfGameContinual();

/* Main program */

int main() {
    GWindow gw(BOGGLE_WINDOW_WIDTH, BOGGLE_WINDOW_HEIGHT);
    initGBoggle(gw);
    welcome();
    giveInstructions();
    playGame();
    cout << "Exited Game" << endl;
    return 0;
}

/*
 * Function: welcome
 * Usage: welcome();
 * -----------------
 * Print out a cheery welcome message.
 */

void welcome() {
    cout << "Welcome!  You're about to play an intense game ";
    cout << "of mind-numbing Boggle.  The good news is that ";
    cout << "you might improve your vocabulary a bit.  The ";
    cout << "bad news is that you're probably going to lose ";
    cout << "miserably to this little dictionary-toting hunk ";
    cout << "of silicon.  If only YOU had a gig of RAM..." << endl << endl;
}

/*
 * Function: giveInstructions
 * Usage: giveInstructions();
 * --------------------------
 * Print out the instructions for the user.
 */

void giveInstructions() {
    cout << endl;
    cout << "The boggle board is a grid onto which I ";
    cout << "I will randomly distribute cubes. These ";
    cout << "6-sided cubes have letters rather than ";
    cout << "numbers on the faces, creating a grid of ";
    cout << "letters on which you try to form words. ";
    cout << "You go first, entering all the words you can ";
    cout << "find that are formed by tracing adjoining ";
    cout << "letters. Two letters adjoin if they are next ";
    cout << "to each other horizontally, vertically, or ";
    cout << "diagonally. A letter can only be used once ";
    cout << "in each word. Words must be at least four ";
    cout << "letters long and can be counted only once. ";
    cout << "You score points based on word length: a ";
    cout << "4-letter word is worth 1 point, 5-letters ";
    cout << "earn 2 points, and so on. After your puny ";
    cout << "brain is exhausted, I, the supercomputer, ";
    cout << "will find all the remaining words and double ";
    cout << "or triple your paltry score." << endl << endl;
    cout << "Hit return when you're ready...";
    getLine();
}

/*
* This is vital function, where combination of cubes are chosen and
* then these appear on window. After initialization human starts game
* and then computers turn comes. It is in while loop, so player
* can restart game easily or exit
*/

void playGame() {
    while (true) {
        int boggleSize = chooseSize();
        string* combinationOfCubes = new string[boggleSize];
        boardSetUp setUp = getChoice();
        fillCombinationOfCubes(setUp, combinationOfCubes, boggleSize);
        shuffleCombsOfCube(combinationOfCubes, boggleSize);
        int minWordSize = getMinWordSize();

        drawBoard(sqrt(boggleSize), sqrt(boggleSize));
        Grid<char> board(sqrt(boggleSize), sqrt(boggleSize));
        fillCubesGrid(combinationOfCubes, board);

        Lexicon allEnglishWords(FILENAME);
        Set<string> humanGuessedWords;
        humanTakesTurn(allEnglishWords, humanGuessedWords, board, minWordSize);
        computerTakesTurn(allEnglishWords, humanGuessedWords, board, minWordSize);

        delete[] combinationOfCubes; // clean up

        gameContinual choice = choiceOfGameContinual();
        if (choice == exitGame) {
            break;
        }
        cout << "Starting a new game" << endl;
    }
}

/*
* We let user choose between big and a small board
*/

int chooseSize() {
    while (true) {
        cout << "Choose size of a board" << endl;
        int choice = getInteger("1)standard  2)big : ");
        if (choice == standard || choice == big) {
            return choice == standard ? STANDARD_BOGGLE_SIZE : BIG_BOGGLE_SIZE;
        }
        cout << "Choose 1 or 2" << endl;
    }
}

/*
* Let's user choose between custom and original board set-up
*/

boardSetUp getChoice() {
    while (true) {
        cout << "Choose board set-up" << endl;
        int choice = getInteger("1)Custom  2)Default : ");
        if (choice == custom || choice == original) {
            return choice == custom ? custom : original;
        }
        cout << "Choose 1 or 2" << endl;
    }
}

/*
* We pass array without reference cause it is passed by pointer by default
*/

void fillCombinationOfCubes(boardSetUp& setUp, string combinationOfCubes[], int& size) {
    if (setUp == custom) {
        fillCustomCombinations(combinationOfCubes, size);
    }
    else { // original
        fillOriginalCombinations(combinationOfCubes, size);
    }
}

/*
* Let's user enter configuration of cubes manually,
* cause user chose custom set-up. And again we pass array
* without reference cause array is being passed by pointer
* by default
*/

void fillCustomCombinations(string combinationOfCubes[], int& size) {
    for (int i = 1; i <= size; i++) {
        string suffix = "th"; // most frequently used
        if (i == 1) {
            suffix = "st";
        }
        else if (i == 2) {
            suffix = "nd";
        }
        else if (i == 3) {
            suffix = "rd";
        }
        // as task says, user will input valid configurations
        string cubeConfig = getLine("Enter " + to_string(i) + suffix + " Configuration: ");
        transform(cubeConfig.begin(), cubeConfig.end(), cubeConfig.begin(), ::toupper);
        combinationOfCubes[i - 1] = cubeConfig;
    }
}

/*
* We just copy original combinations as task said
*/

void fillOriginalCombinations(string combinationOfCubes[], int& size) {
    for (int i = 0; i < size; i++) {
        if (size == STANDARD_BOGGLE_SIZE) {
            combinationOfCubes[i] = STANDARD_CUBES[i];
        }
        else {
            combinationOfCubes[i] = BIG_BOGGLE_CUBES[i];
        }
    }
}

/*
* Shuffles array randomly
*/

void shuffleCombsOfCube(string combinationOfCubes[], int& size) {
    for (int i = 0; i < size; i++) {
        int randomIndex = randomInteger(0, size - 1);
        swap(combinationOfCubes[randomIndex], combinationOfCubes[i]);
    }
}

/*
* We let user enter min length of word
* (which previously was 4)
*/

int getMinWordSize() {
    while (true) {
        int n = getInteger("Enter minimum length of word : ");
        if (n > 0) {
            return n;
        }
        cout << "Minimum length must be positive" << endl;
    }
}

/*
* Fills our grid with random char from combinations
*/

void fillCubesGrid(string combinationOfCubes[], Grid<char>& board) {
    int index = 0;
    for (int r = 0; r < board.numRows(); r++) {
        for (int c = 0; c < board.numCols(); c++) {
            string combination = combinationOfCubes[index++];
            int randomIndex = randomInteger(0, combination.size() - 1);
            char randomChar = combination[randomIndex];
            board[r][c] = tolower(randomChar); // it's better to store lowercase
            labelCube(r, c, randomChar);
        }
    }
}

/*
* Human starts playing and enters word till they give up and enter empty string.
* We check inserted word if it is valid ( with requirements task provided) and
* if it is correct we record it. Also I added some feedback to enrich user experience
*/

void humanTakesTurn(Lexicon& allEnglishWords, Set<string>& humanGuessedWords, Grid<char>& board, int& minWordSize) {
    while (true) {
        string word = getLine("Enter word : ");
        transform(word.begin(), word.end(), word.begin(), ::tolower);
        if (word.empty())return; // user gave up
        checkWord(word, allEnglishWords, humanGuessedWords, board, minWordSize);
    }
}

/*
* THis function checks if word is valid and gives user sepcific feedback
*/

void checkWord(string& word, Lexicon& allEnglishWords, Set<string>& humanGuessedWords, Grid<char>& board, int& minWordSize) {
    Grid<bool> visited(board.numCols(), board.numRows()); // initialized with false
    if (word.size() < minWordSize) {
        cout << "Word must be at least " << minWordSize << " characters long" << endl;
    }
    else if (!allEnglishWords.contains(word)) {
        cout << "Not a valid English word" << endl;
    }
    else if (humanGuessedWords.contains(word)) {
        cout << "You've already guessed that word" << endl;
    }
    else if (!validOnBoard(board, visited, word)) {
        cout << "Word cannot be formed from the board" << endl;
    }
    else {
        humanGuessedWords.insert(word);
        recordWordForPlayer(word, HUMAN);
        cout << "Correct" << endl;
    }
}

/*
* This is important function, which iterates through board and if it
* finds valid start of word then it uses helper recursive function
* to check if whole word can be made on board. Importantly we track
* path too, so if found valid path, we highlight answer. False is
* returned if word can't be made from that board
*/

bool validOnBoard(Grid<char>& board, Grid<bool>& visited, string word) {
    for (int r = 0; r < board.numRows(); r++) {
        for (int c = 0; c < board.numCols(); c++) {
            if (board[r][c] == word[0]) { // if valid start of the word
                visited[r][c] = true;
                Vector<pair<int, int>> path; // initialize path
                path.add({ r, c });
                if (possible(board, visited, word.substr(1), r, c, path)) {
                    highlightAnswer(path);
                    return true;
                }
                visited[r][c] = false; // undo
            }
        }
    }
    return false;
}

/*
* Recursive helper function for validOnBoard, which cheks if
* whole word can be made from board. We use two nested loops
* to check all 8 directions from that component. If word
* can't be emptied from these 8 directions, it is not possible
*/

bool possible(Grid<char>& board, Grid<bool>& visited, string word, int r, int c, Vector<pair<int, int>>& path) {
    if (word.empty()) {
        return true;
    }
    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            if (i == 0 && j == 0)continue; // we should not remain at same component
            int newR = r + i;
            int newC = c + j;
            if (board.inBounds(newR, newC) && board[newR][newC] == word[0] && !visited[newR][newC]) {
                visited[newR][newC] = true;
                path.add({ newR, newC });
                if (possible(board, visited, word.substr(1), newR, newC, path)) {
                    return true;
                }
                path.remove(path.size() - 1);
                visited[newR][newC] = false;
            }
        }
    }
    return false;
}

/*
* If we get to this function, it means word could be made on board.
* We use gboggle.cpp function to highlight path so user can see it.
*/

void highlightAnswer(Vector<pair<int, int>>& path) {
    for (pair<int, int> component : path) {
        highlightCube(component.first, component.second, true);
        pause(DELAY);
    }
    for (pair<int, int> component : path) {
        highlightCube(component.first, component.second, false);
    }
}

/*
* This function is for computer turn. It iterates over board and
* checks word formations from every single char. We store path
* here for same reason too.
*/

void computerTakesTurn(Lexicon& allEnglishWords, Set<string>& humanGuessedWords, Grid<char>& board, int& minWordSize) {
    Set<string> computerWords;
    for (int r = 0; r < board.numRows(); r++) {
        for (int c = 0; c < board.numCols(); c++) {
            Grid<bool> visited(board.numCols(), board.numRows()); // not to use same cells 
            visited[r][c] = true;
            Vector<pair<int, int>> path;
            path.push_back({ r, c });
            findWordsFromCell(allEnglishWords, humanGuessedWords, computerWords, board, "", visited, r, c, path, minWordSize);
            visited[r][c] = false;
        }
    }
}

/*
* Recursive function that explores all ways word can be formed.
* It is being passed MANY parameters, but every single of them has vital role.
*/

void findWordsFromCell(Lexicon& allEnglishWords, Set<string>& humanGuessedWords, Set<string>& computerWords,
    Grid<char>& board, string soFar, Grid<bool>& visited, int r, int c, Vector<pair<int, int>>& path, int& minWordSize) {

    if (!allEnglishWords.containsPrefix(soFar))return; // no point to continue search 
    if (isValidWordForComputer(soFar, humanGuessedWords, computerWords, allEnglishWords, minWordSize)) {
        computerWords.insert(soFar);
        recordWordForPlayer(soFar, COMPUTER);
        highlightAnswer(path);
    }
    exploreNeighbours(allEnglishWords, humanGuessedWords, computerWords, board, soFar, visited, r, c, path, minWordSize);
}

bool isValidWordForComputer(string& soFar, Set<string>& humanGuessedWords,
    Set<string>& computerWords, Lexicon& allEnglishWords, int& minWordSize) {

    return (soFar.size() >= minWordSize &&
        allEnglishWords.contains(soFar) &&
        !humanGuessedWords.contains(soFar) &&
        !computerWords.contains(soFar));
}

/*
* Checks all 8 possible continual of word formations
*/

void exploreNeighbours(Lexicon& allEnglishWords, Set<string>& humanGuessedWords, Set<string>& computerWords,
    Grid<char>& board, string soFar, Grid<bool>& visited, int r, int c, Vector<pair<int, int>>& path, int& minWordSize) {

    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            if (i == 0 && j == 0)continue;
            int newR = r + i;
            int newC = c + j;
            if (board.inBounds(newR, newC) && !visited[newR][newC]) {
                visited[newR][newC] = true;
                path.push_back({ newR, newC });

                findWordsFromCell(allEnglishWords, humanGuessedWords, computerWords,
                    board, soFar + board[newR][newC], visited, newR, newC, path, minWordSize);

                visited[newR][newC] = false;
                path.remove(path.size() - 1);
            }
        }
    }
}

gameContinual choiceOfGameContinual() {
    while (true) {
        int choice = getInteger("1)Restart Game  2)Exit ");
        if (choice == restart || choice == exitGame) {
            return choice == restart ? restart : exitGame;
        }
        cout << "Enter 1 or 2" << endl;
    }
}