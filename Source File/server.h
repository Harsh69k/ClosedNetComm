#ifndef server_h
#define server_h
/************************************************************************

 "┌─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐\n"\
 "│  ██████╗██╗      ██████╗ ███████╗███████╗██████╗ ███╗   ██╗███████╗████████╗ ██████╗ ██████╗ ███╗   ███╗███╗   ███╗ │\n"\
 "│ ██╔════╝██║     ██╔═══██╗██╔════╝██╔════╝██╔══██╗████╗  ██║██╔════╝╚══██╔══╝██╔════╝██╔═══██╗████╗ ████║████╗ ████║ │\n"\
 "│ ██║     ██║     ██║   ██║███████╗█████╗  ██║  ██║██╔██╗ ██║█████╗     ██║   ██║     ██║   ██║██╔████╔██║██╔████╔██║ │\n"\
 "│ ██║     ██║     ██║   ██║╚════██║██╔══╝  ██║  ██║██║╚██╗██║██╔══╝     ██║   ██║     ██║   ██║██║╚██╔╝██║██║╚██╔╝██║ │\n"\
 "│ ╚██████╗███████╗╚██████╔╝███████║███████╗██████╔╝██║ ╚████║███████╗   ██║   ╚██████╗╚██████╔╝██║ ╚═╝ ██║██║ ╚═╝ ██║ │\n"\
 "│  ╚═════╝╚══════╝ ╚═════╝ ╚══════╝╚══════╝╚═════╝ ╚═╝  ╚═══╝╚══════╝   ╚═╝    ╚═════╝ ╚═════╝ ╚═╝     ╚═╝╚═╝     ╚═╝ │\n"\
 "└─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘\n"
/
 *  MIT License
 *
 * Copyright (c) 2023-2024 Harsh harshanand427@yahoo.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ***********************************************************************************/

#include <chrono>
#include <cstring>
#include <iostream>
#include <thread>
#include <unistd.h>
#include <vector>
#include <unordered_map>

#ifdef __MACH__
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#endif

#ifdef __unix__
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#define __DARWIN_FD_ZERO(p) FD_ZERO(p)
#define __DARWIN_FD_SET(fd, p) FD_SET(fd, p)
#define __DARWIN_FD_ISSET(fd, p) FD_ISSET(fd, p)
#endif

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#define __DARWIN_FD_ZERO(p) FD_ZERO(p)
#define __DARWIN_FD_SET(fd, p) FD_SET(fd, p)
#define __DARWIN_FD_ISSET(fd, p) FD_ISSET(fd, p)
#endif

int32_t backlog = 3;
const uint32_t SIZE = 8192;
bool log_enabled = false;
#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"

struct ClientDetails {
      int32_t ClientSocketFileDiscription;
      int32_t serverSocketFileDiscription;
      std::vector<int> whichClient;
      bool broadCastingPrevMsgToClient;
      char MsgResv[8192]{};
      int32_t tempfd;
      std::unordered_map<int, std::string> userName;
      explicit ClientDetails(int32_t fd) {
            this->ClientSocketFileDiscription = fd;
            this->serverSocketFileDiscription = -1;
            this->tempfd = 0;
            this->broadCastingPrevMsgToClient = false;
            memset(this->MsgResv, '\0', 8192);
      }
};
typedef struct ClientDetails *CliDetails;

/* structure for node*/

struct subNode {
      char *answer;
      subNode *down;
      explicit subNode(char ans[SIZE]) {
            this->answer = (char *) calloc(SIZE, sizeof(char));
            strcpy(this->answer, ans);
            this->down = nullptr;
            //      memset(this->answer, '\0', SIZE);
      }
};

struct node {
      int32_t setNo;
      node *next;
      subNode *down;
      explicit node(int32_t set) {
            this->setNo = set;
            this->next = nullptr;
            this->down = nullptr;
      }
} *head = nullptr, *tail = nullptr;

