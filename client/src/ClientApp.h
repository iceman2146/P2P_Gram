#include <cstdlib>
#include <iostream>
#include <queue>
#include <string>


#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/bind/bind.hpp>
#include <boost/thread.hpp>

/*------------------------------------------------------------------------*/
using namespace std;
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;

typedef boost::shared_ptr<tcp::socket> socket_ptr;
typedef boost::shared_ptr<string> string_ptr;
typedef boost::shared_ptr<queue<string_ptr>> messageQueue_ptr;
/*------------------------------------------------------------------------*/

class ClientApp

{
private:
  io_service service;            // Boost Asio io_service
  messageQueue_ptr messageQueue; // Queue for producer consumer pattern
  tcp::endpoint ep;              // TCP socket for connecting to server
  const int inputSize;           // Maximum size for input buffer
  string_ptr promptCpy;          // Terminal prompt displayed to chat users
  ~ClientApp() {}


  boost::thread_group threads;
socket_ptr sock;
public:
  ClientApp()
      : inputSize(256), messageQueue(new queue<string_ptr>),
        ep(ip::address::from_string("127.0.0.1"), 8001),
		sock(new tcp::socket(service)) {}
  string *buildPrompt() {
    const int inputSize = 256;
    char nameBuf[inputSize] = {0};
    string *prompt = new string(": ");

    cout << "Please input a new username: ";
    cin.getline(nameBuf, inputSize);
    *prompt = (string)nameBuf + *prompt;
    boost::algorithm::to_lower(*prompt);

    return prompt;
  }

  void inboundLoop(socket_ptr sock, string_ptr prompt) {
    int bytesRead = 0;
    char readBuf[1024] = {0};

    for (;;) {
      if (sock->available()) {
        bytesRead = sock->read_some(buffer(readBuf, inputSize));
        string_ptr msg(new string(readBuf, bytesRead));

        messageQueue->push(msg);
      }

      boost::this_thread::sleep(boost::posix_time::millisec(int(1000)));
    }
  }

  void writeLoop(socket_ptr sock, string_ptr prompt) {
    char inputBuf[inputSize] = {0};
    string inputMsg;

    for (;;) {
      cin.getline(inputBuf, inputSize);
      inputMsg = *prompt + (string)inputBuf + '\n';

      if (!inputMsg.empty()) {
        sock->write_some(buffer(inputMsg, inputSize));
      }

      if (inputMsg.find("exit") != string::npos)
        exit(1);

      inputMsg.clear();
      memset(inputBuf, 0, inputSize);
    }
  }

  void displayLoop(socket_ptr sock) {
    for (;;) {
      if (!messageQueue->empty()) {
        if (!isOwnMessage(messageQueue->front())) {
          cout << "\n" + *(messageQueue->front());
        }

        messageQueue->pop();
      }

      boost::this_thread::sleep(boost::posix_time::millisec(int(1000)));
    }
  }

  bool isOwnMessage(string_ptr message) {
    if (message->find(*promptCpy) != string::npos)
      return true;
    else
      return false;
  }
}