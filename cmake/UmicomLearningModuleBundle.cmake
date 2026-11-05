#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomLearningModuleBundle.cmake
#
# PURPOSE:
#   Bundle explicitly selected content modules beside an application executable.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#---------------------------------------------------------------------------

include_guard(GLOBAL)
include(GNUInstallDirs)

# A module consists of a learning manifest and its declared resources. It does
# not contain a post-install program. The application owns the offer list;
# Framework validates and copies the payload without writing source resources.
function(umicom_bundle_learning_module)
    cmake_parse_arguments(M "NO_INSTALL" "TARGET;NAME;SOURCE_DIR;PACK_ID;PRODUCT" "EXTRA_FILES" ${ARGN})
    if(NOT TARGET "${M_TARGET}" OR NOT M_NAME MATCHES "^[A-Za-z0-9_-]+$")
        message(FATAL_ERROR "A learning module needs an existing target and a simple folder name")
    endif()
    if(NOT IS_ABSOLUTE "${M_SOURCE_DIR}")
        message(FATAL_ERROR "Learning module source must be an explicit absolute directory")
    endif()
    set(UMI_MODULE_SOURCE "${M_SOURCE_DIR}")
    set(UMI_MODULE_ID "${M_PACK_ID}")
    set(UMI_MODULE_PRODUCT "${M_PRODUCT}")
    set(UMI_MODULE_EXTRA_FILES "${M_EXTRA_FILES}")
    # Configure validates the manifest without rewriting its resource hashes.
    # Staging repeats validation on every requested application build so a
    # content-only update is not dependent on relinking the application.
    set(UMI_MODULE_VALIDATE_ONLY ON)
    include("${CMAKE_CURRENT_FUNCTION_LIST_DIR}/UmicomStageLearningModule.cmake")
    set(_files "${UMI_MODULE_FILES}")
    set(_stage "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/UmicomStageLearningModule.cmake")
    set(_config "${CMAKE_CURRENT_BINARY_DIR}/learning-modules/${M_NAME}.cmake")
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/learning-modules")
    # Bracket quoting preserves spaces and prevents path text being evaluated.
    foreach(_value IN ITEMS "${M_SOURCE_DIR}" "${M_PACK_ID}" "${M_PRODUCT}" "${M_EXTRA_FILES}")
        if(_value MATCHES "\\]====\\]")
            message(FATAL_ERROR "Unsupported bracket delimiter in module configuration")
        endif()
    endforeach()
    file(WRITE "${_config}"
        "set(UMI_MODULE_SOURCE [====[${M_SOURCE_DIR}]====])\n"
        "set(UMI_MODULE_ID [====[${M_PACK_ID}]====])\n"
        "set(UMI_MODULE_PRODUCT [====[${M_PRODUCT}]====])\n"
        "set(UMI_MODULE_EXTRA_FILES [====[${M_EXTRA_FILES}]====])\n"
        "include([====[${_stage}]====])\n")
    add_custom_target("${M_TARGET}-module-${M_NAME}"
        COMMAND "${CMAKE_COMMAND}"
            "-DUMI_MODULE_DESTINATION=$<TARGET_FILE_DIR:${M_TARGET}>/learning-modules/${M_NAME}"
            -P "${_config}"
        COMMENT "Preparing ${M_NAME} learning module"
        VERBATIM)
    add_dependencies("${M_TARGET}" "${M_TARGET}-module-${M_NAME}")
    # Test bundles exercise the production staging contract in an isolated
    # executable directory. They must never add fixture content to an install.
    # Default application installation retains the existing checks and files.
    if(NOT M_NO_INSTALL)
        # Installation rechecks content, including installs performed without a build.
        install(CODE "set(UMI_MODULE_VALIDATE_ONLY ON)\ninclude([====[${_config}]====])\n")
        foreach(_file IN LISTS _files)
            get_filename_component(_parent "${_file}" DIRECTORY)
            install(FILES "${M_SOURCE_DIR}/${_file}"
                DESTINATION "${CMAKE_INSTALL_BINDIR}/learning-modules/${M_NAME}/${_parent}")
        endforeach()
    endif()
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${M_SOURCE_DIR}/learning-pack.json")
endfunction()
