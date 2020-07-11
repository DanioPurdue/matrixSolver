
CC = g++
CFLAGS = -Wall -Wextra -std=c++11
OBJFILES = matrixSolver.o
TARGET = matrixSolver
BOOSTLIB = -L /usr/lib/ -lboost_system -lboost_thread -lpthread
$(TARGET): $(OBJFILES)
	$(CC) -o $(TARGET) $(OBJFILES) $(BOOSTLIB)
matrixSolver.o: matrixSolver.cpp
	$(CC) $(CFLAGS) -c matrixSolver.cpp
clean:
	rm -f $(OBJFILES) $(TARGET)