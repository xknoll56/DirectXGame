#pragma once



extern bool global_windowDidResize;
extern int windowWidth;
extern int windowHeight;
extern float windowAspectRatio;
extern HWND hwnd;
extern double currentTimeInSeconds;
extern Camera camera;


class Renderer
{
public:

    ID3D11RenderTargetView* d3d11FrameBufferView;
    ID3D11DepthStencilView* depthBufferView;
    IDXGISwapChain1* d3d11SwapChain;
    ID3D11RasterizerState* rasterizerState;
    ID3D11DepthStencilState* depthStencilState;

    DemoScene demoScene;





    //bool global_keyIsDown[GameActionCount];

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

    int init()
    {

        {
            ID3D11Device* baseDevice;
            ID3D11DeviceContext* baseDeviceContext;
            D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
            UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(DEBUG_BUILD)
            creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

            HRESULT hResult = D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE,
                0, creationFlags,
                featureLevels, ARRAYSIZE(featureLevels),
                D3D11_SDK_VERSION, &baseDevice,
                0, &baseDeviceContext);
            if (FAILED(hResult)) {
                MessageBoxA(0, "D3D11CreateDevice() failed", "Fatal Error", MB_OK);
                return GetLastError();
            }

            // Get 1.1 interface of D3D11 Device and Context
            hResult = baseDevice->QueryInterface(__uuidof(ID3D11Device1), (void**)&d3d11Device);
            assert(SUCCEEDED(hResult));
            baseDevice->Release();

            hResult = baseDeviceContext->QueryInterface(__uuidof(ID3D11DeviceContext1), (void**)&d3d11DeviceContext);
            assert(SUCCEEDED(hResult));
            baseDeviceContext->Release();
        }

#ifdef DEBUG_BUILD
        // Set up debug layer to break on D3D11 errors
        ID3D11Debug* d3dDebug = nullptr;
        d3d11Device->QueryInterface(__uuidof(ID3D11Debug), (void**)&d3dDebug);
        if (d3dDebug)
        {
            ID3D11InfoQueue* d3dInfoQueue = nullptr;
            if (SUCCEEDED(d3dDebug->QueryInterface(__uuidof(ID3D11InfoQueue), (void**)&d3dInfoQueue)))
            {
                d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
                d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
                d3dInfoQueue->Release();
            }
            d3dDebug->Release();
        }
#endif

        // Create Swap Chain

        {
            // Get DXGI Factory (needed to create Swap Chain)
            IDXGIFactory2* dxgiFactory;
            {
                IDXGIDevice1* dxgiDevice;
                HRESULT hResult = d3d11Device->QueryInterface(__uuidof(IDXGIDevice1), (void**)&dxgiDevice);
                assert(SUCCEEDED(hResult));

                IDXGIAdapter* dxgiAdapter;
                hResult = dxgiDevice->GetAdapter(&dxgiAdapter);
                assert(SUCCEEDED(hResult));
                dxgiDevice->Release();

                DXGI_ADAPTER_DESC adapterDesc;
                dxgiAdapter->GetDesc(&adapterDesc);

                OutputDebugStringA("Graphics Device: ");
                OutputDebugStringW(adapterDesc.Description);

                hResult = dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), (void**)&dxgiFactory);
                assert(SUCCEEDED(hResult));
                dxgiAdapter->Release();
            }

            DXGI_SWAP_CHAIN_DESC1 d3d11SwapChainDesc = {};
            d3d11SwapChainDesc.Width = 0; // use window width
            d3d11SwapChainDesc.Height = 0; // use window height
            d3d11SwapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
            d3d11SwapChainDesc.SampleDesc.Count = 1;
            d3d11SwapChainDesc.SampleDesc.Quality = 0;
            d3d11SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            d3d11SwapChainDesc.BufferCount = 2;
            d3d11SwapChainDesc.Scaling = DXGI_SCALING_STRETCH;
            d3d11SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
            d3d11SwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
            d3d11SwapChainDesc.Flags = 0;

            HRESULT hResult = dxgiFactory->CreateSwapChainForHwnd(d3d11Device, hwnd, &d3d11SwapChainDesc, 0, 0, &d3d11SwapChain);
            assert(SUCCEEDED(hResult));

            dxgiFactory->Release();
        }

        // Create Render Target and Depth Buffer

        win32CreateD3D11RenderTargets(d3d11Device, d3d11SwapChain, &d3d11FrameBufferView, &depthBufferView);

        UINT shaderCompileFlags = 0;
        // Compiling with this flag allows debugging shaders with Visual Studio
