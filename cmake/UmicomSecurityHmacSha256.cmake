#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomSecurityHmacSha256.cmake
#
# PURPOSE:
#   Register reusable C23 HMAC-SHA-256 over the canonical Framework SHA core.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
get_filename_component(_umicom_hmac_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

# The existing Framework base target is always the authority in full SDK
# builds. The federated-wire standalone target already imports the very same
# source; reuse that target rather than compiling an independent SHA core.
if(TARGET Umicom::base)
    set(_umicom_hmac_sha_owner Umicom::base)
elseif(TARGET umicom_ai_federated_wire_base)
    set(_umicom_hmac_sha_owner umicom_ai_federated_wire_base)
else()
    if(NOT TARGET umicom_security_hmac_sha_base)
        add_library(umicom_security_hmac_sha_base STATIC
            "${_umicom_hmac_root}/src/base/sha256.c")
        target_include_directories(umicom_security_hmac_sha_base PUBLIC
            "${_umicom_hmac_root}/include")
        target_compile_features(umicom_security_hmac_sha_base PUBLIC c_std_23)
    endif()
    set(_umicom_hmac_sha_owner umicom_security_hmac_sha_base)
endif()

if(NOT TARGET umicom_security_hmac_sha256)
    add_library(umicom_security_hmac_sha256 STATIC
        "${_umicom_hmac_root}/src/security/hmac_sha256.c")
    add_library(Umicom::SecurityHmacSha256 ALIAS umicom_security_hmac_sha256)
    target_include_directories(umicom_security_hmac_sha256 PUBLIC
        "${_umicom_hmac_root}/include")
    target_link_libraries(umicom_security_hmac_sha256 PUBLIC
        ${_umicom_hmac_sha_owner})
    target_compile_features(umicom_security_hmac_sha256 PUBLIC c_std_23)
    if(MSVC)
        target_compile_options(umicom_security_hmac_sha256 PRIVATE /W4)
    else()
        target_compile_options(umicom_security_hmac_sha256 PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
endif()
