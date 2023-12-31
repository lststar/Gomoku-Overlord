#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <fstream>
#include <ncurses.h>

#include "checkwin.h"
#include "ai.h"

#define BOARD_SIZE 15

using namespace std;

vector<vector<int>> board(BOARD_SIZE, vector<int>(BOARD_SIZE, 0));
int current_player = 1;
// 0 for player vs. player; 1 for player vs. AI
int ai_mode = 0;
// 2 for easy, 4 for normal, and 6 for hard
int ai_depth = 4;
// 1 if AI goes first, 0 player goes first
int ai_first = 0;
// Initialize coordinates for arrow key input
int last_x = 7;
int last_y = 7;

// Load current player and game board to "game_state.txt"
void loadGame()
{
    ifstream fin("game_state.txt");
    // game_state.txt follows a consistent format
    // first line: current_player
    // followed by: board state

    if (fin.is_open())
    {
        // Read the current_player from the file
        fin >> current_player;
        fin >> ai_mode;
        fin >> ai_depth;
        fin >> ai_first;
        fin >> last_x;
        fin >> last_y;

        // Read the board state from the file
        for (int i = 0; i < BOARD_SIZE; ++i)
        {
            for (int j = 0; j < BOARD_SIZE; ++j)
            {
                fin >> board[i][j];
            }
        }

        fin.close();
    }
    else
    {
        cout << "Error." << endl;
    }
    cout << "Game successfully loaded." << endl;
}

// Print welcome message
void printWelcomeMsg()
{
    cout << "\033c";
    cout << "##################################" << endl;
    cout << "#                                #" << endl;
    cout << "#   \033[32m\033[1mWelcome to Gomoku-Overlord\033[0m   #" << endl;
    cout << "#                                #" << endl;
    cout << "##################################" << endl;
    cout << endl;
}

// Switch the current player between 1 and -1
void changePlayer()
{
    current_player *= -1;
}

// Initialize an empty game board or load an existing game board
// Allow player(s) to select game mode
void initBoard()
{
    printWelcomeMsg();
    // Check if there exists a "game_state.txt" in the same folder
    int load_game = 0;
    ifstream file("game_state.txt");
    if (file.good())
    {
        cout << "A previously saved game detected, enter \033[32m\033[1m1\033[0m to load the game, or \033[32m\033[1m0\033[0m to start a fresh game: ";
        cin >> load_game;
        cout << endl;
    }
    file.close(); // Close the file

    if (load_game != 0 && load_game != 1)
    {
        cout << "Invalid input, try again." << endl
             << endl;
        cout << "A previously saved game detected, enter \033[32m\033[1m1\033[0m to load the game, or \033[32m\033[1m0\033[0m to start a fresh game: ";
        cin >> load_game;
        cout << endl;
    }

    if (load_game == 1)
    {
        loadGame();
    }
    else
    {
        for (int i = 0; i < BOARD_SIZE; i++)
        {
            for (int j = 0; j < BOARD_SIZE; j++)
            {
                board[i][j] = 0;
            }
        }
        cout << "A fresh game is ready." << endl;

        cout << endl
             << "Please choose your game mode." << endl;

        // Player selection of ai_mode
        while (true)
        {
            cout << endl
                 << "Enter \033[32m\033[1m0\033[0m for player vs. player mode, or \033[32m\033[1m1\033[0m for player vs. AI mode: ";
            cin >> ai_mode;

            if (ai_mode != 0 && ai_mode != 1)
            {
                cout << "Invalid input, try again." << endl;
                continue;
            }

            if (ai_mode == 1)
            {
                cout << endl
                     << "Good call, now choose your AI difficulty:" << endl;
                cout << endl
                     << "Enter \033[32m\033[1m1\033[0m for easy, \033[32m\033[1m2\033[0m for normal, and \033[32m\033[1m3\033[0m for hard: ";
                cin >> ai_depth;
                while (ai_depth != 1 && ai_depth != 2 && ai_depth != 3)
                {
                    cout << "Invalid input, try again." << endl;
                    cin >> ai_depth;
                }
                ai_depth *= 2;
                if (ai_depth == 6)
                {
                    cout << endl
                         << "At hard difficulty, it takes longer for AI to calculate its steps." << endl;
                    cout << endl
                         << "Enter \033[32m\033[1m1\033[0m to confirm your selection, or \033[32m\033[1m0\033[0m to lower the difficulty to normal: ";
                    int flag;
                    cin >> flag;
                    if (flag == 0)
                    {
                        ai_depth = 4;
                    }
                }
                cout << endl
                     << "Now, enter \033[32m\033[1m0\033[0m for you to go first, or \033[32m\033[1m1\033[0m for AI to go first: ";
                cin >> ai_first;
                while (ai_first != 1 && ai_first != 0)
                {
                    cout << "Invalid input, try again." << endl;
                    cin >> ai_first;
                }
                if (ai_first == 1)
                {
                    changePlayer();
                }
            }
            break;
        }
    }

    cout << endl
         << "Great! You are all set, enjoy your game:)" << endl;
}

