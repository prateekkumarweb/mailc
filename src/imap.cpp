#include "imap.h"

IMAPConnection::IMAPConnection(const std::string &hostname, int port) {
    id_string = "imapmailcPVclientpv";
    rgx = std::regex("imapmailcPVclientpv ((OK)|(NO)|(BAD)) .*");
    std::tuple<bool, std::string> e = socket.create(hostname, port);
    if(!std::get<0>(e)) std::cerr << std::get<1>(e) << std::endl;
    std::tuple<bool, std::string> e1 = socket.createSSL();
    if(!std::get<0>(e1)) std::cerr << std::get<1>(e1) << std::endl;
}

bool IMAPConnection::login(const std::string &username, const std::string &password) {
    std::cerr << "login" << std::endl;
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

    if (regex_search(response, std::regex (OKrgx) ))
        return 1;
            
    if (regex_search(response, std::regex(NOrgx) ))
        return 0;
    
    if (regex_search(response, std::regex(BADrgx) )){
        std::cerr << "[ERROR] BAD command: " << response;
        return 0;
    }
    
    std::cerr << "Program must not reach here .........................." << std::endl << std::endl;
}

bool IMAPConnection::createMailbox(const std::string &mailbox) {
    std::string command = id_string + " create " + mailbox +"\r\n";
    socket.send(command);
    std::string response = socket.receive(rgx);
    return check_response(response, id_string);   
}

bool IMAPConnection::deleteMailbox(const std::string &mailbox) {
    std::string command = id_string + " delete " + mailbox +"\r\n";
    socket.send(command);
    std::string response = socket.receive(rgx);
    return check_response(response, id_string);
}

bool IMAPConnection::renameMailbox(const std::string &oldmailbox, const std::string &newmailbox) {
    std::string command = id_string + " rename " + oldmailbox + " " + newmailbox + "\r\n";
    socket.send(command);
    std::string response = socket.receive(rgx);
    return check_response(response, id_string);
}

bool IMAPConnection::noop(){
    std::string command = "a noop\r\n";
    socket.send(command);
    std::string response = socket.receive(rgx);
    return check_response(response, "a");
}

std::tuple<int, int, int> IMAPConnection::getCount(const std::string &mailbox) {
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
        if (regex_search(response, sm, std::regex("([0-9]+) RECENT"))) {
            b = std::stoi(sm[1]);
        }
        if (regex_search(response, sm, std::regex("UNSEEN ([0-9]+)"))) {
            c = std::stoi(sm[1]);
        }
        return std::make_tuple(a, b, c);
    } else return std::make_tuple(-1,-1,-1);
}

bool IMAPConnection::deleteMail(Mail mail) {
    bool response_id = select(response);
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

std::vector<int> IMAPConnection::search(const std::string &mailbox, const std::string &from,
                const std::string &to, const std::string &subject, const std::string &text,
                const std::string &nottext, const std::string &since, const std::string &before){
    std::vector<int> uids;

    bool response_id = select(mailbox);
    if (!response_id) {
        return uids;
    }

    command = id_string + " uid search";
    if (from != "")
        command += " from \"" + from + "\"";
    if (to != "")
        command += " to \"" + to + "\"";
    if (subject != "")
        command += " subject \"" + subject + "\"";
    if (text != "")
        command += " text \"" + text + "\"";
    if (since != "")
        command += " since \"" + since + "\"";
    if (before != "")
        command += " before \"" + before + "\"";
    if (nottext != "")
        command += " not text \"" + nottext + "\"";

    command += "\r\n";
    socket.send(command);
    response = socket.receive(rgx);
    response_id = check_response(response, id_string);
    if (response_id) {
        return uids;
    }

    std::regex number("([0-9]+)");
    std::smatch sm;
    std::cerr << "In search mails, recieved: " << response;

    while (regex_search(response, sm, number)){
        uids.push_back(std::stoi(sm[1]));
        response = sm.suffix();
    }

    return uids;
}

std::vector<Mail> IMAPConnection::getUnseenMails(const std::string &mailbox){
    std::vector<Mail> mails;

    bool response_id = select(response);
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
        
        // Fetching mails from servers makes them seen
        // To restore the original position, we remove the flag Seen from these mails
        it = uids.begin();
        for(; it!=uids.end(); it++){
            command = id_string + " uid store " + std::to_string(*it) +" -flags (\\Seen) \r\n";
            socket.send(command);
            std::string response = socket.receive(rgx);
            response_id = check_response(response, id_string);
        }
    }
    return mails;
}

