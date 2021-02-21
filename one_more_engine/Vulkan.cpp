
#include "PCH.h"

const u32 
g_numDeviceExtensions = 1;
const char* 
g_deviceExtensions[g_numDeviceExtensions] =
{
  VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

const int 
g_numValidationLayers = 1;
const char* 
g_validationLayers[g_numValidationLayers] =
{
  "VK_LAYER_KHRONOS_validation"
};

const char* VK_ErrorToString(VkResult result)
{
  switch (result) 
  {
    VK_ERROR_STRING(VK_SUCCESS);
    VK_ERROR_STRING(VK_NOT_READY);
    VK_ERROR_STRING(VK_TIMEOUT);
    VK_ERROR_STRING(VK_EVENT_SET);
    VK_ERROR_STRING(VK_EVENT_RESET);
    VK_ERROR_STRING(VK_INCOMPLETE);
    VK_ERROR_STRING(VK_ERROR_OUT_OF_HOST_MEMORY);
    VK_ERROR_STRING(VK_ERROR_OUT_OF_DEVICE_MEMORY);
    VK_ERROR_STRING(VK_ERROR_INITIALIZATION_FAILED);
    VK_ERROR_STRING(VK_ERROR_DEVICE_LOST);
    VK_ERROR_STRING(VK_ERROR_MEMORY_MAP_FAILED);
    VK_ERROR_STRING(VK_ERROR_LAYER_NOT_PRESENT);
    VK_ERROR_STRING(VK_ERROR_EXTENSION_NOT_PRESENT);
    VK_ERROR_STRING(VK_ERROR_FEATURE_NOT_PRESENT);
    VK_ERROR_STRING(VK_ERROR_INCOMPATIBLE_DRIVER);
    VK_ERROR_STRING(VK_ERROR_TOO_MANY_OBJECTS);
    VK_ERROR_STRING(VK_ERROR_FORMAT_NOT_SUPPORTED);
    VK_ERROR_STRING(VK_ERROR_SURFACE_LOST_KHR);
    VK_ERROR_STRING(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR);
    VK_ERROR_STRING(VK_SUBOPTIMAL_KHR);
    VK_ERROR_STRING(VK_ERROR_OUT_OF_DATE_KHR);
    VK_ERROR_STRING(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR);
    VK_ERROR_STRING(VK_ERROR_VALIDATION_FAILED_EXT);
    VK_ERROR_STRING(VK_ERROR_INVALID_SHADER_NV);
    //VK_ERROR_STRING(VK_RESULT_BEGIN_RANGE);
    //VK_ERROR_STRING(VK_RESULT_RANGE_SIZE);
  default: return "UNKNOWN";
  };
}

std::vector<const char*> 
GetRequiredExtensions()
{
  uint32_t glfwExtensionCount = 0;
  const char** glfwExtensions;
  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

  if (vkEnableValidationLayers)
  {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  return extensions;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL 
debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
  VkDebugUtilsMessageTypeFlagsEXT messageType,
  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
  void* pUserData)
{
  if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
  {
    std::cerr << "validation layer: " << pCallbackData->pMessage << '\n';
  }

  return VK_FALSE;
}

static void
populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
  createInfo = {
      VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
      nullptr,
      0,
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
      VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
      debugCallback,
      nullptr };
}

static void
CreateInstance(Context& context)
{
  VkApplicationInfo appInfo = {};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "one_more_engine";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "one_more_engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_2;

  VkInstanceCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;

  const bool enableLayers = vkEnableValidationLayers;

  context.instanceExtentions.clear();
  context.deviceExtentions.clear();
  context.validationLayers.clear();

  context.instanceExtentions = GetRequiredExtensions();

  if (enableLayers)
  {
    for (size_t i = 0; i < g_numValidationLayers; i++)
    {
      context.validationLayers.push_back(g_validationLayers[i]);
    }

    VkDebugUtilsMessengerCreateInfoEXT createInfo;

    populateDebugMessengerCreateInfo(createInfo);
    createInfo.pNext = &createInfo;
  }

  createInfo.enabledExtensionCount = (u32)context.instanceExtentions.size();
  createInfo.ppEnabledExtensionNames = context.instanceExtentions.data();
  createInfo.enabledLayerCount = (u32)context.validationLayers.size();
  createInfo.ppEnabledLayerNames = context.validationLayers.data();

  VK_CHECK(vkCreateInstance(&createInfo, nullptr, &context.instance));
}

static void
CreateSurface(const VkInstance& instance, GLFWwindow* window, VkSurfaceKHR* surface)
{
  VK_CHECK(glfwCreateWindowSurface(instance, window, nullptr, surface));
}

static void
EnumeratePhysicalDevices(Context& context)
{
  u32 numDevices = 0;
  VK_CHECK(vkEnumeratePhysicalDevices(context.instance, &numDevices, nullptr));
  VK_VALIDATE(numDevices > 0, "vkEnumeratePhysicalDevices returned zero devices.");

  std::vector<VkPhysicalDevice> devices(numDevices);

  VK_CHECK(vkEnumeratePhysicalDevices(context.instance, &numDevices, devices.data()));
  VK_VALIDATE(numDevices > 0, "vkEnumeratePhysicalDevices returned zero devices.");

  context.gpus.resize(numDevices);

  for (u32 i = 0; i < numDevices; i++)
  {
    vkGPU& gpu = context.gpus[i];
    gpu.device = devices[0];
    
    {
      u32 numQueues = 0;
      vkGetPhysicalDeviceQueueFamilyProperties(gpu.device, &numQueues, nullptr);
      VK_VALIDATE(numQueues > 0, "vkGetPhysicalDeviceQueueFamilyProperties returned zero queues.");

      gpu.queueFamilyProps.resize(numQueues);
      vkGetPhysicalDeviceQueueFamilyProperties(gpu.device, &numQueues, gpu.queueFamilyProps.data());
      VK_VALIDATE(numQueues > 0, "vkGetPhysicalDeviceQueueFamilyProperties returned zero queues.");
    }

    {
      u32 numExtensions;
      VK_CHECK(vkEnumerateDeviceExtensionProperties(gpu.device, nullptr, &numExtensions, nullptr));
      VK_VALIDATE(numExtensions > 0,"vkEnumerateDeviceExtensionProperties returned zero extensions.");

      gpu.extensionsProps.resize(numExtensions);
      VK_CHECK(vkEnumerateDeviceExtensionProperties(gpu.device, nullptr, &numExtensions, gpu.extensionsProps.data()));
      VK_VALIDATE(numExtensions > 0, "vkEnumerateDeviceExtensionProperties returned zero extensions.");
    }

    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu.device, context.surface, &gpu.surfaceCaps));

    {
      u32 numFormats;
      VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(gpu.device, context.surface, &numFormats, nullptr));
      VK_VALIDATE(numFormats > 0, "vkGetPhysicalDeviceSurfaceFormatsKHR returned zero surface formats.");

      gpu.surfaceFormats.resize(numFormats);
      VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(gpu.device, context.surface, &numFormats, gpu.surfaceFormats.data()));
      VK_VALIDATE(numFormats > 0, "vkGetPhysicalDeviceSurfaceFormatsKHR returned zero surface formats.");
    }

    {
      u32 numPresentModes;
      VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(gpu.device, context.surface, &numPresentModes, nullptr));
      VK_VALIDATE(numPresentModes > 0, "vkGetPhysicalDeviceSurfacePresentModesKHR returned zero present modes.");

      gpu.presentModes.resize(numPresentModes);
      VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(gpu.device, context.surface, &numPresentModes, gpu.presentModes.data()));
      VK_VALIDATE(numPresentModes > 0, "vkGetPhysicalDeviceSurfacePresentModesKHR returned zero present modes.");
    }

    vkGetPhysicalDeviceMemoryProperties(gpu.device, &gpu.memProps);

    vkGetPhysicalDeviceProperties(gpu.device, &gpu.props);
  }
}

