#include "socket.h"

#include <iostream>
#include <string>
// #include <unistd.h>

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

#define CHK_NULL(x) if ((x)==NULL) exit (1)
#define CHK_ERR(err,s) if ((err)==-1) { perror(s); exit(1); }
#define CHK_SSL(err) if ((err)==-1) { ERR_print_errors_fp(stderr); exit(2); }

Socket::Socket(std::string &hostname, int port) {
	sockid = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		// TODO Handle this error gracefully
	}

	struct sockaddr_in servaddr;
	memset(&servaddr, '\0', sizeof(servaddr));
	struct hostent *hp;

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(serverport);

	// TODO find the ip address after dns (How to do?)
	
	inet_pton(AF_INET,"204.141.32.119",&(servaddr.sin_addr));

	if(connect(sockid, (struct sockaddr *)&servaddr,sizeof(servaddr)) == -1) {
		// TODO handle this error gracefully
	}

}

void Socket::createSSL() {

	SSL_CTX* ctx;
    SSL*     ssl;
    X509* server_cert;
    
    char*    str;
    char buf [BUF_LEN];

    OpenSSL_add_ssl_algorithms();
    const SSL_METHOD* meth = TLSv1_2_client_method();
    SSL_load_error_strings();
    ctx = SSL_CTX_new (meth); CHK_NULL(ctx);

    ssl = SSL_new (ctx);                         CHK_NULL(ssl);    
    SSL_set_fd (ssl, sockid);
    int err = SSL_connect (ssl); CHK_SSL(err);

    printf ("SSL connection using %s\n", SSL_get_cipher (ssl));

    server_cert = SSL_get_peer_certificate (ssl);       CHK_NULL(server_cert);
    printf ("Server certificate:\n");

    str = X509_NAME_oneline (X509_get_subject_name (server_cert),0,0);
    CHK_NULL(str);
    printf ("\t subject: %s\n", str);
    OPENSSL_free (str);

    str = X509_NAME_oneline (X509_get_issuer_name  (server_cert),0,0);
    CHK_NULL(str);
    printf ("\t issuer: %s\n", str);
    OPENSSL_free (str);

    X509_free (server_cert);

    std::string reply = "";

    do{
	    err = SSL_read (ssl, buf, sizeof(buf) - 1);                     
	    CHK_SSL(err);
	    buf[err] = '\0';
	    
	    reply += std::string(buf);
    }while(err == sizeof(buf) - 1);
    
    std::cout << "Got " << std::to_string(reply.length()) << " chars:" << reply;
    
    std::cout << "Connection established to mail server." << std::endl;
}

bool Socket::send(std::string &s) {
	int err = SSL_write (ssl, message.c_str(), message.size());
    CHK_SSL(err);
}

std::string Socket::recieve(){
    std::string reply = "";
    int err;
    
    do{
	    err = SSL_read (ssl, buf, sizeof(buf) - 1);                     
	    CHK_SSL(err);
	    buf[err] = '\0';
	    
	    reply += std::string(buf);
    }while(err == sizeof(buf) - 1);
    
    std::cout << "Got " << std::to_string(reply.length()) << " chars:" << reply;
    
    return reply;
}

Socket::~Socket(){
    SSL_shutdown (ssl); /* send SSL/TLS close_notify */
    close(sockid);
    SSL_free (ssl);
    SSL_CTX_free (ctx);
    std::cout << "Closed connection to mail server" << std::endl;
}
