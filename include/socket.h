#ifndef SOCKET_H
#define SOCKET_H

#include <tuple>
#include <string>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

class Socket {
public:
	Socket();
	std::tuple<bool, std::string> create(std::string &hostname, int port);
	void createSSL();
	bool send(std::string &s);
	std::string receive();
	~Socket();
private:
	int sockid;
	SSL_CTX *ctx;
    SSL *ssl;
};

#endif