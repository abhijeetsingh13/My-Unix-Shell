#include "read_command.h"
#include <termios.h>
#include <unistd.h>
#include <algorithm>
#include <cstdio>
#include <iostream>
#include <set>
#include "history.h"
using namespace std;

int to_insert = 0;
extern bool ctrlC, ctrlZ, ctrlD;

int add(int a, int b, int mod)
{
    return (((a % mod) + (b % mod)) + mod) % mod;
}
int sub(int a, int b, int mod)
{
    return (((a % mod) - (b % mod)) + mod) % mod;
}

// Displays the prompt for the shell
void displayPrompt()
{
    char buf[DIR_LENGTH];
    getcwd(buf, DIR_LENGTH);
    string dir(buf);
    dir = dir.substr(dir.find_last_of("/") + 1);
    cout << COLOR_GREEN << "NAAG: " << COLOR_BLUE << dir << COLOR_RESET << "$ ";
}
bool UP_ARROW = false, DOWN_ARROW = false;

// Performs necessay actions for each character input
int handleChar(char c, string &buf)
{
    // check for arrow keys using the buffer
    if (c == CTRL_CZ)
    { // Ctrl + C, Ctrl + Z
        if (ctrlC)
        {
            printf("\n");
            buf = "";
            ctrlC = 0;
            return 2;
        }
        else if (ctrlZ)
        {
            ctrlZ = 0;
            return 0;
        }
    }
    else if (c == CTRL_D)
    { // Ctrl + D
        ctrlD = 1;
        buf = "";
        return 4;
    }
    else if (c == BACKSPACE)
    { // Backspace
        if (to_insert > 0)
        {
            for (int i = to_insert; i < (int)buf.size(); i++)
                printf("%s", "\033[1C");
            for (int i = 0; i < (int)buf.size(); i++)
                printf("\b \b");

            buf.erase(to_insert - 1, 1);
            to_insert--;
            cout << buf;
            for (int i = 0; i < (int)buf.size(); i++)
                printf("%s", "\033[1D");
            for (int i = 0; i < to_insert; i++)
                printf("%s", "\033[1C");
        }
        return 0;
    }
    else if (c == ENTER)
    { // Enter
        printf("\n");
        // cout<<buf.size()<<endl;
        // for(int i=0;i<buf.size();i++){
        //     cout<<(int)buf[i]<<endl;
        // }
        return 1;
    }
    else if (c > 31 && c < 127)
    { // Printable characters
        if (to_insert == (int)buf.size())
        {
            printf("%c", c);
            buf.insert(to_insert, 1, c);
            to_insert++;
        }
        else
        {
            for (int i = to_insert; i < (int)buf.size(); i++)
                printf("%s", "\033[1C");
            for (int i = 0; i < (int)buf.size(); i++)
                printf("\b \b");
            buf.insert(to_insert, 1, c);
            to_insert++;
            cout << buf;
            for (int i = 0; i < (int)buf.size(); i++)
                printf("%s", "\033[1D");
            for (int i = 0; i < to_insert; i++)
                printf("%s", "\033[1C");
        }
        return 0;
    }
    return 0;
}

void clearbuf(string &buf)
{
    for (int i = to_insert; i < (int)buf.size(); i++)
        printf("%s", "\033[1C");
    for (int i = 0; i < (int)buf.size(); i++)
        printf("\b \b");
    buf = "";
    to_insert = 0;
}

// Reads a command from the user (with autocomplete and history search)
string readCommand()
{
    struct termios old_tio, new_tio;
    signed char c;

    // Get the terminal settings for stdin
    tcgetattr(STDIN_FILENO, &old_tio);

    // We want to keep the old setting to restore them at the end
    new_tio = old_tio;

    // Disable canonical mode (bufered i/o) and local echo
    new_tio.c_lflag &= (~ICANON & ~ECHO);
    new_tio.c_cc[VMIN] = 1;
    new_tio.c_cc[VTIME] = 0;

    // Set the new settings immediately
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);

    string buf;
    vector<string> cmds = searchInHistory("");
    int hist_idx = (int)cmds.size() - 1;
    to_insert = 0;
    while (1)
    {
        int ret_out;
        c = getchar();
        if (c == '\x01')
        {
            if (to_insert)
            {
                for (int i = 0; i < to_insert; i++)
                    printf("%s", "\033[1D");
            }
            to_insert = 0;
        }
        else if (c == '\x05')
        {
            if (to_insert != (int)buf.length())
            {
                for (int i = to_insert; i < (int)buf.size(); i++)
                    printf("%s", "\033[1C");
            }
            to_insert = buf.length();
        }
        else
        {
            // to_insert = buf.size();
            ret_out = handleChar(c, buf);
            if (to_insert > 1)
            {
                // int i = (int)buf.size() - 1;
                if (buf[to_insert - 1] == 65 && buf[to_insert - 2] == 91)
                {
                    // clear command line
                    if (UP_ARROW)
                    {
                        // hist_idx = sub(hist_idx, 1, (int)cmds.size());
                        hist_idx--;
                    }
                    clearbuf(buf);
                    // displayPrompt();
                    if (cmds.size() == 0)
                    {
                    }
                    else
                    {
                        // printf("Similar commands matched (most recent first):\n");
                        if (hist_idx < 0)
                        {
                            hist_idx = -1;
                            buf = cmds.front();
                        }
                        else
                            buf = cmds[hist_idx];
                        to_insert = buf.size();
                        printf("%s", buf.c_str());
                        // displayPrompt();
                    }
                    UP_ARROW = true;
                }
                else if (buf[to_insert - 1] == 66 && buf[to_insert - 2] == 91)
                {
                    if (DOWN_ARROW)
                    {
                        // hist_idx = add(hist_idx, 1, (int)cmds.size());
                        hist_idx++;
                    }
                    clearbuf(buf);
                    // displayPrompt();
                    if (cmds.size() == 0)
                    {
                    }
                    else
                    {
                        // printf("Similar commands matched (most recent first):\n");
                        if (hist_idx >= (int)cmds.size())
                        {
                            hist_idx = cmds.size();
                            buf = "";
                        }
                        else
                            buf = cmds[hist_idx];
                        to_insert = buf.size();
                        printf("%s", buf.c_str());
                        // displayPrompt();
                    }
                    DOWN_ARROW = true;
                }
            }
            if (ret_out != 0)
                break;
        }
    }
    UP_ARROW = false;
    DOWN_ARROW = false;

    tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);

    return buf;
}
