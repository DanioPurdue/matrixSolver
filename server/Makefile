
CC = g++
PROJPATH = ..
CFLAGS = -Wall -Wextra -std=c++11 -I$(PROJPATH)
OBJFILES = matrixSolver.o matrix.o
TARGET = matrixSolver
BOOSTLIB = -L /usr/lib/ -lboost_system -lboost_thread -lpthread
$(TARGET): $(OBJFILES)
	$(CC) -o $(TARGET) $(OBJFILES) $(BOOSTLIB)

matrixSolver.o: matrixSolver.cpp
	$(CC) $(CFLAGS) -c matrixSolver.cpp

matrix.o: $(PROJPATH)/matrix/Matrix.cpp
	$(CC) $(CFLAGS) -c $(PROJPATH)/matrix/Matrix.cpp

clean:
	rm -f $(OBJFILES) $(TARGET)