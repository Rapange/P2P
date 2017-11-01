#include "cpeer.h"

CPeer::CPeer(int query_port, int download_port, int keepAlive_port)
{
  m_query_port = query_port;
  m_download_port = download_port;
  m_keepAlive_port = keepAlive_port;
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

std::string CPeer::formatChunks(std::vector<int> chunks){
  string chunks_formatted;
  if(chunks.size() > 0){
    chunks_formatted += std::to_string(chunks[0]);
  }
  for(unsigned int i = 1; i < chunks.size(); i++){
    chunks_formatted += ',';
    chunks_formatted += chunks[i];
  }
  return chunks_formatted;
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
  int QuerySD = createServerSocket(m_query_port);
  int DownloadSD = createServerSocket(m_download_port);
  int KeepAliveSD = createServerSocket(m_keepAlive_port);

  std::thread(&CPeer::listenForClients,this,QuerySD,ACT_RCV_QUERY).detach();
  
  
}

void CPeer::listenForClients(int serverSD, char action)
{
  int ConnectFD = 0;
  while(true)
  {
    ConnectFD = accept(serverSD, NULL, NULL);

    if(0 > ConnectFD)
    {
       perror("error accept failed");
       close(serverSD);
       exit(EXIT_FAILURE);
    }
    if(action == ACT_RCV_QUERY)
      std::thread(&CPeer::opQueryS,this,ConnectFD).detach();
 }
}

void CPeer::iniClientBot()
{
  int QuerySD = createClientSocket(40000,"127.0.0.1");
  std::thread(&CPeer::opQuery,this,QuerySD, "hola").detach();

  int DownloadSD = createClientSocket(m_download_port,"127.0.0.1");
  /*std::thread(&CPeer::opRead,this,DownloadSD).detach();
    std::thread(&CPeer::opWrite,this,DownloadSD).detach();*/

  int KeepAliveSD = createClientSocket(m_keepAlive_port,"127.0.1.1");
  /*std::thread(&CPeer::opRead,this,KeepAliveSD).detach();
  std::thread(&CPeer::opWrite,this,KeepAliveSD).detach();*/

  while(true){}
}


void CPeer::opReadQuery(int clientSD, string file_name)
{
  char* buffer;
  int size_chunk_list, size_file_name;
  
  buffer = new char[5];
  read(clientSD, buffer, 4);
  buffer[4] = '\0';

  size_chunk_list = stoi(buffer);
  cout<<"Cliente lee tamanho de lista de chunks del servidor: "<<size_chunk_list<<endl;
  
  delete[] buffer;
  buffer = new char[1];

  read(clientSD, buffer, 1);
  if(buffer[0] == 'q'){
    delete[] buffer;
    buffer = new char[size_chunk_list+1];

    read(clientSD,buffer,size_chunk_list);
    buffer[size_chunk_list] = '\0';

    delete[] buffer;
    buffer = new char[4];
    read(clientSD,buffer,3);

    buffer[3] = '\0';
    size_file_name = stoi(buffer);

    delete[] buffer;
    buffer = new char[size_file_name+1];
    read(clientSD,buffer,size_file_name);

    buffer[size_file_name] = '\0';
    cout<<"El nombre del archivo que me ha enviado es: "<<buffer<<endl;
  }
  
  
  delete[] buffer;
  buffer = NULL;
}

void CPeer::opWriteQuery(int clientSD, string file_name)
{
  char* buffer;
  string protocol = intToStr(file_name.size(),3);
  protocol += ACT_SND_QUERY;
  protocol += file_name;

  buffer = new char[protocol.size()];
  protocol.copy(buffer,protocol.size(),0);

  cout<<"Cliente envia: "<<protocol<<endl;
  write(clientSD,buffer,protocol.size());

  delete[] buffer;
  buffer = NULL;
}

void CPeer::opQuery(int clientSD, string file_name)
{
  opWriteQuery(clientSD, file_name);
  opReadQuery(clientSD, file_name);
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

void CPeer::opQueryS(int clientSD)
{
  string file_name;
  while(true){
    file_name = opReadQueryS(clientSD);
    opWriteQueryS(clientSD, file_name);
  }
}

string CPeer::opReadQueryS(int clientSD){
  char* buffer;
  int size_file_name;
  buffer = new char[4];
  read(clientSD,buffer,3);
  buffer[3] = '\0';
   
  size_file_name = std::stoi(buffer);
  delete[] buffer;
  buffer = new char[2];

  read(clientSD, buffer, 1);
  buffer[1] = '\0';
  
  if(buffer[0] == ACT_SND_QUERY){
    delete[] buffer;
    buffer = new char[size_file_name+1];

    read(clientSD, buffer, size_file_name);
    buffer[size_file_name] = '\0';
    cout<<"Servidor lee nombre del archivo: "<<buffer<<endl;
  }

  return buffer;
  delete[] buffer;
  buffer = NULL;
}

void CPeer::opWriteQueryS(int clientSD, string file_name){
  string protocol;
  char *buffer;
  
  protocol = intToStr(m_num_chunks[file_name].size(),4);
  protocol += ACT_RCV_QUERY;
  protocol += formatChunks(m_num_chunks[file_name]);
  protocol += intToStr(file_name.size(),3);
  protocol += file_name;

  cout<<"Enviando protocolo del servidor: "<<protocol<<endl;
  buffer = new char[protocol.size()];
  protocol.copy(buffer,protocol.size(),0);
  write(clientSD,buffer,protocol.size());
}

CPeer::~CPeer()
{
    //dtor
}
