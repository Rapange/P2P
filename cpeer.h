#ifndef CPEER_H
#define CPEER_H

#include "red.h"
class CPeer
{
    public:
        //Size of the types of messages
        const unsigned int HEADER_SIZE = 1; //[action]..
        const unsigned int QUERY_SIZE = 3;  //[size][of][query]..
        //Posible actions
        const string ACT_SND_QUERY= "Q"; //Move  Action

        std::vector< std::string > lstPeersIp;
        CPeer();
        virtual ~CPeer();

        std::string intToStr(int num, int size);
        void iniServerBot();
        void iniClientBot();
        int createClientSocket(int portNumber, string serverIP);
        int createServerSocket(int portNumber);
        void opRead(int clientSD);
        void opWrite(int clientSD);

        void opReadQuery(int clientSD);
        void opWriteQuery(int clientSD);
        void opReadDownload(int clientSD);
        void opWriteDownload(int clientSD);
        void opReadKeep(int clientSD);
        void opWriteKeep(int clientSD);
};

#endif // CPEER_H
