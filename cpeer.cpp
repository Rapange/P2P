#include "cpeer.h"

CPeer::CPeer(int query_port, int download_port, int keepAlive_port, string file_name)
{
  std::vector<std::string> empty_chunks(MAX_NUM_CHUNK);
  m_query_port = query_port;
  m_download_port = download_port;
  m_keepAlive_port = keepAlive_port;
  final_chunk = std::string(CHUNK_SIZE,TRASH);
  cout<<final_chunk<<endl;
  if(file_name != "-1"){
    chunks[file_name] = empty_chunks; //Fill with real chunks
    uploadFile(file_name);
  }
    //ctor
}

bool CPeer::isInIp(std::string ip)
{
  for(unsigned int i = 0; i < lstPeersIp.size();i++){
    if(lstPeersIp[i] == ip) return true;
  }
  return false;
}

bool CPeer::finished(std::string file_name)
{

  for(unsigned int i = 1; i < chunks[file_name].size(); i++){
    cout<<chunks[file_name][i]<<" ";
    if(chunks[file_name][i] == final_chunk){
      return true;
    }
    else if(chunks[file_name][i] == ""){
      return false;
    }
  }
  return false;
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

std::string CPeer::formatChunks(string file_name){
  string chunks_formatted;
  std::vector<unsigned int> num_of_chunks;

  for(unsigned int i = 1; i < chunks[file_name].size(); i++){
    if(chunks[file_name][i] != "")
      num_of_chunks.push_back(i);
    if(chunks[file_name][i] == final_chunk) break;
  }
  
  if(num_of_chunks.size() > 0){
    chunks_formatted += std::to_string(num_of_chunks[0]);
  }
  for(unsigned int i = 1; i < num_of_chunks.size(); i++){
    chunks_formatted += ',';
    chunks_formatted += std::to_string(num_of_chunks[i]);
  }
  return chunks_formatted;
}

void CPeer::fillIPs(string ip_list){
  string ip;

  for(unsigned int i = 0; i < ip_list.size(); i++){
    if(ip_list[i] != ','){
      ip += ip_list[i];
    }
    if(ip_list[i] == ',' || i == ip_list.size()-1){
      lstPeersIp.push_back(ip);
      ip = "";
    }
  }

  return;
}

std::vector<unsigned int> CPeer::deformatChunks(std::string num_of_chunks)
{
  std::vector<unsigned int> chunk_nums;
  std::string chunk_num;
  for(unsigned int i = 0; i < num_of_chunks.size(); i++)
  {
    if(num_of_chunks[i] == ',')
    {
      chunk_nums.push_back(stoi(chunk_num));
      chunk_num = "";
    }
    else
      chunk_num += num_of_chunks[i];
  }
  if(chunk_num.size() > 0)
    chunk_nums.push_back(stoi(chunk_num));
  return chunk_nums;
}

std::vector<unsigned int> CPeer::askForChunks(std::vector<std::vector<unsigned int> > &chunks_per_peer, std::vector<std::string> &available_chunks)
{
  std::vector<unsigned int> petition_for_peers(chunks_per_peer.size(),0);
  bool chunks_taken[MAX_NUM_CHUNK] = {0}; 
  bool is_useful = false;
  unsigned int chunk_to_ask;
  for(unsigned int i = 0; i < chunks_per_peer.size(); i++){
    is_useful = false;
    for(unsigned int j = 0; j < chunks_per_peer[i].size(); j++){
      chunk_to_ask = chunks_per_peer[i][j];
      if(available_chunks[chunk_to_ask] == "" && !chunks_taken[chunk_to_ask]){
	is_useful = true;
	chunks_taken[chunk_to_ask] = true;
	cout<<"I ask for: "<<chunk_to_ask<<endl;
	break;
      }
    }
    if(is_useful){
      petition_for_peers[i] = chunk_to_ask;
    }
  }

  return petition_for_peers;
}

void CPeer::uploadFile(std::string file_name)
{
  char buffer[CHUNK_SIZE + 1];
  ifstream file(file_name, std::ios::binary);
  unsigned int i = 1;
  do{
    file.read(buffer,CHUNK_SIZE);
    buffer[CHUNK_SIZE] = '\0';
    chunks[file_name][i] = buffer;
    cout<<i<<" "<<buffer<<endl;
    i++;
    bzero(buffer, CHUNK_SIZE+1);
  }while(file.gcount() == CHUNK_SIZE);
  chunks[file_name][i-1] = std::string(CHUNK_SIZE - chunks[file_name][i-1].size(), TRASH) + chunks[file_name][i-1];

  cout<<chunks[file_name][i-1]<<endl;
  
  chunks[file_name][i] = final_chunk;
  file.close();

  cout<<"File uploaded"<<endl;
  cout<<chunks[file_name].size()<<endl;
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
  std::thread(&CPeer::listenForClients,this,KeepAliveSD,ACT_RCV_KEEP).detach();
  std::thread(&CPeer::listenForClients,this,DownloadSD,ACT_RCV_DWLD).detach();
  
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
  	if(action == ACT_RCV_KEEP)
  	  std::thread(&CPeer::opKeep,this,ConnectFD).detach();
    if(action == ACT_RCV_DWLD)
      std::thread(&CPeer::opDownloadS,this,ConnectFD).detach();

 }
}