struct Chat {
      char *msg;
      Chat *next;
      explicit Chat(char msg[SIZE]) {
            this->msg = (char *) calloc(SIZE, sizeof(char));
            strcpy(this->msg, msg);
            this->next = nullptr;
      }
} *chatHead = nullptr, *chatTail = nullptr;
void saveChat(char msg[SIZE]);
void ChatMemoryManagement();
void messageBroadcast(CliDetails);
void broadcastChat(CliDetails client);

void __attribute__((destructor)) ChatMemoryManagement();

void cleanUp(CliDetails client) {
      char ch;
      while (true) {
            scanf("Press any key to exit");
            std::cin >> ch;
            if (std::cin.fail()) {
                  std::cin.clear();
                  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                  std::cout << RED << "Something went wrong in while taking input\n" << RESET;
                  continue;
            }
            if (ch == 'q') {
                  std::cout << RED << "shutting down server and client \n" << RESET;
                  const char exitNote[] = {'1', 'e', '\0'};
                  for (auto fd : client->whichClient) {
                        std::cout << "sending death note to client " << client->userName[fd] << "\n";
                        std::this_thread::sleep_for(std::chrono::milliseconds(5));
                        send(fd, exitNote, sizeof exitNote, 0);
                  }
                  for (auto fd : client->whichClient) {
                        close(fd);
                        shutdown(fd, SHUT_RDWR);
                  }
                  close(client->serverSocketFileDiscription);
                  shutdown(client->serverSocketFileDiscription, SHUT_RDWR);
                  std::this_thread::sleep_for(std::chrono::milliseconds(3000));
                  // system("clear");
                  delete client;
                  exit(0);
            } else system("clear");
      }
}