#if defined(DEBUG_BUILD)
        shaderCompileFlags |= D3DCOMPILE_DEBUG;
#endif



        {
            //LoadedObj obj = loadObj("cube.obj");
            //cubeVertexBuffer = new VertexBuffer(obj.vertexBuffer, obj.numVertices, sizeof(VertexData), 0, false);
            //cubeIndexBuffer = new IndexBuffer(obj.indexBuffer, obj.numIndices);

            //freeLoadedObj(obj);
        }

        // Create Sampler State

        {
            D3D11_SAMPLER_DESC samplerDesc = {};
            samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
            samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
            samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
            samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
            samplerDesc.BorderColor[0] = 1.0f;
            samplerDesc.BorderColor[1] = 1.0f;
            samplerDesc.BorderColor[2] = 1.0f;
            samplerDesc.BorderColor[3] = 1.0f;
            samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

            d3d11Device->CreateSamplerState(&samplerDesc, &samplerState);
        }

        // Load Image
        int texWidth, texHeight, texNumChannels;
        int texForceNumChannels = 4;
        unsigned char* testTextureBytes = stbi_load("test.png", &texWidth, &texHeight,
            &texNumChannels, texForceNumChannels);
        assert(testTextureBytes);
        int texBytesPerRow = 4 * texWidth;

        // Create Texture
        
        {
            D3D11_TEXTURE2D_DESC textureDesc = {};
            textureDesc.Width = texWidth;
            textureDesc.Height = texHeight;
            textureDesc.MipLevels = 1;
            textureDesc.ArraySize = 1;
            textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
            textureDesc.SampleDesc.Count = 1;
            textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
            textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

            D3D11_SUBRESOURCE_DATA textureSubresourceData = {};
            textureSubresourceData.pSysMem = testTextureBytes;
            textureSubresourceData.SysMemPitch = texBytesPerRow;

            ID3D11Texture2D* texture;
            d3d11Device->CreateTexture2D(&textureDesc, &textureSubresourceData, &texture);

            d3d11Device->CreateShaderResourceView(texture, nullptr, &textureView);
            texture->Release();
        }

        free(testTextureBytes);

        // Create Vertex Shader for rendering our lights


        {
            D3D11_RASTERIZER_DESC rasterizerDesc = {};
            rasterizerDesc.FillMode = D3D11_FILL_SOLID;
            rasterizerDesc.CullMode = D3D11_CULL_BACK;
            rasterizerDesc.FrontCounterClockwise = TRUE;

            d3d11Device->CreateRasterizerState(&rasterizerDesc, &rasterizerState);
        }

        {
            D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
            depthStencilDesc.DepthEnable = TRUE;
            depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
            depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

            d3d11Device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
        }

        // Camera

        global_windowDidResize = true; // To force initial perspectiveMat calculation

        
        demoScene.init();

        return 0;
    }

    void update(float dt)
    {

        setStates();
        
        demoScene.update(dt);
        d3d11SwapChain->Present(0, 0);
    }

    void setStates()
    {
        if (global_windowDidResize)
        {
            d3d11DeviceContext->OMSetRenderTargets(0, 0, 0);
            d3d11FrameBufferView->Release();
            depthBufferView->Release();

            HRESULT res = d3d11SwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
            assert(SUCCEEDED(res));

            win32CreateD3D11RenderTargets(d3d11Device, d3d11SwapChain, &d3d11FrameBufferView, &depthBufferView);
            camera.perspectiveMat = MatrixPerspective(windowAspectRatio, degreesToRadians(84), 0.1f, 1000.f);

            global_windowDidResize = false;
        }

        FLOAT backgroundColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
        d3d11DeviceContext->ClearRenderTargetView(d3d11FrameBufferView, backgroundColor);

        d3d11DeviceContext->ClearDepthStencilView(depthBufferView, D3D11_CLEAR_DEPTH, 1.0f, 0);

        D3D11_VIEWPORT viewport = { 0.0f, 0.0f, (FLOAT)windowWidth, (FLOAT)windowHeight, 0.0f, 1.0f };
        d3d11DeviceContext->RSSetViewports(1, &viewport);

        d3d11DeviceContext->RSSetState(rasterizerState);
        d3d11DeviceContext->OMSetDepthStencilState(depthStencilState, 0);

        d3d11DeviceContext->OMSetRenderTargets(1, &d3d11FrameBufferView, depthBufferView);
    }
};
