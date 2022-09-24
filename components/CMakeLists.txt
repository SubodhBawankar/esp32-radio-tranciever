# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

set(srcs "spi.c")
set(include_dirs "include")
idf_component_register(SRCS "${srcs}"
INCLUDE_DIRS "${include_dirs}"
REQUIRES driver esp32 freertos log
#set(EXTRA_COMPONENT_DIRS ../components)
#set(COMPONENT_SRC "src/spi.c")
#set(COMPONENT_ADD_INCLUDEDIRS "./include")
#register_component()
