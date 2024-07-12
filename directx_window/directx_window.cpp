// directx_window.cpp : Defines the entry point for the application.
//

#include <windows.h>
#include <d3d11.h>
#include "resource.h"

/* Global Declarations */
IDXGISwapChain* SwapChain;
ID3D11Device* d3d11Device;
ID3D11DeviceContext* d3d11DevCon;
ID3D11RenderTargetView* renderTargetView;

float red = 0.0f;
float green = 0.0f;
float blue = 0.0f;
int colormodr = 1;
int colormodg = 1;
int colormodb = 1;

// Global Variables:
LPCTSTR WndClassName = L"DirectX Window";
HWND hwnd = NULL;

const int Width = 800;
const int Height = 600;

/* Function Prototypes */
bool InitializeDirect3d11App(HINSTANCE hIntance);
void ReleaseObjects();
bool InitScene();
void UpdateScene();
void DrawScene();

// Forward declarations of functions included in this code module:
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

bool InitializeWindow(HINSTANCE hInstance, int ShowWnd, int width, int height, bool windowed);

int messageLoop();

int WINAPI wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nShowCmd)
{
	// Initialize Window
	if (!InitializeWindow(hInstance, nShowCmd, Width, Height, true)) {
		MessageBox(0, L"Window Initialization - Failed", L"Error", MB_OK);
		return 0;
	}

	// Initialize Direct3D
	if (!InitializeDirect3d11App(hInstance)) {
		MessageBox(0, L"Direct3D Initialization - Failed", L"Error", MB_OK);
		return 0;
	}

	// Initialize the scene
	if (!InitScene()) {
		MessageBox(0, L"Scene Initialization - Failed", L"Error", MB_OK);
		return 0;
	}

	messageLoop();

	ReleaseObjects();

	return 0;
}

bool InitializeWindow(HINSTANCE hInstance, int ShowWnd, int width, int height, bool windowed)
{
	WNDCLASSEX wc;

	HICON hIcon = static_cast<HICON>(::LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 48, 48, LR_DEFAULTCOLOR));
	HICON hIconSm = static_cast<HICON>(::LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON2), IMAGE_ICON, 48, 48, LR_DEFAULTCOLOR));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = NULL;
	wc.cbWndExtra = NULL;
	wc.hInstance = hInstance;
	wc.hIcon = hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = WndClassName;
	wc.hIconSm = hIconSm;

	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, L"Error registering class", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	hwnd = CreateWindowEx(
		NULL,
		WndClassName,
		L"DirectX Window",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		width,
		height,
		NULL,
		NULL,
		hInstance,
		NULL);

	if (!hwnd) {
		MessageBox(NULL, L"Error creating window", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	ShowWindow(hwnd, ShowWnd);
	UpdateWindow(hwnd);

	return true;
}

bool InitializeDirect3d11App(HINSTANCE hInstance) {
	// Describe the Buffer
	DXGI_MODE_DESC bufferDesc;

	ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));

	bufferDesc.Width = Width;
	bufferDesc.Height = Height;
	bufferDesc.RefreshRate.Numerator = 60;
	bufferDesc.RefreshRate.Denominator = 1;
	bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Describe the SwapChain
	DXGI_SWAP_CHAIN_DESC swapChainDesc;

	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapChainDesc.BufferDesc = bufferDesc;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// Create the SwapChain
	D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL,
		D3D11_SDK_VERSION, &swapChainDesc, &SwapChain, &d3d11Device, NULL, &d3d11DevCon);

	// Create the BackBuffer
	ID3D11Texture2D* BackBuffer;
	SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBuffer);

	// Create the Render Target
	d3d11Device->CreateRenderTargetView(BackBuffer, NULL, &renderTargetView);
	BackBuffer->Release();

	// Set the Render Target
	d3d11DevCon->OMSetRenderTargets(1, &renderTargetView, NULL);

	return true;
}

void ReleaseObjects() {
	/* Release the COM Objects that were created */
	SwapChain->Release();
	d3d11Device->Release();
	d3d11DevCon->Release();
}

bool InitScene() {
	return true;
}

void UpdateScene() {
	/* Update the colors of the scene */
	red += colormodr * 0.00005f;
	green += colormodg * 0.00002f;
	blue += colormodb * 0.00001f;

	if (red >= 1.0f || red <= 0.0f) {
		colormodr *= -1;
	}
	if (green >= 1.0f || green <= 0.0f) {
		colormodg *= -1;
	}
	if (blue >= 1.0f || blue <= 0.0f) {
		colormodb *= -1;
	}
}

void DrawScene() {
	// Clear the backbuffer to the updated color
	float bgColor[4] = { red, green, blue, 1.0f };

	d3d11DevCon->ClearRenderTargetView(renderTargetView, bgColor);

	// Present the backbuffer to the screen
	SwapChain->Present(0, 0);
}

int messageLoop() {
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while (true) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				break;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			// run the game
			UpdateScene();
			DrawScene();
		}
	}
	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_KEYDOWN:
		{
		switch (wParam)
		{
		case VK_ESCAPE: {
			if (MessageBox(0, L"Are you sure uou want to exit?", L"Really", MB_YESNO | MB_ICONQUESTION) == IDYES) {
				DestroyWindow(hwnd);
			}
		}
					  break;
		}
		return 0;
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code that uses hdc here...
			EndPaint(hWnd, &ps);
		}
		break;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		
		}
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}