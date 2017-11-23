
#ifndef CPEER_H
#define CPEER_H

#include "red.h"
#define ACTION_SIZE 1
#define FILE_NAME_SIZE 3
#define CHUNK_LIST_SIZE 7
#define CHUNK_SIZE 1000
#define CHUNK_NUM_SIZE 5
#define IP_LIST_SIZE 3
#define MAX_NUM_CHUNK 10000
#define ACT_SND_JOIN 'J'
#define ACT_RCV_JOIN 'j'
#define ACT_SND_QUERY 'Q' //Receive query Action
#define ACT_RCV_QUERY 'q' //Send query Action
#define ACT_SND_KEEP 'K'
#define ACT_RCV_KEEP 'k'

#define ACT_SND_DWLD 'D'
#define ACT_RCV_DWLD 'd'
#define TRASH '`'


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

	std::string final_chunk;
        std::vector< std::string > lstPeersIp;
	std::vector< int > query_sockets,download_sockets;
	std::unordered_map <std::string, std::vector<std::string> > chunks;
        
        CPeer(int query_port, int download_port, int keepAlive_port, string file_name);
        virtual ~CPeer();
	
	bool isInIp(std::string ip);
	bool finished(std::string file_name);
        std::string intToStr(int num, int size);
	std::string formatChunks(std::string file_name);
	void fillIPs(string ip_list);
	std::vector<unsigned int> deformatChunks(std::string num_of_chunks);
	std::vector<unsigned int> askForChunks(std::vector<std::vector<unsigned int> >& chunks_per_peer, std::vector<std::string> &available_chunks);
	void uploadFile(std::string file_name);
	
        void iniServerBot();
	void listenForClients(int serverSD, char action);
	
        void iniClientBot(std::string file_name, std::string Ip_tracker);
	void iniProcess();
        int createClientSocket(int portNumber, string serverIP);
        int createServerSocket(int portNumber);
        
	void opReadJoin(int clientSD);
	void opWriteJoin(int clientSD);
	void opJoin(int clientSD);
	
	std::vector<unsigned int> opReadQuery(int clientSD, string file_name);
        void opWriteQuery(int clientSD, string file_name);
	void opQuery(int clientSD, string file_name);

	std::string opReadDownload(int clientSD);
        void opWriteDownload(int clientSD, string file_name, int num_chunk);
	void opDownload(int clientSD, string file_name, int num_chunk);

	void opReadKeep(int clientSD);
        void opWriteKeep(int clientSD);
	void opKeep(int clientSD);

	void opQueryS(int clientSD);
	string opReadQueryS(int clientSD);
	void opWriteQueryS(int clientSD, string file_name);

	void opDownloadS(int clientSD);
        unsigned int opReadDownloadS(int clientSD, std::string &file_name);
	void opWriteDownloadS(int clientSD, string file_name, string chunk);
};

#endif // CPEER_H
