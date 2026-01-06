/*
  CandyCrushGame.cpp
*/

#include <iostream>
#include <fstream>
#include <ctime> //Game timer
#include <cstdlib>
#include <conio.h>   //Arrows or ENtery
#include <windows.h> //Colors,Sleep,
#include <string>
#include <cctype>    //Character Checking
#include <algorithm> //Swap

using namespace std;

/*--------- Constants & Globals ---------- */
const int MAX_N = 10;
char board[MAX_N][MAX_N];
int rows = 8, cols = 8;
int timeLimitSec = 60;
int scoreGlobal = 0;
char candiesEasy[5] = {'@', '#', '&', '$', '%'};
char candiesHard[7] = {'@', '#', '&', '$', '%', '!', '*'};
int candyCount = 5;
char candySet[7];
const char BLANK = ' ';

/*---------- Styling ---------*/
void setConsoleColor(int fg, int bg = 0)
{
    int color = (bg << 4) | fg;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}
void resetConsoleColor() { setConsoleColor(7, 0); }
void clearScreen() { system("cls"); }
// For Random Candies
char randomCandy()
{
    return candySet[rand() % candyCount];
}
// It act as Game Boundry
bool inBounds(int r, int c)
{
    return r >= 0 && r < rows && c >= 0 && c < cols;
}

/* ---------- Check Matches if exist left right top bottom and also if 4 ---------- */
bool createsMatchIfPlaced(int r, int c, char ch)
{
    int count, i;

    // horizontal only
    count = 1;
    i = c - 1;
    while (i >= 0 && board[r][i] == ch)
    {
        count++;
        i--;
    }
    i = c + 1;
    while (i < cols && board[r][i] == ch)
    {
        count++;
        i++;
    }
    if (count >= 3)
        return true;

    // vertical only
    count = 1;
    i = r - 1;
    while (i >= 0 && board[i][c] == ch)
    {
        count++;
        i--;
    }
    i = r + 1;
    while (i < rows && board[i][c] == ch)
    {
        count++;
        i++;
    }
    if (count >= 3)
        return true;

    return false;
}
/* --------- It initialize char matrix by checking matches  -------- */
void initBoardNoMatches()
{
    for (int r = 0; r < rows; r++)
        for (int c = 0; c < cols; c++)
            board[r][c] = BLANK;

    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols; c++)
        {
            char ch;
            do
            {
                ch = randomCandy();
                board[r][c] = ch;
            } while (createsMatchIfPlaced(r, c, ch));
        }
    }
}

/* ---------- Display Game After Menu ---------- */
void printStatusAndBoard(int remainingSec)
{
    clearScreen();
    int mm = remainingSec / 60;
    int ss = remainingSec % 60;

    cout << "---------------------------------------------\n";
    cout << " Score: " << scoreGlobal << "    Time Remaining: ";
    if (mm < 10)
        cout << '0';
    cout << mm << ":";
    if (ss < 10)
        cout << '0';
    cout << ss << "\n";
    cout << "---------------------------------------------\n";

    cout << "      ";
    for (int c = 0; c < cols; c++)
        cout << c << " ";
    cout << "\n";

    for (int r = 0; r < rows; r++)
    {
        cout << (r < 10 ? " " : "") << r << " | ";
        for (int c = 0; c < cols; c++)
        {
            char ch = board[r][c];
            if (ch == BLANK)
                cout << ". ";
            else
            {
                int color = 7;
                switch (ch)
                {
                case '@':
                    color = 14;
                    break;
                case '#':
                    color = 9;
                    break;
                case '&':
                    color = 12;
                    break;
                case '$':
                    color = 10;
                    break;
                case '%':
                    color = 13;
                    break;
                case '!':
                    color = 11;
                    break;
                case '*':
                    color = 15;
                    break;
                }
                setConsoleColor(color, 0);
                cout << ch << " ";
                resetConsoleColor();
            }
        }
        cout << "\n";
    }
    cout << "---------------------------------------------\n";
    cout << "Controls: Select cell then arrow/WASD. P=Pause, Q=Quit.\n";
}

/* ---------- Swap ---------- */
void swapCells(int r1, int c1, int r2, int c2)
{
    char tmp = board[r1][c1];
    board[r1][c1] = board[r2][c2];
    board[r2][c2] = tmp;
}

