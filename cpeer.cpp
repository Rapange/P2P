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

  Res = inet_pton(AF_INET, serverIP.data(), &stSockAddr.sin_addr);

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
  int QuerySD = createClientSocket(40000,"192.168.1.4");
  std::thread(&CPeer::opRead,this,QuerySD).detach();
  std::thread(&CPeer::opWrite,this,QuerySD).detach();

  int DownloadSD = createClientSocket(40001,"192.168.1.4");
  std::thread(&CPeer::opRead,this,DownloadSD).detach();
  std::thread(&CPeer::opWrite,this,DownloadSD).detach();

  int KeepAliveSD = createClientSocket(40002,"192.168.1.4");
  std::thread(&CPeer::opRead,this,KeepAliveSD).detach();
  std::thread(&CPeer::opWrite,this,KeepAliveSD).detach();

  while(true){}
}

void CPeer::opRead(int clientSD)
{
    string x,y,ptcPlayer,ptcAction;
    char *protocol;
    int dynMessageSize;
    string dySizeStr;
    int n;
    protocol = new char[HEADER_SIZE];

    //Reading the next messages sent by the server
    while(true){
      n = read(clientSD,protocol,HEADER_SIZE);
      if (n < 0) perror("ERROR reading from socket");
      //Which action is going to do
      ptcAction = protocol[1];
      printf("Reading protocol: %s", protocol);

      //Verifying which action is going to take place
      if (ptcAction == ACT_SND_QUERY) {
        dynMessageSize= QUERY_SIZE;
        //Retrieve the message
        delete[] protocol;
        protocol = new char[dynMessageSize];
        n = read(clientSD, protocol, dynMessageSize);
        if (n < 0) perror("ERROR writing to socket");
        printf("%s\n", protocol);

        dySizeStr =  protocol[0];
        dySizeStr += protocol[1];
        dySizeStr += protocol[2];
        dynMessageSize =  atoi(dySizeStr.c_str());

        delete[] protocol;
        protocol = new char[dynMessageSize];

         //Retrieve the message
        n = read(clientSD,protocol,dynMessageSize);
        if (n < 0) perror("ERROR writing to socket");
        printf("%s\n", protocol);

      }
    shutdown(clientSD, SHUT_RDWR);
    close(clientSD);
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
