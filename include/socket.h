/**
 * @file
 */

#ifndef SOCKET_H
#define SOCKET_H

#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <tuple>

#include <sys/socket.h>
#include <sys/types.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>

#include <openssl/crypto.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>

#include <chrono>
#include <iostream>
#include <mutex>
#include <queue>
#include <regex>
#include <thread>

/**
 * @brief Socket class
 * @details Socket class that creates TCP connection to
 * a server and saves the state, has methods to send and
 * receive messages from server.
 *
 */
class Socket {
 public:
  /**
   * @brief Create function creates a socket
   * @details Create function creates a TCP Connection
   * over which the messages and sent to and received from.
   *
   * @param hostname The hostname of the server to connect to.
   * @param port The port of the server to connect to.
   *
   * @return Status of creation and any error message if failed to
   * create socket.
   */
  std::tuple<bool, std::string> create(std::string hostname, int port);

  /**
   * @brief CreateSSL function creates a wrapper of OpenSSL over the socket
   * @details This enables to connect to server using TLSv1.2. It retrieves the
   * certificate of the server and performs TLS handshake.
   * @return Satus of ssl connection and error message if failed.
   */
  std::tuple<bool, std::string> createSSL();

  /**
   * @brief Send function sends a message to the server.
   * @details The message is sent to the server over a TCP connection.
   *
   * @param s The message that is to be sent.
   * @return Status whether the send was successful.
   */
  bool send(const std::string &s);

  /**
   * @brief Receive function receives a message from the server.
   * @details A thread receives in parallel and stores the message
   * in a queue from which this function returns one.
   * @return The message received from the server.
   */
  std::string receive();

  /**
   * @brief Receive function receives a message from the server
   * @details This function pops all those messages form the queues until
   * some message matches the regex provided.
   *
   * @param rgx The regex which should be matched with message.
   * @return The message received form the server.
   */
  std::string receive(std::regex rgx);
  ~Socket();

 private:
  /**
   * @brief The id of the socket.
   */
  int sockid;
  /**
   * @brief SSL Context required by OpenSSL
   */
  SSL_CTX *ctx;
  /**
   * @brief The OpenSSL object
   */
  SSL *ssl;
  /**
   * @brief Thread that receives messages from the server in the background
   */
  std::thread receiver;
  /**
   * @brief Lock used while pushing and poping from the messages queue
   */
  std::mutex mtx;
  /**
   * @brief The queue where receiver thread stores the messages
   */
  std::queue<std::string> messages;
  /**
   * @brief Private function to create receiver thread
   */
  void createThread();
};

#endif