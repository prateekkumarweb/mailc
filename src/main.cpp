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

    imap.createMailbox("TEST4");

    return 0;
}
