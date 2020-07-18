#include <iostream>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <string>
#include "matrix/Matrix.hpp"
#include "server/SolverRequest.hpp"
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
    boost::asio::streambuf buf;
    boost::asio::read(socket, buf, ec);
    if (ec != boost::asio::error::eof) {
        cerr << "Error Code" << ec << endl;
        throw string("fail to read the request from client");
    }
    const solverreq::request * currReqPtr = boost::asio::buffer_cast<const solverreq::request*>(buf.data());
    if (*currReqPtr != solverreq::inverse && *currReqPtr != solverreq::solve) {
        throw string("invalid request type");
    }
    return *currReqPtr;
}

Matrix read_matrix_(tcp::socket & socket, boost::system::error_code & ec) {
    boost::asio::streambuf buf;
    boost::asio::read(socket, buf, ec);
    if (ec != boost::asio::error::eof) {
        cerr << "Error Code" << ec << endl;
        throw string("fail to read the request from client");
    }
    const char * rawBytes = boost::asio::buffer_cast<const char *>(buf.data());
    
    //get the dimension of the matrix
    string dimenData;
    size_t dataIdx;
    for (size_t idx = 0; idx < buf.size(); idx++) {
        if (rawBytes[idx] == '|') {
            dimenData = string(rawBytes, idx);
            dataIdx = idx+1;
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
    cout << "test | dimension row: " << rowNum << " col: " << colNum << endl;
    //create the matrix object
    return Matrix(rowNum, colNum, rawBytes+dataIdx);
}

/* recieve a message */
int main() {
    if (solverreq::inverse != solverreq::solve) {
        std::cout<<"testing enum" << endl;
    }
    std::cout<<"This is a matrix solver."<<std::endl;
    boost::system::error_code ec;
    boost::asio::io_service io_service;
    // the alternative approach to the following part to instantiate first and then bind the port
    tcp::acceptor acceptor_(io_service, tcp::endpoint(tcp::v6(), 80)); //acceptor represents acceptor socket which is pasisve
    while (1) {
        try {
            tcp::socket socket_(io_service); //instantiate asocket that used for teh connection
            acceptor_.listen(MAXCONNSIZE);
            acceptor_.accept(socket_); //switched to the active socket and start the connection
            solverreq::request req = read_request_(socket_, ec);
            cout<<"Show request: " << req << endl;
            Matrix sampleMatrix = read_matrix_(socket_, ec);
            cout << "test | sample matrix: " << sampleMatrix.to_string() << endl;
            //write operation
            send_(socket_, "hello this is a matrix solver");
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