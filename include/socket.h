#ifndef SOCKET_H
#define SOCKET_H

class Socket {
public:
	Socket(int port);
	void createSSL();
	bool send(std::string &s);
	std::string receive();
	~Socket();
private:
	int sockid;
};

#endif