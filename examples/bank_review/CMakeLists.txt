# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
cmake_minimum_required(VERSION 3.24)
project(UmicomBankReview LANGUAGES C)
include(CTest)
include(GNUInstallDirs)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
option(UMICOM_BANK_REVIEW_SQLITE "Compile the canonical Data Server SQLite backend" ON)
option(UMICOM_BANK_REVIEW_SANITIZE "Enable address/undefined behaviour checks" OFF)
option(UMICOM_BANK_REVIEW_GTK4 "Build the actual GTK banking adapter and its tests" OFF)
get_filename_component(_framework "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
function(umicom_apply_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /WX)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
endfunction()
function(umicom_apply_sanitizers target)
    if(UMICOM_BANK_REVIEW_SANITIZE AND NOT MSVC)
        target_compile_options(${target} PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
        target_link_options(${target} PRIVATE -fsanitize=address,undefined)
    endif()
endfunction()
# The standalone review uses the same hash, CSV and status symbols as the
# Framework-built receipt and report paths. Declare their owners before the
# banking module so configured tests also see canonical file reservation.
add_library(umicom_base STATIC
    "${_framework}/src/base/status.c" "${_framework}/src/base/sha256.c"
    "${_framework}/src/base/utf8.c" "${_framework}/src/base/csv_document.c"
    "${_framework}/src/base/value_archive.c")
add_library(Umicom::base ALIAS umicom_base)
add_library(umicom_platform STATIC
    "${_framework}/src/platform/path.c" "${_framework}/src/platform/filesystem.c"
    "${_framework}/src/platform/output_file.c" "${_framework}/src/platform/input_file.c")
add_library(Umicom::platform ALIAS umicom_platform)
set_target_properties(umicom_base PROPERTIES EXPORT_NAME base)
set_target_properties(umicom_platform PROPERTIES EXPORT_NAME platform)
target_link_libraries(umicom_platform PUBLIC Umicom::base)
# Compile canonical dependencies, not replacement implementations. The focused
# finance target is a subset of the installed production finance capability.
add_library(umicom_finance STATIC
    "${_framework}/src/finance/core/types.c" "${_framework}/src/finance/identifier.c"
    "${_framework}/src/finance/banking/types.c" "${_framework}/src/finance/banking/interest_accrual.c"
    "${_framework}/src/finance/accounting/types.c"
    "${_framework}/src/finance/accounting/journal_line.c"
    "${_framework}/src/finance/accounting/journal_entry.c")
add_library(Umicom::finance ALIAS umicom_finance)
add_library(umicom_data STATIC "${_framework}/src/data/data_server.c")
# The simulation copy API uses Framework's checked SQLite snapshot owner.
target_sources(umicom_data PRIVATE "${_framework}/src/data/backup.c")
add_library(Umicom::data ALIAS umicom_data)
target_link_libraries(umicom_finance PUBLIC Umicom::base)
target_link_libraries(umicom_data PUBLIC Umicom::base)
target_link_libraries(umicom_data PRIVATE Umicom::platform)
set_target_properties(umicom_finance PROPERTIES EXPORT_NAME finance)
set_target_properties(umicom_data PROPERTIES EXPORT_NAME data)
set(_config "include(CMakeFindDependencyMacro)\n")
if(UMICOM_BANK_REVIEW_SQLITE)
    find_package(SQLite3 REQUIRED)
    include("${_framework}/cmake/UmicomSQLiteTarget.cmake")
    umicom_resolve_sqlite_target(_sqlite_target)
    target_link_libraries(umicom_data PUBLIC ${_sqlite_target})
    target_compile_definitions(umicom_data PRIVATE UMICOM_HAS_SQLITE=1)
    string(APPEND _config "find_dependency(SQLite3)\ninclude(\"\${CMAKE_CURRENT_LIST_DIR}/UmicomSQLiteTarget.cmake\")\numicom_resolve_sqlite_target(_sqlite_target)\n")
endif()
foreach(_target IN ITEMS umicom_base umicom_platform)
    target_include_directories(${_target} PUBLIC $<BUILD_INTERFACE:${_framework}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
    umicom_apply_warnings(${_target})
    umicom_apply_sanitizers(${_target})
    install(TARGETS ${_target} EXPORT UmicomFrameworkTargets ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
endforeach()
foreach(_target IN ITEMS umicom_finance umicom_data)
    target_include_directories(${_target} PUBLIC $<BUILD_INTERFACE:${_framework}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
    umicom_apply_warnings(${_target})
    umicom_apply_sanitizers(${_target})
    install(TARGETS ${_target} EXPORT UmicomFrameworkTargets ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
endforeach()
if(UMICOM_BANK_REVIEW_GTK4)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4>=4.10)
    # The banking adapter uses GTK directly; no stub Framework UI functions.
    add_library(umicom_review_gui_host INTERFACE)
    add_library(Umicom::ui_gtk4 ALIAS umicom_review_gui_host)
    target_link_libraries(umicom_review_gui_host INTERFACE PkgConfig::GTK4)
    install(TARGETS umicom_review_gui_host EXPORT UmicomFrameworkTargets)
    string(APPEND _config "find_dependency(PkgConfig)\npkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4>=4.10)\n")
endif()
include("${_framework}/cmake/UmicomBankOperations.cmake")
install(FILES "${_framework}/include/umicom/base/status.h" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/base)
# Exported subset targets need the headers that describe their included services.
install(FILES "${_framework}/include/umicom/base/sha256.h"
    "${_framework}/include/umicom/base/utf8.h"
    "${_framework}/include/umicom/base/csv_document.h"
    "${_framework}/include/umicom/base/value_archive.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/base)
install(FILES "${_framework}/include/umicom/platform/path.h"
    "${_framework}/include/umicom/platform/filesystem.h"
    "${_framework}/include/umicom/platform/output_file.h"
    "${_framework}/include/umicom/platform/input_file.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/platform)
install(FILES "${_framework}/include/umicom/data/data_server.h" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/data)
install(FILES "${_framework}/include/umicom/data/backup.h" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/data)
install(FILES "${_framework}/include/umicom/finance/types.h" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/finance)
install(DIRECTORY "${_framework}/include/umicom/finance/accounting" "${_framework}/include/umicom/finance/core"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/finance)
install(FILES "${_framework}/include/umicom/finance/banking/types.h"
    "${_framework}/include/umicom/finance/banking/interest_accrual.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/finance/banking)
install(FILES "${_framework}/cmake/UmicomSQLiteTarget.cmake" DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/UmicomBankReview)
install(EXPORT UmicomFrameworkTargets NAMESPACE Umicom:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/UmicomBankReview)
string(APPEND _config "include(\"\${CMAKE_CURRENT_LIST_DIR}/UmicomFrameworkTargets.cmake\")\n")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/UmicomBankReviewConfig.cmake" "${_config}")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/UmicomBankReviewConfig.cmake" DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/UmicomBankReview)
