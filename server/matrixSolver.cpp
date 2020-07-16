#include <iostream>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <string>
#include "matrix/Matrix.hpp"
using namespace boost::asio;
using ip::tcp;
using std::string;
using std::cout;
using std::endl;
#define MAXCONNSIZE 10
/* send a mesage */
void send_(tcp::socket & socket, const string& message) {
    const string msg = message + "\n";
    boost::asio::write( socket, boost::asio::buffer(message) );
}

Matrix read_(tcp::socket & socket) {
    boost::asio::streambuf buf;
    boost::asio::read_until(socket, buf, "\n");
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
    Matrix matrix(rowNum, colNum, rawBytes+dataIdx);
    return matrix;
}

/* recieve a message */
int main() {
    std::cout<<"This is a matrix solver."<<std::endl;
    boost::asio::io_service io_service;
    // the alternative approach to the following part to instantiate first and then bind the port
    tcp::acceptor acceptor_(io_service, tcp::endpoint(tcp::v6(), 80)); //acceptor represents acceptor socket which is pasisve
    while (1) {
        try {
            tcp::socket socket_(io_service); //instantiate asocket that used for teh connection
            acceptor_.listen(MAXCONNSIZE);
            acceptor_.accept(socket_); //switched to the active socket and start the connection
            Matrix sampleMatrix = read_(socket_);
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