int32_t server(char ip[], int port, int logging) {

      int argv[2];
      argv[1] = logging;
      auto clientDetails = new ClientDetails(0);
      system("clear");
      std::thread t1(cleanUp, clientDetails);
      t1.detach();
      if (argv[1]) {
            std::cout << "Server is running on port " << port << "\n";
            std::cout << GREEN << "logging enable\n" << RESET;
            log_enabled = true;
      } else {
            std::cout << RED << "logging disable\n" << RESET;
            log_enabled = false;
      }
//      std::cout << RED << welcome << RESET;

      int32_t serverSocketFileDiscription = socket(AF_INET, SOCK_STREAM, 0);
      if (serverSocketFileDiscription <= 0) {
            if (log_enabled) perror("socket creation error\n");
            exit(1);
      }
      //

      clientDetails->whichClient.reserve(20);
      // setting serverSocketFileDiscription to allow multiple connection
      int opt = 1;
      if (setsockopt(serverSocketFileDiscription, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof(opt)) < 0) {
            if (log_enabled)perror("setsockopt(SO_REUSEADDR) failed");
            exit(2);
      }
      struct sockaddr_in address{};
      address.sin_family = AF_INET;
      address.sin_port = htons(port);
      inet_pton(AF_INET, ip, &address.sin_addr.s_addr);
      int32_t BindResult = bind(serverSocketFileDiscription, (struct sockaddr *) &address, sizeof address);
      //  BindResult?exit(2):printf("binding successful\n");
      if (BindResult == 0) {
            if (log_enabled)printf("binding successful\n");
      } else {
            if (log_enabled) perror("biding unsuccessful\n");
            exit(2);
      }

      uint64_t listenRusult = listen(serverSocketFileDiscription, backlog);
      if (listenRusult == 0) {
            if (log_enabled) printf("listening successful\n");
      } else {
            if (log_enabled)perror("listening unsuccessful\n");
            exit(3);
      }

      if (argv[1]) std::cout << "Server is running on port " << port << "\n waiting for client to connect\n";
      int activity;
      int sd = 0;
      fd_set readfd;
      size_t valread;
      int maxfd;
      while (true) {
            if (log_enabled) printf("waiting for activity\n");
            // clear the socket set
            FD_ZERO(&readfd);
            FD_SET(serverSocketFileDiscription, &readfd);
            maxfd = serverSocketFileDiscription;
            for (auto sd : clientDetails->whichClient)
                  if (sd > 0) {
                        FD_SET(sd, &readfd);
                  }
            if (sd > maxfd) {
                  maxfd = sd;
            }
            activity = select(maxfd + 1, &readfd, nullptr, nullptr, nullptr);
            if (activity < 0) {
                  perror("select error\n");
            }
            /*
             * if something happen on master socket then it's an incoming connection
             */
            if (FD_ISSET(serverSocketFileDiscription, &readfd)) {
                  socklen_t addressLen = sizeof address;
                  clientDetails->ClientSocketFileDiscription =
                        accept(serverSocketFileDiscription, (struct sockaddr *) &address, &addressLen);

                  if (clientDetails->ClientSocketFileDiscription < 0) {
                        perror("error in accepting incoming connection\n");
                        break;
                  }
                  clientDetails->whichClient.push_back(clientDetails->ClientSocketFileDiscription);
                  if (argv[1])
                        std::cout << GREEN << "New client joined the server\n"
                                  << RESET;
                  if (log_enabled)
                        printf("New connection , socket fd is %d , ip is : %s , port : %d\n",
                               clientDetails->ClientSocketFileDiscription,
                               inet_ntoa(address.sin_addr),
                               ntohs(address.sin_port));
                  clientDetails->broadCastingPrevMsgToClient = true;
                  clientDetails->tempfd = clientDetails->ClientSocketFileDiscription;
                  std::thread broadcastThread(broadcastChat, clientDetails);
                  broadcastThread.detach();

            }


            /*
             * else it's some IO operation on some other socket
             */

            char buffer[8192];
            for (int i = 0; i < clientDetails->whichClient.size(); ++i) {
                  sd = clientDetails->whichClient[i];
                  if (FD_ISSET(sd, &readfd)) {

                        if ((valread = read(sd, buffer, 8192)) == 0) {
                              getpeername(sd, (struct sockaddr *) &address, (socklen_t *) &address);
                              if (argv[1]) std::cout << RED << "Host disconnected " << RESET;
                              if (log_enabled)
                                    if (argv[1])
                                          std::cout << RED << "Host disconnected ,ip " << inet_ntoa(address.sin_addr)
                                                    << " ,port " << ntohs(address.sin_port) << "\n" << RESET;
                              close(sd);


                              char clientExitNote[22];
                              clientExitNote[0] = '1';
                              clientExitNote[1] = 'Y';
                              /* copying UserName to exit note message*/
                              clientDetails->userName[sd] += '\n';
                              strcpy(clientExitNote + 2, clientDetails->userName[sd].c_str());
                              std::cout << RED << "\ndisconnected " << RESET << sd << " : " << YELLOW
                                        << clientDetails->userName[sd] << RESET << "\n";
                              /* removing the client form the map*/
                              clientDetails->userName.erase(sd);
                              auto it = find(clientDetails->whichClient.begin(), clientDetails->whichClient.end(), sd);
                              if (it != clientDetails->whichClient.end())
                                    clientDetails->whichClient.erase(it);

//                            saving client disconnection message
                              saveChat(clientExitNote);
                              /*
                               * telling all the connected client that @userName left the chat
                               */
                              for (auto fd : clientDetails->whichClient) {
                                    send(fd, clientExitNote, sizeof clientExitNote, 0);
                              }

                        } else {
                              if (valread > 1) {
                                    // adding name to clientDetails->userName[sd]
                                    if (buffer[1] == 'X') {
                                          clientDetails->userName[sd] = buffer + 2;
                                          size_t pos = clientDetails->userName[sd].find('\n');
                                          // If the newline is found, erase it
                                          if (pos != std::string::npos) {
                                                clientDetails->userName[sd].erase(pos, 1);
                                          }
                                    }
                                    strcpy(clientDetails->MsgResv, buffer);
//                                  saving all chat
                                    saveChat(clientDetails->MsgResv);
                                    clientDetails->tempfd = sd;
                                    // send to all the connected client as it is
                                    std::thread broadcastThread(messageBroadcast, clientDetails);
                                    broadcastThread.detach();
                                    // for creating the set on server
                                    if (buffer[1] == 'a') {
                                          if (argv[1])
                                                std::cout << RED << "ATTENTION! " << RESET << " new set  : " << GREEN
                                                          << buffer[0] - 48 << RESET
                                                          << " added by " << clientDetails->tempfd << " : "
                                                          << clientDetails->userName[clientDetails->tempfd] << "\n";
                                    }
                                    // if index encounter b then some  client have sent message to crate sub set;
                                    if (buffer[1] == 'b') {
                                          if (argv[1])
                                                std::cout << RED << "ATTENTION! " << RESET
                                                          << "New Answer added in set :- "
                                                          << GREEN << buffer[0] - 48 << RESET << ",  by user "
                                                          << clientDetails->tempfd << " : " << BLUE
                                                          << clientDetails->userName[clientDetails->tempfd] << RESET
                                                          << "\n";
                                    }
                                    memset(buffer, '\0', 8192);
                              }
                        }
                  }
            }
      }

/* clean up to prevent memory leak */

      close(serverSocketFileDiscription);
      shutdown(serverSocketFileDiscription,
               SHUT_RDWR);
      for (auto fd : clientDetails->whichClient) {
            close(fd);
            shutdown(fd,
                     SHUT_RDWR);
      }
      delete clientDetails;
      system("clear");
      system("exit");
      return 0;
}
/*used to broadcast the message to all the connected client except the sender */
void messageBroadcast(CliDetails client) {
//      char newMessageWithUserName[8196];
//      newMessageWithUserName[0]=client->MsgResv[0];
//      newMessageWithUserName[1]=client->MsgResv[1];
//      strcpy(newMessageWithUserName+2, client->userName[client->tempfd].c_str());
//      client->MsgResv[1]=' ';
//      client->MsgResv[0]=':';
//      // copying message with username
//      strcpy(newMessageWithUserName + 10,client->MsgResv);
      for (auto fd : client->whichClient) {
            if (log_enabled)
                  std::cout << RED << "file descriptor: " << fd << " " << client->userName[fd] << "\n" << RESET;
            if (fd != client->tempfd) {
                  if (log_enabled)
                        std::cout << GREEN << "Broadcasting message to client: " << RESET << fd << RED
                                  << client->userName[fd] << RESET << std::endl;
                  send(fd, client->MsgResv, strlen(client->MsgResv), 0);
            }
      }
      memset(client->MsgResv, '\0', 8192);
      return;
}
/*
 * saving all message
 */