static bool
CheckPhysicalDeviceExtensionSupport(const vkGPU& gpu, const std::vector<const char*> extensions)
{
  for (u32 i = 0; i < extensions.size(); i++)
  {
    bool found = false;

    for (u32 j = 0; j < gpu.extensionsProps.size(); j++)
    {
      if (strcmp(gpu.extensionsProps[j].extensionName, extensions[i]) == 0)
      {
        found = true;
        break;
      }
    }

    if (!found)
    {
      return false;
    }
  }

  return true;
}

static void
SelectPhysicalDevice(Context& context)
{
  for (i32 i = 0; i < context.gpus.size(); i++)
  {
    vkGPU& gpu = context.gpus[i];

    i32 graphicsIdx = -1;
    i32 presentIdx = -1;

    if (!CheckPhysicalDeviceExtensionSupport(gpu, context.deviceExtentions))
    {
      continue;
    }

    if (gpu.surfaceFormats.size() == 0)
    {
      continue;
    }

    if (gpu.presentModes.size() == 0)
    {
      continue;
    }

    for (u32 j = 0; j < gpu.queueFamilyProps.size(); j++)
    {
      VkQueueFamilyProperties& props = gpu.queueFamilyProps[j];

      if (props.queueCount == 0)
      {
        continue;
      }

      if (props.queueFlags & VK_QUEUE_GRAPHICS_BIT)
      {
        graphicsIdx = j;
        break;
      }
    }

    for (u32 j = 0; j < gpu.queueFamilyProps.size(); j++)
    {
      //if (j == graphicsIdx)
      //{
      //  continue;
      //}

      VkQueueFamilyProperties& props = gpu.queueFamilyProps[j];

      if (props.queueCount == 0)
      {
        continue;
      }

      VkBool32 supportsPresent = VK_FALSE;
      vkGetPhysicalDeviceSurfaceSupportKHR(gpu.device, j, context.surface, &supportsPresent);
      if (supportsPresent)
      {
        presentIdx = j;
        break;
      }
    }

    if (graphicsIdx >= 0 && presentIdx >= 0)
    {
      context.graphicsFamilyIdx = graphicsIdx;
      context.presentFamilyIdx = presentIdx;
      context.physicalDevice = gpu.device;
      context.gpu = &gpu;
      return;
    }
  }

  platform_assert(false); // no device
}

