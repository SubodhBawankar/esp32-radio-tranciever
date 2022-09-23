# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

set(COMPONENT_SRC "src/SPI.c")
set(COMPONENT_ADD_INCLUDEDIRS "./include")
register_component()
