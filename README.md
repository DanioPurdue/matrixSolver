# matrixSolver

Multithreaded Asynchronous Server supports TCP connection.

```cpp
namespace solverreq {
    /*
        The type of request that the server can handle. 
        If the server recieved non of these two requests, it aborts the connection.
    */
    enum request {
        inverse=1, //require one square matrix and invert the square matrix
        solve=2 //require two matrix of matching dimenions and equation
    };
}
```



Matrix Solver server

1. find the inverse of matrix
2. solve the matrix

