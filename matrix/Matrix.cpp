#include <cstring>
#include "Matrix.hpp"
#include <string>
#include <iostream>

Matrix::Matrix(size_t rowNum, size_t colNum, const char * rawData): _rowNum(rowNum), _colNum(colNum), _matrixData(new int [_rowNum*_colNum]) {
    std::cout << "matrix constructor is called" << std::endl;
    memcpy(_matrixData.get(), rawData, _colNum * _rowNum * sizeof(int));
}

Matrix::Matrix(const Matrix & matrix) {
    std::cout << "matrix copy constructor has been called" << std::endl;
    if (this == &matrix) {
        return;
    }
    _colNum = matrix._colNum;
    _rowNum = matrix._rowNum;
    _matrixData.reset(new int[_colNum * _rowNum]);
    memcpy(_matrixData.get(), matrix._matrixData.get(), _colNum * _rowNum * sizeof(int));
    return;
}

string Matrix::to_string() {
    std::string resString = "Dim: " + std::to_string(_colNum) + ", " + std::to_string(_rowNum) + " |";
    for (size_t i = 0; i < _colNum * _rowNum; i++) {
        resString.append(" ");
        resString.append(std::to_string(_matrixData[i]));
    }
    return resString;
}