/*-------- Match Detection (Fixed L-shape scoring) ------ */
int detectAndMarkMatches(bool matched[MAX_N][MAX_N])
{
    for (int r = 0; r < rows; r++)
        for (int c = 0; c < cols; c++)
            matched[r][c] = false;

    int totalScore = 0;

    /* --- Horizontal --- */
    for (int r = 0; r < rows; r++)
    {
        int c = 0;
        while (c < cols)
        {
            if (board[r][c] == BLANK)
            {
                c++;
                continue;
            }
            char ch = board[r][c]; // ch stores the current candy.
            int k = c + 1;         // k moves it to forward
            while (k < cols && board[r][k] == ch)
                k++;
            int len = k - c;
            if (len >= 3)
                for (int t = c; t < k; t++)
                    matched[r][t] = true;
            c = k; // for skipping
        }
    }

    /* --- Vertical --- */
    for (int c = 0; c < cols; c++)
    {
        int r = 0;
        while (r < rows)
        {
            if (board[r][c] == BLANK)
            {
                r++;
                continue;
            }
            char ch = board[r][c];
            int k = r + 1;
            while (k < rows && board[k][c] == ch)
                k++;
            int len = k - r;
            if (len >= 3)
                for (int t = r; t < k; t++)
                    matched[t][c] = true;
            r = k;
        }
    }

    // Score only by longest straight line per segment.

    // Horizontal scoring
    for (int r = 0; r < rows; r++)
    {
        int c = 0;
        while (c < cols)
        {
            if (!matched[r][c])
            {
                c++;
                continue;
            }
            int k = c + 1;
            while (k < cols && matched[r][k])
                k++;
            int len = k - c;
            if (len == 3)
                totalScore += 10;
            else if (len == 4)
                totalScore += 15;
            else if (len >= 5)
                totalScore += 20;
            c = k;
        }
    }

    // Vertical scoring
    for (int c = 0; c < cols; c++)
    {
        int r = 0;
        while (r < rows)
        {
            if (!matched[r][c])
            {
                r++;
                continue;
            }
            int k = r + 1;
            while (k < rows && matched[k][c])
                k++;
            int len = k - r;
            if (len == 3)
                totalScore += 10;
            else if (len == 4)
                totalScore += 15;
            else if (len >= 5)
                totalScore += 20;
            r = k;
        }
    }

    return totalScore;
}

int removeMarked(bool matched[MAX_N][MAX_N])
{
    int removed = 0;
    for (int r = 0; r < rows; r++)
        for (int c = 0; c < cols; c++)
            if (matched[r][c])
            {
                board[r][c] = BLANK;
                removed++;
            }
    return removed;
}

/* ---------- Gravity & Refill (fixed: prevents new immediate matches) ---------- */
void applyGravityAndRefill()
{
    for (int c = 0; c < cols; c++)
    {
        int write = rows - 1; // Tell where candy lowest lands

        for (int r = rows - 1; r >= 0; r--)
        {
            if (board[r][c] != BLANK)
                board[write--][c] = board[r][c];
        }
        for (int r = write; r >= 0; r--)
        {
            char ch;
            do
            {
                ch = randomCandy();
                board[r][c] = ch;
            } while (createsMatchIfPlaced(r, c, ch));
        }
    }
}

/* ---------- Cascades ---------- */
int processCascades()
{
    int total = 0;
    while (true)
    {
        bool matched[MAX_N][MAX_N];
        int gained = detectAndMarkMatches(matched);
        if (gained == 0)
            break;
        scoreGlobal += gained;
        total += gained;
        removeMarked(matched);
        applyGravityAndRefill();
        printStatusAndBoard(1); // Just a placeholder time for animation
        Sleep(200);
    }
    return total;
}

/* ---------- High Scores ---------- */
struct HSItem
{
    string name;
    int score;
};

void loadHighScores(HSItem list[], int &count)
{
    count = 0;
    ifstream fin("highestScore.txt");
    if (!fin)
        return;
    while (fin >> list[count].name >> list[count].score)
    {
        count++;
        if (count >= 100)
            break;
    }
}
void saveHighScores(HSItem list[], int count)
{
    ofstream fout("highestScore.txt");
    for (int i = 0; i < count; i++)
        fout << list[i].name << " " << list[i].score << "\n";
}
void addHighScore(const string &name, int score)
{
    HSItem list[100];
    int cnt;
    loadHighScores(list, cnt);
    list[cnt].name = name;
    list[cnt].score = score;
    cnt++;
    // sort
    for (int i = 0; i < cnt - 1; i++)
        for (int j = 0; j < cnt - i - 1; j++)
            if (list[j].score < list[j + 1].score)
                swap(list[j], list[j + 1]);
    if (cnt > 10)
        cnt = 10;
    saveHighScores(list, cnt);
}
void showHighScores()
{
    HSItem list[100];
    int cnt;
    loadHighScores(list, cnt);
    clearScreen();
    cout << "---- Top Scores ----\n";
    if (cnt == 0)
        cout << "No scores yet.\n";
    for (int i = 0; i < cnt; i++)
        cout << i + 1 << ". " << list[i].name << " " << list[i].score << "\n";
    cout << "Press any key...\n";
    _getch();
}

