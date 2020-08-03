#pragma once
#include <boost/asio.hpp>
#include <thread>
#include <atomic>
#include <memory>
#include <iostream>
#include <boost/numeric/ublas/matrix.hpp>
#include "matrix/Matrix.hpp"
#include "server/SolverRequest.hpp"
using namespace boost;
namespace ublas = boost::numeric::ublas;
class MatrixService {
public:
    MatrixService(std::shared_ptr<asio::ip::tcp::socket> sock);
    void StartHandling();
private:
    void onRequestReceived(const boost::system::error_code& ec, std::size_t bytes_transferred);
    void onResponseSent(const boost::system::error_code& ec, std::size_t bytes_transferred);
    std::string ProcessRequest(Matrix& input_mat);
    Matrix readMatrix(asio::ip::tcp::socket & socket, boost::system::error_code & ec);
    Matrix parseMatrix(const char * rawBytes, size_t & offset, size_t total_size);
    void onFinish();
    std::shared_ptr<ublas::matrix<float>> convertMatToUblasMat(const Matrix & mat);
    bool inverseMatrix(const Matrix & input, Matrix & inverse);
private:
    solverreq::request req;
    std::shared_ptr<asio::ip::tcp::socket> m_sock;
    std::string m_response; // this response might be changed in the future
    Matrix m_request; //this can be changed as a pointer
    boost::asio::streambuf buf;
};