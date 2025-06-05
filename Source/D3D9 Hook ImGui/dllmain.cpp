#include "Hook.h"
#include "MusicManager.h"
#include "WebSocketClient.h"
#include <cstdio>
#include <iostream>
#include "nlohmann/json.hpp"
#include <fstream> 


BOOL WINAPI DllMain(const HINSTANCE hinstDLL, const DWORD fdwReason, LPVOID lpReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		// Alloc console for debugging purposes
		AllocConsole();
		FILE* f;
		freopen_s(&f, "CONOUT$", "w", stdout);
		freopen_s(&f, "CONOUT$", "w", stderr);
		freopen_s(&f, "CONIN$", "r", stdin);
		SetConsoleTitleA("D3D9 Hook ImGui Debug Console");
		printf("D3D9 Hook ImGui Debug Console\n");


		DisableThreadLibraryCalls(hinstDLL);
		Hook::hDDLModule = hinstDLL;


		// Read all config from bear.json
		std::ifstream configFile("bear.json"); 
		if (!configFile.is_open()) {
            // Existing code remains unchanged
            std::ifstream configFile("bear.json"); 
            if (!configFile.is_open()) {
				// Create config file if it doesn't exist
				std::ofstream newConfigFile("bear.json");
				if (newConfigFile.is_open()) {
					nlohmann::json defaultConfig;
					defaultConfig["token"] = "guest";
					newConfigFile << defaultConfig.dump(4);
					newConfigFile.close();
				}
			}
		}

		nlohmann::json config;
		configFile >> config;

		WebSocketClient client("wss://127.0.0.1/ws", config["token"]);
		client.start();

		auto& manager = MusicManager::getInstance(&client);

		for (int i = 0; i < 5; i++) {
			Sleep(200);
			const auto& list = manager.getCheoroList();
			for (const auto& song : list) {
				std::cout << song.id << " - " << song.title << " by " << song.author << std::endl;
			}
			if (list.size() > 1) {
				break;
			}
		}

		CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)Hook::HookDirectX, nullptr, 0, nullptr);
	}

	if (fdwReason == DLL_PROCESS_DETACH)
		Hook::UnHookDirectX();

	return TRUE;
}

