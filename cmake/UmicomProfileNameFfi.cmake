#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomProfileNameFfi.cmake
#
# PURPOSE:
#   Compose a small portable foreign-language adapter independently of account and GUI backends.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

include_guard(GLOBAL)
if(NOT TARGET Umicom::profile_name)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomProfileName.cmake")
endif()
add_library(umicom_profile_name_ffi STATIC
    "${CMAKE_CURRENT_LIST_DIR}/../adapters/ffi/profile_name.c")
add_library(Umicom::profile_name_ffi ALIAS umicom_profile_name_ffi)
set_target_properties(umicom_profile_name_ffi PROPERTIES EXPORT_NAME profile_name_ffi
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO POSITION_INDEPENDENT_CODE YES)
target_link_libraries(umicom_profile_name_ffi PUBLIC Umicom::profile_name)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom_profile_name_ffi)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom_profile_name_ffi)
endif()
