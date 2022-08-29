# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/aryan/esp/esp-idf/components/bootloader/subproject"
  "/home/aryan/eklavya/esp32-radio-tranciever/1_gpio_control/build/bootloader"
  "/home/aryan/eklavya/esp32-radio-tranciever/1_gpio_control/build/bootloader-prefix"
  "/home/aryan/eklavya/esp32-radio-tranciever/1_gpio_control/build/bootloader-prefix/tmp"
  "/home/aryan/eklavya/esp32-radio-tranciever/1_gpio_control/build/bootloader-prefix/src/bootloader-stamp"
  "/home/aryan/eklavya/esp32-radio-tranciever/1_gpio_control/build/bootloader-prefix/src"
  "/home/aryan/eklavya/esp32-radio-tranciever/1_gpio_control/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/aryan/eklavya/esp32-radio-tranciever/1_gpio_control/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
