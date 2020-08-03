#include <iostream>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <string>
#include "server/MatrixService.hpp"
#include "matrix/Matrix.hpp"
using namespace boost;

MatrixService::MatrixService(std::shared_ptr<asio::ip::tcp::socket> sock):m_sock(sock){

}

//the start handling has two function call, it first parse and figure out the request method, 
//based on the request method, it figure out the right approach.
void MatrixService::StartHandling() {
    asio::async_read_until(*m_sock.get(), buf, '\n',
    [this](const boost::system::error_code& ec, std::size_t bytes_transferred)
    {
        std::cout<< "test | just in async_read " << "Bytes Recieved " << bytes_transferred << std::endl;
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
            std::cout<<"test | about to make a second part" << std::endl;
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
    asio::async_write(*m_sock.get(), asio::buffer(m_response), [this](
        const boost::system::error_code& ec,
        std::size_t bytes_transferred) {
            onResponseSent(ec, bytes_transferred);
        }
    );
}

void MatrixService::onResponseSent(const boost::system::error_code& ec, std::size_t bytes_transferred) {
    if (ec.value() != 0) {
         std::cout << "Error occured! Error code = " << ec.value() << ". Message: " << ec.message();
    }
    onFinish();
}

void MatrixService::onFinish() {
    delete this;
}

std::string MatrixService::ProcessRequest(Matrix & input_mat) {
    std::cout << input_mat.to_string() << std::endl;
    std::string response = "This is a dummy Response\n";
    return response;
}

Matrix MatrixService::parseMatrix(const char * rawBytes, size_t & offset, size_t total_size) {
    //get the dimension of the matrix
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
    size_t rowNum = std::stoi(dimenData.substr(0, spaceLoc));
    size_t colNum = std::stoi(dimenData.substr(spaceLoc+1, dimenData.length() - spaceLoc - 1));
    std::cout << "test | dimension row: " << rowNum << " col: " << colNum << std::endl;
    size_t matrixSize(sizeof(float) * rowNum * colNum);
    offset += matrixSize;
    //create the matrix object
    return Matrix(rowNum, colNum, rawBytes+offset-matrixSize);
}

