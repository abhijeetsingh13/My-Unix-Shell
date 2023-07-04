#ifndef __DEL_H__
#define __DEL_H__

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

void get_pids_with_open_file(char *filename, int *pids, int *pid_count);
void delep(char *filename);

#endif // __DEL_H__