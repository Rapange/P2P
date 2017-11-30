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
  vector<int> sockets;
  int m_keepal_port; 
public:
  string convert();
  int IPsconectadas = 0;
  vector<string> temp;
  Tracker(int keepal_port);
  void join(int c_socket, string IP);
  void keepalive();
  void QuitarElementos();
};

Tracker::Tracker(int keepal_port){
  m_keepal_port = keepal_port;
}

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
	 IPsconectadas++;
     //cout<<IP<<endl;
     
     shutdown(c_socket, SHUT_RDWR);
 
      close(c_socket);
}

void Tracker::QuitarElementos()
{
	vector<string> tmp;
	bool eliminar = false;
	//IPsconectadas = 0;
	for ( std::vector<string>::iterator it=IPs.begin(); it!=IPs.end(); it++)
	{
		eliminar = false;
		for ( std::vector<string>::iterator it2=temp.begin(); it2!=temp.end(); it2++)
		{
			if((*it2) == (*it));
			{
				eliminar = true;
				IPsconectadas--;
			}
		}
		if(!eliminar)
			tmp.push_back(*it);
		//cout<< "asdsadsd"<<endl;
		//cout<< *it<<endl;
	//	if(*it != str)
	//	{
	//		//cout<< "asdsd"<<endl;
	//		temp.push_back((*it).c_str());
	//		IPsconectadas++;
	//	}
	}
	IPs.clear();
	for ( std::vector<string>::iterator it=tmp.begin(); it!=tmp.end(); it++)
	{
		IPs.push_back(*it);
	}
	temp.clear();
	//if(IPsconectadas > 0)
	//{
		
	//}
	//else
	//	cout<<"ggg"<<IPsconectadas<<endl;
}

void Tracker::keepalive()
{
	for (std::vector<string>::iterator it=IPs.begin(); it!=IPs.end(); ++it)
	{
	  struct timeval timeout;
	  timeout.tv_sec = 3;
	  timeout.tv_usec = 0;
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
	  stSockAddr.sin_port = htons(m_keepal_port);
	  Res = inet_pton(AF_INET, (*it).c_str(), &stSockAddr.sin_addr);
	  printf("Connecting IP %s ... \n",(*it).c_str());
	   if (0 > Res)
	   {
	     perror("error: first parameter is not a valid address family");
	     close(SocketFD);
	     exit(EXIT_FAILURE);
	   }
	   else if (0 == Res)
	   {
	    perror("char string second parameter does not contain valid ipaddress");
	     close(SocketFD);
	     exit(EXIT_FAILURE);
	    }

	   int ll = connect(SocketFD, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in));
	   cout<<ll<<endl;
	   if (-1 == ll)
	   {
	     printf("IP not connected \n");
	     
	   }
	   else
	   {
					//printf("IP connected \n");
	     setsockopt(SocketFD,SOL_SOCKET,SO_RCVTIMEO, (char *)&timeout,sizeof(timeout));
	     setsockopt(SocketFD,SOL_SOCKET,SO_SNDTIMEO, (char *)&timeout,sizeof(timeout));
	     sockets.push_back(SocketFD);	
	   }
	}
	int my_socket;
	while(1)
	{
	  
	  for(unsigned int i = 0; i < sockets.size(); i++){
	    my_socket = sockets[i];
	    char* buffer;
	    string protocol = "009";//intToStr(file_name.size(),FILE_NAME_SIZE);
	    protocol += "K";
	    protocol += "keepalive";

	    buffer = new char[protocol.size()];
	    protocol.copy(buffer,protocol.size(),0);

	    cout<<"Envio keepalive: "<<protocol<<endl;
	    write(my_socket,buffer,protocol.size());
	    delete[] buffer;
					
	    buffer = new char[6 + 1];
	    if(read(my_socket,buffer,6) == 6){
	      buffer[6] = '\0';


	      delete[] buffer;
	      buffer = NULL;
	      cout<<"IP:" <<IPs[i]<<" conectada"<<endl;
	    }
	    else{
	      sockets.erase(sockets.begin()+i);
	      IPs.erase(IPs.begin()+i);
	    }
	  }
	  sleep(10);
	}
}




  int main(void)
  {

    int keepal_port;
    cout<<"Ingrese puerto de keeps";
    cin>>keepal_port;
    Tracker tracker(keepal_port);
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
	  
      std::thread(&Tracker::keepalive,&tracker).detach();
      //cout<<str<<endl;
     bzero(buffer,256);
     
 
     /* perform read write operations ... */
 
      
    }

    
    close(SocketFD);
    return 0;
  }

