CC = gcc
CCFLAGS = -W -Wall

SOURCES = Hex_Parser.c Main.c Protocol.c UART_Linux.c UART_Windows.c

BINARY = Explorer

all:
	$(CC) $(CCFLAGS) $(SOURCES) -o $(BINARY)

debug:
	$(CC) $(CCFLAGS) -DCONFIGURATION_ENABLE_DEBUG=1 $(SOURCES) -o $(BINARY)
	
clean:
	rm -f $(BINARY)
