# Umicom Framework - installed SDK metadata
# Sammy Hegab, Umicom Foundation. MIT licence.
include_guard(GLOBAL)

# Generate data from this Framework build, including the actual archive name for
# each configuration. Inspectors must not substitute their own running version.
file(STRINGS "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/base/version.h"
    _abi_lines REGEX "^#define UMICOM_FRAMEWORK_ABI_VERSION [0-9]+U?$")
list(LENGTH _abi_lines _abi_count)
if(NOT _abi_count EQUAL 1)
    message(FATAL_ERROR "Cannot determine the Framework ABI for installed SDK metadata")
endif()
string(REGEX REPLACE ".*UMICOM_FRAMEWORK_ABI_VERSION ([0-9]+)U?$" "\\1" _sdk_abi "${_abi_lines}")

# The receipt is relocatable only when the SDK headers and archives are beneath
# the prefix. Existing absolute/custom installs remain supported by CMake; they
# do not receive a receipt that would falsely claim a relocatable layout.
set(_sdk_receipt_supported TRUE)
foreach(_directory IN ITEMS "${CMAKE_INSTALL_INCLUDEDIR}" "${CMAKE_INSTALL_LIBDIR}")
    if(IS_ABSOLUTE "${_directory}" OR _directory STREQUAL "" OR
       NOT _directory MATCHES "^[A-Za-z0-9_+./ -]+$" OR
       _directory MATCHES "(^|/)(\\.\\.?|[^/]*[ .]| [^/]*)(/|$)" OR
       _directory MATCHES "//" OR _directory MATCHES "/$")
        set(_sdk_receipt_supported FALSE)
    endif()
endforeach()
if(_sdk_receipt_supported)
    file(GENERATE
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/sdk-metadata/$<CONFIG>/sdk.properties"
        CONTENT "schema=umicom.sdk\nframework_version=${PROJECT_VERSION}\nframework_abi=${_sdk_abi}\ninclude_directory=${CMAKE_INSTALL_INCLUDEDIR}\nlibrary_directory=${CMAKE_INSTALL_LIBDIR}\nbase_library=${CMAKE_INSTALL_LIBDIR}/$<TARGET_FILE_NAME:umicom_base>\n")
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/sdk-metadata/$<CONFIG>/sdk.properties"
        DESTINATION share/umicom/framework COMPONENT Framework)
else()
    message(STATUS "SDK metadata unavailable for this non-relative installation layout")
endif()
