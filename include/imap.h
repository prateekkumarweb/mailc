/**
 * @file
 */

#ifndef IMAP_H
#define IMAP_H

#include <iostream>
#include <regex>
#include <string>
#include <tuple>
#include <vector>

#include "socket.h"

/**
 * @brief Mail structure
 * @details Stores information about a mail
 *
 */
struct Mail {
  /**
   * UID of the mail
   */
  int uid;
  /**
   * Mailbox of the mail
   */
  std::string mailbox;
  /**
   * From address
   */
  std::string from;
  /**
   * To address
   */
  std::string to;
  /**
   * Subject of the mail
   */
  std::string subject;
  /**
   * Received date of the mail
   */
  std::string date;
  /**
   * Body of the mail
   */
  std::string text;
};

/**
 * @brief IMAPConnection class
 * @details Stores socket which connect to IMAP server, has methos
 * which implemnet the IMAP commands
 *
 */
class IMAPConnection {
 public:
  /**
   * @brief Create IMAP connection
   * @details Create socket, connect using ssl
   *
   * @param hostname The hostname of the IMAP server
   * @param port The port of the IMAP server
   */
  IMAPConnection(const std::string &hostname, int port);

  /**
   * @brief Login in the server
   * @details Authenticate with server
   *
   * @param username The username of the user
   * @param password The password of the user
   *
   * @return Whether the login was successful
   */
  bool login(const std::string &username, const std::string &password);

  /**
   * @brief No operatinon
   * @details Keeps connection alive
   * @return Whether NOOP was successfully sent
   */
  bool noop();

  /**
   * @brief Logout of the server
   * @details Logout of the server
   * @return Whether logout was successful
   */
  bool logout();

  /**
   * @brief Get unseen mails
   * @details Return all the unseen mails in a mailbox
   *
   * @param mailbox The mailbox name
   * @return List of unseen mails
   */
  std::vector<Mail> getUnseenMails(const std::string &mailbox);

  /**
   * @brief Get top k mails
   * @details Return all the top k mails in a mailbox
   *
   * @param mailbox The mailbox name
   * @param k The number of the mails to return
   *
   * @return K mails from the mailbox
   */
  std::vector<Mail> getTopMails(const std::string &mailbox, int k);

  /**
   * @brief Search mails
   * @details Search mails from a mailbox with different search criteria
   *
   * @param mailbox The mailbox name
   * @param from From criteria
   * @param to To criteria
   * @param subject Search in the subject of the mails
   * @param text Search in the text of the mails
   * @param nottext Search the mails not containing a text
   * @param since Search mails since a date
   * @param before Search mails before a date
   *
   * @return [description]
   */
  std::vector<int> search(
      const std::string &mailbox, const std::string &from = "",
      const std::string &to = "", const std::string &subject = "",
      const std::string &text = "", const std::string &nottext = "",
      const std::string &since = "", const std::string &before = "");

  /**
   * @brief Get mail from server
   * @details Get a mail from mailbox with given uid
   *
   * @param mailbox The mailbox name
   * @param uid The uid of the mail
   *
   * @return Mail object of the mail
   */
  Mail getMail(const std::string &mailbox, const int uid);

  /**
   * @brief Get mails form server
   * @details Get mails from mailbox with given uids
   *
   * @param mailbox The mailbox name
   * @param uids The uids of the mails
   *
   * @return vector of mail objects
   */
  std::vector<Mail> getMails(const std::string &mailbox, std::vector<int> uids);

  /**
   * @brief Delete a mail
   * @details Delete a mail given its details
   *
   * @param mail Mail object containg details of the mail
   * @return Status whether the mail is deleted
   */
  bool deleteMail(Mail mail);

  /**
   * @brief Get mail counts
   * @details Get mail count in a mailbox
   *
   * @param mailbox The mailbox name
   * @return The exists, recent and unseen mails count
   */
  std::tuple<int, int, int> getCount(const std::string &mailbox);

  /**
   * @brief Create mailbox
   * @details Create a mailbox
   *
   * @param mailbox The mailbix to be created
   *
   * @return Status whether mailbox is created
   */
  bool createMailbox(const std::string &mailbox);

  /**
   * @brief Rename mailbox
   * @details Rename a mailbox
   *
   * @param oldmailbox Old name of the mailbox
   * @param newmailbox New name of the mailbox
   *
   * @return Status whether the mailbox was renamed
   */
  bool renameMailbox(const std::string &oldmailbox,
                     const std::string &newmailbox);

  /**
   * @brief Delete mailbox
   * @details Delete a mailbox
   *
   * @param mailbox The mailbox to be deleted
   *
   * @return Status whether mailbox is deleted
   */
  bool deleteMailbox(const std::string &mailbox);

  /**
   * @brief Get mailboxes
   * @details Get mailboxes form the IMAP server
   * @return Vector of all the mailboxes from server
   */
  std::vector<std::string> getmailboxes();

  /**
   * @brief Select a mailbox
   * @details Select a mailbox using IMAP protocol
   *
   * @param mailbox The mailbox name
   * @return Status if the mail is selected
   */
  bool select(const std::string &mailbox);

  /**
   * @brief Get all mails
   * @details Get all mail uids from aa mailbox
   *
   * @param mailbox The mailbox
   * @return List of mail uids
   */
  std::vector<int> getAllmails(const std::string &mailbox);

  /**
   * @brief Move mail
   * @details Move a mail from one mailbox to another
   *
   * @param oldmailbox Current mailbox
   * @param uid Uid of the mail
   * @param newmailbox Mailbox into which mail is to be mm=oved
   * @return Status whether mail was successfully moved
   */
  bool moveMail(const std::string &oldmailbox, int uid,
                const std::string &newmailbox);

 private:
  Socket socket;
  std::string username;
  std::string password;
  std::regex rgx;
  std::string id_string;
  bool check_response(const std::string &response,
                      const std::string &id_string);
};

#endif