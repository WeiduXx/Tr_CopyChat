// dllmain.cpp : 定义 DLL 应用程序的入口点。

// SFAPI & GAMEAPI
#include "SAMPFUNCS_API.h"
#include "game_api.h"

SAMPFUNCS* SF = new SAMPFUNCS();

bool state = false;
int  Targetid = -1;

/*
* 指令回调
* 参数：
*		str: 字符串;
* 返回值:
*       Null
*/
void CALLBACK CommandHandler(std::string str)
{
    if (state == true)
    {
        state = false;
        Targetid = -1;
        return SF->getSAMP()->getChat()->AddChatMessage(-1, "{A4EC2F}[Tr_复读机]: {2FEC96}停止复读！");
    }

    if (str.empty()) return SF->getSAMP()->getChat()->AddChatMessage(-1, "{A4EC2F}[Tr_复读机]: {2FEC96}指令格式 /tr.uf <[玩家ID]>");

    if (sscanf_s(str.c_str(), "%i", &Targetid))
    {
        if (Targetid == SF->getSAMP()->getPlayers()->sLocalPlayerID) return SF->getSAMP()->getChat()->AddChatMessage(-1, "{A4EC2F}[Tr_复读机]: {2FEC96}复读自己是吧？！");
        if (!SF->getSAMP()->getPlayers()->iIsListed[Targetid]) return SF->getSAMP()->getChat()->AddChatMessage(-1, "{A4EC2F}[Tr_复读机]: {2FEC96}目标对象不在服务器！");
        
        state = true;
        SF->getSAMP()->getChat()->AddChatMessage(-1, "{A4EC2F}[Tr_复读机]: {2FEC96}开始复读，目标对象 <%s[%i]>", SF->getSAMP()->getPlayers()->GetPlayerName(Targetid), Targetid);
    }
    else
    {
        return SF->getSAMP()->getChat()->AddChatMessage(-1, "{A4EC2F}[Tr_复读机]: {2FEC96}请正确填写参数！");
    }
}


/*
* 传出RPC包回调
* 参数：
*		params: RakNet钩子;
* 返回值:
*       true: 成功
*/
bool CALLBACK IN_RPC(stRakNetHookParams* params)
{
    if (params->packetId == ScriptRPCEnumeration::RPC_ScrClientMessage && state)
    {
        D3DCOLOR color;
        int length;
        char message[144]{ 0 };
        char meg[144]{ 0 };

        params->bitStream->Read(color);
        params->bitStream->Read(length);
        params->bitStream->Read(message, length);

        // 判断目标对象名称 && 信息当中没有出现自己的名称
        if (strstr(message, SF->getSAMP()->getPlayers()->GetPlayerName(Targetid)) &&
            !strstr(message, SF->getSAMP()->getPlayers()->szLocalPlayerName))
        {
            std::string::size_type n;
            std::string const s(message);
            int size = s.size();
            n = s.find(':'); // 返回搜索到的字符
            std::string msg = s.substr(n + 1); // 返回 从 @n 后面的字符串
            sprintf_s(meg, "%s", msg.c_str()); // string 转 char
            SF->getSAMP()->getPlayers()->pLocalPlayer->Say(meg);
        }
        
    }
    return true;
}

/* 主循环 */
void CALLBACK Mainloop()
{
    static bool Init = false; // @bool 声明初始化变量

    if (!Init)
    {
        if (!GAME || GAME->GetSystemState() != eSystemState::GS_PLAYING_GAME) return;
        if (!SF->getSAMP()->IsInitialized()) return;

        SF->getSAMP()->registerChatCommand("tr.fd", CommandHandler);
        SF->getRakNet()->registerRakNetCallback(RAKHOOK_TYPE_INCOMING_RPC, IN_RPC);
        SF->getSAMP()->getChat()->AddChatMessage(-1, "{A4EC2F}[Tr_复读机]: {2FEC96}加载完成，作者: Weidu");
        SF->getSAMP()->getChat()->AddChatMessage(-1, "{A4EC2F}[Tr_复读机]: {2FEC96}指令/tr.fd <[玩家ID]>");
        Init = true;
    }
}

/* 核心入口事件 */
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
        SF->initPlugin(Mainloop, hModule); // 初始化插件
    return TRUE;
}