static void
CreateLogicalDeviceAndQueues(Context& context)
{
  std::vector<u32> uniqueIdx;
  uniqueIdx.push_back(context.graphicsFamilyIdx);
  uniqueIdx.push_back(context.presentFamilyIdx);

  std::vector<VkDeviceQueueCreateInfo> devqInfo;

  const f32 priority = 1.0f;
  for (i32 i = 0; i < uniqueIdx.size(); i++)
  {
    VkDeviceQueueCreateInfo qinfo = {};
    qinfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    qinfo.queueFamilyIndex = uniqueIdx[i];
    qinfo.queueCount = 1;

    qinfo.pQueuePriorities = &priority;

    devqInfo.push_back(qinfo);
  }

  VkPhysicalDeviceFeatures deviceFeatures = {};
  deviceFeatures.textureCompressionBC = VK_TRUE;
  deviceFeatures.imageCubeArray = VK_TRUE;
  deviceFeatures.depthClamp = VK_TRUE;
  deviceFeatures.depthBiasClamp = VK_TRUE;
  deviceFeatures.depthBounds = VK_TRUE;
  deviceFeatures.fillModeNonSolid = VK_TRUE;

  for (size_t i = 0; i < g_numDeviceExtensions; i++)
  {
    context.deviceExtentions.push_back(g_deviceExtensions[i]);
  }

  VkDeviceCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  info.queueCreateInfoCount = devqInfo.size();
  info.pQueueCreateInfos = devqInfo.data();
  info.pEnabledFeatures = &deviceFeatures;
  info.enabledExtensionCount = context.deviceExtentions.size();
  info.ppEnabledExtensionNames = context.deviceExtentions.data();

  if (vkEnableValidationLayers)
  {
    info.enabledLayerCount = context.validationLayers.size();
    info.ppEnabledLayerNames = context.validationLayers.data();
  }
  else
  {
    info.enabledLayerCount = 0;
  }

  VK_CHECK(vkCreateDevice(context.physicalDevice, &info, nullptr, &context.device));

  vkGetDeviceQueue(context.device, context.graphicsFamilyIdx, 0, &context.graphicsQueue);
  vkGetDeviceQueue(context.device, context.presentFamilyIdx, 0, &context.presentQueue);
}

