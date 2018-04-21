#include "socket.h"

#include <random>
#include <regex>

void dieWithError(std::string s){
    std::cerr << s.c_str();
    throw s;
}

std::string Socket::get_id()
{    
    std::string s = "a";// + std::to_string(id);
    id++;
    return s;
}

short check_response(std::string response, std::string id_string){
    
    response.erase(std::remove(response.begin(), response.end(), '\n'), response.end());
    response.erase(std::remove(response.begin(), response.end(), '\r'), response.end());
    
    std::string OKstr = ".*" + id_string + " [Oo][Kk].*";
    std::regex OKrgx(OKstr);
    if (std::regex_match (response, OKrgx ))
        return 1;
            
    std::string NOrgx = ".*" + id_string + " [Nn][Oo].*";        
    if (std::regex_match (response, std::regex(NOrgx) )){
        std::cout << "Command failure: " << response;
        return 0;
    }
    
    std::string BADrgx = ".*" + id_string + " [Bb][Aa][Dd].*";
    if (std::regex_match (response, std::regex(BADrgx) )){
        std::cout << "Invalid command: " << response;
        return -1;
    }
    
    std::cout << "Program must not reach here .........................." << std::endl << std::endl;
}

Socket::Socket(std::string hostname, int serverport){
    
    OpenSSL_add_ssl_algorithms();
    const SSL_METHOD* meth = TLSv1_2_client_method();
    SSL_load_error_strings();
    ctx = SSL_CTX_new (meth); CHK_NULL(ctx);
    
    if((sockid = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        dieWithError("Creation failed\n");  
    
    //bzero(&servaddr,sizeof(servaddr));
    memset (&servaddr, '\0', sizeof(servaddr));
    
    struct hostent *hp;

    /*if((hp = gethostbyname(hostname.c_str())) == NULL)
        dieWithError("Could not resolve hostname\n");

    std::memcpy(hp->h_addr, &servaddr.sin_addr, hp->h_length);
    //servaddr.sin_addr = hp->h_addr;
    std::cout << hp->h_addr << " " << hp->h_name << " "  << hp->h_addr_list[1] << inet_ntoa(servaddr.sin_addr) << std::endl;*/
    
    servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(serverport);
	inet_pton(AF_INET,"204.141.32.119",&(servaddr.sin_addr)); // Set the IP address
	if(connect(sockid, (struct sockaddr *)&servaddr,sizeof(servaddr)) == -1)
        dieWithError("Connect to server failed\n");
        
    ssl = SSL_new (ctx);                         CHK_NULL(ssl);    
    SSL_set_fd (ssl, sockid);
    int err = SSL_connect (ssl); CHK_SSL(err);

    printf ("SSL connection using %s\n", SSL_get_cipher (ssl));
  
    /* Get server's certificate (note: beware of dynamic allocation) - opt */

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

    /* We could do all sorts of certificate verification stuff here before
     deallocating the certificate. */

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
    
    id = 0; 
}

void Socket::IMAPConnect(std::string username, std::string pass){
    
    std::string id_string = get_id();
    std::string command = id_string + " login " + username + " " + pass + "\r\n";
    send(command);
    std::string response = recieve();
    check_response(response, id_string);
}

void Socket::IMAPList(std::string mailbox, std::string search){
    
    std::string id_string = get_id();
    std::string command = id_string + " list \"" + mailbox + "\" \"" + search + "\"\r\n";
    send(command);
    std::string response = recieve();
    check_response(response, id_string);
}

void Socket::IMAPStatus(std::string mailbox, std::string status){
    // FIXME
    std::string id_string = get_id();
    std::string command = id_string + " status " + mailbox + " (" + status + ")\r\n";
    send(command);
    std::string response = recieve();
    check_response(response, id_string); 
}

void Socket::IMAPNoop(){
    
    std::string id_string = get_id();
    std::string command = id_string + " nop\r\n";
    send(command);
    std::string response = recieve();
    check_response(response, id_string);
}

void Socket::IMAPlogout(){
    // FIXME Client has to end the connection.
    std::string id_string = get_id();
    std::string command = id_string + " logout\r\n";
    send(command);
    std::string response = recieve();
    check_response(response, id_string);
}

void Socket::IMAPSelect(std::string mailbox){
    std::string id_string = get_id();
    std::string command = id_string + " select " + mailbox +"\r\n";
    send(command);
    std::string response = recieve();
    check_response(response, id_string);
}

void Socket::IMAPExamine(std::string mailbox){
    std::string id_string = get_id();
    std::string command = id_string + " examine " + mailbox +"\r\n";
    send(command);
    std::string response = recieve();
    check_response(response, id_string);
}

void Socket::IMAPCreate(std::string mailbox){
    std::string id_string = get_id();
    std::string command = id_string + " create " + mailbox +"\r\n";
    send(command);
    std::string response = recieve();
    check_response(response, id_string);
}

void Socket::IMAPDelete(std::string mailbox){
    std::string id_string = get_id();
    std::string command = id_string + " delete " + mailbox +"\r\n";
    send(command);
    std::string response = recieve();
    check_response(response, id_string);
}

void Socket::IMAPRename(std::string oldname, std::string newname){
    std::string id_string = get_id();
    std::string command = id_string + " rename " + oldname + " " + newname + "\r\n";
    send(command);
    std::string response = recieve();
    check_response(response, id_string);
}
void Socket::send(std::string message){
    int err;
    
    std::cout << message;
    
    err = SSL_write (ssl, message.c_str(), message.size());
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



















