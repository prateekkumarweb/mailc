#include "cliutils.h"

#include <cstdlib>
#include <algorithm>

std::string MAIL_PATH() {
    std::string HOME_DIR(std::getenv("HOME"));
    return HOME_DIR+"/.mailc/";
}


void GetReqDirs(const std::string& path, std::vector<std::string>& dirs){
    DIR *dpdf;
    struct dirent *epdf;
    dpdf = opendir(path.c_str());
    if (dpdf != NULL){
        while ((epdf = readdir(dpdf)) != NULL){
            if(epdf->d_type==DT_DIR && strstr(epdf->d_name,"..") == NULL && strstr(epdf->d_name,".") == NULL){
                dirs.push_back((path+epdf->d_name).substr(MAIL_PATH().size()));
                GetReqDirs(path+epdf->d_name+"/", dirs);               
            }            
        }
    }
    closedir(dpdf);
}

template<class T>
void print_vectors(std::vector<T> v, char sep = ' ') {
    for (auto &t: v) {
        std::cout << t << sep;
    }
    std::cout << std::endl;
}

std::string tolowercase(const std::string &s) {
    std::string l = s;
    std::transform(l.begin(), l.end(), l.begin(), ::tolower);
    return l;
}

bool cliutils::sync(IMAPConnection &imap){

    if (!fs::is_directory(fs::path(MAIL_PATH()))) {
        fs::create_directory(fs::path(MAIL_PATH()));
    }
    
    // List the folders and check if present
    std::vector<std::string> offline_mailboxes;
    GetReqDirs(MAIL_PATH(), offline_mailboxes);
    std::transform(offline_mailboxes.begin(), offline_mailboxes.end(), offline_mailboxes.begin(), tolowercase);

    // print_vectors(offline_mailboxes, '\n');
        
    std::vector<std::string> mailboxes = imap.getmailboxes();
    std::transform(mailboxes.begin(), mailboxes.end(), mailboxes.begin(), tolowercase);

    print_vectors(mailboxes);
    
    
    for (auto it = mailboxes.begin(); it != mailboxes.end(); it++){
        if (std::find(offline_mailboxes.begin(), offline_mailboxes.end(), *it) == offline_mailboxes.end()){
            //Create the mailbox (*it)
            std::string p = MAIL_PATH()+tolowercase(*it);
            fs::create_directory(p);
        }
    }
    
    for(auto it = offline_mailboxes.begin(); it != offline_mailboxes.end(); it++){
        if (std::find(mailboxes.begin(), mailboxes.end(), tolowercase(*it)) == mailboxes.end()){
            //Delete the mailbox (*it)
            fs::remove(MAIL_PATH()+tolowercase(*it));
        }
    }
    // Within each folder sync mails
        // Mails offline list
        // a select inbox
        // a uid search all
        // find the missing mails
        // getMails and add them    
        
    
    for (auto it = mailboxes.begin(); it != mailboxes.end(); it++){
        std::string mailbox = *it;
        
        imap.select(mailbox);
                
        std::string mailbox_path = MAIL_PATH() + mailbox;
        std::vector<int> offline_uids;
        for (auto & p : fs::directory_iterator(mailbox_path)){
            if(!is_directory(p)){
                std::smatch sm;
                std::string path = p.path().string();
                if (std::regex_search(path, sm, std::regex("/([0-9]+).m")))
                    offline_uids.push_back(std::stoi(sm[1]));
            }
        }
        
        std::vector<int> uids = imap.getAllmails(mailbox);
        
        //Delete the extra mails
        for(auto it2 = offline_uids.begin(); it2 != offline_uids.end(); it2++){
            if (std::find(uids.begin(), uids.end(), *it2) == uids.end()){
                //Delete the mail
                std::string mailpath = MAIL_PATH() + mailbox + "/" + std::to_string(*it2) + ".m";
                fs::remove(mailpath);
            }
        }
        
        print_vectors(uids);
        print_vectors(offline_uids);

        for(auto it2 = uids.begin(); it2 != uids.end(); it2++){
            if (std::find(offline_uids.begin(), offline_uids.end(), *it2) == offline_uids.end()){
                std::string mailpath = MAIL_PATH() + mailbox + "/" + std::to_string(*it2) + ".m";
                std::cout << mailpath << std::endl;
                std::ofstream outfile (mailpath);
                
                Mail mail = imap.getMail(mailbox, *it2);
                if (mail.uid >= 0) {
                    outfile << mail.from << std::endl;
                    outfile << mail.to << std::endl;
                    outfile << mail.subject << std::endl;
                    outfile << mail.date << std::endl;
                    outfile << mail.text;
                    /*outfile << "From: " << mail.from << std::endl;
                    outfile << "To: " << mail.to << std::endl;
                    outfile << "Suject: " << mail.subject << std::endl;
                    outfile << "Date: " << mail.date << std::endl;
                    outfile << "Body: " << << std::endl << mail.from;*/
                }
                
                outfile.close();
            }
        }
    }
    
    return true;
}

