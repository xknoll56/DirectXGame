#pragma once

#include "VectorMath.h"
#include <map>
#include <string>
extern ID3D11Device1* d3d11Device;
extern ID3D11DeviceContext1* d3d11DeviceContext;

enum ShaderInputType
{
    POSITION = 0,
    POSITION_TEXTURE = 1,
    POSITION_NORMAL = 2,
    POSITION_TEXTURE_NORMAL = 3,
    POSITION_TEXTURE_2D = 4,
    POSITION_2D = 5

};


struct ConstantBuffer
{
    ID3D11Buffer* buffer;
    uint32_t reg;
    D3D11_MAPPED_SUBRESOURCE mappedSubresource;

    template<typename T>
    static ConstantBuffer CreateConstantBuffer(uint32_t reg)
    {
        ID3D11Buffer* constantBuffer;
        {
            D3D11_BUFFER_DESC constantBufferDesc = {};
            // ByteWidth must be a multiple of 16, per the docs
            constantBufferDesc.ByteWidth = sizeof(T) + 0xf & 0xfffffff0;;
            constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
            constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

            HRESULT hResult = d3d11Device->CreateBuffer(&constantBufferDesc, nullptr, &constantBuffer);
            assert(SUCCEEDED(hResult));
        }
        ConstantBuffer cb;
        cb.buffer = constantBuffer;
        cb.reg = reg;
        return cb;
    }
};

class Shader
{
public:
    ID3DBlob* vsBlob;
    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;
    ID3D11InputLayout* inputLayout;
    std::map<std::string, ConstantBuffer> uniformBuffers;



    template<typename T>
    void SetVertexShaderUniformBuffer(std::string bufferName, const T& value)
    {
        d3d11DeviceContext->Map(uniformBuffers[bufferName].buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &uniformBuffers[bufferName].mappedSubresource);
        memcpy((T*)uniformBuffers[bufferName].mappedSubresource.pData, &value, sizeof(T));
        d3d11DeviceContext->Unmap(uniformBuffers[bufferName].buffer, 0);
        d3d11DeviceContext->VSSetConstantBuffers(uniformBuffers[bufferName].reg, 1, &uniformBuffers[bufferName].buffer);
    }

    template<typename T>
    void SetPixelShaderUniformBuffer(std::string bufferName, const T& value)
    {
        d3d11DeviceContext->Map(uniformBuffers[bufferName].buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &uniformBuffers[bufferName].mappedSubresource);
        memcpy((T*)uniformBuffers[bufferName].mappedSubresource.pData, &value, sizeof(T));
        d3d11DeviceContext->Unmap(uniformBuffers[bufferName].buffer, 0);
        d3d11DeviceContext->PSSetConstantBuffers(uniformBuffers[bufferName].reg, 1, &uniformBuffers[bufferName].buffer);
    }

    Shader(const wchar_t* path, ShaderInputType type)
    {
        //Compile the vertex shader
        {
            ID3DBlob* shaderCompileErrorsBlob;
            HRESULT hResult = D3DCompileFromFile(path, nullptr, nullptr, "vs_main", "vs_5_0", 0, 0, &vsBlob, &shaderCompileErrorsBlob);
            if (FAILED(hResult))
            {
                const char* errorString = NULL;
                if (hResult == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
                    errorString = "Could not compile shader; file not found";
                else if (shaderCompileErrorsBlob) {
                    errorString = (const char*)shaderCompileErrorsBlob->GetBufferPointer();
                    shaderCompileErrorsBlob->Release();
                }
                MessageBoxA(0, errorString, "Shader Compiler Error", MB_ICONERROR | MB_OK);
            }

            hResult = d3d11Device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &vertexShader);
            assert(SUCCEEDED(hResult));
        }

        //Compile the pixel shader
        {
            ID3DBlob* psBlob;
            ID3DBlob* shaderCompileErrorsBlob;
            HRESULT hResult = D3DCompileFromFile(path, nullptr, nullptr, "ps_main", "ps_5_0", 0, 0, &psBlob, &shaderCompileErrorsBlob);
            if (FAILED(hResult))
            {
                const char* errorString = NULL;
                if (hResult == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
                    errorString = "Could not compile shader; file not found";
                else if (shaderCompileErrorsBlob) {
                    errorString = (const char*)shaderCompileErrorsBlob->GetBufferPointer();
                    shaderCompileErrorsBlob->Release();
                }
                MessageBoxA(0, errorString, "Shader Compiler Error", MB_ICONERROR | MB_OK);
            }

            hResult = d3d11Device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pixelShader);
            assert(SUCCEEDED(hResult));
            psBlob->Release();
        }
        //Create the input layout
        {
            SetInputLayoutFromType(type);
        }
    }

