#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomSdkArchive.cmake
#
# PURPOSE:
#   Package the registered SDK component without source checkouts or product builds.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

# The archive uses the existing FrameworkSDK install component as its sole
# authority. Keep packaging independent of a parent's CPack settings and ALL
# target: optional SDK libraries and exported developer tools join the existing
# aggregate, while unrelated product executables remain outside this dependency.
function(umicom_add_sdk_archive)
    if(NOT ARGC EQUAL 0 OR NOT TARGET umicom-sdk-libraries)
        message(FATAL_ERROR "SDK archive requires the registered SDK library target")
    endif()
    if(TARGET umicom-sdk-package)
        message(FATAL_ERROR "SDK archive target name is already in use")
    endif()
    foreach(_directory CMAKE_INSTALL_BINDIR CMAKE_INSTALL_LIBDIR CMAKE_INSTALL_INCLUDEDIR CMAKE_INSTALL_DATADIR)
        if(NOT DEFINED ${_directory} OR "${${_directory}}" STREQUAL "" OR
           IS_ABSOLUTE "${${_directory}}" OR
           "${${_directory}}" MATCHES "(^|/)\\.\\.?(/|$)|[;\\\\]" OR
           "${${_directory}}" MATCHES "^/|//|/$")
            message(FATAL_ERROR "SDK archive requires relative installation directories: ${_directory}")
        endif()
    endforeach()
    foreach(_field PROJECT_NAME PROJECT_VERSION CMAKE_SYSTEM_NAME CMAKE_C_COMPILER_ID CMAKE_SIZEOF_VOID_P)
        if(NOT "${${_field}}" MATCHES "^[A-Za-z0-9_.+-]+$")
            message(FATAL_ERROR "SDK archive requires a declared portable ${_field}")
        endif()
    endforeach()
    # A per-configuration output prevents Debug and Release replacing one
    # another. Pointer width is labelled as such, never as an ISA guarantee.
    set(_sdk_archive_name "umicom-framework-sdk-${PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}-${CMAKE_C_COMPILER_ID}-p${CMAKE_SIZEOF_VOID_P}-$<CONFIG>")
    set(_sdk_archive_build "${CMAKE_CURRENT_BINARY_DIR}")
    set(_sdk_archive_output "${CMAKE_CURRENT_BINARY_DIR}/sdk-packages/$<CONFIG>")
    foreach(_value _sdk_archive_build _sdk_archive_output)
        # Bracket arguments preserve spaces, quotes and literal dollar signs.
        # Refuse their delimiter and CPack's project-list separator explicitly.
        if("${${_value}}" MATCHES "\\]==\\]|[;\r\n]")
            message(FATAL_ERROR "SDK archive path contains an unsupported delimiter")
        endif()
    endforeach()
    get_filename_component(_cmake_bin "${CMAKE_COMMAND}" DIRECTORY)
    find_program(_sdk_cpack NAMES cpack HINTS "${_cmake_bin}" NO_DEFAULT_PATH REQUIRED)
    set(_configured "${CMAKE_CURRENT_BINARY_DIR}/sdk-packaging/CPackSdkConfig.cmake.in")
    configure_file("${CMAKE_CURRENT_FUNCTION_LIST_DIR}/UmicomSdkArchiveConfig.cmake.in"
        "${_configured}" @ONLY)
    file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/sdk-packaging/$<CONFIG>/CPackSdkConfig.cmake"
        INPUT "${_configured}")
    add_custom_target(umicom-sdk-package
        COMMAND "${_sdk_cpack}" --config
            "${CMAKE_CURRENT_BINARY_DIR}/sdk-packaging/$<CONFIG>/CPackSdkConfig.cmake"
            -C "$<CONFIG>"
        DEPENDS umicom-sdk-libraries
        COMMENT "Packaging the Framework SDK and SHA256 checksum"
        VERBATIM USES_TERMINAL)
endfunction()
