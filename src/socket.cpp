#include "socket.h"

#include <iostream>

#define CHK_NULL(x) if ((x)==NULL) exit (1)
#define CHK_ERR(err,s) if ((err)==-1) { perror(s); exit(1); }
#define CHK_SSL(err) if ((err)==-1) { ERR_print_errors_fp(stderr); exit(2); }

std::tuple<bool, std::string> Socket::create(std::string &hostname, int port) {

	struct hostent *host;
	struct sockaddr_in addr;

	if ((host == gethostbyname(hostname.c_str())) == NULL) {
		std::cerr << hostname << std::endl;
		return {false, "DNS failed."};
	}

	sockid = socket(AF_INET, SOCK_STREAM, 0);
	if (sockid == -1) {
		// TODO Handle this error gracefully
		std::cerr << "Socket creation failed." << std::endl;
		return {false, "Socket unable to create."};
	}

	memset(&addr, '\0', sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = 	*(long *)(host->h_addr);

	if(connect(sockid, (struct sockaddr *)&addr,sizeof(addr)) != 0) {
		close(sockid);
		return {false, "Socket connect failed."};
	}

}

std::tuple<bool, std::string> Socket::createSSL() {
	SSL_METHOD *meth;

	OpenSSL_add_all_algorithms();
	SSL_load_error_strings();

	meth = TLSv1_2_client_method();
	ctx = SSL_CTX_new(meth);

	if (ctx == NULL) {
		return {false, "OpenSSL SSL_CTX_new failed."}
	}

    char buf[1024];

    X509 *cert;
    char *str;

    ssl = SSL_new(ctx);  
    SSL_set_fd (ssl, sockid);

    if (SSL_connect(ssl) == FAIL) {
    	ERR_print_errors_fp(stderr);
    } else {
    	char *msg = "Hello???";

        printf("Connected with %s encryption\n", SSL_get_cipher(ssl));
        {
        	cert = SSL_get_peer_certificate(ssl); // How to get ssl?

		    if (cert != NULL) {
		    	printf("Server certificates:\n");
		        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
		        printf("Subject: %s\n", line);
		        free(line);							/* free the malloc'ed string */
		        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
		        printf("Issuer: %s\n", line);
		        free(line);							/* free the malloc'ed string */
		        X509_free(cert);					/* free the malloc'ed certificate copy */
		    } else {
		    	return {false, "No certificate."};
		    }
        }
        SSL_write(ssl, msg, strlen(msg));			/* encrypt & send message */
        bytes = SSL_read(ssl, buf, sizeof(buf));	/* get reply & decrypt */
        buf[bytes] = 0;
        printf("Received: \"%s\"\n", buf);
        SSL_free(ssl);
    }
}

Socket::~Socket() {
	close(sockid);
	SSL_CTX_free(ctx);
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