bool deleteMail(IMAPConnection &imap, const std::string& mailbox, int uid){
    Mail mail;
    mail.uid = uid;
    mail.mailbox = mailbox;
    bool response = imap.deleteMail(Mail);
    if (response)
        sync();
    return response;
}

std::vector<std::string> readMail(const std::string& mailbox, int uid){
    std::string path = MAIL_PATH() + mailbox + "/" + std::to_string(uid) + ".m";
    std::ifstream mailfile(path);
    std::string line;
    std::vector<std::string> contents;
    if (mailfile.is_open()){
        std::getline(mailfile, line);
        contents.push_back(line);
        getline(mailfile, line);
        contents.push_back(line);
        getline(mailfile, line);
        contents.push_back(line);
        getline(mailfile, line);
        contents.push_back(line);
        std::string text;
        while (getline(mailfile, line)){
            text += line + '\n';
        }
        contents.push_back(text);
        mailfile.close();
    }
    return contents;    
}

std::vector<int> searchmails(IMAPConnection &imap, const std::string &mailbox, const std::string &from,
                const std::string &to, const std::string &subject, const std::string &text,
                const std::string &nottext, const std::string &since, const std::string &before){

    if (since != ""){
        std::string segment;
        std::vector<std::string> sincelist;

        while(std::getline(std::stringstream(since), segment, '-')){
            sincelist.push_back(segment);
        }

        if (sincelist.size() != 3 || sincelist[1].size() != 3){
            return std::vector<int> ();
        }
    }

    if (before != ""){
        std::string segment;
        std::vector<std::string> beforelist;

        while(std::getline(std::stringstream(before), segment, '-')){
            beforelist.push_back(segment);
        }

        if (beforelist.size() != 3 || beforelist[1].size() != 3){
            return std::vector<int> ();
        }
    }

    return imap.search(mailbox, from, to, subject, text, nottext, since, before);
}

bool createMailbox(IMAPConnection &imap, const std::string &mailbox){
    bool response = imap.createMailbox(mailbox);
    if (response)
        sync();
    return response;
}

bool deleteMailbox(IMAPConnection &imap, const std::string &mailbox){
    bool response = imap.deleteMailbox(mailbox);
    if (response)
        sync();
    return response;
}

bool renameMailbox(IMAPConnection &imap, const std::string &oldmailbox, const std::string &newmailbox){
    bool response = imap.renameMailbox(oldmailbox, newmailbox);
    if (response)
        sync();
    return response;
}

bool sendMail(config &config, const std::string &from, const std::string &to, const std::string &subject, const std::string &msg) {
    SMTPConnection smtp(config.smtp_server, config.smtp_port);
    smtp.auth(config.username, config.password);
    std::string body = "";

    // TODO send
}
