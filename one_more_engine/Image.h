#pragma once

#define	MAX_IMAGE_NAME	256

enum textureType 
{
  TT_DISABLED,
  TT_2D,
  TT_CUBIC
};

enum textureFormat
{
  FMT_NONE,
  FMT_RGBA8,
  FMT_ALPHA,
  FMT_DEPTH
};

enum textureSamples
{
  SAMPLE_1 = BIT(0),
  SAMPLE_2 = BIT(1),
  SAMPLE_4 = BIT(2),
  SAMPLE_8 = BIT(3),
  SAMPLE_16 = BIT(4)
};

enum textureColor
{
  CFM_DEFAULT,
  CFM_GREEN_ALPHA
};

struct ImageOpts 
{
  textureType textureType;
  textureFormat format;
  textureColor colorFormat;
  textureSamples samples;
  int					width;
  int					height;			// not needed for cube maps
  int					numLevels;		// if 0, will be 1 for NEAREST / LINEAR filters, otherwise based on size
  bool				gammaMips;		// if true, mips will be generated with gamma correction
  bool				readback;		// 360 specific - cpu reads back from this texture, so allocate with cached memory
};

struct Image
{
  
  // image manager
  // render backend

  std::string imgName;
  // cubeMap type

  // usageType
  // imgOpts

  // tex filter type
  // tex repeat type

  bool isSwapchainImage;

  VkFormat internalFormat;
  VkImage apiImage;
  VkImageView view;
  VkImageLayout layout;
  VkSampler sampler;

  VmaAllocation allocation;
	static std::vector<VmaAllocation> allocationGarbage[2];

  static int garbageIndex;
	static std::vector<VkImage> imageGarbage[2];
	static std::vector<VkImageView> viewGarbage[2];
	static std::vector<VkSampler> samplerGarbage[2];
};

//void CreateFromSwapImage(VkImage, VkImageView imageView, VkFormat format, const VkExtent2D& extent);

struct ImageManager
{
  bool insideLevelLoad = false;
  bool preloadingMapImages = false;

  Image* defaultImage;
  Image* flatNormalImage;

  Image* whiteImage;// full of 0xff
  Image* blackImage;//full of 0x00

  Image* currentRenderImage; // post process
  Image* currentDepthImage; // motion blur

  Image* accumImage;

  Image* loadingIconImage;

  std::vector<Image*> images;
  u32 imageHash;
};

//extern ImageManager	*globalImages;

//Image* ImageFromFile(const char* name); // filter, repeate, usage, cubemap

//Image* ScratchImage(const char* name); // imgOpt

//Image* LoadImage(const char* name);// look for loaded