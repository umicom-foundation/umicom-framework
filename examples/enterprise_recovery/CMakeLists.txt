# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Standalone SDK laboratory: canonical sources, not replacement service stubs.
cmake_minimum_required(VERSION 3.24)
project(UmicomEnterpriseRecovery LANGUAGES C)
include(CTest)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
get_filename_component(_default_root "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
set(UMICOM_FRAMEWORK_SOURCE "${_default_root}" CACHE PATH "Complete Framework checkout")
option(UMICOM_RECOVERY_SQLITE "Use the canonical SQLite Data Server backend" ON)
option(UMICOM_RECOVERY_SANITIZERS "Address/undefined-behaviour instrumentation" OFF)
find_package(Threads REQUIRED)
function(umicom_apply_warnings target)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
endfunction()
function(umicom_apply_sanitizers target)
    if(UMICOM_RECOVERY_SANITIZERS AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PUBLIC -fsanitize=address,undefined -fno-omit-frame-pointer)
        target_link_options(${target} PUBLIC -fsanitize=address,undefined)
    endif()
endfunction()
add_library(umicom_recovery_dependencies STATIC
    "${UMICOM_FRAMEWORK_SOURCE}/src/base/status.c"
    "${UMICOM_FRAMEWORK_SOURCE}/src/platform/threading.c")
set_target_properties(umicom_recovery_dependencies PROPERTIES EXPORT_NAME recovery_dependencies)
add_library(umicom_data STATIC "${UMICOM_FRAMEWORK_SOURCE}/src/data/data_server.c")
add_library(Umicom::data ALIAS umicom_data)
set_target_properties(umicom_data PROPERTIES EXPORT_NAME data)
add_library(umicom_security STATIC
    "${UMICOM_FRAMEWORK_SOURCE}/src/security/authorisation.c"
    "${UMICOM_FRAMEWORK_SOURCE}/src/security/permission.c"
    "${UMICOM_FRAMEWORK_SOURCE}/src/security/policy.c"
    "${UMICOM_FRAMEWORK_SOURCE}/src/security/role.c")
add_library(Umicom::security ALIAS umicom_security)
set_target_properties(umicom_security PROPERTIES EXPORT_NAME security)
target_link_libraries(umicom_security PUBLIC umicom_recovery_dependencies)
target_link_libraries(umicom_recovery_dependencies PUBLIC Threads::Threads)
if(UNIX)
    target_compile_definitions(umicom_recovery_dependencies PRIVATE _POSIX_C_SOURCE=200809L)
endif()
if(UMICOM_RECOVERY_SQLITE)
    find_package(SQLite3 REQUIRED)
    include("${UMICOM_FRAMEWORK_SOURCE}/cmake/UmicomSQLiteTarget.cmake")
    umicom_resolve_sqlite_target(_sqlite)
    target_link_libraries(umicom_data PUBLIC ${_sqlite})
    target_compile_definitions(umicom_data PUBLIC UMICOM_HAS_SQLITE=1)
endif()
foreach(target umicom_recovery_dependencies umicom_data umicom_security)
    target_include_directories(${target} PUBLIC
        $<BUILD_INTERFACE:${UMICOM_FRAMEWORK_SOURCE}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
    umicom_apply_warnings(${target})
    umicom_apply_sanitizers(${target})
    install(TARGETS ${target} EXPORT UmicomFrameworkTargets ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")
endforeach()
include("${UMICOM_FRAMEWORK_SOURCE}/cmake/UmicomEnterpriseWorkspace.cmake")
foreach(path base data platform security)
    install(DIRECTORY "${UMICOM_FRAMEWORK_SOURCE}/include/umicom/${path}"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom")
endforeach()
install(FILES "${UMICOM_FRAMEWORK_SOURCE}/cmake/UmicomSQLiteTarget.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/UmicomFramework")
configure_package_config_file("${CMAKE_CURRENT_LIST_DIR}/UmicomFrameworkConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/UmicomFrameworkConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/UmicomFramework")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/UmicomFrameworkConfig.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/UmicomFramework")
install(EXPORT UmicomFrameworkTargets NAMESPACE Umicom:: DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/UmicomFramework")