/* ---------- Save / Load Snapshot (fixed board reading) ---------- */
void saveGameSnapshot(const string &name, int remainingSec)
{
    ofstream fout("savedGame.txt");
    fout << name << "\n";
    fout << rows << " " << cols << " " << timeLimitSec << " " << remainingSec << " " << scoreGlobal << "\n";
    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols; c++)
            fout << board[r][c];
        fout << "\n";
    }
}

bool loadGameSnapshot(string &player, int &remaining)
{
    ifstream fin("savedGame.txt");
    if (!fin)
        return false;

    getline(fin, player);
    int savedRows, savedCols, savedLimit, savedRem, savedScore;
    fin >> savedRows >> savedCols >> savedLimit >> savedRem >> savedScore;
    rows = savedRows;
    cols = savedCols;
    timeLimitSec = savedLimit;
    remaining = savedRem;
    scoreGlobal = savedScore;
    string line;
    getline(fin, line);

    for (int r = 0; r < rows; r++)
    {
        getline(fin, line);
        for (int c = 0; c < cols; c++)
            board[r][c] = (c < (int)line.size() ? line[c] : BLANK);
    }

    if (rows == 8)
    {
        candyCount = 5;
        for (int i = 0; i < candyCount; i++)
            candySet[i] = candiesEasy[i];
    }
    else
    {
        candyCount = 7;
        for (int i = 0; i < candyCount; i++)
            candySet[i] = candiesHard[i];
    }
    return true;
}

/* ---------- Controls ---------- */
int getDirectionFromKey()
{
    int ch = _getch();
    if (ch == 0 || ch == 224)
    {
        int ch2 = _getch();
        if (ch2 == 72)
            return 0;
        if (ch2 == 80)
            return 1;
        if (ch2 == 75)
            return 2;
        if (ch2 == 77)
            return 3;
        return -1;
    }
    else
    {
        if (ch == 'w' || ch == 'W')
            return 0;
        if (ch == 's' || ch == 'S')
            return 1;
        if (ch == 'a' || ch == 'A')
            return 2;
        if (ch == 'd' || ch == 'D')
            return 3;
        if (ch == 'p' || ch == 'P')
            return 9;
        if (ch == 'q' || ch == 'Q')
            return 8;
        return -1;
    }
}

