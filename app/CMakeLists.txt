cmake_minimum_required(VERSION 3.13)

project(SampleApp VERSION 1.0)

# Specify C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

# Include PostgreSQL
find_package(PostgreSQL REQUIRED)

# Add executable
add_executable(SampleApp src/main.c)

# Link the executable to the C++ library
target_link_libraries(SampleApp LINK_PUBLIC MsiAuthLib)

# Link libpq
target_link_libraries(SampleApp PRIVATE PostgreSQL::PostgreSQL)

# Include the headers from the library
target_include_directories(SampleApp PRIVATE ../lib/include)