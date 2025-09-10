# ft_irc - IRC Server

## 📌 Description
`ft_irc` is a custom implementation of an **IRC server** written in **C++98**, following the IRC protocol specification (**RFC 1459**).  

It handles multiple clients simultaneously using **non-blocking sockets** and **epoll**, supports the essential IRC commands, and can interact seamlessly with standard IRC clients such as **irssi** or testing tools like **netcat**.  

The goal of this project is to explore **network programming**, **protocol parsing**, and **event-driven server design**.

---

## ⚙️ Features

### Core Features

- Multiple client connections using `epoll` (no multi-threading).
- Client authentication with `PASS`, `NICK`, and `USER`.
- User management:
  - Change nickname (`NICK`)
  - Quit (`QUIT`)
- Channel management:
  - Join and leave channels (`JOIN`)
  - Channel topics (`TOPIC`)
  - Channel modes (`MODE`)
  - Kicking and inviting users (`KICK`, `INVITE`)
  - Automatic channel deletion when empty
- Private messaging (`PRIVMSG`).
- Proper numeric error replies (RFC-compliant).

### Custom Commands

- **`MYSERVER`**  
  Displays current server state (only available with netcat) :
 
  - Connected clients (nicknames + file descriptors)  
  - Active channels with:
    - User count
    - Operator nicknames
    - Topic
    - Modes  

- **`DISPLAYCMDS`**  
  Enables/disables **per-client command debugging** on the server terminal.  
  When active, the server logs every command received from this client with:
  
  - Command type  
  - Parameters  
  - Trailing part  

### Extra Goodies

- Built-in debug outputs for server activity.

---

## 🛠️ Compilation

The project is written in **C++98**. To compile:

make


## 🚀 Usage

Start the server with:

./ircserv <port> <password>

- <port> → the listening port (e.g. 6667)

- <password> → the password required for client connections

Example :

./ircserv 6667 mypass

---

## 💻 Connecting Clients


Using netcat (for testing):

nc -C localhost 6667

