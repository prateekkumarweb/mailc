#include "imap.h"
#include "smtp.h"
#include "cliutils.h"

int main(){

    std::cout << "IMAP SMTP Mail Client" << std::endl;
    config config{"imap.zoho.com", 993,
        "smtp.zoho.com", 465, "test@prateekkumar.in",
        "testtest"};
    // Check if IMAP config exists
    // If exists then read them

    // if (true /* If config does not exist */) {
    //     std::cout << "Enter IMAP settings:" << std::endl;
    //     std::cout << "IMAP server: ";
    //     std::cin >> config.imap_server;
    //     std::cout << "IMAP port: ";
    //     std::cin >> config.imap_port;
    //     std::cout << "SMTP server: ";
    //     std::cin >> config.smtp_server;
    //     std::cout << "SMTP port: ";
    //     std::cin >> config.smtp_port;
    // } else {
    //     // populate config
    // }

    IMAPConnection imap(config.imap_server, config.imap_port);
    if (imap.login(config.username, config.password)) {
        std::cout << "Login successful" << std::endl;
    }
    // SMTPConnection smtp(config.smtp_server, config.smtp_port);

    std::string cmd;
    std::cout << "client> ";

    while (std::getline(std::cin, cmd)) {
        if (cmd == "help") {
            // TODO
        }
        else if (cmd == "quit") {
            break;
        }
        else if (cmd == "send") {
            std::string from, to, subject, body, line;
            std::cout << "from> ";
            std::getline(std::cin, from);
            std::cout << "to> ";
            std::getline(std::cin, to);
            std::cout << "subject> ";
            std::getline(std::cin, subject);
            std::cout << "body> (Type RETURN.RETURN to end input)" << std::endl;
            while(body.size() < 5 || body.substr(body.size()-5, 5) != "\r\n.\r\n") {
                std::getline(std::cin, line);
                body += line + "\r\n";
            }
            SMTPConnection smtp(config.smtp_server, config.smtp_port);
            if (smtp.auth(config.username, config.password))
            {
                if (smtp.send(from, to, subject, body)) {
                    std::cout << "Sent mail." << std::endl;
                } else {
                    std::cout << "Mail could not be sent." << std::endl;
                }
            } else {
                std::cout << "Auth failed" << std::endl;
            }
        }
        else if (cmd == "sync") {
            cliutils::sync(imap);
        }
        std::cout << "client> ";
    }

    std::cout << std::endl;

    return 0;
}
