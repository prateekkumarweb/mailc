#include "imap.h"

IMAPConnection::IMAPConnection(std::string hostname, int port) {
    std::cerr << "IMAPConnection" << std::endl;
    std::tuple<bool, std::string> e = socket.create(hostname, port);
    std::cerr << "IMAPConnection" << std::endl;
    if(std::get<0>(e)) std::cerr << std::get<1>(e) << std::endl;
    std::tuple<bool, std::string> e1 = socket.createSSL();
    std::cerr << "IMAPConnection" << std::endl;
    if(std::get<0>(e1)) std::cerr << std::get<1>(e1) << std::endl;
    std::cerr << "IMAPConnection" << std::endl;
}

bool IMAPConnection::login(std::string username, std::string password) {
    std::cerr << "login" << std::endl;
    std::string id_string = "a";
    std::string command = id_string + " login " + username + " " + password +"\r\n";
    socket.send(command);
    std::string response = socket.receive();
    std::cerr << response << std::endl;
    return check_response(response, id_string);
}

/**
 * Returns 0 if BAD or NO is received
 * Returns 1 if OK received
 */
bool IMAPConnection::check_response(std::string &response, std::string &id_string){
    
    // TODO Check if [.\r\n]*  is working
    // response.erase(std::remove(response.begin(), response.end(), '\n'), response.end());
    // response.erase(std::remove(response.begin(), response.end(), '\r'), response.end());
    
    std::string OKrgx = id_string + " [Oo][Kk][.\\r\\n]*";
    std::string NOrgx = id_string + " [Nn][Oo][.\\r\\n]*";
    std::string BADrgx = id_string + " [Bb][Aa][Dd][.\\r\\n]*";
    
    if (std::regex_match (response, std::regex (OKrgx) ))
        return 1;
            
    if (std::regex_match (response, std::regex(NOrgx) ))
        return 0;
    
    if (std::regex_match (response, std::regex(BADrgx) )){
        std::cerr << "[ERROR] BAD command: " << response;
        return 0;
    }
    
    std::cerr << "Program must not reach here .........................." << std::endl << std::endl;
}

bool IMAPConnection::createMailbox(std::string &mailbox){
    std::string id_string = "a";
    std::string command = id_string + " create " + mailbox +"\r\n";
    socket.send(command);
    std::string response = socket.receive();
    return check_response(response, id_string);   
}

bool IMAPConnection::deleteMailbox(std::string &mailbox){
    std::string id_string = "a";
    std::string command = id_string + " delete " + mailbox +"\r\n";
    socket.send(command);
    std::string response = socket.receive();
    return check_response(response, id_string);
}

bool IMAPConnection::renameMailbox(std::string &oldmailbox, std::string &newmailbox){
    std::string id_string = "a";
    std::string command = id_string + " rename " + oldmailbox + " " + newmailbox + "\r\n";
    socket.send(command);
    std::string response = socket.receive();
    return check_response(response, id_string);
}

std::tuple<int, int, int> IMAPConnection::getCount(const std::string &mailbox){
    std::string id_string = "a";
    std::string command = id_string + " examine " + mailbox +"\r\n";
    socket.send(command);
    std::string response = socket.receive();
    bool response_id = check_response(response, id_string);
    if (response_id){
        std::smatch sm1;
        regex_match(response, sm1, std::regex("[.\\r\\n]*([0-9]+) EXISTS[.\\r\\n]*([0-9]+)"
            " RECENT[.\\r\\n]*\\[UNSEEN ([0-9]+)\\][.\\r\\n]*"));
        return std::make_tuple(std::stoi(sm1[1]), std::stoi(sm1[2]), std::stoi(sm1[3]));
    } else return std::make_tuple(-1,-1,-1);
}

bool IMAPConnection::deleteMail(Mail mail){
    std::string id_string = "a";
    std::string command = id_string + " select " + mail.mailbox +"\r\n";
    socket.send(command);
    std::string response = socket.receive();
    bool response_id = check_response(response, id_string);
    if (!response_id) return response_id;

    command = id_string + " uid store " + std::to_string(mail.uid) + " +FLAGS (\\Deleted)\r\n";
    socket.send(command);
    response = socket.receive();
    response_id = check_response(response, id_string);
    if (!response_id) return response_id;

    command = id_string + " expunge " + std::to_string(mail.uid) + "\r\n";
    socket.send(command);
    response = socket.receive();
    return response_id = check_response(response, id_string);
}

Mail IMAPConnection::getMail(const std::string &mailbox, const int uid){
    Mail mail;
    mail.mailbox = mailbox;
    
    std::string id_string = "a";
    std::string command = id_string + " select " + mail.mailbox +"\r\n";
    socket.send(command);
    std::string response = socket.receive();
    bool response_id = check_response(response, id_string);
    if (!response_id) {
        mail.uid = -1; 
        return mail;
    }    
    
    command = id_string + " uid fetch " + std::to_string(mail.uid) + " (BODY[HEADER.FIELDS (from to subject date)])\r\n";
    socket.send(command);
    response = socket.receive();
    response_id = check_response(response, id_string);
    if (response_id){
        std::smatch sm;
        regex_match(response, sm, std::regex("[.\\r\\n]*From: (.*)\\nDate: (.*)"
            "\\nSubject: (.*)\\nTo: (.*)[.\\r\\n]*"));
        mail.from = sm[1];
        mail.date = sm[2];
        mail.subject = sm[3];
        mail.to = sm[4];
    } else {
        mail.uid = -1; 
        return mail;
    }
    
    mail.uid = uid;
    
    command = id_string + " uid fetch " + std::to_string(mail.uid) + " rfc822.text\r\n";
    socket.send(command);
    response = socket.receive();
    response_id = check_response(response, id_string);
    if (response_id){
        std::stringstream ss(response);
        std::string s;
        std::string sep;
        std::string text;
        std::string html;

        int count = 1;
        int read = 0;
        while(ss >> s){
            if (count == 2){
                sep = s;
                sep.erase(std::remove(sep.begin(), sep.end(), '-'), sep.end());
                ss >> s;
                ss >> s;
                read = 1;
            }
            else if (read == 1){
                if (s.find(sep) != std::string::npos){
                    text += s + '\n';
                } else {
                    read = 2;
                    ss >> s;
                    ss >> s;
                }
            }
            else if (read == 2){
                if (s.find(sep) != std::string::npos){
                    html += s + '\n';
                } else break;
            }
        }
        mail.text = text;
        mail.html = html;
    } else{
        mail.text = "\n";
        mail.html = "\n";
    }
    
    return mail;
}
