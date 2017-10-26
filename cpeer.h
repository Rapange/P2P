#ifndef CPEER_H
#define CPEER_H

#include "red.h"
class CPeer
{
    public:
        CPeer();
        virtual ~CPeer();

    protected:

    private:
        //Size of the types of messages
        const unsigned int HEADER_SIZE = 1; //[action]..
        const unsigned int QUERY_SIZE = 3;  //[size][of][query]..
        std::vector< std::string > lstPeersIp;
        std::string intToStr(int num, int size);
        void iniServerBot();
        void iniClientBot();
        int createClientSocket();
        int createServerSocket();
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
