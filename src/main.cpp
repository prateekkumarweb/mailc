#include "imap.h"
#include "smtp.h"
#include "cliutils.h"

int main(){

    std::cout << "IMAP SMTP Mail Client" << std::endl;
    config config{993, 465, "imap.zoho.com",
        "smtp.zoho.com", "test@prateekkumar.in",
        "testtest", "Prateek Test"};

    IMAPConnection imap(config.imap_server, config.imap_port);
    if (imap.login(config.username, config.password)) {
        std::cout << "Login successful" << std::endl;
    }

    std::string cmd;
    std::cout << "client> ";

    while (std::getline(std::cin, cmd)) {
        if (cmd == "help") {
            std::cout << "send\tSend an email" << std::endl;
            std::cout << "quit\tQuit the program" << std::endl;
            std::cout << "sync\tSync the folders and mails" << std::endl;
        }
        else if (cmd == "quit") {
            break;
        }
        else if (cmd == "send") {
            std::string to, subject, body, line;
            std::cout << "to> ";
            std::getline(std::cin, to);
            std::cout << "subject> ";
            std::getline(std::cin, subject);
            std::cout << "body> (Type RETURN.RETURN to end input)" << std::endl;
            while(body.size() < 5 || body.substr(body.size()-5, 5) != "\r\n.\r\n") {
                std::getline(std::cin, line);
                body += line + "\r\n";
            }

            body = body.substr(0, body.size()-5);

            if (cliutils::sendMail(config, to, subject, body)) {
                std::cout << "Sent mail." << std::endl;
            } else {
                std::cout << "Mail could not be sent." << std::endl;
            }
        }
        else if (cmd == "sync") {
            cliutils::sync(imap);
        } else {
            std::cout << "Invalid command. Type help for valid commands." << std::endl;
        }
        std::cout << "client> ";
    }

    std::cout << std::endl;

    return 0;
}
