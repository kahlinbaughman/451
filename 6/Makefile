CC=gcc
CFLAGS=-g -Wall
LDFLAGS=
DEPS=checksum.h

SRC=checksum.c icmp_ex.c
OBJ=checksum.o icmp_ex.o
BIN=icmp_ex

all: $(BIN)

$(BIN): $(SRC) $(OBJ)

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) $(LDfLAGS) -c -o $@ $<

icmp_ex: checksum.o icmp_ex.o
	$(CC) $(CFLAGS) $(LDFLAGS)    -o icmp_ex checksum.o icmp_ex.o


clean:
	rm -f $(BIN) *.o


