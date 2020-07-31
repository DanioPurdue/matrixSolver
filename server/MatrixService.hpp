#pragma once
#include <boost/asio.hpp>
#include <thread>
#include <atomic>
#include <memory>
#include <iostream>
#include "matrix/Matrix.hpp"
#include "server/SolverRequest.hpp"
using namespace boost;
class MatrixService {
public:
    Service(std::shared_ptr<asio::ip::tcp::socket> sock);
    void StartHandling();
private:
    void onRequestReceived(const boost::system::error_code& ec, std::size_t bytes_transferred);
    void onResponseSent(const boost::system::error_code& ec, std::size_t bytes_transferred);
    void std::string ProcessRequest(Matrix& input_mat);
    Matrix readMatrix(tcp::socket & socket, boost::system::error_code & ec);
private:
    solverreq::reqeust req;
    std::shared_ptr<asio::ip::tcp::socket> m_sock;
    std::string m_response; // this response might be changed in the future
    Matrix m_request; //this can be changed as a pointer
};