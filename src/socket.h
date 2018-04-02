#ifndef SOCKET_H
#define SOCKET_H

#include <iostream>
#include <string>
#include <cstring>
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


#define CHK_NULL(x) if ((x)==NULL) exit (1)
#define CHK_ERR(err,s) if ((err)==-1) { perror(s); exit(1); }
#define CHK_SSL(err) if ((err)==-1) { ERR_print_errors_fp(stderr); exit(2); }

#define BUF_LEN 10

class Socket{
    
    int sockid;
    struct sockaddr_in servaddr;
    
    SSL_CTX* ctx;
    SSL*     ssl;
    X509* server_cert;
    
    // SSL_METHOD *meth;
    
    char*    str;
    char buf [BUF_LEN];
    
    unsigned short id;
    public:
        
        /**
         * @brief Create the socket and connect to the server
         */
        Socket(std::string, int);
        
        /**
         * @brief Close the socket
         */
        ~Socket();
        
        /**
         * @brief Connect a user to the IMAP server
         */
        void IMAPConnect(std::string, std::string);
        
        /**
         * @brief Wrapper for the list command
         */
        void IMAPList(std::string, std::string);
        
        /**
         * @brief Wrapper for the status command
         */
        void IMAPStatus(std::string, std::string);
        
        /**
         * @brief Wrapper for the Noop command
         * 
         */
        void IMAPNoop();
        
        /**
         * @brief Wrapper for the logout command
         */
        void IMAPlogout();
        
        void IMAPSelect(std::string);
        
        void IMAPExamine(std::string);
        
        void IMAPCreate(std::string);
        
        void IMAPDelete(std::string);
        
        void IMAPRename(std::string, std::string);
        
        void send(std::string);
        
        std::string recieve();
        
        std::string get_id();
        
};  

#endif
