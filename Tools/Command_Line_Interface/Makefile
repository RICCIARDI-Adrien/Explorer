CC = gcc
CCFLAGS = -W -Wall

INCLUDES = -ISerial_Port_Library/Includes
SOURCES = Hex_Parser.c Main.c Protocol.c Serial_Port_Library/Sources/Serial_Port_Linux.c Serial_Port_Library/Sources/Serial_Port_Windows.c

BINARY = Explorer

all:
	$(CC) $(CCFLAGS) $(SOURCES) $(INCLUDES) -o $(BINARY)

debug:
	$(CC) $(CCFLAGS) -DCONFIGURATION_ENABLE_DEBUG=1 $(SOURCES) $(INCLUDES) -o $(BINARY)
	
clean:
	rm -f $(BINARY)