// (Regular) Print the current state of the game board
void printBoard(int x = -1, int y = -1)
{
    cout << "\033c";

    // Print the column numbers
    for (int i = 0; i <= BOARD_SIZE; i++)
    {
        cout << setw(2) << setfill('0') << i << " ";
    }
    cout << endl;
    // Print each row with its row number and the state of each position
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        cout << setw(2) << setfill('0') << i + 1 << " ";
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            if (board[i][j] == 0)
            {
                cout << "[ ]";
            }
            else if (board[i][j] == 1)
            {
                if (x == i && y == j)
                {
                    cout << "[\033[46mX\033[0m]";
                }
                else
                {
                    cout << "[\033[36mX\033[0m]";
                }
            }
            else if (board[i][j] == -1)
            {
                if (x == i && y == j)
                {
                    cout << "[\033[41mO\033[0m]";
                }
                else
                {
                    cout << "[\033[31mO\033[0m]";
                }
            }
        }
        cout << endl;
    }
}

// (In ncurses) Print the current state of the game board
void printBoard_in_ncurses(int x = -1, int y = -1)
{
    clear();

    // Print the column numbers
    for (int i = 0; i <= BOARD_SIZE; i++)
    {
        printw("%02d ", i);
    }
    printw("\n");

    // Print each row with its row number and the state of each position
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        printw("%02d ", i + 1);
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            if (board[i][j] == 0)
            {
                if (x == i && y == j)
                {
                    printw("[");
                    attron(COLOR_PAIR(3));
                    printw(" ");
                    attroff(COLOR_PAIR(3));
                    printw("]");
                }
                else
                {
                    printw("[ ]");
                }
            }
            else if (board[i][j] == 1)
            {
                if (x == i && y == j)
                {
                    printw("[");
                    attron(COLOR_PAIR(11));
                    printw("X");
                    attroff(COLOR_PAIR(11));
                    printw("]");
                }
                else
                {
                    printw("[");
                    attron(COLOR_PAIR(1));
                    printw("X");
                    attroff(COLOR_PAIR(1));
                    printw("]");
                }
            }
            else if (board[i][j] == -1)
            {
                if (x == i && y == j)
                {
                    printw("[");
                    attron(COLOR_PAIR(22));
                    printw("O");
                    attroff(COLOR_PAIR(22));
                    printw("]");
                }
                else
                {
                    printw("[");
                    attron(COLOR_PAIR(2));
                    printw("O");
                    attroff(COLOR_PAIR(2));
                    printw("]");
                }
            }
        }
        printw("\n");
    }
    refresh();
}

// Returns true if the move is successful, false otherwise
bool makeMove(int player, int x, int y)
{
    // Check if the player is valid (1 or -1)
    if (player != 1 && player != -1)
    {
        cout << endl
             << "Error: Player is not defined." << endl;
        return false;
    }
    // Check if the move is within the boundaries of the board
    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE)
    {
        cout << endl
             << "Error: Position out of boundary." << endl;
        return false;
    }
    // Check if the position on the board is already occupied
    if (board[x][y] != 0)
    {
        cout << endl
             << "Error: This position has been occupied." << endl;
        return false;
    }
    // If all checks pass, make the move and return true
    board[x][y] = player;

    printBoard_in_ncurses(x, y);

    return true;
}

// Saving current_player, ai_mode, ai_depth, ai_first and board
void saveGame()
{
    ofstream fout("game_state.txt");
    if (fout.is_open())
    {
        fout << current_player << "\n";
        fout << ai_mode << "\n";
        fout << ai_depth << "\n";
        fout << ai_first << "\n";
        fout << last_x << "\n";
        fout << last_y << "\n";

        for (int i = 0; i < BOARD_SIZE; ++i)
        {
            for (int j = 0; j < BOARD_SIZE; ++j)
            {
                fout << board[i][j] << " ";
            }
            fout << "\n";
        }
        fout.close();
    }
    else
    {
        cout << "Unable to open file." << endl;
    }

    cout << endl
         << "\033[32m\033[1mGame saved. See you around.\033[0m"
         << endl;
}

