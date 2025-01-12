#pragma once

#ifdef __cplusplus

// Standard Library
#include <string>
#include <memory>
#include <iostream>
#include <sstream>
#include <vector>
#include <array>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <functional>
#include <cstdint>
#include <ctime>
#include <chrono>
#include <fstream>
#include <sstream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <queue>

// OpenGL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Engine
#include "Logging.h"
#endif

// Platform detection
#ifdef _WIN32
    #define PLATFORM_WINDOWS
#elif defined(__linux__)
    #define PLATFORM_LINUX
#elif defined(__APPLE__)
    #define PLATFORM_MAC
#endif