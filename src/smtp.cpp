/**
 * @file
 */

#include "smtp.h"
#include "utils.h"

SMTPConnection::SMTPConnection(const std::string &hostname, int port) {
  std::tuple<bool, std::string> e = socket.create(hostname, port);
  if (!std::get<0>(e)) {
    std::cerr << std::get<1>(e) << std::endl;
  }
  std::tuple<bool, std::string> e1 = socket.createSSL();
  if (!std::get<0>(e1)) {
    std::cerr << std::get<1>(e1) << std::endl;
  }
}

bool SMTPConnection::auth(const std::string &username,
                          const std::string &password) {
  std::string out;
  socket.send("EHLO prateekkumar.in\r\n");
  out = socket.receive();
  if (!std::regex_search(out, std::regex("^250"))) {
    return false;
  }
  out = socket.receive();
  if (!std::regex_search(out, std::regex("^250"))) {
    return false;
  }
  out = socket.receive();
  if (!std::regex_search(out, std::regex("^250 "))) {
    return false;
  }

  socket.send("AUTH LOGIN\r\n");
  out = socket.receive();
  if (!std::regex_search(out, std::regex("^334 "))) {
    return false;
  }
  socket.send(utils::Encode(username) + "\r\n");
  out = socket.receive();
  if (!std::regex_search(out, std::regex("^334 "))) {
    return false;
  }
  socket.send(utils::Encode(password) + "\r\n");
  out = socket.receive();
  if (!std::regex_search(out, std::regex("^235 "))) {
    return false;
  }
  // std::cerr << "AUTH success" << std::endl;
  return true;
}

bool SMTPConnection::send(const std::string &from, const std::string &to,
                          const std::string &subject, const std::string &body) {
  std::string out;
  socket.send("MAIL FROM: " + from + "\r\n");
  out = socket.receive();
  if (!std::regex_search(out, std::regex("^250 "))) {
    // std::cerr << out << std::endl;
    return false;
  }
  socket.send("RCPT TO: " + to + "\r\n");
  out = socket.receive();
  if (!std::regex_search(out, std::regex("^250 "))) {
    // std::cerr << out << std::endl;
    return false;
  }
  socket.send("DATA\r\n");
  out = socket.receive();
  if (!std::regex_search(out, std::regex("^354 "))) {
    // std::cerr << out << std::endl;
    return false;
  }
  std::string body2 = body;
  if (body.size() < 5 || body.substr(body.size() - 5, 5) != "\r\n.\r\n") {
    body2 += "\r\n.\r\n";
  }
  socket.send(body2);
  out = socket.receive();
  if (!std::regex_search(out, std::regex("^250 "))) {
    // std::cerr << out << std::endl;
    return false;
  }

  // std::cerr << out << std::endl;

  return true;
}