std::vector<Mail> IMAPConnection::getTopMails(const std::string &mailbox, int k){
    std::vector<Mail> mails;

    bool response_id = select(mailbox);
    if (!response_id) {
        return mails;
    }    
    
    std::string command = id_string + " uid search all\r\n";
    socket.send(command);
    std::string response = socket.receive(rgx);
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

std::vector<std::string> IMAPConnection::getmailboxes(){
    std::vector<std::string> mailboxes;

    std::string command = id_string + " list \"\" *\r\n";
    socket.send(command);
    std::string response = socket.receive(rgx);
    bool response_id = check_response(response, id_string);
    if (response_id) {
        std::regex name("\"/\" \"(.*)\"\r\n");
        std::smatch sm;
        
        while (regex_search(response, sm, name)){
            mailboxes.push_back(sm[1]);
            response = sm.suffix();
            std::cerr << "Detected mailbox:  " << sm[1] << std::endl;
        }
    }
    
    return mailboxes;
}

bool IMAPConnection::select(const std::string &mailbox) {
    std::string command = id_string + " select " + mailbox +"\r\n";
    socket.send(command);
    std::string response = socket.receive(rgx);
    return check_response(response, id_string);
}

std::vector<int> IMAPConnection::getAllmails(const std::string &mailbox){
    std::vector<int> uids;

    bool response_id = select(mailbox);
    if (response_id){
        std::string command = id_string + " uid search all\r\n";
        socket.send(command);
        std::string response = socket.receive(rgx);
        response_id = check_response(response, id_string);
        if (response_id) {
            std::regex number("([0-9]+)");
            std::smatch sm;
            response.erase(std::remove(response.begin(), response.end(), '\r'), response.end());
            std::replace(response.begin(), response.end(), '\n', ' ');
            std::stringstream res(response);

            while (!res.eof()) {
                std::string id_str;
                res >> id_str;
                if (std::regex_match(id_str, number)) {
                    uids.push_back(std::stoi(id_str));
                }
            }
        }
    }
    
    return uids;
}

Mail IMAPConnection::getMail(const std::string &mailbox, const int uid){
    Mail mail;
    mail.mailbox = mailbox;

    bool response_id = select(mailbox);
    if (!response_id) {
        mail.uid = -1; 
        return mail;
    }    
    
    std::string command = id_string + " uid fetch " + std::to_string(uid) + " (BODY[HEADER.FIELDS (from to subject date)])\r\n";
    socket.send(command);
    std::string response = socket.receive(rgx);
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
    
    command = id_string + " uid fetch " + std::to_string(mail.uid) + " body[1]\r\n";
    socket.send(command);
    response = socket.receive(rgx);
    response_id = check_response(response, id_string);
    if (response_id) {
        // std::cerr << "Here" << std::endl;
        response.erase(std::remove(response.begin(), response.end(), '\r'), response.end());
        std::stringstream ss(response);
        std::string s = "";
        std::vector<std::string> text;

        int count = 1;
        int read = 0;
        std::cerr << response << std::endl;
        std::getline(ss, s);
        while(std::getline(ss, s)){
            text.push_back(s);            
        }
        text.pop_back();
        text.pop_back();
        mail.text = "";
        for (auto &s: text) {
            mail.text += s+"\n";
        }
    } else{
        mail.text = "\n";
    }
    
    return mail;
}

std::vector<Mail> IMAPConnection::getMails(const std::string &mailbox, std::vector<int> uids){
    std::vector<Mail> mails;

    for(auto it = uids.begin(); it != uids.end(); it++){
        mails.push_back(getMail(mailbox, *it));
    }

    return mails;
}
