CC = g++
PROJPATH = ../
CFLAGS = -Wall -Wextra -std=c++14 -I$(PROJPATH)
OBJFILES = matrixClient.o
TARGET = matrixClient
BOOSTLIB = -L /usr/lib/ -lboost_system -lboost_thread -lpthread
$(TARGET): $(OBJFILES)
	$(CC) -o $(TARGET) $(OBJFILES) $(BOOSTLIB)
matrixClient.o: matrixClient.cpp
	$(CC) $(CFLAGS) -c matrixClient.cpp
clean:
	rm -f $(OBJFILES) $(TARGET)