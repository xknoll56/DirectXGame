#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define UNICODE
#include <windows.h>
#include <d3d11_1.h>
#pragma comment(lib, "d3d11.lib")
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

#include <assert.h>
#include <stdint.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "VectorMath.h"
#include "ObjLoading.h"
#include "Shader.h"
#include "Input.h"
#include "Constants.h"
#include "VertexBuffer.h"
struct Camera
{
    Vector3 cameraPos = { 0, 0, 2 };
    Vector3 cameraFwd = { 0, 0, -1 };
    float cameraPitch = 0.f;
    float cameraYaw = 0.f;
    Matrix4 perspectiveMat = {};
};

Camera camera;
#include "Scene.h"
#include "Renderer.h"

static bool global_windowDidResize = false;


HWND hwnd;
float dt;
double currentTimeInSeconds = 0.0;

int windowWidth, windowHeight;
float windowAspectRatio;

ID3D11Device1* d3d11Device;
ID3D11DeviceContext1* d3d11DeviceContext;
ID3D11ShaderResourceView* textureView;
ID3D11SamplerState* samplerState;


bool win32CreateD3D11RenderTargets(ID3D11Device1* d3d11Device, IDXGISwapChain1* swapChain, ID3D11RenderTargetView** d3d11FrameBufferView, ID3D11DepthStencilView** depthBufferView)
{
    ID3D11Texture2D* d3d11FrameBuffer;
    HRESULT hResult = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&d3d11FrameBuffer);
    assert(SUCCEEDED(hResult));

    hResult = d3d11Device->CreateRenderTargetView(d3d11FrameBuffer, 0, d3d11FrameBufferView);
    assert(SUCCEEDED(hResult));

    D3D11_TEXTURE2D_DESC depthBufferDesc;
    d3d11FrameBuffer->GetDesc(&depthBufferDesc);

    d3d11FrameBuffer->Release();

    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    ID3D11Texture2D* depthBuffer;
    d3d11Device->CreateTexture2D(&depthBufferDesc, nullptr, &depthBuffer);

    d3d11Device->CreateDepthStencilView(depthBuffer, nullptr, depthBufferView);

    depthBuffer->Release();

    return true;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LRESULT result = 0;
    switch (msg)
    {
    case WM_KEYDOWN:
    {
        if (!keys[wparam])
            keysDown[wparam] = true;
        keys[wparam] = true;
        if (wparam == VK_ESCAPE)
            DestroyWindow(hwnd);
        break;
    }
    case WM_KEYUP:
    {
        keys[wparam] = false;
        break;
    }
    case WM_LBUTTONDOWN:
    {
        if (!mouse[MOUSE_LEFT])
            mouseDown[MOUSE_LEFT] = true;
        mouse[MOUSE_LEFT] = true;
        break;
    }
    case WM_LBUTTONUP:
    {
        mouse[MOUSE_LEFT] = false;
        break;
    }
    case WM_RBUTTONDOWN:
    {
        if (!mouse[MOUSE_RIGHT])
            mouseDown[MOUSE_RIGHT] = true;
        mouse[MOUSE_RIGHT] = true;
        break;
    }
    case WM_RBUTTONUP:
    {
        mouse[MOUSE_RIGHT] = false;
        break;
    }
    case WM_MBUTTONDOWN:
    {
        if (!mouse[MOUSE_MIDDLE])
            mouseDown[MOUSE_MIDDLE] = true;
        mouse[MOUSE_MIDDLE] = true;
        break;
    }
    case WM_MBUTTONUP:
    {
        mouse[MOUSE_MIDDLE] = false;
        break;
    }
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        break;
    }
    case WM_SIZE:
    {
        global_windowDidResize = true;
        break;
    }
    default:
        result = DefWindowProcW(hwnd, msg, wparam, lparam);
    }
    return result;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nShowCmd*/)
{
    // Open a window
    //HWND hwnd;
    {
        WNDCLASSEXW winClass = {};
        winClass.cbSize = sizeof(WNDCLASSEXW);
        winClass.style = CS_HREDRAW | CS_VREDRAW;
        winClass.lpfnWndProc = &WndProc;
        winClass.hInstance = hInstance;
        winClass.hIcon = LoadIconW(0, IDI_APPLICATION);
        winClass.hCursor = LoadCursorW(0, IDC_ARROW);
        winClass.lpszClassName = L"MyWindowClass";
        winClass.hIconSm = LoadIconW(0, IDI_APPLICATION);

        if (!RegisterClassExW(&winClass)) {
            MessageBoxA(0, "RegisterClassEx failed", "Fatal Error", MB_OK);
            return GetLastError();
        }

        RECT initialRect = { 0, 0, 1024, 768 };
        AdjustWindowRectEx(&initialRect, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_OVERLAPPEDWINDOW);
        LONG initialWidth = initialRect.right - initialRect.left;
        LONG initialHeight = initialRect.bottom - initialRect.top;

        hwnd = CreateWindowExW(WS_EX_OVERLAPPEDWINDOW,
            winClass.lpszClassName,
            L"10. Blinn-Phong Lighting",
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            CW_USEDEFAULT, CW_USEDEFAULT,
            initialWidth,
            initialHeight,
            0, 0, hInstance, 0);

        if (!hwnd) {
            MessageBoxA(0, "CreateWindowEx failed", "Fatal Error", MB_OK);
            return GetLastError();
        }
    }

    Renderer renderer;
    if (renderer.init())
        return 1;

    // Create D3D11 Device and Context
    
    

    // Timing
    LONGLONG startPerfCount = 0;
    LONGLONG perfCounterFrequency = 0;
    {
        LARGE_INTEGER perfCount;
        QueryPerformanceCounter(&perfCount);
        startPerfCount = perfCount.QuadPart;
        LARGE_INTEGER perfFreq;
        QueryPerformanceFrequency(&perfFreq);
        perfCounterFrequency = perfFreq.QuadPart;
    }

    //clear the inputs
    memset(keys, false, INPUT_SIZE);
    memset(keysDown, false, INPUT_SIZE);
    memset(mouse, false, MOUSE_SIZE);
    memset(mouseDown, false, MOUSE_SIZE);
   

    // Main Loop
    bool isRunning = true;
    while (isRunning)
    {
        
        {
            double previousTimeInSeconds = currentTimeInSeconds;
            LARGE_INTEGER perfCount;
            QueryPerformanceCounter(&perfCount);

            currentTimeInSeconds = (double)(perfCount.QuadPart - startPerfCount) / (double)perfCounterFrequency;
            dt = (float)(currentTimeInSeconds - previousTimeInSeconds);
            if (dt > (1.f / 60.f))
                dt = (1.f / 60.f);
        }

        //reset input downs
        memset(keysDown, false, INPUT_SIZE);
        memset(mouseDown, false, MOUSE_SIZE);


        MSG msg = {};
        while (PeekMessageW(&msg, 0, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                isRunning = false;
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }



        // Get window dimensions

        {
            RECT clientRect;
            GetClientRect(hwnd, &clientRect);
            windowWidth = clientRect.right - clientRect.left;
            windowHeight = clientRect.bottom - clientRect.top;
            windowAspectRatio = (float)windowWidth / (float)windowHeight;
        }

        renderer.update(dt);

    }

    return 0;
}
