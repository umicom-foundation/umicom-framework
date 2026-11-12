# -----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomAiWorkerTransport.cmake
# PURPOSE: Publish a reusable native worker adapter without replacing AI services.
# EDUCATIONAL OVERVIEW: Both full Framework clients and a focused thin host can
# link the same source through Umicom::ai_worker_transport.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
# -----------------------------------------------------------------------------
include_guard(GLOBAL)
find_package(PkgConfig REQUIRED)
pkg_check_modules(UMICOM_WORKER_CURL REQUIRED IMPORTED_TARGET libcurl>=7.85)
pkg_check_modules(UMICOM_WORKER_JSONC REQUIRED IMPORTED_TARGET json-c>=0.15)
add_library(umicom_ai_worker_transport STATIC "${CMAKE_CURRENT_LIST_DIR}/../src/ai_workspace/worker_transport.c")
add_library(Umicom::ai_worker_transport ALIAS umicom_ai_worker_transport)
set_target_properties(umicom_ai_worker_transport PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_include_directories(umicom_ai_worker_transport PUBLIC "${CMAKE_CURRENT_LIST_DIR}/../include")
target_link_libraries(umicom_ai_worker_transport PRIVATE PkgConfig::UMICOM_WORKER_CURL PkgConfig::UMICOM_WORKER_JSONC)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(umicom_ai_worker_transport PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
endif()
