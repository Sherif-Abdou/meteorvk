cmake_minimum_required(VERSION 3.26)
project(vulkan_engine)

set(CMAKE_CXX_STANDARD 20)

if (WIN32)
    include_directories("C:\\VulkanSDK\\glfw\\glfw-3.3.8.bin.WIN64\\include")
    include_directories("C:\\VulkanSDK\\1.3.250.0\\Include")
    include_directories("C:\\VulkanSDK\\glm")

    link_directories("C:\\VulkanSDK\\glfw\\glfw-3.3.8.bin.WIN64\\lib-vc2022")
    link_directories("C:\\VulkanSDK\\1.3.250.0\\Lib")
endif (WIN32)
find_package(Vulkan REQUIRED)
if (APPLE)
    include_directories("/Users/sherif/VulkanSDK/1.3.250.1/macOS/include")
    link_directories("/Users/sherif/VulkanSDK/1.3.250.1/macOS/lib")
    include_directories("/opt/homebrew/include")
    link_directories("/opt/homebrew/lib")
endif(APPLE)


add_executable(vulkan_engine main.cpp
        src/VulkanContext.h
        src/VulkanContext.cpp
        src/VulkanAllocator.h
        src/VulkanAllocator.cpp
        src/Vertex.h
        src/Vertex.cpp
        src/VertexBuffer.h
        src/VertexBuffer.cpp
)
if (APPLE)
    target_link_libraries(vulkan_engine "-framework CoreFoundation")
    target_link_libraries(vulkan_engine "-framework CoreGraphics")
    target_link_libraries(vulkan_engine "-framework IOKit")
    target_link_libraries(vulkan_engine "-framework Cocoa")
    target_link_libraries(vulkan_engine vulkan glfw)
    target_include_directories(vulkan_engine PUBLIC ${VULKAN_INCLUDE_DIRS})
endif (APPLE)
