#include "Hook.h"
#include <cstdio>
#include "MusicManager.h"
#include <iostream>

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

		auto& manager = MusicManager::getInstance();
		const auto& list = manager.getMusicList();

		for (const auto& song : list) {
			std::cout << song.id << " - " << song.title << " by " << song.author << std::endl;
		}


		CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)Hook::HookDirectX, nullptr, 0, nullptr);
	}

	if (fdwReason == DLL_PROCESS_DETACH)
		Hook::UnHookDirectX();

	return TRUE;
}

