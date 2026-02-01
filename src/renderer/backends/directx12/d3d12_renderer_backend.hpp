#ifndef D3D12_RENDERER_BACKEND_HPP
#define D3D12_RENDERER_BACKEND_HPP

#include "../../renderer_backend.hpp"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <vector>

class D3D12RendererBackend : public RendererBackend {
private:
    ID3D12Device* device = nullptr;
    ID3D12CommandQueue* commandQueue = nullptr;
    IDXGISwapChain3* swapChain = nullptr;
    ID3D12DescriptorHeap* rtvHeap = nullptr;
    ID3D12DescriptorHeap* dsvHeap = nullptr;
    ID3D12Resource* renderTargets[2] = {};
    ID3D12Resource* depthStencil = nullptr;
    ID3D12CommandAllocator* commandAllocator = nullptr;
    ID3D12GraphicsCommandList* commandList = nullptr;
    ID3D12Fence* fence = nullptr;
    UINT64 fenceValue = 0;
    HANDLE fenceEvent = nullptr;
    UINT rtvDescriptorSize = 0;
    UINT frameIndex = 0;
    
    ID3D12Resource* constantBuffers[3] = {};
    void* constantBufferData[3] = {};
    
    bool createDevice();
    bool createCommandQueue();
    bool createSwapChain(void* hwnd);
    bool createDescriptorHeaps();
    bool createRenderTargets();
    bool createDepthStencil();
    bool createCommandObjects();
    bool createFence();
    bool createConstantBuffers();
    void waitForGPU();
    
public:
    ~D3D12RendererBackend();
    bool init() override;
    bool initWindowContext() override;
    void clear() override;
    void draw(const Mesh&) override;
    void setUniforms(void* shaderProgram) override;
    void onCameraSet() override;
    unsigned int createCubemapTexture(const std::vector<std::string>& faces) override;
    GraphicsAPI getGraphicsAPI() const override;
    void renderSkybox(const Mesh& mesh, unsigned int shaderProgram, unsigned int textureID) override;
    void present();
    
    ID3D12Device* getDevice() const { return device; }
    ID3D12GraphicsCommandList* getCommandList() const { return commandList; }
    void updateConstantBuffer(int binding, const void* data, size_t size);
    void setHwnd(void* hwnd) { this->hwnd = hwnd; }
    
private:
    void* hwnd = nullptr;
};

#endif