static void
CreateSemaphores(Context& context)
{
  VkSemaphoreCreateInfo semaphoreCreateInfo = {};
  semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  
  for (i32 i = 0; i < NUM_FRAME_DATA; i++)
  {
    VK_CHECK(vkCreateSemaphore(context.device, &semaphoreCreateInfo, nullptr, &context.acquireSemaphores[i]));
    VK_CHECK(vkCreateSemaphore(context.device, &semaphoreCreateInfo, nullptr, &context.renderCompleteSemaphores[i]));
  }
}

static void
CreateCommandPool(Context& context)
{
  VkCommandPoolCreateInfo commandPoolCreateInfo = {};
  commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;

  commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

  commandPoolCreateInfo.queueFamilyIndex = context.graphicsFamilyIdx;

  VK_CHECK(vkCreateCommandPool(context.device, &commandPoolCreateInfo, nullptr, &context.commandPool));
}

static void
CreateCommandBuffer(Context& context)
{
  VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
  commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;

  commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

  commandBufferAllocateInfo.commandPool = context.commandPool;

  commandBufferAllocateInfo.commandBufferCount = NUM_FRAME_DATA;

  VK_CHECK(vkAllocateCommandBuffers(context.device, &commandBufferAllocateInfo, context.commandBuffer));

  VkFenceCreateInfo fenceCreateInfo = {};
  fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

  for (i32 i = 0; i < NUM_FRAME_DATA; i++)
  {
    VK_CHECK(vkCreateFence(context.device, &fenceCreateInfo, nullptr, &context.commandBufferFences[i]));
  }
}

VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats)
{
  VkSurfaceFormatKHR result;

  if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
  {
    result.format = VK_FORMAT_R8G8B8A8_UNORM;
    result.colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    return result;
  }

  for (i32 i = 0; i < formats.size(); i++)
  {
    const VkSurfaceFormatKHR& fmt = formats[i];
    if (fmt.format == VK_FORMAT_R8G8B8A8_UNORM
      && fmt.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
    {
      return fmt;
    }
  }

  return formats[0];
}

VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& modes)
{
  const VkPresentModeKHR desiredMode = VK_PRESENT_MODE_MAILBOX_KHR;

  for (i32 i = 0; i < modes.size(); i++)
  {
    if (modes[i] == desiredMode)
    {
      return desiredMode;
    }
  }

  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D ChooseSurfaceExtent(VkSurfaceCapabilitiesKHR& caps, Window window)
{
  VkExtent2D extent;

  if (caps.currentExtent.width == -1)
  {
    int width, height;
    glfwGetWindowSize(window.glfw, &width, &height);
    extent.width = width;
    extent.height = height;
  }
  else
  {
    extent = caps.currentExtent;
  }

  return extent;
}

static void
CreateImage(Context& context, VkExtent2D& extent, VkFormat format, VkImageUsageFlags usage, Image* image)
{
  VkImageCreateInfo imageInfo{};
  imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageInfo.imageType = VK_IMAGE_TYPE_2D;
  imageInfo.extent.width =  extent.width;
  imageInfo.extent.height = extent.height;
  imageInfo.extent.depth = 1;
  imageInfo.mipLevels = 1;
  imageInfo.arrayLayers = 1;
  imageInfo.format = format;
  imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
  imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  imageInfo.usage = usage;
  imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VmaMemoryRequirements vmaReq = {};
	vmaReq.usage = VMA_MEMORY_USAGE_GPU_ONLY;

	VK_CHECK( vmaCreateImage( vmaAllocator, &imageInfo, &vmaReq, &image->apiImage, &image->allocation, NULL ) );
}

static void
CreateSwapChain(Context& context, Window& window)
{
  vkGPU& gpu = *context.gpu;

  VkSurfaceFormatKHR surfaceFormat = ChooseSurfaceFormat(gpu.surfaceFormats);
  VkPresentModeKHR presentMode = ChoosePresentMode(gpu.presentModes);
  VkExtent2D extent = ChooseSurfaceExtent(gpu.surfaceCaps, window);

  VkSwapchainCreateInfoKHR info = {};
  info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  info.surface = context.surface;

  info.minImageCount = NUM_FRAME_DATA;

  info.imageFormat = surfaceFormat.format;
  info.imageColorSpace = surfaceFormat.colorSpace;
  info.imageExtent = extent;
  info.imageArrayLayers = 1;

  info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;


  if (context.graphicsFamilyIdx != context.presentFamilyIdx)
  {
    u32 indices[] = { context.graphicsFamilyIdx, context.presentFamilyIdx };
    info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    info.queueFamilyIndexCount = 2;
    info.pQueueFamilyIndices = indices;
  }
  else
  {
    info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  }

  info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
  info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  info.presentMode = presentMode;

  info.clipped = VK_TRUE;

  info.oldSwapchain = VK_NULL_HANDLE;

  VkResult result = vkCreateSwapchainKHR(context.device, &info, 0, &context.swapchain);

  //VK_CHECK(vkCreateSwapchainKHR(context.device, &info, nullptr, &swapchain));

  context.swapchainFormat = surfaceFormat.format;
  context.presentMode = presentMode;
  context.swapchainExtent = extent;

  u32 numImages = 0;
  VkImage swapchainImages[NUM_FRAME_DATA];
  VK_CHECK(vkGetSwapchainImagesKHR(context.device, context.swapchain, &numImages, nullptr));
  VK_VALIDATE(numImages > 0, "vkGetSwapchainImagesKHR returned a zero image count.");

  VK_CHECK(vkGetSwapchainImagesKHR(context.device, context.swapchain, &numImages, swapchainImages));
  VK_VALIDATE(numImages > 0, "vkGetSwapchainImagesKHR returned a zero image count.");

  for (u32 i = 0; i < NUM_FRAME_DATA; i++)
  {
    VkImageViewCreateInfo imageViewCreateInfo = {};
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

    imageViewCreateInfo.image = swapchainImages[i];

    imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

    imageViewCreateInfo.format = context.swapchainFormat;

    imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
    imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
    imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
    imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;

    imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;

    imageViewCreateInfo.subresourceRange.levelCount = 1;

    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = 1;
    imageViewCreateInfo.flags = 0;

    VkImageView imageView;
    VK_CHECK(vkCreateImageView(context.device, &imageViewCreateInfo, nullptr, &imageView));

    // TODO: create platform image
    //idImage* image = new idImage(va("_swapchain%d", i));
    //image->CreateFromSwapImage(
    //  swapchainImages[i],
    //  imageView,
    //  vkcontext.swapchainFormat,
    //  vkcontext.swapchainExtent);
    //vkcontext.swapchainImages[i] = image;

    context.swapchainImages[i].view = imageView;
    context.swapchainImages[i].apiImage = swapchainImages[i];

    // CreateImage(context, imageView, &context.swapchainImages[i]);
  }
}

static VkFormat
ChooseSupportedFormat(Context& context, VkFormat* formats, i32 numFormats, VkImageTiling tiling, VkFormatFeatureFlags features)
{
  for (i32 i = 0; i < numFormats; i++)
  {
    VkFormat format = formats[i];

    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(context.physicalDevice, format, &props);

    if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
    {
      return format;
    }
    else 
    if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) 
    {
      return format;
    }
  }

  platform_assert(false);

  return VK_FORMAT_UNDEFINED;
}

