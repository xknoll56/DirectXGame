#pragma once

extern ID3D11Device1* d3d11Device;
extern ID3D11DeviceContext1* d3d11DeviceContext;

class Buffer
{
protected:
    ID3D11Buffer* buffer;

public:
    virtual void Bind() = 0;

};

class VertexBuffer : Buffer
{
private:
    //number of vertices
    UINT numVerts;
    //the stride of each vertex in bytes
    UINT stride;
    //offset from the start of the array
    UINT offset;
public:
    VertexBuffer(float vertexData[], unsigned int numVerts, unsigned int stride, unsigned int offset, bool dynamic)
    {
        this->numVerts = numVerts;
        this->stride = stride;
        this->offset = offset;

        D3D11_BUFFER_DESC vertexBufferDesc = {};
        if (!dynamic)
        {
            vertexBufferDesc.ByteWidth = numVerts * stride;
            vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
            vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        }
        else
        {
            vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
            vertexBufferDesc.ByteWidth = numVerts * stride;
            vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            //vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        }

        D3D11_SUBRESOURCE_DATA vertexSubresourceData = { vertexData };

        HRESULT hResult = d3d11Device->CreateBuffer(&vertexBufferDesc, &vertexSubresourceData, &buffer);
        assert(SUCCEEDED(hResult));
    }

    VertexBuffer(VertexData vertexData[], unsigned int numVerts, unsigned int stride, unsigned int offset, bool dynamic)
    {
        this->numVerts = numVerts;
        this->stride = stride;
        this->offset = offset;

        D3D11_BUFFER_DESC vertexBufferDesc = {};
        if (!dynamic)
        {
            vertexBufferDesc.ByteWidth = numVerts * stride;
            vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
            vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        }
        else
        {
            vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
            vertexBufferDesc.ByteWidth = numVerts * stride;
            vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            //vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        }

        D3D11_SUBRESOURCE_DATA vertexSubresourceData = { vertexData };

        HRESULT hResult = d3d11Device->CreateBuffer(&vertexBufferDesc, &vertexSubresourceData, &buffer);
        assert(SUCCEEDED(hResult));
    }

    void UpdateDynamicVertexBuffer(const float vertexData[])
    {
        D3D11_MAPPED_SUBRESOURCE resource;
        ZeroMemory(&resource, sizeof(D3D11_MAPPED_SUBRESOURCE));
        d3d11DeviceContext->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
        memcpy((void*)resource.pData, vertexData, numVerts * stride);
        d3d11DeviceContext->Unmap(buffer, 0);
    }

    unsigned int NumVerts()
    {
        return numVerts;
    }

    unsigned int Stride()
    {
        return stride;
    }

    unsigned int Offset()
    {
        return offset;
    }

    void Bind() override
    {
        d3d11DeviceContext->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
    }
};

class IndexBuffer : Buffer
{
private:
    //number of indices
    UINT numIndices;
public:
    IndexBuffer(uint16_t indices[], uint32_t numIndices)
    {
        this->numIndices = numIndices;

        D3D11_BUFFER_DESC indexBufferDesc = {};
        indexBufferDesc.ByteWidth = numIndices * sizeof(uint16_t);
        indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
        indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

        D3D11_SUBRESOURCE_DATA indexSubresourceData = { indices };

        HRESULT hResult = d3d11Device->CreateBuffer(&indexBufferDesc, &indexSubresourceData, &buffer);
        assert(SUCCEEDED(hResult));
    }

    uint32_t NumIndices()
    {
        return numIndices;
    }

    void Bind() override
    {
        d3d11DeviceContext->IASetIndexBuffer(buffer, DXGI_FORMAT_R16_UINT, 0);
    }

};


static float boundingBoxVerts[] =
{
      -0.505f, 0.505f, -0.505f,  -0.505f, 0.505f, 0.505f,
      -0.505f, 0.505f, 0.505f,  0.505f, 0.505f, 0.505f,
      0.505f, 0.505f, 0.505f,  0.505f, 0.505f, -0.505f,
      0.505f, 0.505f, -0.505f,  -0.505f, 0.505f, -0.505f,
                                
      -0.505f, -0.505f, -0.505f, -0.505f, -0.505f, 0.505f,
      -0.505f, -0.505f, 0.505f, 0.505f, -0.505f, 0.505f,
      0.505f, -0.505f, 0.505f, 0.505f, -0.505f, -0.505f,
      0.505f, -0.505f, -0.505f, -0.505f, -0.505f, -0.505f,
                                
      -0.505f, 0.505f, -0.505f,  -0.505f, -0.505f, -0.505f,
      -0.505f, 0.505f, 0.505f,  -0.505f, -0.505f, 0.505f,
      0.505f, 0.505f, 0.505f,  0.505f, -0.505f, 0.505f,
      0.505f, 0.505f, -0.505f,  0.505f, -0.505f, -0.505f
};

static float boundingPlaneVerts[] =
{
    -1.0f, 0.0f, -1.0f, -1.0f, 0.0f, 1.0f,
    -1.0f, 0.0f, -1.0f, 1.0f, 0.0f, -1.0f,
     1.0f, 0.0f, -1.0f, 1.0f, 0.0f, -1.0f,
     1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f
};