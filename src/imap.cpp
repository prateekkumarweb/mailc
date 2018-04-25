#include "imap.h"

IMAPConnection::IMAPConnection(const std::string &hostname, int port) {
    rgx = std::regex("^[a-zA-Z0-9].* (OK)|(NO)|(BAD) ");
    std::tuple<bool, std::string> e = socket.create(hostname, port);
    if(!std::get<0>(e)) std::cerr << std::get<1>(e) << std::endl;
    std::tuple<bool, std::string> e1 = socket.createSSL();
    if(!std::get<0>(e1)) std::cerr << std::get<1>(e1) << std::endl;
}

bool IMAPConnection::login(const std::string &username, const std::string &password) {
    std::cerr << "login" << std::endl;
    std::string id_string = "a";
    std::string command = id_string + " login " + username + " " + password +"\r\n";
    socket.send(command);
    std::string response = socket.receive(rgx);
    std::cerr << "DONE" << response << std::endl;
    return check_response(response, id_string);
}

/**
 * Returns 0 if BAD or NO is received
 * Returns 1 if OK received
 */
bool IMAPConnection::check_response(const std::string &response, const std::string &id_string){
    
    // TODO Check if [.\r\n]*  is working
    // response.erase(std::remove(response.begin(), response.end(), '\n'), response.end());
    // response.erase(std::remove(response.begin(), response.end(), '\r'), response.end());
    
    std::string OKrgx = id_string + " [Oo][Kk]";
    std::string NOrgx = id_string + " [Nn][Oo]";
    std::string BADrgx = id_string + " [Bb][Aa][Dd]";
    
    // std::cerr << "response: " << response << " :response" << std::endl;

    if (std::regex_search(response, std::regex (OKrgx) ))
        return 1;
            
    if (std::regex_search(response, std::regex(NOrgx) ))
        return 0;
    
    if (std::regex_search(response, std::regex(BADrgx) )){
        std::cerr << "[ERROR] BAD command: " << response;
        return 0;
    }
    
    std::cerr << "Program must not reach here .........................." << std::endl << std::endl;
}

bool IMAPConnection::createMailbox(const std::string &mailbox){
    std::string id_string = "a";
    std::string command = id_string + " create " + mailbox +"\r\n";
    socket.send(command);
    std::string response = socket.receive(rgx);
    return check_response(response, id_string);   
}

bool IMAPConnection::deleteMailbox(const std::string &mailbox){
    std::string id_string = "a";
    std::string command = id_string + " delete " + mailbox +"\r\n";
    socket.send(command);
    std::string response = socket.receive(rgx);
    return check_response(response, id_string);
}

bool IMAPConnection::renameMailbox(const std::string &oldmailbox, const std::string &newmailbox){
    std::string id_string = "a";
    std::string command = id_string + " rename " + oldmailbox + " " + newmailbox + "\r\n";
    socket.send(command);
    std::string response = socket.receive(rgx);
    return check_response(response, id_string);
}

std::tuple<int, int, int> IMAPConnection::getCount(const std::string &mailbox){
    std::string id_string = "a";
    std::string command = id_string + " examine " + mailbox +"\r\n";
    socket.send(command);
    std::string response = socket.receive(rgx);
    bool response_id = check_response(response, id_string);
    if (response_id){
        std::smatch sm;
        int a = 0, b = 0, c = 0;
        if (regex_search(response, sm, std::regex("([0-9]+) EXISTS"))) {
            a = std::stoi(sm[1]);
        }
        if (regex_search(response, sm, std::regex("([0-9]+) EXISTS"))) {
            b = std::stoi(sm[1]);
        }
        if (regex_search(response, sm, std::regex("([0-9]+) EXISTS"))) {
            c = std::stoi(sm[1]);
        }
        return std::make_tuple(a, b, c);
    } else return std::make_tuple(-1,-1,-1);
}

bool IMAPConnection::deleteMail(Mail mail){
    std::string id_string = "a";
    std::string command = id_string + " select " + mail.mailbox +"\r\n";
    socket.send(command);
    std::string response = socket.receive(rgx);
    bool response_id = check_response(response, id_string);
    if (!response_id) return response_id;

    command = id_string + " uid store " + std::to_string(mail.uid) + " +FLAGS (\\Deleted)\r\n";
    socket.send(command);
    response = socket.receive(rgx);
    response_id = check_response(response, id_string);
    if (!response_id) return response_id;

    command = id_string + " expunge " + std::to_string(mail.uid) + "\r\n";
    socket.send(command);
    response = socket.receive(rgx);
    return response_id = check_response(response, id_string);
}