void CPeer::iniClientBot(std::string file_name, std::string Ip_tracker)
{
  std::vector<string> empty_chunks(MAX_NUM_CHUNK); //Solo si no sube archivo, inicializacion.
  
  
  int JoinSD = createClientSocket(1100, Ip_tracker);
  int dummy;
  //std::thread(&CPeer::opJoin,this,JoinSD).detach();
  opJoin(JoinSD);
  cout<<lstPeersIp.size()<<endl;
  
  int KeepAliveSD = createClientSocket(m_keepAlive_port,Ip_tracker);
  //std::thread(&CPeer::opKeep,this,KeepAliveSD).detach();

  if(chunks[file_name].empty()){
    chunks[file_name] = empty_chunks;
    
  }

  else
    while(true);
  
  std::vector<std::vector<unsigned int> > available_chunks;
  std::vector<unsigned int> chunks_to_ask_for;
  int QuerySD, DownloadSD;
  std::string received_chunk;
  for(unsigned int i = 0; i < lstPeersIp.size(); i++)
  {
    QuerySD = createClientSocket(m_query_port,lstPeersIp[i]);
    query_sockets.push_back(QuerySD);
  }

  for(unsigned int i = 0; i < lstPeersIp.size(); i++){
    DownloadSD = createClientSocket(m_download_port,lstPeersIp[i]);
    download_sockets.push_back(DownloadSD);
  }

  cout<<"before"<<endl;
  while(!finished(file_name)){
    //cout<<"h"<<endl;
    cout<<"I have the following chunks: "<<endl;
    for(unsigned int i = 1; i < 7; i++){
      cout<<i<<" "<<chunks[file_name][i]<<endl;
    }
    available_chunks.clear();
    chunks_to_ask_for.clear();
    for(unsigned int i = 0; i < query_sockets.size(); i++){
      std::thread(&CPeer::opWriteQuery,this,query_sockets[i], file_name).detach();
    }
  
    for(unsigned int i = 0; i < query_sockets.size(); i++)
      {
	available_chunks.push_back(opReadQuery(query_sockets[i],file_name));
      }

    cout<<"finish available chunks"<<endl;
    for(unsigned int i = 0; i < available_chunks.size(); i++){
      for(unsigned int j = 0; j < available_chunks[i].size(); j++){
	cout<<available_chunks[i][j]<<" ";
      }
      cout<<endl;
    }
    cout<<endl;
    //cin>>dummy;

    chunks_to_ask_for = askForChunks(available_chunks,chunks[file_name]);




    cout<<"finished chunks to ask for "<<endl;
    for(unsigned int i = 0; i < chunks_to_ask_for.size(); i++){
      cout<<chunks_to_ask_for[i]<<" ";
    }
    cout<<endl;
    //cin>>dummy;

    for(unsigned int i = 0; i < download_sockets.size(); i++){
      if(chunks_to_ask_for[i] != 0){
	std::thread(&CPeer::opWriteDownload,this,download_sockets[i],file_name,chunks_to_ask_for[i]).detach();
      }
    }

    for(unsigned int i = 0; i < download_sockets.size(); i++){
      if(chunks_to_ask_for[i] != 0){
	received_chunk = opReadDownload(download_sockets[i]);
	if(received_chunk.size() == CHUNK_SIZE)
	  chunks[file_name][chunks_to_ask_for[i]] = received_chunk;
      }
    }


  }
  

  ofstream file("holaout", std::ios::binary);
  std::string my_chunk;
  char buffer[CHUNK_SIZE+1];
  for(unsigned int i = 1; i < chunks[file_name].size(); i++){
    my_chunk = chunks[file_name][i];
    if(my_chunk == final_chunk) break;
    if(my_chunk[0] == TRASH){
      while(my_chunk[0] == TRASH) my_chunk.erase(0,1);
    }
    my_chunk.copy(buffer,my_chunk.size(),0);
    buffer[my_chunk.size()] = '\0';
    
    file.write(buffer, my_chunk.size());
  }
  file.close();




  while(true);
}

void CPeer::iniProcess()
{
  
}

