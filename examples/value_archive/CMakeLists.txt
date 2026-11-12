# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# An installed consumer uses exported targets and public headers only.
cmake_minimum_required(VERSION 3.24)
project(UmicomPortableState LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
find_package(UmicomFramework CONFIG REQUIRED)
add_executable(umicom-portable-state main.c)
target_link_libraries(umicom-portable-state PRIVATE Umicom::platform Umicom::data)
include(CTest)
if(BUILD_TESTING)
    add_test(NAME portable-state-memory COMMAND umicom-portable-state)
endif()

# Use explicit staging storage without relying on the consuming thread stack.
add_executable(umicom-workspace-import workspace.c)
target_link_libraries(umicom-workspace-import PRIVATE Umicom::platform)
if(BUILD_TESTING)
    add_test(NAME workspace-import COMMAND umicom-workspace-import)
endif()

# This independent target consumes the installed public project codec.
add_executable(umicom-native-state-comparison compare.c)
target_link_libraries(umicom-native-state-comparison PRIVATE Umicom::project)
if(WIN32)
    enable_language(RC)
endif()
umicom_apply_application_branding(TARGET umicom-native-state-comparison
    PRODUCT_NAME "Umicom Native State Comparison"
    INTERNAL_NAME umicom-native-state-comparison NO_RUNTIME_DEPLOYMENT)
if(BUILD_TESTING)
    add_test(NAME native-state-comparison COMMAND umicom-native-state-comparison)
endif()
