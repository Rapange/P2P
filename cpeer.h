#ifndef CPEER_H
#define CPEER_H

#include "red.h"
#define ACT_SND_QUERY 'Q' //Receive query Action
#define ACT_RCV_QUERY 'q' //Send query Action

class CPeer
{
    public:
        //Size of the types of messages
        const unsigned int HEADER_SIZE = 1; //[action]..
        const unsigned int QUERY_SIZE = 3;  //[size][of][query]..
        //Posible actions
        //char ACT_SND_QUERY= 'Q'; //Send query  Action
	//char ACT_RCV_QUERY= 'q'; //Receive query Action
	
	int m_query_port;
	int m_download_port;
	int m_keepAlive_port;

        std::vector< std::string > lstPeersIp;
	std::map <std::string, std::vector<int> > m_num_chunks;
        
        CPeer(int query_port, int download_port, int keepAlive_port);
        virtual ~CPeer();

        std::string intToStr(int num, int size);
	std::string formatChunks(std::vector<int> chunks);
	  
        void iniServerBot();
	void listenForClients(int serverSD, char action);
	
        void iniClientBot();
        int createClientSocket(int portNumber, string serverIP);
        int createServerSocket(int portNumber);
        

        void opReadQuery(int clientSD, string file_name);
        void opWriteQuery(int clientSD, string file_name);
	void opQuery(int clientSD, string file_name);
        void opReadDownload(int clientSD);
        void opWriteDownload(int clientSD);
        void opReadKeep(int clientSD);
        void opWriteKeep(int clientSD);

	void opQueryS(int clientSD);
	string opReadQueryS(int clientSD);
	void opWriteQueryS(int clientSD, string file_name);
};

#endif // CPEER_H