    void Bind() const
    {
        d3d11DeviceContext->IASetInputLayout(inputLayout);
        d3d11DeviceContext->VSSetShader(vertexShader, nullptr, 0);
        d3d11DeviceContext->PSSetShader(pixelShader, nullptr, 0);
    }

private:
    void SetInputLayoutFromType(ShaderInputType type)
    {
        switch (type)
        {
        case ShaderInputType::POSITION:
        {
            D3D11_INPUT_ELEMENT_DESC inputElementDesc0 = { "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
            D3D11_INPUT_ELEMENT_DESC inputElementDescArray[] = { inputElementDesc0 };
            unsigned int size = ARRAYSIZE(inputElementDescArray);
            HRESULT hResult = d3d11Device->CreateInputLayout(inputElementDescArray, size, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayout);
            assert(SUCCEEDED(hResult));
            vsBlob->Release();
            break;
        }
        case ShaderInputType::POSITION_TEXTURE:
        {
            D3D11_INPUT_ELEMENT_DESC inputElementDesc0 = { "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
            D3D11_INPUT_ELEMENT_DESC inputElementDesc1 = { "TEX", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
            D3D11_INPUT_ELEMENT_DESC inputElementDescArray[] = { inputElementDesc0, inputElementDesc1 };
            unsigned int size = ARRAYSIZE(inputElementDescArray);
            HRESULT hResult = d3d11Device->CreateInputLayout(inputElementDescArray, size, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayout);
            assert(SUCCEEDED(hResult));
            vsBlob->Release();
            break;
        }
        case ShaderInputType::POSITION_NORMAL:
        {
            D3D11_INPUT_ELEMENT_DESC inputElementDesc0 = { "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
            D3D11_INPUT_ELEMENT_DESC inputElementDesc1 = { "NORM", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
            D3D11_INPUT_ELEMENT_DESC inputElementDescArray[] = { inputElementDesc0, inputElementDesc1 };
            unsigned int size = ARRAYSIZE(inputElementDescArray);
            HRESULT hResult = d3d11Device->CreateInputLayout(inputElementDescArray, size, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayout);
            assert(SUCCEEDED(hResult));
            vsBlob->Release();
            break;
        }
        case ShaderInputType::POSITION_TEXTURE_NORMAL:
        {
            D3D11_INPUT_ELEMENT_DESC inputElementDesc0 = { "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
            D3D11_INPUT_ELEMENT_DESC inputElementDesc1 = { "NORM", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
            D3D11_INPUT_ELEMENT_DESC inputElementDesc2 = { "TEX", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
            D3D11_INPUT_ELEMENT_DESC inputElementDescArray[] = { inputElementDesc0, inputElementDesc2, inputElementDesc1 };
            unsigned int size = ARRAYSIZE(inputElementDescArray);
            HRESULT hResult = d3d11Device->CreateInputLayout(inputElementDescArray, size, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayout);
            assert(SUCCEEDED(hResult));
            vsBlob->Release();
            break;
        }
        case ShaderInputType::POSITION_TEXTURE_2D:
        {
            D3D11_INPUT_ELEMENT_DESC inputElementDesc0 = { "POS", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
            D3D11_INPUT_ELEMENT_DESC inputElementDesc1 = { "TEX", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
            D3D11_INPUT_ELEMENT_DESC inputElementDescArray[] = { inputElementDesc0, inputElementDesc1 };
            unsigned int size = ARRAYSIZE(inputElementDescArray);
            HRESULT hResult = d3d11Device->CreateInputLayout(inputElementDescArray, size, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayout);
            assert(SUCCEEDED(hResult));
            vsBlob->Release();
            break;
        }
        case ShaderInputType::POSITION_2D:
        {
            D3D11_INPUT_ELEMENT_DESC inputElementDesc0 = { "POS", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
            D3D11_INPUT_ELEMENT_DESC inputElementDescArray[] = { inputElementDesc0 };
            unsigned int size = ARRAYSIZE(inputElementDescArray);
            HRESULT hResult = d3d11Device->CreateInputLayout(inputElementDescArray, size, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayout);
            assert(SUCCEEDED(hResult));
            vsBlob->Release();
            break;
        }
        }
    }
};