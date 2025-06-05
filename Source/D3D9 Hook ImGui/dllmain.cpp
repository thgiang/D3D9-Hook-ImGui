#include "Hook.h"
#include "MusicManager.h"
#include "WebSocketClient.h"
#include <cstdio>
#include <iostream>
#include "nlohmann/json.hpp"
#include <fstream> 


// Global hoặc static để có thể truy cập được từ các luồng khác nếu cần
// Hoặc truyền qua tham số luồng
WebSocketClient* g_pWebSocketClient = nullptr; // Con trỏ toàn cục để quản lý client
MusicManager* g_pMusicManager = nullptr;     // Con trỏ toàn cục cho MusicManager

// Hàm sẽ chạy trong luồng riêng cho WebSocket và MusicManager
void WebSocketThread(LPVOID lpParam) {
    std::cout << "WebSocketThread started." << std::endl;

    // Đọc config ngay trong luồng này hoặc truyền từ DllMain
    // Để đơn giản, tôi sẽ đọc lại config ở đây, nhưng bạn có thể tối ưu
    nlohmann::json config;
    std::ifstream configFile("bear.json");
    if (!configFile.is_open()) {
        std::cout << "bear.json not found in WebSocketThread. Creating default config." << std::endl;
        std::ofstream newConfigFile("bear.json");
        if (newConfigFile.is_open()) {
            config["token"] = "guest";
            newConfigFile << config.dump(4);
            newConfigFile.close();
        }
        else {
            std::cout << "Failed to create bear.json in WebSocketThread!" << std::endl;
            return; // Thoát luồng với mã lỗi
        }
    }
    else {
        configFile >> config;
        configFile.close();
    }

    // Đảm bảo rằng "token" tồn tại
    if (!config.count("token")) {
        config["token"] = "guest";
    }

    // Tạo WebSocketClient và lưu vào con trỏ toàn cục
    g_pWebSocketClient = new WebSocketClient("ws://127.0.0.1:3000/ws", config["token"]);
    std::cout << "WebSocketClient instance created in WebSocketThread." << std::endl;

    // Bắt đầu kết nối WebSocket (hàm này sẽ block trong luồng này)
    try {
        g_pWebSocketClient->start();
        std::cout << "WebSocket client connected successfully." << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "WebSocket client connection failed: " << e.what() << std::endl;
        delete g_pWebSocketClient;
        g_pWebSocketClient = nullptr;
        return; // Thoát luồng với mã lỗi
    }
    catch (...) {
        std::cerr << "WebSocket client connection failed with unknown error." << std::endl;
        delete g_pWebSocketClient;
        g_pWebSocketClient = nullptr;
        return; // Thoát luồng với mã lỗi
    }

    // Sau khi kết nối, khởi tạo MusicManager
    g_pMusicManager = &MusicManager::getInstance(g_pWebSocketClient); // Truyền client đã kết nối

    std::cout << "WebSocketThread finished (or client still running in background)." << std::endl;
    return;
}


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


		CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)Hook::HookDirectX, nullptr, 0, nullptr);
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)WebSocketThread, nullptr, 0, nullptr);
	}

	if (fdwReason == DLL_PROCESS_DETACH)
		Hook::UnHookDirectX();

	return TRUE;
}

