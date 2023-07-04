#include "del.h"

void get_pids_with_open_file(char *filename, int *pids, int *pid_count)
{
    char buffer[1024];
    strcpy(buffer, "lsof ");
    strcat(buffer, filename);
    FILE *fp = popen(buffer, "r");
    if (fp == NULL)
    {
        perror("popen");
        return;
    }
    while (fgets(buffer, 1024, fp) != NULL)
    {
        int pid;
        printf("%s\n", buffer);
        if (sscanf(buffer, "%*s %d", &pid) == 1)
            pids[(*pid_count)++] = pid;
    }
    pclose(fp);
}

void delep(char *filepname)
{
    int pids[1024];
    int pid_count = 0;
    get_pids_with_open_file(filepname, pids, &pid_count);
    if (pid_count == 0)
    {
        printf("No processes holding locks on file %s\n", filepname);
        return;
    }
    printf("Processes holding locks on file %s: \n", filepname);
    for (int i = 0; i < pid_count; i++)
        printf("PID %d\n", pids[i]);
    printf("Kill processes holding locks on file %s? (y/n): ", filepname);
    char answer;
    scanf("%c", &answer);
    if (answer == 'y')
    {
        for (int i = 0; i < pid_count; i++)
            kill(pids[i], SIGKILL);
    }
    else if (answer == 'n')
        return;
    else
    {
        printf("Invalid input\n");
        return;
    }
    remove(filepname);
    printf("File %s deleted\n", filepname);
}