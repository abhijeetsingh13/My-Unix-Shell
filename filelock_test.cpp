#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/file.h>

int main()
{
    int pid = fork();
    if (pid == 0)
    {
        // Child process
        int fd = open("a.txt", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        if (fd < 0)
        {
            std::cout << "Error opening file" << std::endl;
            return 1;
        }
        if (flock(fd, LOCK_EX) < 0)
        {
            std::cout << "Error locking file" << std::endl;
            return 1;
        }
        std::cout << "Child process has locked the file" << std::endl;
        // loop until the file is locked
        while (1)
        {
            sleep(1);
            std::cout << "Child process is running" << std::endl;
        }
    }
    else
    {
        // Parent process
        sleep(1); // Wait for child process to lock the file
        waitpid(pid, NULL, 0);
    }
    return 0;
}