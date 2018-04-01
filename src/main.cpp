#include "socket.h"

int main(){
    Socket client("pop.zoho.com", 993);
    client.IMAPConnect("test@prateekkumar.in", "testtest");
    client.IMAPNoop();
    client.IMAPList("","*");
    //client.IMAPStatus("INBOX", "MESSAGES");
    client.IMAPlogout();
    return 0;
}
