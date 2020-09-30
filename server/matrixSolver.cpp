#include <iostream>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <string>
#include "matrix/Matrix.hpp"
#include "server/SolverRequest.hpp"
#include "server/MatrixService.hpp"
#include "server/MatrixAcceptor.hpp"
using namespace boost::asio;
using ip::tcp;
using std::string;
using std::cout;
using std::cerr;
using std::endl;
#define MAXCONNSIZE 10
#define DEFAULT_THREAD_POOL_SIZE 1
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

class Server {
public:
	Server() {
		m_work.reset(new asio::io_service::work(m_ios));
	}

	// Start the server.
    // Server has threaded pools, and also accept asynchronous connections
	void Start(unsigned short port_num,
		unsigned int thread_pool_size) {

		assert(thread_pool_size > 0);

		// Create and start Acceptor.
		acc.reset(new MatrixAcceptor(m_ios, port_num));
		acc->Start(); //this will return immediately.

		// Create specified number of threads and 
		// add them to the pool.
		for (unsigned int i = 0; i < thread_pool_size; i++) {
			std::unique_ptr<std::thread> th(
                new std::thread([this]()
                {
                    m_ios.run();
                }));
			m_thread_pool.push_back(std::move(th));
		}
	}

	// Stop the server.
	void Stop() {
		acc->Stop();
		m_ios.stop();

		for (auto& th : m_thread_pool) {
			th->join();
		}
	}

private:
	asio::io_service m_ios;
	std::unique_ptr<asio::io_service::work> m_work;
	std::unique_ptr<MatrixAcceptor> acc;
	std::vector<std::unique_ptr<std::thread>> m_thread_pool;
};

int main() {
    int port_num = 80;
    std::cout<<"This is a matrix solver. Port num: " << port_num <<std::endl;
    boost::system::error_code ec;

    Server srv;
    unsigned int thread_pool_size = 1;
        // std::thread::hardware_concurrency() * 2;
    if (thread_pool_size == 0) {
        thread_pool_size = DEFAULT_THREAD_POOL_SIZE;
    }
    srv.Start(port_num, thread_pool_size);
    std::this_thread::sleep_for(std::chrono::minutes(10));
	srv.Stop();
}