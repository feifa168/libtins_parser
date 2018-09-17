#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <exception>

#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

using namespace std;
using namespace boost;

namespace sniff_ft {
    struct SniffTuple {
        std::string proto;
        std::vector<int> ports;

        SniffTuple() {}
        SniffTuple(SniffTuple &data) {
            if (this != &data) {
                proto = data.proto;
                ports = data.ports;
            }
        }

        SniffTuple& operator=(SniffTuple &data) {
            proto = data.proto;
            ports = data.ports;
        }
    };

    class SniffBase {
    };

    class ThreadSafeQueueException : public std::exception
    {
    public:
        typedef std::exception _Mybase;

        explicit ThreadSafeQueueException(const std::string& _Message) : _Mybase(_Message.c_str()) {}

        explicit ThreadSafeQueueException(const char *_Message) : _Mybase(_Message) {}
    };

    class ThreadSafeQueue {
    public:
        ThreadSafeQueue(int qsize) : queue_size(qsize) {
            if (qsize < max_queue_size) { queue_size = max_queue_size; }
        }
        bool push(SniffTuple &data) {
            boost::unique_lock<boost::mutex> bst_ulk(bst_mtx);
            if (sniff_queue.size() >= max_queue_size) {
                return false;
            }
            sniff_queue.push_back(std::move(data));
            bst_condition.notify_one();
        }

        SniffTuple pop() {
            //         while (sniff_queue.empty()) {
            //             boost::this_thread::yield();
            //         }

            boost::unique_lock<boost::mutex> bst_ulk(bst_mtx);

            if (sniff_queue.empty()) {
                throw ThreadSafeQueueException("queue is empty");
            }
            bst_condition.wait(bst_ulk);
            SniffTuple data = std::move(sniff_queue.front());
            sniff_queue.pop_front();
            return SniffTuple(data);
        }

        static const int max_queue_size = 10000;

    private:
        boost::mutex bst_mtx;
        boost::condition_variable bst_condition;

        std::list<SniffTuple> sniff_queue;
        int queue_size;
    };

    class SniffClient {
    public:
        SniffClient(asio::io_service &ioservice, unsigned short port) :
            ioservice_(ioservice),
            socket_(ioservice),
            port_(port),
            safe_queue(ThreadSafeQueue::max_queue_size)
        { }

        void start() {
            boost::thread build_thread([this]() {});
            do_connect();

            ioservice_.run();
        }
    private:
        void handle_read(const boost::system::error_code &ec) {
            if (!ec) {
                asio::ip::tcp::endpoint ep = socket_.remote_endpoint();
                cout << ep.address().to_v4().to_string() << ":" << ep.port() << " <== " << write_buf << endl;

                do_read();
            }
            else {
                cout << ec.message() << endl;
            }

        }
        void do_read() {
            socket_.async_read_some(asio::buffer(read_buf), boost::bind(&SniffClient::handle_read, this, boost::asio::placeholders::error));
        }

        void handle_write(const boost::system::error_code &ec) {
            if (!ec) {
                asio::ip::tcp::endpoint ep = socket_.remote_endpoint();
                cout << ep.address().to_v4().to_string() << ":" << ep.port() << " ==> " << write_buf << endl;

                boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));

                do_write();
            }
            else {
                cout << ec.message() << endl;
            }
        }
        void do_write() {
            static int i = 0;
            char buf[32];
            itoa(++i, buf, 10);
            write_buf = buf;

            try {
                SniffTuple &data = safe_queue.pop();
                memcpy((void *)write_buf.c_str(), (void*)&data, sizeof(data));
            }
            catch (ThreadSafeQueueException &ec) {
                cout << ec.what() << endl;
            }
            socket_.async_write_some(asio::buffer(write_buf), boost::bind(&SniffClient::handle_write, this, boost::asio::placeholders::error));
        }

        void handle_connect(const boost::system::error_code &ec) {
            if (!ec) {
                cout << "client is connect" << endl;

                //do_read();
                do_write();
            }
            else {
                cout << ec.message() << endl;
            }
        }
        void do_connect() {
            socket_.async_connect(asio::ip::tcp::endpoint(asio::ip::address_v4::from_string("127.0.0.1"), port_),
                boost::bind(&SniffClient::handle_connect, this, boost::asio::placeholders::error));
        }
    private:
        asio::io_service &ioservice_;
        asio::ip::tcp::socket socket_;
        unsigned short port_;
        std::string read_buf;
        std::string write_buf;

        ThreadSafeQueue  safe_queue;
    };

    void test_thread_safe_queue() {
        ThreadSafeQueue safe_queue(100);

        boost::thread put_thread([&safe_queue]() {
            char buf[32];
            for (int i = 0; i < 1000; i++) {
                SniffTuple data;
                itoa(++i, buf, 10);
                data.proto = "proto ";
                data.proto += buf;

                for (int j = 0; j < i; j++) {
                    if (i > 3)
                        break;

                    data.ports.push_back(i + 1);
                }

                cout << "               <== proto " << data.proto << " ports ";
                for (int port : data.ports) {
                    cout << port << ", ";
                }
                cout << endl;
                safe_queue.push(data);

                boost::this_thread::sleep_for(boost::chrono::milliseconds(500));
            }
        });
        boost::thread get_thread([&safe_queue]() {
            for (int i = 0; i < 1000; i++) {
                try {
                    SniffTuple &data = safe_queue.pop();
                    cout << " ==> proto " << data.proto << " ports ";
                    for (int port : data.ports) {
                        cout << port << ", ";
                    }
                    cout << endl;
                }
                catch (ThreadSafeQueueException &ec) {
                    cout << ec.what() << endl;
                }
                boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
            }
        });

        put_thread.join();
        get_thread.join();
    }
}
int main(int num, char **argvs) {

    sniff_ft::test_thread_safe_queue();
    return 0;

    asio::io_service ioservice;
    sniff_ft::SniffClient client(ioservice, 1688);
    client.start();

    return 0;
}