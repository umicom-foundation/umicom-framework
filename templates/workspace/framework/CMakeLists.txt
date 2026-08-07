include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_library(umicom_base STATIC
    src/base/status.c
    src/base/result.c
    src/base/memory.c
)
add_library(Umicom::base ALIAS umicom_base)
target_include_directories(umicom_base PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_base)
umicom_apply_sanitizers(umicom_base)

add_library(umicom_diagnostics STATIC
    src/diagnostics/diagnostic.c
    src/diagnostics/log.c
)
add_library(Umicom::diagnostics ALIAS umicom_diagnostics)
target_link_libraries(umicom_diagnostics PUBLIC Umicom::base)
target_include_directories(umicom_diagnostics PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_diagnostics)
umicom_apply_sanitizers(umicom_diagnostics)

add_library(umicom_messaging STATIC
    src/messaging/event_bus.c
    src/messaging/command_bus.c
    src/messaging/query_bus.c
    src/messaging/journal.c
)
add_library(Umicom::messaging ALIAS umicom_messaging)
target_link_libraries(umicom_messaging PUBLIC Umicom::base)
target_include_directories(umicom_messaging PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_messaging)
umicom_apply_sanitizers(umicom_messaging)

find_package(SQLite3 QUIET)

add_library(umicom_data STATIC
    src/data/data_server.c
)
add_library(Umicom::data ALIAS umicom_data)
target_link_libraries(umicom_data PUBLIC Umicom::base)
if(SQLite3_FOUND)
    target_link_libraries(umicom_data PUBLIC SQLite::SQLite3)
    target_compile_definitions(umicom_data PUBLIC UMICOM_HAS_SQLITE=1)
endif()
target_include_directories(umicom_data PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_data)
umicom_apply_sanitizers(umicom_data)

add_library(umicom_platform STATIC
    src/platform/config.c
    src/platform/filesystem.c
    src/platform/process.c
    src/platform/workspace.c
    src/platform/document.c
    src/platform/clock.c
)
add_library(Umicom::platform ALIAS umicom_platform)
target_link_libraries(umicom_platform PUBLIC Umicom::base Umicom::diagnostics)
target_include_directories(umicom_platform PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_platform)
umicom_apply_sanitizers(umicom_platform)

add_library(umicom_runtime STATIC
    src/runtime/module_registry.c
    src/runtime/master_controller.c
    src/runtime/scheduler.c
)
add_library(Umicom::runtime ALIAS umicom_runtime)
target_link_libraries(umicom_runtime PUBLIC
    Umicom::base
    Umicom::diagnostics
    Umicom::messaging
    Umicom::data
    Umicom::platform
)
target_include_directories(umicom_runtime PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_runtime)
umicom_apply_sanitizers(umicom_runtime)


add_library(umicom_plugin STATIC
    src/plugin/plugin.c
)
add_library(Umicom::plugin ALIAS umicom_plugin)
target_link_libraries(umicom_plugin PUBLIC Umicom::base Umicom::runtime)
target_include_directories(umicom_plugin PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
if(UNIX AND NOT APPLE)
    target_link_libraries(umicom_plugin PRIVATE dl)
endif()
umicom_apply_warnings(umicom_plugin)
umicom_apply_sanitizers(umicom_plugin)

add_library(umicom_scaffold STATIC
    src/scaffold/scaffold.c
)
add_library(Umicom::scaffold ALIAS umicom_scaffold)
target_link_libraries(umicom_scaffold PUBLIC Umicom::base Umicom::platform)
target_include_directories(umicom_scaffold PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
umicom_apply_warnings(umicom_scaffold)
umicom_apply_sanitizers(umicom_scaffold)

add_library(umicom_framework INTERFACE)
add_library(Umicom::Framework ALIAS umicom_framework)
target_link_libraries(umicom_framework INTERFACE
    Umicom::base
    Umicom::diagnostics
    Umicom::messaging
    Umicom::data
    Umicom::platform
    Umicom::runtime
    Umicom::plugin
    Umicom::scaffold
)

add_executable(umicom-framework-demo examples/framework_demo/main.c)
target_link_libraries(umicom-framework-demo PRIVATE Umicom::Framework)
umicom_apply_warnings(umicom-framework-demo)
umicom_apply_sanitizers(umicom-framework-demo)

if(BUILD_TESTING)
    add_executable(umicom-framework-tests tests/test_framework.c)
    target_link_libraries(umicom-framework-tests PRIVATE Umicom::Framework)
    umicom_apply_warnings(umicom-framework-tests)
    umicom_apply_sanitizers(umicom-framework-tests)
    add_test(NAME framework.foundation COMMAND umicom-framework-tests)

    if(EXISTS "${CMAKE_SOURCE_DIR}/templates/workspace")
        add_executable(umicom-scaffold-tests tests/test_scaffold.c)
        target_link_libraries(umicom-scaffold-tests PRIVATE Umicom::Framework)
        target_compile_definitions(umicom-scaffold-tests PRIVATE UMICOM_TEST_TEMPLATE_ROOT="${CMAKE_SOURCE_DIR}/templates")
        umicom_apply_warnings(umicom-scaffold-tests)
        umicom_apply_sanitizers(umicom-scaffold-tests)
        add_test(NAME framework.scaffold COMMAND umicom-scaffold-tests)
    endif()
endif()

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(TARGETS
    umicom_base umicom_diagnostics umicom_messaging umicom_data umicom_platform umicom_runtime umicom_plugin umicom_scaffold umicom_framework
    EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(EXPORT UmicomFrameworkTargets
    FILE UmicomFrameworkTargets.cmake
    NAMESPACE Umicom::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/UmicomFramework
)
configure_package_config_file(
    cmake/UmicomFrameworkConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/UmicomFrameworkConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/UmicomFramework
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/UmicomFrameworkConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/UmicomFrameworkConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/UmicomFrameworkConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/UmicomFramework
)
