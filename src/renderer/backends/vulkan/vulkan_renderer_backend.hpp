#ifndef VULKAN_RENDERER_BACKEND_HPP
#define VULKAN_RENDERER_BACKEND_HPP

#include <vulkan/vulkan.h>
#include "../../renderer_backend.hpp"
#include <vector>

struct SDL_Window;
class VulkanRendererBackend : public RendererBackend {
private:
    VkInstance instance = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    VkQueue graphicsQueue = VK_NULL_HANDLE;
    VkQueue presentQueue = VK_NULL_HANDLE;
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    VkRenderPass renderPass = VK_NULL_HANDLE;
    VkCommandPool commandPool = VK_NULL_HANDLE;
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> descriptorSets;
    
    std::vector<VkImage> swapchainImages;
    std::vector<VkImageView> swapchainImageViews;
    std::vector<VkFramebuffer> framebuffers;
    std::vector<VkCommandBuffer> commandBuffers;
    
    VkImage depthImage = VK_NULL_HANDLE;
    VkDeviceMemory depthImageMemory = VK_NULL_HANDLE;
    VkImageView depthImageView = VK_NULL_HANDLE;
    
    VkBuffer uniformBuffer = VK_NULL_HANDLE;
    VkDeviceMemory uniformBufferMemory = VK_NULL_HANDLE;
    VkBuffer materialBuffer = VK_NULL_HANDLE;
    VkDeviceMemory materialBufferMemory = VK_NULL_HANDLE;
    VkBuffer lightDataBuffer = VK_NULL_HANDLE;
    VkDeviceMemory lightDataBufferMemory = VK_NULL_HANDLE;
    
    VkSemaphore imageAvailableSemaphore = VK_NULL_HANDLE;
    VkSemaphore renderFinishedSemaphore = VK_NULL_HANDLE;
    VkFence inFlightFence = VK_NULL_HANDLE;
    
    uint32_t graphicsQueueFamily = 0;
    uint32_t presentQueueFamily = 0;
    uint32_t currentImageIndex = 0;
    VkFormat swapchainFormat;
    VkExtent2D swapchainExtent;
    SDL_Window* window;
    
    bool createInstance();
    bool pickPhysicalDevice();
    bool createLogicalDevice();
    bool createSwapchain();
    bool createImageViews();
    bool createRenderPass();
    bool createDescriptorSetLayout();
    bool createFramebuffers();
    bool createCommandPool();
    bool createDepthResources();
    bool createUniformBuffer();
    bool createMaterialBuffer();
    bool createLightDataBuffer();
    bool createDescriptorPool();
    bool createCommandBuffers();
    bool createSyncObjects();
    
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    
public:
    ~VulkanRendererBackend();
    bool init() override;
    bool initWindowContext() override;
    void bindCamera(Camera* camera) override {return;};
    void clear(Camera* camera) override;
    void draw(const Mesh&) override;
    void setUniforms(void* shaderProgram) override;
    void onCameraSet() override;
    unsigned int createCubemapTexture(const std::vector<std::string>& faces) override;
    GraphicsAPI getGraphicsAPI() const override;
    void renderSkybox(const Mesh& mesh, unsigned int shaderProgram, unsigned int textureID) override;
    void present();
    
    VkDevice getDevice() const { return device; }
    VkPhysicalDevice getPhysicalDevice() const { return physicalDevice; }
    VkCommandPool getCommandPool() const { return commandPool; }
    VkInstance getInstance() const { return instance; }
    VkDeviceMemory getMaterialBufferMemory() const { return materialBufferMemory; }
    VkDeviceMemory getLightDataBufferMemory() const { return lightDataBufferMemory; }
    VkExtent2D getSwapchainExtent() const { return swapchainExtent; }
    VkRenderPass getRenderPass() const { return renderPass; }
    VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }
    void setSurface(VkSurfaceKHR surf) { surface = surf; }
    void setWindow(SDL_Window* win) { window = win; }
    unsigned int getRequiredWindowFlags() const override;
    bool init(SDL_Window* window) override;
};

#endif // VULKAN_RENDERER_BACKEND_HPP
