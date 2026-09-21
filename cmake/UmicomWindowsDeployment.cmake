#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomWindowsDeployment.cmake
#
# PURPOSE:
#   Own click-ready Windows runtime deployment and the suite installer catalogue.
#   Applications supply identity and targets, not duplicated packaging logic.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
include(CMakeParseArguments)
include("${CMAKE_CURRENT_LIST_DIR}/windows/DeploymentCommon.cmake")

option(UMICOM_WINDOWS_DEPLOY_RUNTIME
    "Prepare private Windows DLLs and resources beside branded executables" ON)
set(UMICOM_WINDOWS_RUNTIME_PREFIX "" CACHE PATH
    "One coherent GTK/toolchain runtime prefix, for example C:/msys64/ucrt64")
set(UMICOM_WINDOWS_INSTALLER_TARGETS "" CACHE STRING
    "Optional semicolon-separated subset of registered GUI executable targets")
set(UMICOM_WINDOWS_INSTALLER_HELPERS "umicom" CACHE STRING
    "Built helper executables installed with the shared runtime, without GUI checkboxes")
set(UMICOM_NSIS_EXECUTABLE "" CACHE FILEPATH
    "Optional full path to the build-machine makensis executable")

# Register once, in the same source directory that owns the executable. This
# preserves CMake's TARGET POST_BUILD ownership requirement in nested projects.
function(umicom_prepare_windows_application)
    if(NOT WIN32 OR NOT UMICOM_WINDOWS_DEPLOY_RUNTIME)
        return()
    endif()
    cmake_parse_arguments(DEPLOY "GUI;AUXILIARY" "TARGET;PRODUCT_NAME;RESOURCE_ROOT" "" ${ARGN})
    if(DEPLOY_UNPARSED_ARGUMENTS OR NOT TARGET "${DEPLOY_TARGET}")
        message(FATAL_ERROR "Windows deployment requires an existing executable TARGET")
    endif()
    get_target_property(_type "${DEPLOY_TARGET}" TYPE)
    get_target_property(_imported "${DEPLOY_TARGET}" IMPORTED)
    if(NOT _type STREQUAL "EXECUTABLE" OR _imported)
        message(FATAL_ERROR "Windows deployment requires a locally built executable")
    endif()
    get_target_property(_already "${DEPLOY_TARGET}" UMICOM_WINDOWS_DEPLOYMENT_REGISTERED)
    if(_already)
        return()
    endif()
    if(NOT CMAKE_HOST_WIN32)
        message(FATAL_ERROR
            "Automatic Windows runtime deployment currently requires a Windows build host. "
            "Use UMICOM_WINDOWS_DEPLOY_RUNTIME=OFF for a cross-compilation-only build.")
    endif()
    umicom_windows_require_identifier("${DEPLOY_TARGET}")
    umicom_windows_require_literal("${DEPLOY_PRODUCT_NAME}")
    umicom_windows_require_literal("${DEPLOY_RESOURCE_ROOT}")
    if(NOT DEPLOY_PRODUCT_NAME)
        message(FATAL_ERROR "Windows deployment requires PRODUCT_NAME")
    endif()

    set(_support "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/windows")
    get_filename_component(_framework "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/.." ABSOLUTE)
    if(NOT EXISTS "${_framework}/src/delivery/windows_runtime_bootstrap.c")
        get_filename_component(_framework "${DEPLOY_RESOURCE_ROOT}/../deployment" ABSOLUTE)
    endif()
    foreach(_required_support IN ITEMS src/delivery/windows_runtime_bootstrap.c
            src/delivery/windows_runtime_cache.c include/umicom/delivery/windows_runtime_cache.h)
        if(NOT EXISTS "${_framework}/${_required_support}")
            message(FATAL_ERROR "The Framework Windows deployment SDK is incomplete: ${_required_support}")
        endif()
    endforeach()
    set_property(GLOBAL PROPERTY UMICOM_WINDOWS_DEPLOYMENT_SUPPORT_ROOT "${_framework}")
    # Export the same source support beside the installed resource catalogue.
    # No second implementation is maintained for installed SDK consumers.
    get_property(_support_installed GLOBAL PROPERTY UMICOM_WINDOWS_DEPLOYMENT_SUPPORT_INSTALLED)
    if(NOT _support_installed)
        include(GNUInstallDirs)
        foreach(_source IN ITEMS src/delivery/windows_runtime_bootstrap.c
                src/delivery/windows_runtime_cache.c include/umicom/delivery/windows_runtime_cache.h
                LICENSE docs/windows-installation.html)
            get_filename_component(_parent "${_source}" DIRECTORY)
            install(FILES "${_framework}/${_source}"
                DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/deployment/${_parent}"
                COMPONENT Framework)
        endforeach()
        install(FILES "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/UmicomWindowsDeployment.cmake"
            DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/UmicomFramework" COMPONENT Framework)
        install(DIRECTORY "${_support}/"
            DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/UmicomFramework/windows" COMPONENT Framework)
        set_property(GLOBAL PROPERTY UMICOM_WINDOWS_DEPLOYMENT_SUPPORT_INSTALLED TRUE)
    endif()
    set(_prefix "${UMICOM_WINDOWS_RUNTIME_PREFIX}")
    if(NOT _prefix)
        # Prefer the runtime that built the application, never an arbitrary GTK
        # directory found first in a developer's inherited machine PATH.
        get_filename_component(_compiler_bin "${CMAKE_C_COMPILER}" DIRECTORY)
        get_filename_component(_candidate "${_compiler_bin}/.." ABSOLUTE)
        if(EXISTS "${_candidate}/bin/libglib-2.0-0.dll")
            set(_prefix "${_candidate}")
        else()
            message(FATAL_ERROR
                "Set UMICOM_WINDOWS_RUNTIME_PREFIX to the GTK runtime used to link the applications. "
                "For the UCRT64 preset use C:/msys64/ucrt64.")
        endif()
    endif()
    file(TO_CMAKE_PATH "${_prefix}" _prefix)
    if(NOT EXISTS "${_prefix}/bin")
        message(FATAL_ERROR "Windows runtime prefix has no bin directory: ${_prefix}")
    endif()
    umicom_windows_require_literal("${_prefix}")
    if(CMAKE_SIZEOF_VOID_P AND NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
        message(FATAL_ERROR "This Windows deployment backend currently supports 64-bit applications")
    endif()
    if(CMAKE_OBJDUMP AND EXISTS "${CMAKE_OBJDUMP}")
        set(_objdump "${CMAKE_OBJDUMP}")
    else()
        find_program(_objdump NAMES objdump llvm-objdump
            HINTS "${_prefix}/bin" NO_DEFAULT_PATH REQUIRED)
    endif()
    set(_directory "${CMAKE_BINARY_DIR}/umicom-deployment/$<CONFIG>")
    set(_config "${_directory}/${DEPLOY_TARGET}.cmake")
    foreach(_literal "${_support}" "${_framework}" "${CMAKE_BINARY_DIR}" "${_objdump}")
        umicom_windows_require_literal("${_literal}")
    endforeach()
    # Generator expressions retain per-configuration paths for Ninja Multi-
    # Config and Visual Studio; no Release executable is mistaken for Debug.
    file(GENERATE OUTPUT "${_config}" CONTENT
"set(UMI_DEPLOY_TARGET [==[${DEPLOY_TARGET}]==])
set(UMI_DEPLOY_PRODUCT [==[${DEPLOY_PRODUCT_NAME}]==])
set(UMI_DEPLOY_EXECUTABLE [==[$<TARGET_FILE:${DEPLOY_TARGET}>]==])
set(UMI_DEPLOY_PREFIX [==[${_prefix}]==])
set(UMI_DEPLOY_FRAMEWORK_RESOURCES [==[${DEPLOY_RESOURCE_ROOT}]==])
set(UMI_DEPLOY_OBJDUMP [==[${_objdump}]==])
set(UMI_DEPLOY_WORK [==[${_directory}]==])
set(UMI_DEPLOY_BUILD_ROOT [==[${CMAKE_BINARY_DIR}]==])
set(UMI_DEPLOY_EXTRA_MODULES [==[$<TARGET_PROPERTY:${DEPLOY_TARGET},UMICOM_WINDOWS_RUNTIME_MODULES>]==])
")
    target_sources("${DEPLOY_TARGET}" PRIVATE
        "${_framework}/src/delivery/windows_runtime_bootstrap.c"
        "${_framework}/src/delivery/windows_runtime_cache.c")
    target_include_directories("${DEPLOY_TARGET}" PRIVATE "${_framework}/include")
    target_link_libraries("${DEPLOY_TARGET}" PRIVATE user32)
    if(NOT DEPLOY_AUXILIARY)
        add_custom_command(TARGET "${DEPLOY_TARGET}" POST_BUILD
        COMMAND "${CMAKE_COMMAND}" "-DUMI_DEPLOY_CONFIG=${_config}"
            -P "${_support}/StageRuntime.cmake"
        COMMENT "Deploying private Windows runtime for ${DEPLOY_PRODUCT_NAME}"
        VERBATIM)
    endif()
    # An ALL refresh target also repairs a deleted staged DLL when the executable
    # does not need relinking. It is not a dependency of the executable itself,
    # which would form a cycle with the scan of the finished executable.
    set(_stage_target "${DEPLOY_TARGET}-windows-runtime")
    if(_stage_target STREQUAL "umicom-windows-runtime")
        set(_stage_target "umicom-command-windows-runtime")
    endif()
    add_custom_target("${_stage_target}" ALL
        COMMAND "${CMAKE_COMMAND}" "-DUMI_DEPLOY_CONFIG=${_config}"
            -P "${_support}/StageRuntime.cmake"
        DEPENDS "${DEPLOY_TARGET}"
        COMMENT "Checking click-ready runtime for ${DEPLOY_PRODUCT_NAME}"
        VERBATIM)
    if(NOT TARGET umicom-windows-runtime)
        add_custom_target(umicom-windows-runtime)
    endif()
    add_dependencies(umicom-windows-runtime "${_stage_target}")

    # Existing cmake --install and CPack component rules gain the same complete
    # runtime. The native suite setup target below additionally supplies the
    # new selection/shortcut interface; old package entry points are preserved.
    install(CODE
"set(UMI_DEPLOY_CONFIG [==[${_config}]==])
set(UMI_DEPLOY_INSTALL_ROOT \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}\")
include([==[${_support}/InstallRuntime.cmake]==])")

    set_property(TARGET "${DEPLOY_TARGET}" PROPERTY UMICOM_WINDOWS_DEPLOYMENT_STAGE_TARGET "${_stage_target}")
    set_property(TARGET "${DEPLOY_TARGET}" PROPERTY UMICOM_WINDOWS_DEPLOYMENT_REGISTERED TRUE)
    set_property(TARGET "${DEPLOY_TARGET}" PROPERTY UMICOM_WINDOWS_DEPLOYMENT_PRODUCT "${DEPLOY_PRODUCT_NAME}")
    set_property(TARGET "${DEPLOY_TARGET}" PROPERTY UMICOM_WINDOWS_DEPLOYMENT_CONFIG "${_config}")
    set_property(TARGET "${DEPLOY_TARGET}" PROPERTY UMICOM_WINDOWS_DEPLOYMENT_RESOURCES "${DEPLOY_RESOURCE_ROOT}")
    set_property(TARGET "${DEPLOY_TARGET}" PROPERTY UMICOM_WINDOWS_DEPLOYMENT_GUI "${DEPLOY_GUI}")
    set_property(GLOBAL APPEND PROPERTY UMICOM_WINDOWS_DEPLOYMENT_TARGETS "${DEPLOY_TARGET}")
    get_property(_scheduled GLOBAL PROPERTY UMICOM_WINDOWS_DEPLOYMENT_FINISH_SCHEDULED)
    if(NOT _scheduled)
        set_property(GLOBAL PROPERTY UMICOM_WINDOWS_DEPLOYMENT_FINISH_SCHEDULED TRUE)
        # Wait until every thin application has registered its native frontend.
        cmake_language(DEFER DIRECTORY "${CMAKE_SOURCE_DIR}"
            CALL umicom_finalise_windows_deployment)
    endif()
endfunction()

# Produce one installer catalogue from configured executable targets, never
# from a list of product names that might only be placeholders in this build.
function(umicom_finalise_windows_deployment)
    get_property(_targets GLOBAL PROPERTY UMICOM_WINDOWS_DEPLOYMENT_TARGETS)
    set(_selected "")
    foreach(_target IN LISTS _targets)
        get_target_property(_gui "${_target}" UMICOM_WINDOWS_DEPLOYMENT_GUI)
        get_target_property(_exclude "${_target}" UMICOM_WINDOWS_INSTALLER_EXCLUDE)
        if(_gui AND NOT _exclude)
            list(APPEND _selected "${_target}")
        endif()
    endforeach()
    if(UMICOM_WINDOWS_INSTALLER_TARGETS)
        foreach(_target IN LISTS UMICOM_WINDOWS_INSTALLER_TARGETS)
            if(NOT _target IN_LIST _selected)
                message(FATAL_ERROR
                    "Installer target '${_target}' is not a registered, enabled GUI application")
            endif()
        endforeach()
        set(_selected "${UMICOM_WINDOWS_INSTALLER_TARGETS}")
    endif()
    list(REMOVE_DUPLICATES _selected)
    if(NOT _selected)
        return()
    endif()
    # Studio's native Framework command is a runtime helper, not another GUI
    # choice. Existing targets can be registered from this deferred root scope;
    # AUXILIARY omits the cross-directory TARGET POST_BUILD operation.
    set(_helpers "")
    list(GET _selected 0 _first_product)
    get_target_property(_resources "${_first_product}" UMICOM_WINDOWS_DEPLOYMENT_RESOURCES)
    foreach(_helper IN LISTS UMICOM_WINDOWS_INSTALLER_HELPERS)
        if(NOT TARGET "${_helper}")
            if(_helper STREQUAL "umicom")
                continue()
            endif()
            message(FATAL_ERROR "Configured installer helper target does not exist: ${_helper}")
        endif()
        if(_helper IN_LIST _selected)
            continue()
        endif()
        umicom_prepare_windows_application(TARGET "${_helper}"
            PRODUCT_NAME "${_helper}" RESOURCE_ROOT "${_resources}" AUXILIARY)
        list(APPEND _helpers "${_helper}")
    endforeach()
    get_property(_framework GLOBAL PROPERTY UMICOM_WINDOWS_DEPLOYMENT_SUPPORT_ROOT)
    set(_config_text "set(UMI_PACKAGE_TARGETS [==[${_selected}]==])\n")
    string(APPEND _config_text "set(UMI_PACKAGE_HELPERS [==[${_helpers}]==])\n")
    set(_package_targets ${_selected} ${_helpers})
    foreach(_target IN LISTS _package_targets)
        get_target_property(_config "${_target}" UMICOM_WINDOWS_DEPLOYMENT_CONFIG)
        string(APPEND _config_text "set(UMI_PACKAGE_CONFIG_${_target} [==[${_config}]==])\n")
    endforeach()
    set(_version "${CMAKE_PROJECT_VERSION}")
    if(NOT _version)
        set(_version "0.1.0")
    endif()
    string(APPEND _config_text
        "set(UMI_PACKAGE_VERSION [==[${_version}]==])\n"
        "set(UMI_PACKAGE_FRAMEWORK_SUPPORT [==[${_framework}]==])\n"
        "set(UMI_PACKAGE_BUILD_ROOT [==[${CMAKE_BINARY_DIR}]==])\n"
        "set(UMI_PACKAGE_OUTPUT [==[${CMAKE_BINARY_DIR}/packages/$<CONFIG>]==])\n"
        "set(UMI_PACKAGE_NSIS [==[${UMICOM_NSIS_EXECUTABLE}]==])\n")
    set(_config "${CMAKE_BINARY_DIR}/umicom-deployment/$<CONFIG>/suite.cmake")
    file(GENERATE OUTPUT "${_config}" CONTENT "${_config_text}")
    add_custom_target(umicom-windows-installer
        COMMAND "${CMAKE_COMMAND}" "-DUMI_PACKAGE_CONFIG=${_config}"
            -P "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/windows/BuildInstaller.cmake"
        COMMENT "Creating the native Umicom application selection installer"
        VERBATIM)
    foreach(_target IN LISTS _package_targets)
        get_target_property(_stage_target "${_target}" UMICOM_WINDOWS_DEPLOYMENT_STAGE_TARGET)
        add_dependencies(umicom-windows-installer "${_stage_target}")
    endforeach()
endfunction()