static void
CretaeRenderTargets(Context& context)
{
  {
    VkFormat formats[] =
    {
      VK_FORMAT_D32_SFLOAT_S8_UINT,
      VK_FORMAT_D24_UNORM_S8_UINT
    };

    context.depthFormat = ChooseSupportedFormat(
      context,
      formats, 2,
      VK_IMAGE_TILING_OPTIMAL, 
      VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
  }

  // image opts: depth options
  // format
  // width
  // height
  // levels

  // globalimages->scratchimage "_viewDepth"

  {
    CreateImage(context, context.swapchainExtent, context.depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, &context.depthImage);

    VkImageViewCreateInfo imageViewCreateInfo = {};
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

    imageViewCreateInfo.image = context.depthImage.apiImage;

    imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

    imageViewCreateInfo.format = context.depthFormat;

    imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;

    imageViewCreateInfo.subresourceRange.levelCount = 1;

    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = 1;
    imageViewCreateInfo.flags = 0;

    VkImageView imageView;
    VK_CHECK(vkCreateImageView(context.device, &imageViewCreateInfo, nullptr, &imageView));

    context.depthImage.view = imageView;
  }
}

static void
CreateRenderPass(Context& context)
{
  std::vector<VkAttachmentDescription> attachments;

  VkAttachmentDescription colorAttachment = {};
  colorAttachment.format = context.swapchainFormat;
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  attachments.push_back(colorAttachment);

  VkAttachmentDescription depthAttachment = {};
  depthAttachment.format = context.depthFormat;
  depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
  attachments.push_back(depthAttachment);

  VkAttachmentReference colorRef = {};
  colorRef.attachment = 0;
  colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depthRef = {};
  depthRef.attachment = 1;
  depthRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass = {};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorRef;
  subpass.pDepthStencilAttachment = &depthRef;

  VkRenderPassCreateInfo renderPassCreateInfo = {};
  renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassCreateInfo.attachmentCount = attachments.size();
  renderPassCreateInfo.pAttachments = attachments.data();
  renderPassCreateInfo.subpassCount = 1;
  renderPassCreateInfo.pSubpasses = &subpass;
  renderPassCreateInfo.dependencyCount = 0;

  VK_CHECK(vkCreateRenderPass(context.device, &renderPassCreateInfo, nullptr, &context.renderPass));
}

static void
CreateFrameBuffers(Context& context)
{
  VkImageView attachments[2];

  // image* depthImg = global.get

  // attachments[1] = depthImg.view

  attachments[1] = context.depthImage.view;

  VkFramebufferCreateInfo frameBufferCreateInfo = {};
  frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;

  frameBufferCreateInfo.renderPass = context.renderPass;

  frameBufferCreateInfo.attachmentCount = 2;
  frameBufferCreateInfo.pAttachments = attachments;

  // frameBufferCreateInfo.width = renderSystem->GetWidth();
  // frameBufferCreateInfo.height = renderSystem->GetHeight();

  frameBufferCreateInfo.width = context.swapchainExtent.width;//renderSystem->GetWidth();
  frameBufferCreateInfo.height = context.swapchainExtent.height;//renderSystem->GetHeight();

  frameBufferCreateInfo.layers = 1;

  for (i32 i = 0; i < NUM_FRAME_DATA; ++i) 
  {
    attachments[0] = context.swapchainImages[i].view; // ->GetView();
    VK_CHECK(vkCreateFramebuffer(context.device, &frameBufferCreateInfo, nullptr, &context.frameBuffers[i]));
  }
}

static Context 
context;

void
InitAPI(Window& window)
{
  CreateInstance(context);

  CreateSurface(context.instance, window.glfw, &context.surface);

  EnumeratePhysicalDevices(context);

  SelectPhysicalDevice(context);

  CreateLogicalDeviceAndQueues(context);

  CreateSemaphores(context);

  // CreateQueryPool

  CreateCommandPool(context);

  CreateCommandBuffer(context);

  // setup allocator

  // start staging manager

  CreateSwapChain(context, window);

  CretaeRenderTargets(context);

  CreateRenderPass(context);

  CreateFrameBuffers(context);
}