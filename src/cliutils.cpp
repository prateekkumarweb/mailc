#include "cliutils.h"

void GetReqDirs(const std::string& path, std::vector<std::string>& dirs){
    DIR *dpdf;
    struct dirent *epdf;
    dpdf = opendir(path.c_str());
    if (dpdf != NULL){
        while ((epdf = readdir(dpdf)) != NULL){
            if(epdf->d_type==DT_DIR && strstr(epdf->d_name,"..") == NULL && strstr(epdf->d_name,".") == NULL){
                dirs.push_back((path+epdf->d_name).substr(2));
                GetReqDirs(path+epdf->d_name+"/", dirs);               
            }            
        }
    }
    closedir(dpdf);
}

bool sync(IMAPConnection imap){
    
    // List the folders and check if present
    std::vector<std::string> offline_mailboxes;
    GetReqDirs("./", offline_mailboxes);
        
    std::vector<std::string> mailboxes = imap.getmailboxes();
    
    
    for (auto it = mailboxes.begin(); it != mailboxes.end(); it++){
        if (std::find(offline_mailboxes.begin(), offline_mailboxes.end(), *it) != offline_mailboxes.end()){
            //Create the mailbox (*it)
            fs::create_directory(*it);
        }
    }
    
    
    for(auto it = offline_mailboxes.begin(); it != offline_mailboxes.end(); it++){
        if (std::find(mailboxes.begin(), mailboxes.end(), *it) != mailboxes.end()){
            //Delete the mailbox (*it)
            fs::remove(*it);
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
                
        std::string mailbox_path = "./" + mailbox;
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
            if (std::find(uids.begin(), uids.end(), *it2) != uids.end()){
                //Delete the mail
                std::string mailpath = "./" + mailbox + "/" + std::to_string(*it2) + ".m";
                fs::remove(mailpath);
            }
        }
        
        for(auto it2 = uids.begin(); it2 != uids.end(); it2++){
            if (std::find(offline_uids.begin(), offline_uids.end(), *it2) != offline_uids.end()){
                std::string mailpath = "./" + mailbox + "/" + std::to_string(*it2) + ".m";
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

std::vector<std::string> readMail(const std::string& mailbox, int uid){
    std::string path = "./" + mailbox + "/" + std::to_string(uid) + ".m";
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

bool send(const std::string &from, const std::string &to,
	const std::string &subject, const std::string &body){
	    
}
