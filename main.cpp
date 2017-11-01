#include <iostream>
#include "cpeer.h"
using namespace std;

int main()
{
  int query_port, download_port, keepAlive_port;
  cout<<"Ingrese puerto para Query: ";
  cin>>query_port;
  cout<<"Ingrese puerto para Download: ";
  cin>>download_port;
  cout<<"Ingrese puerto para KeepAlive: ";
  cin>>keepAlive_port;
  CPeer *newPeer = new CPeer(query_port, download_port, keepAlive_port);
    newPeer->iniServerBot();
    newPeer->iniClientBot();
    cout << "Hello world!" << endl;
    return 0;
}
