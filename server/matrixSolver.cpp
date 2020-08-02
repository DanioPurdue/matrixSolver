#include <iostream>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <string>
#include "matrix/Matrix.hpp"
#include "server/SolverRequest.hpp"
#include "server/MatrixService.hpp"
using namespace boost::asio;
using ip::tcp;
using std::string;
using std::cout;
using std::cerr;
using std::endl;
#define MAXCONNSIZE 10

/* send a mesage */
void send_(tcp::socket & socket, const string& message) {
    const string msg = message + "\n";
    boost::asio::write( socket, boost::asio::buffer(message) );
}

solverreq::request read_request_(tcp::socket & socket, boost::system::error_code & ec) {
    // boost::asio::streambuf buf;
    solverreq::request req;
    boost::asio::read(socket, boost::asio::buffer(&req, sizeof(solverreq::request)), ec);
    if (req != solverreq::inverse && req != solverreq::solve) {
        throw string("invalid request type");
    }
    return req;
}

Matrix parse_matrix(const char * rawBytes, size_t & offset, size_t total_size) {
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

Matrix read_matrix_(tcp::socket & socket, boost::system::error_code & ec) {
    boost::asio::streambuf buf;
    boost::asio::read(socket, buf, ec);
    if (ec != boost::asio::error::eof) {
        cerr << "Error Code" << ec << endl;
        throw string("fail to read the request from client");
    }
    const char * rawBytes = boost::asio::buffer_cast<const char *>(buf.data());
    cout << "test | print raw data: " << buf.size() << endl;
    size_t offset(0);
    return parse_matrix(rawBytes, offset, buf.size());
}

/* recieve a message */
int main() { 
    int port_num = 80;
    std::cout<<"This is a matrix solver. Port num: " << port_num <<std::endl;
    boost::system::error_code ec;
    boost::asio::io_service io_service;
    // the alternative approach to the following part to instantiate first and then bind the port
    tcp::acceptor acceptor_(io_service, tcp::endpoint(tcp::v6(), port_num)); //acceptor represents acceptor socket which is pasisve
    while (1) {
        try {
            tcp::socket socket_(io_service); //instantiate asocket that used for teh connection
            std::shared_ptr<asio::ip::tcp::socket> sock(new asio::ip::tcp::socket(io_service));
            acceptor_.listen(MAXCONNSIZE);
            acceptor_.accept(*sock.get()); //switched to the active socket and start the connection
            (new MatrixService(sock))->StartHandling();
            cout << "request has been sent" << endl;
        } catch(boost::system::system_error & sysError) {
            cout << "System Error | Error Code: " <<sysError.code() << " | Error Message: " << sysError.what()<<endl;
        } catch (string & errMsg) {
            cout << "Error message: " << errMsg << endl;
        } catch (...) {
            cout << "recieved an invalid request" << endl;
        }
    }
    return 0;
}