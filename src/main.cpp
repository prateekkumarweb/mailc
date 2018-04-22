#include "imap.h"

int main(){
    // Socket client("pop.zoho.com", 993);
    // client.IMAPConnect("test@prateekkumar.in", "testtest");
    // client.IMAPNoop();
    // client.IMAPList("","*");
    // client.IMAPSelect("INBOX");
    // client.IMAPExamine("INBOX");
    // client.IMAPCreate("TEST2");
    // client.IMAPList("", "*");
    // client.IMAPCreate("TEST2/CHILD");
    // client.IMAPList("", "*");
    // client.IMAPDelete("TEST2");
    // client.IMAPDelete("TEST2/CHILD");
    // client.IMAPDelete("TEST2");
    // client.IMAPList("", "*");
    // //client.IMAPStatus("INBOX", "MESSAGES");
    // client.IMAPlogout();

    std::string hostname = "imap.zoho.com";

    IMAPConnection imap(hostname, 993);

    imap.login("test@prateekkumar.in", "testtest");

    std::string s1 = "TEST5";
    std::string s2 = "INBOX";

    // auto [a, b, c] = imap.getCount(s2);
    // std::cerr << a << " " << b << " " << c << std::endl;

    /*Mail mail = imap.getMail("INBOX", 6);

    std::cerr << mail.from << std::endl;
    std::cerr << mail.to << std::endl;
    std::cerr << mail.subject << std::endl;
    std::cerr << mail.date << std::endl;

    std::cerr << mail.text << std::endl;
    std::cerr << mail.html << std::endl;*/
    
    std::vector<Mail> mails = imap.getUnseenMails("INBOX");
    for (int i=0; i<mails.size(); i++){
        std::cerr << mails[i].from << std::endl;
        std::cerr << mails[i].to << std::endl;
        std::cerr << mails[i].subject << std::endl;
        std::cerr << mails[i].date << std::endl;
        
        std::cerr << mails[i].text << std::endl;
        std::cerr << mails[i].html << std::endl;
    }
    

    return 0;
}
