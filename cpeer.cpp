#include "cpeer.h"

CPeer::CPeer()
{
    //ctor
}

//Transforms an int to a sized string e.g., intToStr(21,4) => 0021, intToStr(9,3) => 009
std::string CPeer::intToStr(int num, int size){
    std::string result;
    for(int i= 0; i < size; i++)
      result+='0';
    for(int i = size - 1; i >= 0; --i){
      result[i] = std::to_string(num%10)[0];
      num = num/10;
    }
    return result;
}

int CPeer::createServerSocket(int portNumber)
{
    vector<unsigned int> holder;
    struct sockaddr_in stSockAddr;
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    int ConnectFD = 0;

    if(-1 == SocketFD)
    {
      perror("can not create socket");
      exit(EXIT_FAILURE);
    }

    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(portNumber);
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

    return SocketFD;

    /*while(true)
    {
      ConnectFD = accept(SocketFD, NULL, NULL);

      if(0 > ConnectFD)
      {
        perror("error accept failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
      }
    }

    close(SocketFD);
    return 0;*/
}

int CPeer::createClientSocket(int portNumber,std::string serverIP)
{
  struct sockaddr_in stSockAddr;
  int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  int n, Res;

  if (-1 == SocketFD)
  {
    perror("cannot create socket");
    exit(EXIT_FAILURE);
  }

  memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
  stSockAddr.sin_family = AF_INET;
  stSockAddr.sin_port = htons(portNumber);

  Res = inet_pton(AF_INET, serverIP, &stSockAddr.sin_addr);

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
  return SocketFD;
}

void CPeer::iniServerBot()
{

}

void CPeer::iniClientBot()
{
  int QuerySD = createSocket(40000,"192.168.1.4");
  std::thread(opRead,QuerySD).detach();
  std::thread(opWrite,QuerySD).detach();

  int DownloadSD = createSocket(40001,"192.168.1.4");
  std::thread(opRead,DownloadSD).detach();
  std::thread(opWrite,DownloadSD).detach();

  int KeepAliveSD = createSocket(40002,"192.168.1.4");
  std::thread(opRead,KeepAliveSD).detach();
  std::thread(opWrite,KeepAliveSD).detach();

  while(true){}
  return 0;
}

void CPeer::opRead(int clientSD)
{

}

void CPeer::opWrite(int clientSD)
{
    unsigned int n;
    std::string msgSend;
    //Communication between client and server
    while(true){
      //Capture the action of the player
      cin>>msgSend;

      //If the player wants to move
      if(msgSend == "q"){
        //Get the query that the client wants to search
        std::string msgChat;
        cin.ignore();
        getline(cin,msgChat);

        //Build of the protocol
        msgSend = ACT_SND_QUERY + intToStr(msgChat.size(),3) + msgChat;
        cout << "Sending Protocol :"<< msgSend << endl;

        //Sending the protocol to the server
        n = write(clientSD, msgSend.data(), HEADER_SIZE + QUERY_SIZE + msgChat.size());
        if (n < 0) perror("ERROR writing to socket");
      }
    }

    //Close the connection when the player leaves
    shutdown(clientSD, SHUT_RDWR);
    //Free the resources it uses
    close(clientSD);
}

void CPeer::opReadQuery(int clientSD)
{
}

void CPeer::opWriteQuery(int clientSD)
{

}

void CPeer::opReadDownload(int clientSD)
{

}

void CPeer::opWriteDownload(int clientSD)
{

}

void CPeer::opReadKeep(int clientSD)
{

}

void CPeer::opWriteKeep(int clientSD)
{

}

CPeer::~CPeer()
{
    //dtor
}
