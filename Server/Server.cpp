
#pragma comment(lib, "ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <WinSock2.h>
#include <thread>
#include <map>
#include <fstream>

const int NumberOfConnections = 100;
const int sizeOfBuffer = 36;
SOCKET Connections[NumberOfConnections];
SOCKET Port8001Connections[NumberOfConnections];


int main()
{
   
    WSAData wsaData;
   
    WORD DLLVersion = MAKEWORD(2, 1);
    if (WSAStartup(DLLVersion, &wsaData) != 0)
    {
       std::cout << "Error: " << WSAGetLastError() << std::endl;
       return 1;
    }

    std::map<std::wstring, std::wstring> IdentCodeMap;

   
    std::thread t1([&]()
        {
   

            SOCKADDR_IN addr_1;
            addr_1.sin_addr.s_addr = inet_addr("127.0.0.1");
            addr_1.sin_port = htons(8000);
            addr_1.sin_family = AF_INET;
           
            SOCKET port8000 = socket(AF_INET, SOCK_STREAM, NULL);
            bind(port8000, (SOCKADDR*)&addr_1, sizeof(addr_1));
            listen(port8000, SOMAXCONN);
            int sizeofaddr_1 = sizeof(addr_1);
            SOCKET newConnection;

            for (int i = 0; i < NumberOfConnections; i++)
            {
                newConnection = accept(port8000, (SOCKADDR*)&addr_1, &sizeofaddr_1);

                if (newConnection == 0)
                {
                    std::cout << "Connection failed" << std::endl;
                }
                else
                {
                    char IdentifierMessage[sizeOfBuffer];
                    recv(newConnection, IdentifierMessage, sizeOfBuffer, NULL);
                    std::cout << "Client ";
                    for (int j = 0; j < sizeOfBuffer; j++)
                    {
                        std::cout << IdentifierMessage[j];
                    }
                    std::cout << " has connected." << std::endl;
                    
                    GUID guidCode;
                    HRESULT hCreateGuid = CoCreateGuid(&guidCode);
                    wchar_t wCode[64] = { 0 };
                    StringFromGUID2(guidCode, wCode, 64);
                    std::wstring Code(wCode);
                    std::wcout << "Code of this client: " << Code << std::endl;
                    
                    char CodeMessage[sizeOfBuffer];
                    
                    for (int j = 1; j < Code.size() - 1; j++)
                    {
                        CodeMessage[j - 1] = Code[j];
                    }
                    send(newConnection, CodeMessage, sizeOfBuffer, NULL);
                 
                    IdentCodeMap.emplace(
                        std::wstring(IdentifierMessage[0], IdentifierMessage[sizeOfBuffer-1]), 
                        std::wstring(CodeMessage[0], CodeMessage[sizeOfBuffer - 1]));
                    Connections[i] = newConnection;
                 
                }

            }
           
        });

    std::thread t2([&]()
        {
            

            SOCKADDR_IN addr_2;
            addr_2.sin_addr.s_addr = inet_addr("127.0.0.1");
            addr_2.sin_port = htons(8001);
            addr_2.sin_family = AF_INET;
           
            SOCKET port8001 = socket(AF_INET, SOCK_STREAM, NULL);
            bind(port8001, (SOCKADDR*)&addr_2, sizeof(addr_2));
            listen(port8001, SOMAXCONN);
            int sizeofaddr_2 = sizeof(addr_2);
            SOCKET newConnection;

            for (int i = 0; i < NumberOfConnections; i++)
            {
                newConnection = accept(port8001, (SOCKADDR*)&addr_2, &sizeofaddr_2);

                if (newConnection == 0)
                {
                    std::cout << "Connection failed" << std::endl;
                }
                else
                {
                    std::cout << "Connection to port 8001" << std::endl;
                    char IdentifierMessage[sizeOfBuffer];
                    char CodeMessage[sizeOfBuffer];
                    recv(newConnection, IdentifierMessage, sizeOfBuffer, NULL);
                    std::cout << "Client ";
                    for (int j = 0; j < sizeOfBuffer; j++)
                    {
                        std::cout << IdentifierMessage[j];
                    }
                    std::cout << " has connected." << std::endl;
                    recv(newConnection, CodeMessage, sizeOfBuffer, NULL);
                    std::cout << "Code of the client is ";
                    for (int j = 0; j < sizeOfBuffer; j++)
                    {
                        std::cout << CodeMessage[j];
                    }
                    std::cout <<std::endl;
                   
                    std::wstring firstKey = std::wstring(IdentifierMessage[0], IdentifierMessage[sizeOfBuffer - 1]);
                    std::wstring secondKey = std::wstring(CodeMessage[0], CodeMessage[sizeOfBuffer - 1]);
                   
                    
                    if (IdentCodeMap.find(firstKey)->second==secondKey)
                    {
                        char Answer[] = "ok";
                        send(newConnection, Answer, 2, NULL);
                        char* MessageToSend = new char[4096];
                        recv(newConnection, MessageToSend, 4096, NULL);
                        int j = 0;
                        while(MessageToSend[j] != '\0')
                        {
                            std::cout << MessageToSend[j];
                            j++;
                        }
                        std::ofstream log_out;                    
                        log_out.open("logfile.txt", std::ios::app);
                        log_out << "Client ";
                        for (int j = 0; j < sizeOfBuffer; j++)
                        {
                            log_out << IdentifierMessage[j];
                            }

                        log_out << " has sent: ";
                        j = 0;
                        while (MessageToSend[j] != '\0')
                        {
                            log_out << MessageToSend[j];
                            j++;
                        }
                            
                        log_out << "\n";
                        log_out.close();
                   }
                    else
                    {
                        char Answer[] = "no";
                        send(newConnection, Answer, 2, NULL);
                    }
                    
                    
                 

                    Port8001Connections[i] = newConnection;
                    
                }

            }

        });

    t1.join();
    t2.join();
    system("pause");
    return 0;
}


