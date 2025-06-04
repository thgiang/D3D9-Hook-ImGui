#include "Drawing.h"
#include "Hook.h"
#include <iostream>

BOOL Drawing::bInit = FALSE; // Status of the initialization of ImGui.
bool Drawing::bDisplay = true; // Status of the menu display.
ImVec2 Drawing::vWindowPos = { 0, 0 }; // Last ImGui window position.
ImVec2 Drawing::vWindowSize = { 0, 0 }; // Last ImGui window size.

struct CUSTOMVERTEX {
	float x, y, z, rhw; // rhw = reciprocal homogeneous w (1.0f để vẽ 2D)
	DWORD color;
};
D3DVIEWPORT9 viewport;
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

struct DeviceStateBackup {
	DWORD alphaBlendEnable;
	DWORD zEnable;
	DWORD srcBlend;
	DWORD destBlend;
	DWORD fvf;
	IDirect3DBaseTexture9* texture0;
};

void BackupDeviceState(LPDIRECT3DDEVICE9 pDevice, DeviceStateBackup& backup) {
	pDevice->GetRenderState(D3DRS_ALPHABLENDENABLE, &backup.alphaBlendEnable);
	pDevice->GetRenderState(D3DRS_ZENABLE, &backup.zEnable);
	pDevice->GetRenderState(D3DRS_SRCBLEND, &backup.srcBlend);
	pDevice->GetRenderState(D3DRS_DESTBLEND, &backup.destBlend);
	pDevice->GetFVF(&backup.fvf);
	pDevice->GetTexture(0, &backup.texture0);
}

void RestoreDeviceState(LPDIRECT3DDEVICE9 pDevice, const DeviceStateBackup& backup) {
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, backup.alphaBlendEnable);
	pDevice->SetRenderState(D3DRS_ZENABLE, backup.zEnable);
	pDevice->SetRenderState(D3DRS_SRCBLEND, backup.srcBlend);
	pDevice->SetRenderState(D3DRS_DESTBLEND, backup.destBlend);
	pDevice->SetFVF(backup.fvf);
	pDevice->SetTexture(0, backup.texture0);
	if (backup.texture0) backup.texture0->Release();
}

void customDraw(LPDIRECT3DDEVICE9 pDevice) {
	// Lấy kích thước cửa sổ game
	D3DVIEWPORT9 viewport;
	pDevice->GetViewport(&viewport);

	float centerX = viewport.Width / 3.0f;
	float centerY = viewport.Height / 3.0f;
	float halfWidth = 100.0f;
	float halfHeight = 50.0f;

	CUSTOMVERTEX vertices[] = {
		{ centerX - halfWidth, centerY - halfHeight, 0.0f, 1.0f, D3DCOLOR_ARGB(255, 0, 255, 0) },
		{ centerX + halfWidth, centerY - halfHeight, 0.0f, 1.0f, D3DCOLOR_ARGB(255, 0, 255, 0) },
		{ centerX - halfWidth, centerY + halfHeight, 0.0f, 1.0f, D3DCOLOR_ARGB(255, 0, 255, 0) },
		{ centerX + halfWidth, centerY + halfHeight, 0.0f, 1.0f, D3DCOLOR_ARGB(255, 0, 255, 0) },
	};

	// Copy all current state
	DeviceStateBackup backup;
	BackupDeviceState(pDevice, backup);

	// Change render state
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	pDevice->SetTexture(0, NULL);
	pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	pDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);

	// Draw
	pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices, sizeof(CUSTOMVERTEX));

	// Restore state
	RestoreDeviceState(pDevice, backup);
}

/**
    @brief : Hook of the EndScene / Present function.
    @param  D3D9Device : Current Direct3D9 Device Object.
    @retval : Result of the original EndScene / Present function.
**/
//HRESULT Drawing::hkEndScene(const LPDIRECT3DDEVICE9 D3D9Device)
HRESULT Drawing::hkPresent(LPDIRECT3DDEVICE9 D3D9Device, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion)
{
	if (!Hook::pDevice)
		Hook::pDevice = D3D9Device;

	if (!bInit)
		InitImGui(D3D9Device);

	if (GetAsyncKeyState(VK_INSERT) & 1)
		bDisplay = !bDisplay;

	if (GetAsyncKeyState(VK_END) & 1)
	{
		Hook::UnHookDirectX();
		CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)FreeLibrary, Hook::hDDLModule, 0, nullptr);
		//return Hook::oEndScene(D3D9Device);
		return Hook::oPresent(D3D9Device, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
	}

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (bDisplay)
	{
		ImGui::Begin("Menu Window Title", &bDisplay);
		{
			ImGui::SetWindowSize({ 500, 300 }, ImGuiCond_Once);
		
			ImGui::Text("Draw your menu here.");
		}
		ImGui::End();
	}

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());


	//customDraw(D3D9Device);

	//return Hook::oEndScene(D3D9Device);
	return Hook::oPresent(D3D9Device, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

/**
    @brief : function that init ImGui for rendering.
    @param pDevice : Current Direct3D9 Device Object given by the hooked function.
**/
void Drawing::InitImGui(const LPDIRECT3DDEVICE9 pDevice)
{
	D3DDEVICE_CREATION_PARAMETERS CP;
	pDevice->GetCreationParameters(&CP);
	Hook::window = CP.hFocusWindow;
	Hook::HookWindow();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.IniFilename = nullptr;
	io.Fonts->AddFontDefault();

	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(Hook::window);
	ImGui_ImplDX9_Init(pDevice);

	bInit = TRUE;
}
