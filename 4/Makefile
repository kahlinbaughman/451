PROGRAM1=server
PROGRAM2=client
CC=gcc

default: compile

run: $(PROGRAM)
	@./$(PROGRAM)

compile: $(PROGRAM1) $(PROGRAM2)

$(PROGRAM1): $(PROGRAM1).o
	$(CC) $(PROGRAM1).o -o $(PROGRAM1) -pthread

$(PROGRAM1).o: $(PROGRAM1).c
	$(CC) -c $(PROGRAM1).c

$(PROGRAM2): $(PROGRAM2).o
	$(CC) $(PROGRAM2).c -o $(PROGRAM2) -pthread

$(PROGRAM2).o: $(PROGRAM2).c
	$(CC) -c $(PROGRAM2).c


clean:
	/bin/rm -f *.o $(PROGRAM1) $(PROGRAM2)

