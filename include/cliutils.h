/**
 * @file
 */

#ifndef CLIUTILS_H
#define CLIUTILS_H

#include <dirent.h>
#include <experimental/filesystem>
#include <fstream>
#include <random>
#include <sstream>
#include <string>
#include "imap.h"
#include "smtp.h"

/**
 * @brief Config structure used to store the details of the server
 * and user.
 *
 */
struct config {
  /**
   * Port of IMAP server
   */
  int imap_port;
  /**
   * Port of SMTP server
   */
  int smtp_port;
  /**
   * Hostname of IMAP server
   */
  std::string imap_server;
  /**
   * Hostname of SMTP server
   */
  std::string smtp_server;
  /**
   * Username of the user
   */
  std::string username;
  /**
   * Password of the user
   */
  std::string password;
  /**
   * Name of the user
   */
  std::string name;
};

/**
 * @brief MAIL_PATH
 * @details Offline mail path i.e. `$HOME/.mailc`
 * @return Returns the offline mail path
 */
std::string MAIL_PATH();

/**
 * @brief Get directories
 * @details List all directories stored in path
 *
 * @param path Path to search directories for
 * @param dirs Appends directories to this vector
 */
void GetReqDirs(const std::string &path, std::vector<std::string> &dirs);

/**
 * @brief Convert string to lowercase
 *
 * @param s String object
 * @return Lowercase of the input string
 */
std::string tolowercase(const std::string &s);

namespace fs = std::experimental::filesystem;

namespace cliutils {
/**
 * @brief Syncs the messages from the server
 * @details This function syncs the messages from the server and are stored
 * $HOME/.mailc directory
 *
 * @param imap IMAPConnection object to sync messages from
 * @return Whether the sync was successful or not.
 */
bool sync(IMAPConnection &imap);

/**
 * @brief Send a mail using SMTP
 * @details This function creates an SMTP Connection and send
 * the mail to using SMTP protocol.
 *
 * @param config Details about server and user.
 * @param to To mail address
 * @param subject Subject of the mail
 * @param msg The body of the mail
 * @return Whether the mail was sent successfully or not.
 */
bool sendMail(config &config, const std::string &to, const std::string &subject,
              const std::string &msg);

/**
 * @brief Deletes a mail
 * @details Deletes the mail with given uid from given mailbox
 *
 * @param imap IMAPConnection to send the delete command
 * @param mailbox The mailbox from which the mail is to be deleted
 * @param uid The uid of the mail to be deleted
 * @return Whether the mail was sucessfully deleted or not
 */
bool deleteMail(IMAPConnection &imap, const std::string &mailbox, int uid);

/**
 * @brief Read a mail
 * @details Read an email stored offline
 *
 * @param mailbox The mailbox from which the mail is to be read
 * @param uid The uid of the mail
 *
 * @return The Mail object containg the mail details
 */
Mail readMail(const std::string &mailbox, int uid);

/**
 * @brief Get Mails from the server
 * @details Fetches mails from imap server in a given mailbox
 *
 * @param imap IMAPConnection object
 * @param mailbox Mailbox from which mails are to be fetched
 * @param uids The uids of the mails which are to be fetched
 * @return Vector of Mail objects.
 */
std::vector<Mail> getMails(IMAPConnection &imap, const std::string &mailbox,
                           std::vector<int> uids);

/**
 * @brief Search mails using imap search command
 * @details Search mails based on various search filters
 *
 * @param imap IMAPConnectio object
 * @param mailbox Mailbox to search mails from
 * @param from Search mails received from
 * @param to Search mails sent to
 * @param subject Search mails containg a text in subject
 * @param text Search the mails containing a text in body
 * @param nottext Search the mails not containing a text
 * @param since Search the mails since a date
 * @param before Search the mails before a date
 * @return All the uids of the mail matching the search filters
 */
std::vector<int> searchMails(IMAPConnection &imap, const std::string &mailbox,
                             const std::string &from, const std::string &to,
                             const std::string &subject,
                             const std::string &text,
                             const std::string &nottext,
                             const std::string &since,
                             const std::string &before);

/**
 * @brief Create mailbox
 * @details Create a mailbox
 *
 * @param imap IMAPConnection object
 * @param mailbox The mailbix to be created
 *
 * @return Status whether mailbox is created
 */
bool createMailbox(IMAPConnection &imap, const std::string &mailbox);

/**
 * @brief Delete mailbox
 * @details Delete a mailbox
 *
 * @param imap IMAPConnection object
 * @param mailbox The mailbox to be deleted
 *
 * @return Status whether mailbox is deleted
 */
bool deleteMailbox(IMAPConnection &imap, const std::string &mailbox);

/**
 * @brief Rename mailbox
 * @details Rename a mailbox
 *
 * @param imap IMAPConnection object
 * @param oldmailbox Old name of the mailbox
 * @param newmailbox New name of the mailbox
 * @return Status whether the mailbox was renamed
 */
bool renameMailbox(IMAPConnection &imap, const std::string &oldmailbox,
                   const std::string &newmailbox);

/**
 * @brief Move mail
 * @details Move a mail from one mailbox to another
 *
 * @param imap IMAPConnection object
 * @param oldmailbox Current mailbox of the mail
 * @param uid The uid of the mail
 * @param newmailbox Mailbox to move the mail into
 * @return Status whether the mail was moved or not
 */
bool moveMail(IMAPConnection &imap, const std::string &oldmailbox, int uid,
              const std::string &newmailbox);

/**
 * @brief List mailboxes
 * @details List all the mailboxes available offline
 * @return List of mailboxed
 */
std::vector<std::string> getMailboxes();

/**
 * @brief Noop imap
 * @details Continue connection with imap server
 * @return Status whether noop was successful
 */
bool noop(IMAPConnection &imap);
};  // namespace cliutils

#endif
