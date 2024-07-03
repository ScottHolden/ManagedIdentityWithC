cmake_minimum_required(VERSION 3.13)

project(MsiAuthLib VERSION 1.0)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include Azure SDK Identitiy
#  Ideally this should be VCPKG instead but this is fine for a demo, just using a known working commit
include(FetchContent)
#  Workaround to remove amqp build steps
set(BUILD_WINDOWS_UWP True)
set(DISABLE_AZURE_CORE_OPENTELEMETRY True)
set(WARNINGS_AS_ERRORS False)
FetchContent_Declare(azuresdkforcpp
    # Set the SDK URL path and release tag
    GIT_REPOSITORY      https://github.com/Azure/azure-sdk-for-cpp.git
    GIT_TAG             b0aa9bb924164bff926929be86d00f8c3f69cd72)
FetchContent_GetProperties(azuresdkforcpp)
if(NOT azuresdkforcpp_POPULATED)
    FetchContent_Populate(azuresdkforcpp)
    add_subdirectory(${azuresdkforcpp_SOURCE_DIR} ${azuresdkforcpp_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# Define the library
add_library(MsiAuthLib src/msi_auth_lib.cpp)

# Specify include directory for the library
target_link_libraries(MsiAuthLib Azure::azure-identity)
target_include_directories(MsiAuthLib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")