/* ---------- Game Loop ---------- */
int playGameSession(const string &player, int startRem)
{

    time_t deadline = time(NULL) + startRem;

    while (true)
    {
        int currentRem = (int)(deadline - time(NULL));
        if (currentRem <= 0)
        {
            printStatusAndBoard(0);
            cout << "Time's up!\n";
            break;
        }

        printStatusAndBoard(currentRem);
        cout << "Enter row col (or P/Q): ";

        string line;
        getline(cin, line);
        if (line.size() == 0)
            continue;

        int pos = 0;
        while (pos < line.size() && isspace(line[pos]))
            pos++;

        // Pause
        if (pos < line.size() && (line[pos] == 'P' || line[pos] == 'p'))
        {
            saveGameSnapshot(player, currentRem);
            cout << "Saved. Press key...\n";
            _getch();
            return -1;
        }
        // Quit
        if (pos < line.size() && (line[pos] == 'Q' || line[pos] == 'q'))
            return -2;

        // Parse r c
        int rsel = -1, csel = -1;
        {
            int a = -1, b = -1;
            bool ok = false;
            int idx = pos;

            bool neg = false;
            if (idx < line.size() && line[idx] == '-')
            {
                neg = true;
                idx++;
            }
            if (idx < line.size() && isdigit(line[idx]))
            {
                a = 0;
                while (idx < line.size() && isdigit(line[idx]))
                    a = a * 10 + (line[idx] - '0'), idx++;
                if (neg)
                    a = -a;
                while (idx < line.size() && isspace(line[idx]))
                    idx++;
                neg = false;
                if (idx < line.size() && line[idx] == '-')
                {
                    neg = true;
                    idx++;
                }
                if (idx < line.size() && isdigit(line[idx]))
                {
                    b = 0;
                    while (idx < line.size() && isdigit(line[idx]))
                        b = b * 10 + (line[idx] - '0'), idx++;
                    if (neg)
                        b = -b;
                    ok = true;
                }
            }
            if (!ok)
            {
                cout << "Invalid. ";
                Sleep(500);
                continue;
            }
            rsel = a;
            csel = b;
        }

        if (!inBounds(rsel, csel))
        {
            cout << "Out of range.\n";
            Sleep(500);
            continue;
        }

        cout << "Press direction key...\n";
        int dir = getDirectionFromKey();
        if (dir == 9)
        {
            saveGameSnapshot(player, currentRem);
            cout << "Saved. Press key...\n";
            _getch();
            return -1;
        }
        if (dir == 8)
            return -2;
        if (dir < 0)
        {
            cout << "Invalid.\n";
            Sleep(400);
            continue;
        }

        int r2 = rsel, c2 = csel;
        if (dir == 0)
            r2--;
        if (dir == 1)
            r2++;
        if (dir == 2)
            c2--;
        if (dir == 3)
            c2++;

        if (!inBounds(r2, c2))
        {
            cout << "Out of range.\n";
            Sleep(500);
            continue;
        }

        swapCells(rsel, csel, r2, c2);

        bool matched[MAX_N][MAX_N];
        int gained = detectAndMarkMatches(matched);
        if (gained == 0)
        {
            swapCells(rsel, csel, r2, c2);
            cout << "No match. Reverted.\n";
            Sleep(500);
            continue;
        }

        scoreGlobal += gained;
        removeMarked(matched);
        applyGravityAndRefill();
        processCascades();
    }

    return scoreGlobal;
}

/* ---------- Menu ---------- */
void showMainMenu()
{
    clearScreen();
    cout << "======================================\n";
    cout << "====== Candy Crush Console Game ======\n";
    cout << "======================================\n\n";
    cout << "1. Play Easy (8x8, 5 candies, 60s)\n";
    cout << "2. Play Hard (10x10, 7 candies, 40s)\n";
    cout << "3. Show Top 10 High Scores\n";
    cout << "4. Load Saved Game\n";
    cout << "5. Exit\n";
    cout << "Choose option: ";
}

int main()
{
    srand((unsigned)time(NULL));

    while (true)
    {
        showMainMenu();
        int opt;
        if (!(cin >> opt))
        {
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }
        cin.ignore();

        if (opt == 5)
            break;

        if (opt == 4)
        {
            showHighScores();
            continue;
        }

        if (opt == 3)
        {
            string player;
            int rem;
            if (!loadGameSnapshot(player, rem))
            {
                cout << "No saved game.\n";
                Sleep(800);
                continue;
            }
            cout << "Loaded " << player << ". Press key...\n";
            _getch();

            int result = playGameSession(player, rem);
            if (result >= 0)
            {
                cout << "Final score: " << result << "\nEnter name: ";
                string nm;
                cin >> nm;
                cin.ignore();
                addHighScore(nm, result);
                cout << "Saved.\n";
                Sleep(800);
            }
            continue;
        }

        if (opt == 1 || opt == 2)
        {
            if (opt == 1)
            {
                rows = cols = 8;
                timeLimitSec = 60;
                candyCount = 5;
                for (int i = 0; i < candyCount; i++)
                    candySet[i] = candiesEasy[i];
            }
            else
            {
                rows = cols = 10;
                timeLimitSec = 40;
                candyCount = 7;
                for (int i = 0; i < candyCount; i++)
                    candySet[i] = candiesHard[i];
            }

            initBoardNoMatches();
            scoreGlobal = 0;

            cout << "Enter player name: ";
            string pname;
            cin >> pname;
            cin.ignore();

            int result = playGameSession(pname, timeLimitSec);
            if (result == -1 || result == -2)
                continue;

            cout << "Final: " << result << "\nEnter name: ";
            string nm;
            cin >> nm;
            cin.ignore();
            addHighScore(nm, result);
            cout << "Saved. Key...\n";
            _getch();
        }
    }

    cout << "Goodbye!\n";
    return 0;
}
