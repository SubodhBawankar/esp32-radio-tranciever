
## File Structure:
- ESP32-RADIO-TRANCIEVER is root folder
- folders with numerical starting are of Task ex: "3_oled"
    - Those folders are esp-idf projects.
    - contains main folder containing code
    - sdkconfig for menuconfig 
    - CMakeList.txt
        - components path need to be added in CmakeList.txt.<br>
        ``` set(EXTRA_COMPONENT_DIRS "../components/")```
- root/Component is where all extra library are present. It has:
    - include: contains header files.
    - src: contains definations of functions of header files.
    - u8g2: added as submodule this repo. It is used for display modules for esp
    - CMakeLists.txt: In order to resister this folder as Extra components in Idf we need this file. It maps the locations of alls file is src, include and u8g2.
- documentation folder has all the needed notes to refer this repo and this project.