std::vector<Mail> IMAPConnection::getUnseenMails(const std::string &mailbox){
    std::vector<Mail> mails;
    
    std::string id_string = "a";
    std::string command = id_string + " select " + mailbox +"\r\n";
    socket.send(command);
    std::string response = socket.receive(rgx);
    bool response_id = check_response(response, id_string);
    if (!response_id) {
        return mails;
    }    
    
    command = id_string + " uid search unseen\r\n";
    socket.send(command);
    response = socket.receive(rgx);
    response_id = check_response(response, id_string);
    if (response_id) {
        std::regex number("([0-9]+)");
        std::smatch sm;
        std::cerr << "In unseen mails, recieved: " << response;
        std::vector<int> uids;
        
        while (regex_search(response, sm, number)){
            uids.push_back(std::stoi(sm[1]));
            response = sm.suffix();
        }
        
        std::vector<int>::iterator it = uids.begin();
        for(; it!=uids.end(); it++){
            std::cerr << "Fetching mail uid: " << *it;
            mails.push_back(getMail(mailbox, *it));
        }
    }
    return mails;
}

std::vector<Mail> IMAPConnection::getTopMails(const std::string &mailbox, int k){
    std::vector<Mail> mails;
    
    std::string id_string = "a";
    std::string command = id_string + " select " + mailbox +"\r\n";
    socket.send(command);
    std::string response = socket.receive(rgx);
    bool response_id = check_response(response, id_string);
    if (!response_id) {
        return mails;
    }    
    
    command = id_string + " uid search all\r\n";
    socket.send(command);
    response = socket.receive(rgx);
    response_id = check_response(response, id_string);
    if (response_id) {
        std::regex number("([0-9]+)");
        std::smatch sm;
        std::cerr << "In top k mails, recieved: " << response;
        std::vector<int> uids;
        
        while (regex_search(response, sm, number)){
            uids.push_back(std::stoi(sm[1]));
            response = sm.suffix();
        }
        
        if (uids.size() < k) k = uids.size();
        std::vector<int>::iterator it = uids.end() - k;
        for(; it!=uids.end(); it++){
            std::cerr << "Fetching mail uid: " << *it;
            mails.push_back(getMail(mailbox, *it));
        }
    }
    return mails;
}

Mail IMAPConnection::getMail(const std::string &mailbox, const int uid){
    Mail mail;
    mail.mailbox = mailbox;
    
    std::string id_string = "a";
    std::string command = id_string + " select " + mail.mailbox +"\r\n";
    socket.send(command);
    std::string response = socket.receive(rgx);
    bool response_id = check_response(response, id_string);
    if (!response_id) {
        mail.uid = -1; 
        return mail;
    }    
    
    command = id_string + " uid fetch " + std::to_string(uid) + " (BODY[HEADER.FIELDS (from to subject date)])\r\n";
    socket.send(command);
    response = socket.receive(rgx);
    response_id = check_response(response, id_string);
    if (response_id){
        std::smatch sm;
        if (regex_search(response, sm, std::regex("From: (.*)\r\n")))
            mail.from = sm[1];
        if (regex_search(response, sm, std::regex("Date: (.*)\r\n")))
            mail.date = sm[1];
        if (regex_search(response, sm, std::regex("Subject: (.*)\r\n")))
            mail.subject = sm[1];
        if (regex_search(response, sm, std::regex("To: (.*)\r\n")))
            mail.to = sm[1];
        /*regex_match(response, sm, std::regex("[.\\r\\n]*From: (.*)\\nDate: (.*)"
            "\\nSubject: (.*)\\nTo: (.*)[.\\r\\n]*"));
        
        mail.date = sm[2];
        mail.subject = sm[3];
        mail.to = sm[4];*/
    } else {
        mail.uid = -1; 
        return mail;
    }
    
    mail.uid = uid;
    
    command = id_string + " uid fetch " + std::to_string(mail.uid) + " rfc822.text\r\n";
    socket.send(command);
    response = socket.receive(rgx);
    response_id = check_response(response, id_string);
    if (response_id){
        std::cout << "Here" << std::endl;
        std::stringstream ss(response);
        std::string s = "";
        std::string sep = "";
        std::string text = "";
        std::string html = "";

        int count = 1;
        int read = 0;
        while(std::getline(ss, s)){
            if (s[s.size()-1] == '\r') s = s.substr(0, s.size()-1);
            if (count == 2){
                sep = s;
                sep.erase(std::remove(sep.begin(), sep.end(), '-'), sep.end());
                std::getline(ss, s);
                std::getline(ss, s);
                read = 1;
            }
            else if (read == 1){
                if (s.find(sep) == std::string::npos){
                    text += s + '\n';
                } else {
                    read = 2;
                    std::getline(ss, s);
                    std::getline(ss, s);
                }
            }
            else if (read == 2){
                if (s.find(sep) == std::string::npos){
                    html += s + '\n';
                } else break;
            }
            count++;
        }
        // std::cout << sep << std::endl << "TEXT: " <<    text << std::endl << html;
        mail.text = text;
        mail.html = html;
    } else{
        mail.text = "\n";
        mail.html = "\n";
    }
    
    return mail;
}
