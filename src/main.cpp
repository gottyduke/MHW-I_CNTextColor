#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <cstdint>
#include <regex>
#include <string>
#include <type_traits>

#include "MinHook.h"


// 缤·纷·色·彩
#define MAKE_COLOR(COLOR)	"<STYL MOJI_" #COLOR "_DEFAULT>$&</STYL>"
#define RED_FMT			MAKE_COLOR(RED)
#define GREEN_FMT		MAKE_COLOR(GREEN)
#define BLUE_FMT		MAKE_COLOR(BLUE)
#define PURPLE_FMT		MAKE_COLOR(PURPLE)
#define YELLOW_FMT		MAKE_COLOR(YELLOW)
#define ORANGE_FMT		MAKE_COLOR(ORANGE)
#define LIGHTBLUE_FMT	MAKE_COLOR(LIGHTBLUE)
#define LIGHTGREEN_FMT	MAKE_COLOR(LIGHTGREEN)
#define LIGHTYELLOW_FMT MAKE_COLOR(LIGHTYELLOW)
#define SLGREEN_FMT		MAKE_COLOR(SLGREEN)


// 15.11.01 (build 421471) 应该不会再更 新  了   吧
enum class MsgFlag : std::uint32_t
{
	kDefault = static_cast<std::uint32_t>(-1)
};

using ShowGameMessage_t = std::add_pointer_t<void(void*, const char*, float, MsgFlag, const bool)>;
ShowGameMessage_t ShowGameMessage = ShowGameMessage_t(0x141A671F0);
ShowGameMessage_t _OriginalFunc = nullptr;
void* MsgManager = (void*)0x14506D340;


// 正则下C洋
std::pair<std::string, std::string> RgxTbl[]{
	// 技能
	{ "\\[[^\\]]+\\]", ORANGE_FMT },
	// 衣装
	{ "^.*?(?=\\s+现在可以使用了。)", ORANGE_FMT },
	// 场地装备
	{ "(可使用)(.*?)(?=了。)", "$1<STYL MOJI_ORANGE_DEFAULT>$2</STYL>"},
	// 狩猎笛演奏
	{ "^.*?(?=的旋律效果。)", ORANGE_FMT },
	// 狩猎笛聆听
	{ "(获得)(.*?)(?=的\\s+旋律效果。)", "$1<STYL MOJI_ORANGE_DEFAULT>$2</STYL>" },
};


void Hook_ShowGameMessage(void* a_this, const char* a_msg, float a_depth = -1.f, MsgFlag a_flag = MsgFlag::kDefault, const bool a_useIceborneColor = false)
{
	std::string msg{ a_msg };

	if (msg.empty()) {
		return;
	}

	for (auto& [rgx, fmt] : RgxTbl) {
		if (std::regex_search(msg, std::regex(rgx))) {
			msg = std::regex_replace(msg, std::regex(rgx), fmt);
			break;
		}
	}

	_OriginalFunc(a_this, &msg[0], a_depth, a_flag, a_useIceborneColor);
}

 
BOOL APIENTRY DllMain(HMODULE a_hModule, DWORD  a_ul_reason_for_call, LPVOID a_lpReserved)
{
	if (a_ul_reason_for_call == DLL_PROCESS_ATTACH) {
		MH_Initialize();

		MH_CreateHook(ShowGameMessage, &Hook_ShowGameMessage, reinterpret_cast<void**>(&_OriginalFunc));

		MH_EnableHook(ShowGameMessage);

		return true;
	}
    return TRUE;
}

