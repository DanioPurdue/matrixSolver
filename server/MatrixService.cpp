#include "MatrixService.hpp"
#include "matrix/Matrix.hpp"
using namespace boost;

MatrixService::Service(std::shared_ptr<asio::ip::tcp::socket> sock):m_sock(sock) {}

//the start handling has two function call, it first parse and figure out the request method, 
//based on the request method, it figure out the right approach.
void MatrixService::StartHandling() {
    asio::async_read(*m_sock.get(), boost::asio::buffer(&req, sizeof(solverreq::reqeust)), 
    [this](const boost::system::error_code& ec, std::size_t bytes_transferred)
    {
        //check the request type of the first read
        if (ec != 0) {
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
            boost::asio::streambuf buf;
            boost::asio::async_read(*m_sock.get(), buf, [this, &buf](const boost::system::error_code& ec, std::size_t bytes_transferred){
                if (ec != 0) {
                    std::cout<< "Error occured! Error code = " 
                    << ec.value()
                    << ". Message: " << ec.message();
                    onFinish();
                    return;
                }
                const char * rawBytes = boost::asio::buffer_cast<const char *>(buf.data());
                cout << "test | print raw data: " << buf.size() << endl;
                size_t offset(0);
                m_request = parse_matrix(rawBytes, offset, buf.size());
                onRequestReceived(ec, bytes_transferred);
            });
        }
    });
}

void MatrixService::onRequestReceived(const boost::system::error_code& ec, std::size_t bytes_transferred) {
    if (ec != 0) {
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
    if (ec != 0) {
         std::cout << "Error occured! Error code = " << ec.value() << ". Message: " << ec.message();
    }
    onFinish();
}

void MatrixService::onFinish() {
    delete this;
}

std::string MatrixService::ProcessRequest(Matrix & input_mat) {
    std::cout << input_mat.to_string() << std::endl;
    std::string response = "Response\n";
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
    cout << "test | dimension row: " << rowNum << " col: " << colNum << endl;
    size_t matrixSize(sizeof(float) * rowNum * colNum);
    offset += matrixSize;
    //create the matrix object
    return Matrix(rowNum, colNum, rawBytes+offset-matrixSize);
}

