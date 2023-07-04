#include "history.h"
#include <fstream>
#include <iostream>
using namespace std;

deque<string> history; // Stores the history in a FIFO structure
const string HIST_FILE = ".shell_history";

// Load the history contents from the file when the shell starts
void loadHistory()
{
    history.clear();
    ifstream file(HIST_FILE);
    if (!file.is_open())
        return;
    else
    {
        string line = "";
        while (getline(file, line))
            history.push_back(line);
    }
    file.close();
}

// Returns the commands matched in the history
vector<string> searchInHistory(string s)
{
    vector<string> t;
    for (auto x : history)
    {
        if (x.find(s) != string::npos)
            t.push_back(x);
    }
    return t;
    // return (s.size() > 2 ? commands : vector<string>());
}

// Display the shell history when the 'history' command has to be executed
void printHistory()
{
    int i = max(0, (int)history.size() - HIST_DISPLAY_SIZE);
    for (int cnt = 0; cnt < min((int)history.size(), HIST_DISPLAY_SIZE); i++, cnt++)
        cout << i + 1 << " " << history[i] << endl;
}

// Add a command to the history
void addToHistory(string s)
{
    if (history.size() == HIST_SIZE)
        history.pop_front();
    history.push_back(s);
}

// Save the history to the file when the shell exits
void updateHistory()
{
    ofstream file(HIST_FILE);
    if (!file.is_open())
        return;
    else
    {
        for (auto it : history)
        {
            string temp = it + "\n";
            file << temp;
        }
    }
    file.close();
}