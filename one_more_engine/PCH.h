#pragma once

#include <type_traits>
#include <cstdint>
#include <cmath>
#include <chrono>
#include <vector>
#include <array>
#include <stddef.h>
#include <iostream>
#include <unordered_map>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtc/quaternion.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vma.h"

#include "types.h"

#define platform_assert(statment) assert(statment)
#define assert_16_byte_aligned( ptr )		platform_assert( ( ((intptr_t)(ptr)) & 15 ) == 0 )

#include "CVar.h"

#include "Staging.h"

#include "BufferObject.h"

#include "vkAllocator.h"

#include "Image.h"

#include "Vulkan.h"

#include "Renderer.h"

#include "Input.h"

#include "Window.h"
#include "Application.h"