void saveChat(char msg[SIZE]) {
      if (log_enabled) std::cout << GREEN << "chat saved\n" << RESET;
      for (int i = 0; i < 20; ++i) {
            if (log_enabled) std::cout << msg[i];
      }
      if (log_enabled) std::cout << ".........\n";
      Chat *newChat = new Chat(msg);
      if (chatHead == nullptr) {
            chatHead = newChat;
            chatTail = newChat;
      } else {
            chatTail->next = newChat;
            chatTail = newChat;
      }
}
/* this will broadcast all the previous chat message and answer  */
void broadcastChat(CliDetails client) {
      if (log_enabled)
            std::cout << GREEN << "broadcasting chat to fd : " << RESET << client->tempfd << RED
                      << client->userName[client->tempfd] << RESET << std::endl;
      Chat *temp = chatHead;
      while (temp and client->broadCastingPrevMsgToClient) {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            send(client->tempfd, temp->msg, strlen(temp->msg), 0);
            if (log_enabled) std::cout << GREEN << "message send: " << RESET << temp->msg;
            temp = temp->next;
      }
      char msg[] = {'1', 'Z', '\0'};
      std::this_thread::sleep_for(std::chrono::seconds(2));
      send(client->tempfd, msg, strlen(msg), 0);
      client->broadCastingPrevMsgToClient = false;
      return;
}

void ChatMemoryManagement() {
      std::cout << RED << "deleting chat memory\n" << RESET;
      while (chatHead) {
            Chat *ChatNext = chatHead->next;
            /* deleting chat node*/
            delete chatHead;
            chatHead = ChatNext;
      }
      chatHead = nullptr;
      std::this_thread::sleep_for(std::chrono::seconds(1));
      if (log_enabled) system("clear");
}
// memory leak management

#endif /* server_h */

