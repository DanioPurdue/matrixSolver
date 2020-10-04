#include <iostream>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <string>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/io.hpp>
#include "server/MatrixService.hpp"
#include "matrix/Matrix.hpp"
using namespace boost;

MatrixService::MatrixService(std::shared_ptr<asio::ip::tcp::socket> sock):m_sock(sock){

}

//the start handling has two function call, it first parse and figure out the request method, 
//based on the request method, it figure out the right approach.
//it read all the way until the end
void MatrixService::StartHandling() {
    asio::async_read_until(*m_sock.get(), buf, '\n',
    [this](const boost::system::error_code& ec, std::size_t bytes_transferred)
    {
        std::cout << "test | just in async_read " << "Bytes Recieved " << bytes_transferred << std::endl;
        const char * rawBytes = boost::asio::buffer_cast<const char *>(buf.data());
        memcpy(&req, rawBytes, sizeof(solverreq::request));
        size_t offset(sizeof(solverreq::request));
        //check the request type of the first read
        if (ec.value() != 0) {
            std::cout<< "Error occured! Error code = " 
                << ec.value()
                << ". Message: " << ec.message();
            onFinish();
            return;
        } else if (req != solverreq::inverse && req != solverreq::solve) {
            std::cout << "In valid request type" << std::endl;
            onFinish();
            return;
        } else {
            // TODO:: need to make decision based on the request type
            // matrix inversion by default by now
            std::cout << "test | about to make a second part" << std::endl;
            m_request = parseMatrix(rawBytes, offset, buf.size());
            
            onRequestReceived(ec, bytes_transferred);
        }
    });
}

void MatrixService::onRequestReceived(const boost::system::error_code& ec, std::size_t bytes_transferred) {
    if (ec.value() != 0) {
        std::cout<< "Error occured! Error code = " 
            << ec.value()
            << ". Message: " << ec.message();
        onFinish();
        return;
    }

    //process the request
    m_response = ProcessRequest(m_request);
    // initiate asynchronous write operation
    // TODO:: add the matrix inversion to here
    asio::async_write(*m_sock.get(), asio::buffer(m_response), [this](
        const boost::system::error_code& ec,
        std::size_t bytes_transferred) {
            onResponseSent(ec, bytes_transferred);
        }
    );
}

void MatrixService::onResponseSent(const boost::system::error_code& ec, std::size_t bytes_transferred) {
    if (ec.value() != 0) {
        std::cout << "Bytes transferred: " << bytes_transferred << std::endl;
        std::cout << "Error occured! Error code = " << ec.value() << ". Message: " << ec.message();
    }
    onFinish();
}

//(new MatrixService(sock))->StartHandling();  this is how matrix service is called
void MatrixService::onFinish() {
    delete this;
}

//
//TODO implement the matrix inversion
std::string MatrixService::ProcessRequest(Matrix & input_mat) {
    std::cout << "testing | input matrix: "<<input_mat.to_string() << std::endl;
    std::shared_ptr<Matrix> inverse_ptr(new Matrix(input_mat));
    if (!inverseMatrix(input_mat, *(inverse_ptr.get()))) {
        std::cerr << "testing | matrix inversion failed! " << std::endl;
    } else {
        //print the inverted matrix
        std::cout <<"testing | matrix after inversion: " <<inverse_ptr->to_string() << std::endl;
    }
    std::string response = "This is a dummy Response\n";
    return inverse_ptr->to_string();
}

Matrix MatrixService::parseMatrix(const char * rawBytes, size_t & offset, size_t total_size) {
    //get the dimension of the matrix
    size_t start_pt = offset;
    string dimenData;
    for (size_t idx = offset; idx < total_size; idx++) {
        if (rawBytes[idx] == '|') {
            dimenData = string(rawBytes, idx);
            offset = idx+1;
            break;
        }
    }

    if (dimenData.length() == 0) { 
        throw string("wrong matrix format");
    }
    //get the dimension
    size_t spaceLoc = dimenData.find(" ");
    size_t rowNum = std::stoi(dimenData.substr(start_pt, spaceLoc));
    size_t colNum = std::stoi(dimenData.substr(spaceLoc+1, dimenData.length() - spaceLoc - 1));
    std::cout << "test | dimension row: " << rowNum << " col: " << colNum << std::endl;
    size_t matrixSize(sizeof(float) * rowNum * colNum);
    offset += matrixSize;
    //create the matrix object
    return Matrix(rowNum, colNum, rawBytes+offset-matrixSize);
}

std::shared_ptr<ublas::matrix<float>> MatrixService::convertMatToUblasMat(const Matrix & mat) {
    std::shared_ptr<ublas::matrix<float>> ublas_mat_ptr(new ublas::matrix<float>(mat._rowNum, mat._colNum));
    for (size_t i = 0; i < mat._rowNum; i++) {
        for (size_t j = 0; j < mat._colNum; j++) {
            (*ublas_mat_ptr.get())(i, j) = mat.get(i, j);
        }
    }
    return ublas_mat_ptr;
}

bool MatrixService::inverseMatrix(const Matrix & input, Matrix & inverse) {
    auto ublas_input_ptr = convertMatToUblasMat(input);
    auto ublas_invert_ptr = convertMatToUblasMat(inverse);
    ublas::permutation_matrix<std::size_t> pm(ublas_input_ptr->size1());
    int res = lu_factorize(*ublas_input_ptr.get(), pm);
    if (res != 0) {
        return false;
    }

    (ublas_invert_ptr.get())->assign(ublas::identity_matrix<float>(ublas_input_ptr->size1()));
    lu_substitute(*ublas_input_ptr.get(), pm, *ublas_invert_ptr.get());
    for (size_t i = 0; i < input._rowNum; i++) {
        for (size_t j = 0; j < input._colNum; j++) {
            inverse.set(i, j, (*ublas_invert_ptr.get())(i, j));
        }
    }
    return true;
}