void CPeer::opReadJoin(int clientSD){
  char* buffer;
  int ip_list_size;
  buffer = new char[IP_LIST_SIZE + 1];

  read(clientSD,buffer,IP_LIST_SIZE);
  buffer[IP_LIST_SIZE] = '\0';

  ip_list_size = stoi(buffer);

  delete[] buffer;
  buffer = new char[2];

  read(clientSD,buffer,1);
  buffer[1] = '\0';

  if(buffer[0] == ACT_RCV_JOIN){
    delete[] buffer;
    buffer = new char[ip_list_size + 1];

    read(clientSD, buffer, ip_list_size);
    
    buffer[ip_list_size] = '\0';

    cout<<"IPs leidas del tracker: "<<buffer<<endl;
    fillIPs(buffer);
 
  }

  shutdown(clientSD,SHUT_RDWR);
  close(clientSD);
}

void CPeer::opWriteJoin(int clientSD){
  char* buffer;
  buffer = new char[1];
  buffer[0] = ACT_SND_JOIN;
  write(clientSD, buffer, 1);
  
}

void CPeer::opJoin(int clientSD){
  cout<<"inijoin"<<endl;
  opWriteJoin(clientSD);
  opReadJoin(clientSD);
  
}

std::vector<unsigned int> CPeer::opReadQuery(int clientSD, string file_name)
{
  char* buffer;
  int size_chunk_list, size_file_name;
  std::vector<unsigned int> peer_num_chunks;
  
  buffer = new char[CHUNK_LIST_SIZE + 1];
  read(clientSD, buffer, CHUNK_LIST_SIZE);
  buffer[CHUNK_LIST_SIZE] = '\0';

  size_chunk_list = stoi(buffer);
  cout<<"Cliente lee tamanho de lista de chunks del servidor: "<<size_chunk_list<<endl;
  
  delete[] buffer;
  buffer = new char[ACTION_SIZE];

  read(clientSD, buffer, ACTION_SIZE);
  if(buffer[0] == 'q'){
    delete[] buffer;
    buffer = new char[size_chunk_list+1];

    read(clientSD,buffer,size_chunk_list);
    buffer[size_chunk_list] = '\0';

    peer_num_chunks = deformatChunks(buffer);
    cout<<"deformatted"<<endl;
    delete[] buffer;
    buffer = new char[FILE_NAME_SIZE + 1];
    read(clientSD,buffer,FILE_NAME_SIZE);

    buffer[FILE_NAME_SIZE] = '\0';
    size_file_name = stoi(buffer);

    delete[] buffer;
    buffer = new char[size_file_name+1];
    read(clientSD,buffer,size_file_name);

    buffer[size_file_name] = '\0';
    cout<<"El nombre del archivo que me ha enviado es: "<<buffer<<endl;
  }
  
  
  delete[] buffer;
  buffer = NULL;

  return peer_num_chunks;
}

