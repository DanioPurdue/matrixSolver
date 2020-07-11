
CC = g++
CFLAGS = -Wall -Wextra -std=c++11
OBJFILES = matrixSolver.o
TARGET = matrixSolver
$(TARGET): $(OBJFILES)
	$(CC) -o $(TARGET) $(OBJFILES)
matrixSolver.o: matrixSolver.cpp
	$(CC) $(CFLAGS) -c matrixSolver.cpp
clean:
	rm -f $(OBJFILES) $(TARGET)