# C++ Client for IMAP and SMTP

## How to install

```sh
git clone https://github.com/prateekkumarweb/imap_smtp_client
cd imap_smtp_client
mkdir build
cd build
cmake ..
```

## How to run

```sh
cd build
./client
```

## How to use the CLI

In the cli, multiple commands are available to send, read, delete, etc. mails
from differnet mail boxex by connecting to the server.
The cli supports terminal like tab completion and history on up and down arrow keys.

### List of commands in the CLI

1. `create`: Creates a new mailbox using IMAP protocol.
2. `delete`: Deletes a mail of given uid from a given mail using IMAP protocol.
3. `deletemb`: Deletes a given mailbox using IMAP protocol.
4. `help`: Lists all the commands.
5. `list`: Lists all the mailboxes available offline.
6. `move`: Moves a given mail from one mailbox to another using IMAP protocol.
7. `noop`: No operation. Just to keep IMAP connection alive.
8. `quit`: Quits the CLI.
9. `read`: Read a mail stored offline.
10. `rename`: Rename a mailbox using IMAP protocol.
11. `search`: Search for mails using different criteria using IMAP protocol.
12. `send`: Send a mail using SMTP protocol.
13. `sync`: Sync mails across mailboxes using IMAP protocol.

### Authors

* Prateek Kumar
* Vaibhav Sinha

