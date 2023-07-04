#include "shell.h"
#include <signal.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <deque>
#include <map>
#include "Command.h"
#include "Pipeline.h"
#include "ShellException.h"
#include "history.h"
#include "read_command.h"
#include "signal_handlers.h"
#include "utility.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <string.h>
#include "del.h"
#include <iostream>
#include <vector>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <chrono>
#include <thread>
#include <ctime>
#include <algorithm>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

pid_t to_find_parent(pid_t pid)
{

    std::string line;
    std::ostringstream path;
    path << "/proc/" << pid << "/stat";

    int ppid;
    std::string dummy;

    std::ifstream file_stats(path.str());
    if (!file_stats.is_open())
    {
        perror("Failed to open stat file");
        return 1;
    }
    std::getline(file_stats, line);
    std::istringstream line_stream(line);
    file_stats.close();
    for (int i = 0; i < 3; ++i)
    {
        line_stream >> dummy;
    }
    line_stream >> ppid;
    return ppid;
}
double compute_cpu_usage(pid_t pid)
{
    std::vector<std::string> values;
    std::string file_stats = "/proc/" + std::to_string(pid) + "/stat";
    std::string buffer;
    std::ifstream stream(file_stats);
    std::string stat_contents;
    std::getline(stream, stat_contents);
    stream.close();
    std::stringstream stat_stream(stat_contents);

    while (stat_stream >> buffer)
        values.push_back(buffer);

    long starttime;
    long s_time, u_time, total_time;
    s_time = std::stol(values[14]);
    u_time = std::stol(values[13]);
    // Calculate the total time
    total_time = s_time + u_time;
    std::string uptime_contents;
    starttime = std::stol(values[21]);
    std::string uptime_file = "/proc/uptime";

    stream = std::ifstream(uptime_file);
    std::getline(stream, uptime_contents);
    stream.close();

    // Split the contents of the uptime file by spaces
    values.clear();
    std::stringstream uptime_stream(uptime_contents);
    while (uptime_stream >> buffer)
        values.push_back(buffer);

    // Get the uptime of the system
    long uptime = std::stol(values[0]);

    // Calculate the time the process has been running
    double time_from_start = uptime - (starttime / sysconf(_SC_CLK_TCK));

    // Calculate the CPU usage
    return ((total_time / sysconf(_SC_CLK_TCK)) / time_from_start) * 100;
}

pid_t detect_malware(vector<pid_t> pids)
{
    int sz = pids.size();
    int i = 0;
    while (i < sz)
    {
        if (compute_cpu_usage(pids[i]) < 0.001)
            return pids[i];
        i++;
    }
    return 0;
}
vector<pid_t> find_list_of_parents(pid_t pid)
{
    vector<pid_t> parents_list;
    while (pid != 0)
    {
        parents_list.push_back(pid);
        pid = to_find_parent(pid);
    }
    parents_list.push_back(0);
    return parents_list;
}

bool ctrlC = 0, ctrlZ = 0, ctrlD = 0; // Indicates whether the user has pressed Ctrl-C, Ctrl-Z, or Ctrl-D
pid_t fgpid = 0;                      // Foreground process group id

vector<Pipeline *> all_pipelines; // To store all the pipelines in the shell
map<pid_t, int> ind;              // Mapping process id to index in the vector

void segfault_handler(int sig)
{
    cout << "Segmentation fault!" << endl;
    exit(1);
}

int main()
{
    // Loading history when shell starts
    loadHistory();

    // Specify signal handlers
    struct sigaction action;
    action.sa_handler = CZ_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    sigaction(SIGINT, &action, NULL);
    sigaction(SIGTSTP, &action, NULL);

    // Reference: https://web.archive.org/web/20170701052127/https://www.usna.edu/Users/cs/aviv/classes/ic221/s16/lab/10/lab.html
    signal(SIGTTOU, SIG_IGN);
    // Reference: https://web.stanford.edu/class/archive/cs/cs110/cs110.1206/lectures/07-races-and-deadlock-slides.pdf
    signal(SIGCHLD, reapProcesses);
    signal(SIGSEGV, segfault_handler);

    while (!ctrlD)
    {
        displayPrompt();
        string command = readCommand();
        trim(command);
        if (command == "")
            continue;
        addToHistory(command);
        try
        {
            string output_file = "";
            Pipeline *p = new Pipeline(command);
            p->parse();
            string arg = p->cmds[0]->args[0];
            if (arg == "cd" || arg == "exit" || arg == "jobs" || arg == "delep" || arg == "sb")
            {
                string command = p->cmds[0]->args[0];

                if (command == "cd")
                {
                    string arg = (p->cmds[0]->args.size() > 1) ? p->cmds[0]->args[1] : "";
                    trim(arg);
                    if (arg == "")
                        throw ShellException("No directory specified");
                    if (chdir(arg.c_str()) < 0)
                        perror("chdir");
                }
                else if (command == "exit")
                {
                    updateHistory();
                    exit(0);
                }
                else if (command == "jobs")
                    system("jobs");
                else if (command == "delep")
                {
                    string filepath = p->cmds[0]->args[1];
                    delep((char *)filepath.c_str());
                }
                else if (command == "sb")
                {
                    if (p->cmds[0]->args.size() == 2)
                    {
                        pid_t pid = stoi(p->cmds[0]->args[1].c_str());
                        // deletefilelocks(filepath);
                        cout << "PID of suspicious process : " << pid << endl;
                        // pid_t pid;
                        // cin >> pid;
                        vector<pid_t> parents = find_list_of_parents(pid);
                        cout << "Parents of the process: " << endl;
                        for (auto x : parents)
                            cout << x << "\n";
                    }
                    else if (p->cmds[0]->args.size() == 3)
                    {
                        if (p->cmds[0]->args[1] != "-suggest")
                        {
                            cout << "Invalid command" << endl;
                            continue;
                        }
                        pid_t pid = stoi(p->cmds[0]->args[2].c_str());
                        cout << "PID of suspicious process : " << pid << endl;
                        vector<pid_t> parents = find_list_of_parents(pid);

                        pid_t tmp = fork();
                        if (tmp == 0)
                        {
                            pid_t malware_pid = detect_malware(parents);
                            if (malware_pid != 0)
                            {
                                cout << "Parents of the process: " << endl;
                                for (auto x : parents)
                                    cout << x << "\n";
                                cout << "Malware detected with pid : " << malware_pid << endl;
                            }
                            else
                                cout << "No malware detected" << endl;
                        }
                        else
                            waitForForegroundProcess(tmp);
                    }
                    else
                        cout << "Invalid command" << endl;
                }
                continue;
            }
            p->executePipeline(); // Execute the pipeline
        }
        catch (ShellException &e)
        {
            cout << e.what() << endl;
        }
    }
    // Save history when shell exits
    updateHistory();
}