void CPeer::opWriteQuery(int clientSD, string file_name)
{
  char* buffer;
  string protocol = intToStr(file_name.size(),FILE_NAME_SIZE);
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

 std::string CPeer::opReadDownload(int clientSD)
{
  char* buffer;
  int size_file_name;
  string my_chunk;
  buffer = new char[FILE_NAME_SIZE + 1];

  read(clientSD, buffer, FILE_NAME_SIZE);
  buffer[FILE_NAME_SIZE] = '\0';

  size_file_name = std::stoi(buffer);
  delete[] buffer;
  buffer = new char[ACTION_SIZE + 1];

  read(clientSD, buffer, ACTION_SIZE);
  buffer[ACTION_SIZE] = '\0';

  delete[] buffer;
  
  buffer = new char[size_file_name + 1];
  read(clientSD, buffer, size_file_name);

  buffer[size_file_name] = '\0';

  delete[] buffer;
  buffer = new char[CHUNK_SIZE + 1];
  read(clientSD, buffer, CHUNK_SIZE);

  buffer[CHUNK_SIZE] = '\0';
  my_chunk = buffer;

  delete[] buffer;
  buffer = NULL;

  cout<<my_chunk<<endl;
  return my_chunk;
}

void CPeer::opWriteDownload(int clientSD, string file_name, int num_chunk)
{
  string protocol;
  char* buffer;
  protocol = intToStr(file_name.size(), FILE_NAME_SIZE);
  protocol += ACT_SND_DWLD;
  protocol += file_name;
  protocol += intToStr(num_chunk,CHUNK_NUM_SIZE);
  buffer = new char[protocol.size()];

  protocol.copy(buffer,protocol.size(),0);

  write(clientSD, buffer, protocol.size());

  cout<<"protocolo de download "<<protocol<<endl;
  

}

void CPeer::opDownload(int clientSD, string file_name, int num_chunk)
{
  opWriteDownload(clientSD, file_name, num_chunk);
  opReadDownload(clientSD);
}

void CPeer::opKeep(int clientSD)
{
	opReadKeep(clientSD);
	opWriteKeep(clientSD);
}

void CPeer::opReadKeep(int clientSD)
{
	char* buffer = new char[13];
    read(clientSD,buffer,13);
    cout<<buffer<<endl;
	delete[] buffer;
}

void CPeer::opWriteKeep(int clientSD)
{
	char* buffer;
	string protocol = "002";//intToStr(file_name.size(),FILE_NAME_SIZE);
	protocol += "k";
	protocol += "Ok";
	
	buffer = new char[protocol.size()];
	protocol.copy(buffer,protocol.size(),0);
	
	cout<<"Envio keepalive: "<<protocol<<endl;
	write(clientSD,buffer,protocol.size());
	
	delete[] buffer;
	buffer = NULL;
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
  string ip;
  struct sockaddr_in client_addr;
  socklen_t addr_size = sizeof(struct sockaddr_in);
  int res = getpeername(clientSD, (struct sockaddr *) &client_addr, &addr_size);
  ip = inet_ntoa(client_addr.sin_addr);
  cout<<"your IP is: "<<ip<<endl;

  if(!isInIp(ip)){
    cout<<"open query"<<endl;
    lstPeersIp.push_back(ip);
    query_sockets.push_back(createClientSocket(m_query_port,ip));
    download_sockets.push_back(createClientSocket(m_download_port,ip));
  }
  
  int size_file_name;
  buffer = new char[FILE_NAME_SIZE + 1];
  read(clientSD,buffer,FILE_NAME_SIZE);
  buffer[FILE_NAME_SIZE] = '\0';
   
  size_file_name = std::stoi(buffer);
  delete[] buffer;
  buffer = new char[ACTION_SIZE + 1];

  read(clientSD, buffer, ACTION_SIZE);
  buffer[ACTION_SIZE] = '\0';
  
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
  string protocol, formatted_chunks = formatChunks(file_name);
  char *buffer;
  cout<<formatted_chunks<<endl;
  
  protocol = intToStr(formatted_chunks.size(),CHUNK_LIST_SIZE);
  protocol += ACT_RCV_QUERY;
  protocol += formatted_chunks;
  protocol += intToStr(file_name.size(),FILE_NAME_SIZE);
  protocol += file_name;

  cout<<"Enviando protocolo del servidor: "<<protocol<<endl;
  buffer = new char[protocol.size()];
  protocol.copy(buffer,protocol.size(),0);
  write(clientSD,buffer,protocol.size());
}

void CPeer::opDownloadS(int clientSD)
{
  std::string file_name;
  unsigned int my_chunk_num;
  while(true){
    my_chunk_num = opReadDownloadS(clientSD,file_name);
    opWriteDownloadS(clientSD, file_name, chunks[file_name][my_chunk_num]);
  }
}

unsigned int CPeer::opReadDownloadS(int clientSD, std::string &file_name)
{
  char* buffer;
  unsigned int my_chunk_num, size_file_name;
  buffer = new char[FILE_NAME_SIZE+1];
  cout<<"Esperando lectura:"<<endl;
  read(clientSD, buffer, FILE_NAME_SIZE);

  cout<<"Ya lei"<<endl;
  buffer[FILE_NAME_SIZE] = '\0';

  cout<<buffer<<endl;
  size_file_name = stoi(buffer);

  cout<<"tam file_name desde server "<<size_file_name<<endl;
  
  delete[] buffer;
  buffer = new char[ACTION_SIZE+1];
  read(clientSD, buffer, ACTION_SIZE);

  buffer[ACTION_SIZE] = '\0';
  delete[] buffer;

  buffer = new char[size_file_name + 1];
  read(clientSD, buffer, size_file_name);
  buffer[size_file_name] = '\0';

  file_name = buffer;
  
  delete[] buffer;

  buffer = new char[CHUNK_NUM_SIZE + 1];
  read(clientSD, buffer, CHUNK_NUM_SIZE);
  buffer[CHUNK_NUM_SIZE] = '\0';
  my_chunk_num = std::stoi(buffer);
  cout<<"Extracted num chunk from server: "<<my_chunk_num<<endl;

  delete[] buffer;
  buffer = NULL;
  return my_chunk_num;
}

void CPeer::opWriteDownloadS(int clientSD, string file_name, string chunk)
{
  
  string protocol;
  char* buffer;
  protocol = intToStr(file_name.size(), FILE_NAME_SIZE);
  protocol += ACT_RCV_DWLD;
  protocol += file_name;
  protocol += chunk;

  buffer = new char[protocol.size()];
  protocol.copy(buffer,protocol.size(),0);
  write(clientSD, buffer, protocol.size());
}

CPeer::~CPeer()
{
    //dtor
}

