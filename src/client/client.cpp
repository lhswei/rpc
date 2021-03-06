#include <stdlib.h>
#include <stdio.h>
#include <direct.h> 
#include <iostream>
#include <memory.h>
#include "lpublic.h"
#include "tcp_client.h"
#include "client.hpp"

lua_State* luaEnv = nullptr;
const char* s_default_ini_script = "./init.lua";
void PrintLuaError(lua_State* L, int sErr);
int init_script()
{
    if (!luaEnv)
        return 0;

    int ret = 0;
    ret = luaL_dofile(luaEnv, s_default_ini_script);
	if (ret != 0)
	{
		PrintLuaError(luaEnv, ret);
	}
    return 1;
}

int init_lua()
{
    luaEnv = luaL_newstate();
    if (!luaEnv)
    {
        printf("lua_open fail!\n");
        getchar();
        return 0;
    }

    luaL_openlibs(luaEnv);
    return 1;
}

int main(int argc, char *argv[])
{
	init_lua();
	init_script();
    if (luaEnv)
    {
        LTcpClientTest tcpClient("127.0.0.1", _TCP_DEFAULT_SERVER_PORT);
        if (tcpClient.Init() == 1 && tcpClient.Connect2Server() == 1)
        {
			tcpClient.InitScriptLib();
            tcpClient.Run();
        }
    }

    lua_close(luaEnv);
    getchar();
    return 0;
}