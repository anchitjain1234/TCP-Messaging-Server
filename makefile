CC:=gcc
FLAGS:=-g
PROG:=server
F:=select_server

all:
	$(CC) $(FLAGS) $(F).c -o $(PROG)

clean:
	rm *$(PROG)
