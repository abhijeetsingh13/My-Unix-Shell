CC=g++
FLAGS=-Wall
RM=rm -f

NAAG: shell.o Command.o Pipeline.o read_command.o utility.o history.o signal_handlers.o ShellException.o del.o
	$(CC) $(FLAGS) shell.o Command.o Pipeline.o read_command.o utility.o history.o signal_handlers.o ShellException.o del.o -o NAAG

shell.o: shell.cpp shell.h
	$(CC) $(FLAGS) -c shell.cpp -o shell.o

Command.o: Command.cpp Command.h
	$(CC) $(FLAGS) -c Command.cpp -o Command.o

Pipeline.o: Pipeline.cpp Pipeline.h
	$(CC) $(FLAGS) -c Pipeline.cpp -o Pipeline.o

read_command.o: read_command.cpp read_command.h
	$(CC) $(FLAGS) -c read_command.cpp -o read_command.o

utility.o: utility.cpp utility.h
	$(CC) $(FLAGS) -c utility.cpp -o utility.o

history.o: history.cpp history.h
	$(CC) $(FLAGS) -c history.cpp -o history.o

signal_handlers.o: signal_handlers.cpp signal_handlers.h
	$(CC) $(FLAGS) -c signal_handlers.cpp -o signal_handlers.o

ShellException.o: ShellException.cpp ShellException.h
	$(CC) $(FLAGS) -c ShellException.cpp -o ShellException.o

del.o: del.cpp del.h
	$(CC) $(FLAGS) -c del.cpp -o del.o

clean:
	$(RM) *.o NAAG