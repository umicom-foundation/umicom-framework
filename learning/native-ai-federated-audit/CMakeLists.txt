#-----------------------------------------------------------------------------
# Umicom Framework
# File: learning/native-ai-federated-audit/CMakeLists.txt
#
# PURPOSE:
#   Run the C23 federated receipt-chain example using the existing native
#   record-chain and SHA-256 sources without creating another hash engine.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomNativeFederatedAudit LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
include(CTest)
get_filename_component(_framework_root "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
if(NOT TARGET Umicom::record_chain)
    add_library(umicom_record_chain STATIC
        "${_framework_root}/src/base/sha256.c"
        "${_framework_root}/src/record_chain/chain.c")
    add_library(Umicom::record_chain ALIAS umicom_record_chain)
    target_include_directories(umicom_record_chain PUBLIC "${_framework_root}/include")
    target_compile_features(umicom_record_chain PUBLIC c_std_23)
endif()
include("${_framework_root}/cmake/UmicomAiFederatedAuditChain.cmake")
add_executable(umicom-native-federated-audit-demo demo.c)
target_link_libraries(umicom-native-federated-audit-demo PRIVATE
    Umicom::AiFederatedAuditChain)
if(NOT MSVC)
    foreach(_t IN ITEMS umicom_record_chain umicom_ai_federated_audit_chain_test
        umicom-native-federated-audit-demo)
        if(TARGET ${_t})
            target_compile_options(${_t} PRIVATE
                -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
        endif()
    endforeach()
endif()
