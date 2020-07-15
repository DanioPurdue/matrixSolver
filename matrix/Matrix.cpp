#include <cstring>
#include "Matrix.hpp"
Matrix::Matrix(size_t rowNum, size_t colNum, char * rawData): _rowNum(rowNum), _colNum(colNum), _matrixData(new int [_rowNum*_colNum]) {
    memcpy(_matrixData.get(), rawData, _colNum * _rowNum * sizeof(int));
}