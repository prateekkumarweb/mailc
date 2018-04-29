/**
 * @file
 */

#ifndef SMTP_H
#define SMTP_H

#include <iostream>
#include "socket.h"

/**
 * @brief SMTPConnection class
 * @details Stores socket which connect to SMTP server, has methos
 * which implemnet the SMTP commands
 *
 */
class SMTPConnection {
 public:
  /**
   * @brief Create an SMPTP Connection
   * @details Create socket, intilize ssl and create socket object
   *
   * @param hostname The hostname of the SMTP server
   * @param port The port of the SMTP server
   */
  SMTPConnection(const std::string &hostname, int port);

  /**
   * @brief Login in the server
   * @details Login in the server with given config
   *
   * @param username The username of the user
   * @param password The password of the user
   *
   * @return Whether the login was successful
   */
  bool auth(const std::string &username, const std::string &password);

  /**
   * @brief Send a mail
   * @details Send a mail using SMTP Protocol
   *
   * @param from From address
   * @param to To address
   * @param subject Subject of the mail
   * @param body Body of the mail
   * @return True if the mail is successfully sent
   */
  bool send(const std::string &from, const std::string &to,
            const std::string &subject, const std::string &body);

 private:
  Socket socket;
  std::string username;
  std::string password;
};

#endif