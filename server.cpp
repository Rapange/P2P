  /* Server code in C */
 
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <unistd.h>
#include <iostream>
#include <vector>
#include <utility>
#include <thread>
#define N_BYTES_TAM 3

using namespace std;

string leftPadding(string str, int total_tam){
  for(unsigned int i = str.size(); i < total_tam; i++)
    str = '0' + str;
  return str;
}

class Tracker{
  //string name;
  vector<string> IPs;
public:
  string convert();
  void join(int c_socket, string IP);
};

string Tracker::convert(){
  string ip_list;
  if(IPs.size() > 0){
    ip_list = IPs[0];
  }
  for(unsigned int i = 1; i < IPs.size(); i++){
    ip_list += ',';
    ip_list += IPs[i];
  }
  return ip_list;
}

void Tracker::join(int c_socket, string IP){

  char buffer[2];
  string ip_list, payload;
  int n;
  //IPs.push_back(IP);
  cout<<IPs.size()<<endl;
  n = read(c_socket,buffer,2);
     if (n < 0) perror("ERROR reading from socket");
     buffer[1] = '\0';
     if(buffer[0] == 'J'){
       printf("Here is the message: [%s]\n",buffer);
       ip_list = convert();
       payload = leftPadding(to_string(ip_list.size()),N_BYTES_TAM);
       payload += 'j';
       payload += ip_list;
       cout<<payload<<endl;
       char j_protocol[payload.size()+1];
       payload.copy(j_protocol,payload.size(),0);
       j_protocol[payload.size()] = '\0';
       n = write(c_socket,j_protocol,payload.size()+1);
     if (n < 0) perror("ERROR writing to socket");
    }
     IPs.push_back(IP);
     //cout<<IP<<endl;
     
     shutdown(c_socket, SHUT_RDWR);
 
      close(c_socket);
}




  int main(void)
  {
    Tracker tracker;
    struct sockaddr_in stSockAddr, client_addr, *pV4Addr;
    struct in_addr ipAddr;
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    char buffer[256], str[INET_ADDRSTRLEN];
    int n;
    socklen_t clen;
 
    if(-1 == SocketFD)
    {
      perror("can not create socket");
      exit(EXIT_FAILURE);
    }
 
    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
 
    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(1100);
    stSockAddr.sin_addr.s_addr = INADDR_ANY;
 
    if(-1 == bind(SocketFD,(const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
    {
      perror("error bind failed");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }
 
    if(-1 == listen(SocketFD, 10))
    {
      perror("error listen failed");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }

    clen = sizeof(client_addr);
    for(;;)
    {
      
      int ConnectFD = accept(SocketFD, (struct sockaddr*) &client_addr, &clen);
      
      if(0 > ConnectFD)
      {
        perror("error accept failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
      }
      pV4Addr = (struct sockaddr_in*)&client_addr;
      ipAddr = pV4Addr->sin_addr;
      inet_ntop(AF_INET, &ipAddr, str, INET_ADDRSTRLEN);
      
      std::thread(&Tracker::join,&tracker,ConnectFD,str).detach();
      //cout<<str<<endl;
     bzero(buffer,256);
     
 
     /* perform read write operations ... */
 
      
    }

    
    close(SocketFD);
    return 0;
  }