// Initialize the game board with / without game_state.txt
// Run the game with ai_mode set by users
int main()
{
    initBoard();

    // ncurses.h activation
    initscr();
    raw();
    keypad(stdscr, TRUE);
    noecho();

    start_color();
    use_default_colors();
    // Initializing color pairs
    init_pair(1, COLOR_CYAN, -1);
    init_pair(11, COLOR_CYAN, COLOR_WHITE);
    init_pair(2, COLOR_RED, -1);
    init_pair(22, COLOR_RED, COLOR_WHITE);
    init_pair(3, -1, COLOR_WHITE);

    int x = last_x;
    int y = last_y;
    // Print the initial state of the game board
    printBoard_in_ncurses(x, y);

    int ch;
    // Game loop
    while (true)
    {
        if (current_player == 1)
        {
            printw("You can pause the game by pressing 'esc'\n");
            printw("Player 1 Make Your Move (use arrow keys to move around, then hit enter): ");
            // Use Enter key to finish moving the piece
            while ((ch = getch()) != '\n')
            {
                // Game in pause, player can choose to return, save & quit, or quit without saving
                if (ch == 27)
                {
                    clear();
                    printBoard_in_ncurses(x, y);
                    printw("Game Paused:\n");
                    printw("(1) Press 's' to save and quit the game;\n");
                    printw("(2) Press 'q' to quit the game without saving;\n");
                    printw("(3) Press any other key to return to the game.\n");
                    int ch_in_pause = getch();
                    if (ch_in_pause == 's')
                    {
                        endwin();
                        saveGame();
                        return 0;
                    }
                    else if (ch_in_pause == 'q')
                    {
                        endwin();
                        cout << endl
                             << "\033[32m\033[1mGame quited without saving.\033[0m"
                             << endl;
                        return 0;
                    }
                    else
                    {
                        printBoard_in_ncurses(x, y);
                        printw("You can pause the game by pressing 'esc'\n");
                        printw("Player 1 Make Your Move (use arrow keys to move around, then hit enter): ");
                    }
                }

                switch (ch)
                {
                case KEY_UP:
                    if (x > 0)
                        x -= 1;
                    break;
                case KEY_DOWN:
                    if (x < 14)
                        x += 1;
                    break;
                case KEY_LEFT:
                    if (y > 0)
                        y -= 1;
                    break;
                case KEY_RIGHT:
                    if (y < 14)
                        y += 1;
                    break;
                default:
                    break;
                }
                printBoard_in_ncurses(x, y);
                printw("You can pause the game by pressing 'esc'\n");
                printw("Player 1 Make Your Move (use arrow keys to move around, then hit enter): ");
            }
        }
        else
        {
            if (ai_mode == 1)
            {
                cout << "AI is thinking..." << endl;
                tuple<int, int, int> ai_result = getAiMove(current_player, board, ai_depth);
                x = get<0>(ai_result);
                y = get<1>(ai_result);
            }
            else
            {
                printw("You can pause the game by pressing 'esc'\n");
                printw("Player 2 Make Your Move (use 'w' 'a' 's' 'd' to move around, then hit 'g'): ");
                // Use 'g' key to finish moving the piece
                while ((ch = getch()) != 'g')
                {
                    // Game in pause, player can choose to return, save & quit, or quit without saving
                    if (ch == 27)
                    {
                        clear();
                        printBoard_in_ncurses(x, y);
                        printw("Game Paused:\n");
                        printw("(1) Press 's' to save and quit the game;\n");
                        printw("(2) Press 'q' to quit the game without saving;\n");
                        printw("(3) Press any other key to return to the game.\n");
                        int ch_in_pause = getch();
                        if (ch_in_pause == 's')
                        {
                            endwin();
                            saveGame();
                            return 0;
                        }
                        else if (ch_in_pause == 'q')
                        {
                            endwin();
                            cout << endl
                                 << "\033[32m\033[1mGame quited without saving.\033[0m"
                                 << endl;
                            return 0;
                        }
                        else
                        {
                            printBoard_in_ncurses(x, y);
                            printw("You can pause the game by pressing 'esc'\n");
                            printw("Player 2 Make Your Move (use 'w' 'a' 's' 'd' to move around, then hit 'g'): ");
                        }
                    }

                    switch (ch)
                    {
                    case 'w':
                        if (x > 0)
                            x -= 1;
                        break;
                    case 's':
                        if (x < 14)
                            x += 1;
                        break;
                    case 'a':
                        if (y > 0)
                            y -= 1;
                        break;
                    case 'd':
                        if (y < 14)
                            y += 1;
                        break;
                    default:
                        break;
                    }
                    printBoard_in_ncurses(x, y);
                    printw("You can pause the game by pressing 'esc'\n");
                    printw("Player 2 Make Your Move (use 'w' 'a' 's' 'd' to move around, then hit 'g'): ");
                }
            }
        }

        if (!makeMove(current_player, x, y))
        {

            continue;
        }
        else
        {
            last_x = x;
            last_y = y;
        }

        if (checkWin(current_player, board))
        {
            endwin(); // Safely closing ncurses.h
            printBoard(x, y);
            cout << endl
                 << "\033[32m\033[1mPlayer \033[0m"
                 << "\033[32m\033[1m" << (3 - current_player) / 2 << "\033[0m"
                 << "\033[32m\033[1m Wins!!!\033[0m" << endl;

            ifstream file("game_state.txt");
            if (file.good())
            {
                file.close();
                // Now we remove it
                remove("game_state.txt");
            }
            return 0;
        }

        changePlayer();
    }
}