
#pragma once
#include <cstddef>
#include <memory>
class Matrix {
    public:
        size_t _rowNum;
        size_t _colNum;
        std::unique_ptr<int []> _matrixData;
    Matrix(size_t rowNum, size_t colNum, char * rawData);
};