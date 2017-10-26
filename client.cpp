 /* Client code in C */
 
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <unistd.h>
#include <iostream>
#define N_BYTES_TAM 3

class Peer{

  int id;
public:
  void q_protocol();
  void d_protocol();
  void k_protocol();
  
}

using namespace std;

  int main(void)
  {
    struct sockaddr_in stSockAddr;
    int Res;
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    int n;
 
    if (-1 == SocketFD)
    {
      perror("cannot create socket");
      exit(EXIT_FAILURE);
    }
 
    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
 
    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(1100);
    Res = inet_pton(AF_INET, "127.0.0.1", &stSockAddr.sin_addr);
 
    if (0 > Res)
    {
      perror("error: first parameter is not a valid address family");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }
    else if (0 == Res)
    {
      perror("char string (second parameter does not contain valid ipaddress");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }
 
    if (-1 == connect(SocketFD, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
    {
      perror("connect failed");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }
    n = write(SocketFD,"J\0",2);

    char* j_p;
    j_p = new char[N_BYTES_TAM+1];
    string j_p_str;
    int bytes_to_read;
    n = read(SocketFD,j_p,N_BYTES_TAM);
    j_p[N_BYTES_TAM] = '\0';
    j_p_str = j_p;
    cout<<j_p_str<<endl;
    bytes_to_read = stoi(j_p_str);

    delete[] j_p;
    j_p = new char[2];
    
    n = read(SocketFD,j_p,1);
    j_p[1] = '\0';
    j_p_str = j_p;
    if(j_p_str[0] == 'j'){
      delete[] j_p;
      j_p = new char[bytes_to_read + 1];
      n = read(SocketFD,j_p,bytes_to_read + 1);
      j_p[bytes_to_read] = '\0';
      j_p_str = j_p;
      cout<<j_p_str<<endl;
    }
    
    
    /* perform read write operations ... */
 
    shutdown(SocketFD, SHUT_RDWR);
 
    close(SocketFD);
    return 0;
  }
