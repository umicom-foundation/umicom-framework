# Umicom Framework application module example
# Sammy Hegab, Umicom Foundation. MIT licence.
cmake_minimum_required(VERSION 3.24)
project(UmicomModuleExample LANGUAGES C)

# The suite supplies Framework targets. A standalone build uses the installed
# SDK; there is no second Framework checkout embedded in this module.
if(NOT TARGET Umicom::runtime)
    find_package(UmicomFramework CONFIG REQUIRED COMPONENTS runtime)
endif()
if(PROJECT_IS_TOP_LEVEL)
    include(CTest)
endif()
add_executable(umicom-module-example main.c)
set_target_properties(umicom-module-example PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED ON C_EXTENSIONS OFF)
target_link_libraries(umicom-module-example PRIVATE Umicom::runtime)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom-module-example)
endif()

# The suite supplies the installer component before entering this directory.
install(TARGETS umicom-module-example RUNTIME DESTINATION bin)
install(FILES application.umicom.yaml
    DESTINATION share/umicom/applications/module-example)
if(BUILD_TESTING)
    add_test(NAME module-example.manifest
        COMMAND umicom-module-example "${CMAKE_CURRENT_SOURCE_DIR}/application.umicom.yaml")
    set_tests_properties(module-example.manifest PROPERTIES
        PASS_REGULAR_EXPRESSION "UMICOM_APPLICATIONS_BUILD_MODULE_EXAMPLE")
endif()
