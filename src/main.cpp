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
            std::cout << "send\t\tSend an email" << std::endl;
            std::cout << "quit\t\tQuit the program" << std::endl;
            std::cout << "read\t\tRead mail from a mailbox" << std::endl;
            std::cout << "search\t\tSearch for mails" << std::endl;
            std::cout << "delete\t\tDelete mail from a mailbox" << std::endl;
            std::cout << "sync\t\tSync the folders and mails" << std::endl;
        }
        else if (cmd == "quit") {
            break;
        }
        else if (cmd == "search") {
            std::string mailbox, from, to, subject, text, nottext, since, before;
            std::cout << "mailbox [inbox]> ";
            std::getline(std::cin, mailbox);
            if (mailbox.size() == 0) mailbox = "inbox";
            std::cout << "from (optional)> ";
            std::getline(std::cin, from);
            std::cout << "to (optional)> ";
            std::getline(std::cin, to);
            std::cout << "subject (optional)> ";
            std::getline(std::cin, subject);
            std::cout << "text (optional)> ";
            std::getline(std::cin, text);
            std::cout << "not_text (optional)> ";
            std::getline(std::cin, nottext);
            std::cout << "since (optional)> ";
            std::getline(std::cin, since);
            std::cout << "before (optional)> ";
            std::getline(std::cin, before);
            std::vector<int> mailuids = cliutils::searchMails(imap, mailbox, from, to, subject,
                text, nottext, since, before);
            auto mails = cliutils::getMails(imap, mailbox, mailuids);
            std::cout << mails.size() << " matching mails found." << std::endl;
            for (const auto &mail: mails) {
                if (mail.uid >= 0) {
                    std::cout << mail.uid << "\t" << mail.from << "\t\t" << mail.to << "\n"
                        << "\t" << mail.subject << std::endl;
                }
            }
        }
        else if (cmd == "read") {
            std::string mailbox, uid_str;
            std::cout << "mailbox [inbox]> ";
            std::getline(std::cin, mailbox);
            if (mailbox.size() == 0) mailbox = "inbox";
            std::cout << "mailuid> ";
            std::getline(std::cin, uid_str);
            Mail mail = cliutils::readMail(mailbox, std::stoi(uid_str));
            if (mail.uid >= 0) {
                std::cout << "From: " << mail.from << std::endl;
                std::cout << "To: " << mail.to << std::endl;
                std::cout << "Subject: " << mail.subject << std::endl;
                std::cout << "Date: " << mail.date << std::endl;
                std::cout << "Body: " << std::endl;
                std::cout << mail.text << std::endl;
            } else {
                std::cout << "Mail not found." << std::endl;
            }
        }
        else if (cmd == "delete") {
            std::string mailbox, uid_str;
            std::cout << "mailbox [inbox]> ";
            std::getline(std::cin, mailbox);
            if (mailbox.size() == 0) mailbox = "inbox";
            std::cout << "mailuid> ";
            std::getline(std::cin, uid_str);
            if(cliutils::deleteMail(imap, mailbox, std::stoi(uid_str))) {
                std::cout << "Deleted." << std::endl;
            } else {
                std::cout << "Error deleting!" << std::endl;
            }
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
