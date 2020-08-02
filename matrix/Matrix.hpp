
#pragma once
#include <cstddef>
#include <memory>
using std::string;
class Matrix {
    public:
        size_t _rowNum;
        size_t _colNum;
        std::unique_ptr<float []> _matrixData;
    Matrix()=default;
    Matrix(size_t rowNum, size_t colNum, const char * rawData);
    Matrix(const Matrix & matrix);
    Matrix& operator=(const Matrix& matrix);
    string to_string();
};