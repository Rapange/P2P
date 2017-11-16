#include <iostream>
#include "cpeer.h"
using namespace std;

int main()
{
  int query_port, download_port, keepAlive_port;
  string file_to_upload, file_to_download;
  cout<<"Ingrese puerto para Query: ";
  cin>>query_port;
  cout<<"Ingrese puerto para Download: ";
  cin>>download_port;
  cout<<"Ingrese puerto para KeepAlive: ";
  cin>>keepAlive_port;
  cout<<"Ingrese el nombre del archivo a subir: ";
  cin>>file_to_upload;
  cout<<"Ingrese el nombre del archivo a descargar: ";
  cin>>file_to_download;
  CPeer *newPeer = new CPeer(query_port, download_port, keepAlive_port, file_to_upload);
    newPeer->iniServerBot();
    newPeer->iniClientBot(file_to_download);
    cout << "Hello world!" << endl;
    return 0;
}
