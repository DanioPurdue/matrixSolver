# server makefile
CC = g++
PROJPATH = ..
CFLAGS = -Wall -Wextra -std=c++11 -I$(PROJPATH)
OBJFILES = matrixSolver.o matrix.o matrixService.o matrixAcceptor.o
TARGET = matrixSolver
BOOSTLIB = -L /usr/lib/ -lboost_system -lboost_thread -lpthread
$(TARGET): $(OBJFILES)
	$(CC) -o $(TARGET) $(OBJFILES) $(BOOSTLIB)

matrixSolver.o: matrixSolver.cpp
	$(CC) $(CFLAGS) -c matrixSolver.cpp

matrixAcceptor.o: MatrixAcceptor.cpp
	$(CC) $(CFLAGS) -c MatrixAcceptor.cpp

matrixService.o: MatrixService.cpp
	$(CC) $(CFLAGS) -c MatrixService.cpp

matrix.o: $(PROJPATH)/matrix/Matrix.cpp
	$(CC) $(CFLAGS) -c $(PROJPATH)/matrix/Matrix.cpp

clean:
	rm -f $(OBJFILES) $(TARGET)