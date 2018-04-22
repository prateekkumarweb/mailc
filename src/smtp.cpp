#include "smtp.h"

SMTPConnection::SMTPConnection(std::string hostname, int port) {
	std::tuple<bool, std::string> e = socket.create(hostname, port);
    if(!std::get<0>(e)) std::cerr << std::get<1>(e) << std::endl;
    std::tuple<bool, std::string> e1 = socket.createSSL();
    if(!std::get<0>(e1)) std::cerr << std::get<1>(e1) << std::endl;
}

bool SMTPConnection::auth(std::string username, std::string password) {
	socket.send("EHLO prateekkumar.in\r\n");
	std::cerr << socket.receive() << std::endl;
	socket.send("AUTH LOGIN\r\n");
	std::cerr << socket.receive() << std::endl;
	socket.send("");
}