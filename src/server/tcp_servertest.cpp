#include <memory.h>
#include <lua/lua.hpp>
#include "tcp_servertest.h"
#include "lpublic.h"

#pragma comment (lib, "ws2_32.lib")  //加载 ws2_32.dll  

extern lua_State* luaEnv;

LTcpServerTest::LTcpServerTest(int nPort)
{
    m_socketServer = -1;

    m_nPort = _TCP_DEFAULT_SERVER_PORT;
    if (nPort > 1024 && nPort < 65534)
    {
        m_nPort = nPort;
    }
}

LTcpServerTest::~LTcpServerTest()
{
    UnInit();
}

int LTcpServerTest::Init()
{
    int nResult = 0;
    int nRetCode = 0;
    LU_PROCESS_ERROR(m_nPort > 1024 && m_nPort < 65534);

	//初始化 DLL  
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

    m_socketServer = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    LU_PROCESS_ERROR(m_socketServer >= 0);

    memset(&m_addrServer, 0, sizeof(m_addrServer));
    m_addrServer.sin_family = AF_INET;
    m_addrServer.sin_addr.s_addr = htonl(INADDR_ANY);
    m_addrServer.sin_port = htons(m_nPort);
    nRetCode = bind(m_socketServer, (sockaddr*)&m_addrServer, sizeof(m_addrServer));
    LU_PROCESS_ERROR(nRetCode >= 0);

    nRetCode = listen(m_socketServer, _TCP_MAX_CONN_WAIT);
    LU_PROCESS_ERROR(nRetCode >= 0);

    nResult = 1;
Exit0:
    printf("tcpserver init result = %d\n", nResult);
    return nResult;
}

void LTcpServerTest::UnInit()
{
    if (m_socketServer >= 0)
    {
		closesocket(m_socketServer);
		//终止 DLL 的使用  
		WSACleanup();
    }
}

void LTcpServerTest::Breath()
{

}

void LTcpServerTest::Run()
{
    int nClientConn = -1;
    sockaddr_in sockaddrClient;
    char pBuffer[_TCP_MAX_BUFFER_SIZE];
    int nRetCode = 0;
    int nsin_size = sizeof(struct sockaddr_in);

    nClientConn = accept(m_socketServer, (struct sockaddr*)&sockaddrClient, &nsin_size);
    LU_PROCESS_ERROR(nClientConn >= 0);
    printf("Received a connection form %s\n", (char*)inet_ntoa(sockaddrClient.sin_addr));

    while (true)
    {
        memset(pBuffer, 0, _TCP_MAX_BUFFER_SIZE);
        nRetCode = recv(nClientConn, pBuffer, _TCP_MAX_BUFFER_SIZE - 1, 0);
        if (nRetCode > 0)
        {
            printf("Received message: %s\n", pBuffer);
			int nret = send(nClientConn, pBuffer, nRetCode, 0);
			printf("send code = %d\n", nret);
        }
        else if(nRetCode == 0)
        {
            printf("connect lost!\n");
            break;
        }
        // printf("waitting for message...\n");
        L_SLEEP(1);
    }

	closesocket(nClientConn);
Exit0:
    return;
}













