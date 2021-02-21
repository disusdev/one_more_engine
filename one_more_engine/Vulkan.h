#pragma once

struct Window;


#define VK_ERROR_STRING( x ) case static_cast< int >( x ): return #x
const char* VK_ErrorToString(VkResult result);

#define VK_VALIDATE(expr, msg) if (!expr) { std::cerr << "vkValidate: " << msg << '\n'; platform_assert(false); } 

#define VK_CHECK(func) { VkResult result = func; VK_VALIDATE(result == VK_SUCCESS, VK_ErrorToString(result)); }


const u32 NUM_FRAME_DATA = 2;

struct vkGPU
{
  VkPhysicalDevice device;
  VkSurfaceCapabilitiesKHR surfaceCaps;
  VkPhysicalDeviceMemoryProperties memProps;
  VkPhysicalDeviceProperties props;
  std::vector<VkExtensionProperties> extensionsProps;
  std::vector<VkQueueFamilyProperties> queueFamilyProps;
  std::vector<VkSurfaceFormatKHR> surfaceFormats;
  std::vector<VkPresentModeKHR> presentModes;
};

struct Context
{
  VkInstance instance;
  VkSurfaceKHR surface;

  u32 graphicsFamilyIdx;
  u32 presentFamilyIdx;
  VkPhysicalDevice physicalDevice;
  vkGPU* gpu;

  VkDevice device;
  VkQueue graphicsQueue;
  VkQueue presentQueue;

  VkSemaphore acquireSemaphores[NUM_FRAME_DATA];
  VkSemaphore renderCompleteSemaphores[NUM_FRAME_DATA];
  VkFence commandBufferFences[NUM_FRAME_DATA];
  VkCommandBuffer commandBuffer[NUM_FRAME_DATA];

  VkCommandPool commandPool;
  VkSwapchainKHR swapchain;

  VkFormat swapchainFormat;
  VkPresentModeKHR presentMode;
  VkExtent2D swapchainExtent;

  // platform images
  Image swapchainImages[NUM_FRAME_DATA];
  //VkImage swapchainImages[NUM_FRAME_DATA];
  //VkImageView swapchainImageViews[NUM_FRAME_DATA];

  Image depthImage;
  //VkImage depthImage;
  //VkImageView depthImageView;
  // VkImageView swapChainImagesView

  VkFormat depthFormat;

  VkRenderPass renderPass;

  VkFramebuffer frameBuffers[NUM_FRAME_DATA];

  std::vector<vkGPU> gpus;
  std::vector<const char*> instanceExtentions;
  std::vector<const char*> deviceExtentions;
  std::vector<const char*> validationLayers;
};

void
InitAPI(Window& window);