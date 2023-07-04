#include "utility.h"
#include <sstream>
#include <cstring>
using namespace std;

// remove spaces from string's begin and string's end
void trim(string &s)
{
    while (1)
    {
        if ((int)s.length() > 0)
        {
            if (s.back() == ' ')
                s.pop_back();
            else
                break;
        }
        else
            break;
    }
    int i = 0;
    while (1)
    {
        if (i < (int)s.length())
        {
            if (s[i] == ' ')
                i++;
            else
                break;
        }
        else
            break;
    }
    s = s.substr(i);
}

// spliting input on the basis of delimiter
vector<string> split(string &str, char delim)
{
    string tmp;
    stringstream ss(str);
    vector<string> tokens;
    while (1)
    {
        if (getline(ss, tmp, delim))
            tokens.push_back(tmp);
        else
            break;
    }
    return tokens;
}

// string to char *
vector<char *> cstrArray(vector<string> &args)
{
    vector<char *> args_((int)args.size() + 1);
    for (int i = 0; i < (int)args.size(); i++)
    {
        args_[i] = (char *)malloc(((int)args[i].length() + 1) * sizeof(char));
        strcpy(args_[i], args[i].c_str());
    }
    args_[(int)args.size()] = (char *)malloc(sizeof(char));
    args_[(int)args.size()] = nullptr;
    return args_;
}