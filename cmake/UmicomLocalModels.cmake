#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomLocalModels.cmake
#
# PURPOSE:
#   Compose one native model service, branded console host and owner-run checks.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

include_guard(GLOBAL)
include(GNUInstallDirs)
include("${CMAKE_CURRENT_LIST_DIR}/UmicomSdkComponent.cmake")
# Standalone Framework builds must own the same AI transport used by products.
if(NOT TARGET Umicom::ai_workspace)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomAiWorkspace.cmake")
endif()
set(_umi_models_root "${CMAKE_CURRENT_LIST_DIR}/..")
set(UMICOM_MODEL_DOWNLOAD_HTTP "AUTO" CACHE STRING "Native model downloads: AUTO, ON or OFF")
set_property(CACHE UMICOM_MODEL_DOWNLOAD_HTTP PROPERTY STRINGS AUTO ON OFF)
string(TOUPPER "${UMICOM_MODEL_DOWNLOAD_HTTP}" _umi_models_mode)
if(NOT _umi_models_mode MATCHES "^(AUTO|ON|OFF)$")
    message(FATAL_ERROR "UMICOM_MODEL_DOWNLOAD_HTTP must be AUTO, ON or OFF")
endif()
set(UMICOM_MODEL_DOWNLOAD_BUILT OFF)
if(NOT _umi_models_mode STREQUAL "OFF")
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(UMICOM_MODELS_CURL QUIET IMPORTED_TARGET GLOBAL libcurl>=7.85.0)
    endif()
    if(TARGET PkgConfig::UMICOM_MODELS_CURL)
        set(UMICOM_MODEL_DOWNLOAD_BUILT ON)
    elseif(_umi_models_mode STREQUAL "ON")
        message(FATAL_ERROR "Native model downloads require pkg-config and libcurl >= 7.85. Use OFF for local-file-only operation.")
    endif()
endif()
message(STATUS "Umicom model downloads: ${UMICOM_MODEL_DOWNLOAD_BUILT}")
add_library(umicom_local_models STATIC
    "${_umi_models_root}/src/local_models/catalogue.c"
    "${_umi_models_root}/src/local_models/inspect.c"
    "${_umi_models_root}/src/local_models/download.c"
    "${_umi_models_root}/src/local_models/plan.c"
    "${_umi_models_root}/src/local_models/session.c"
    "${_umi_models_root}/src/local_models/cli.c")
add_library(Umicom::local_models ALIAS umicom_local_models)
set_target_properties(umicom_local_models PROPERTIES EXPORT_NAME local_models)
target_include_directories(umicom_local_models PUBLIC
    $<BUILD_INTERFACE:${_umi_models_root}/include> $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(umicom_local_models PUBLIC Umicom::platform Umicom::ai_workspace)
if(UMICOM_MODEL_DOWNLOAD_BUILT)
    target_compile_definitions(umicom_local_models PRIVATE UMICOM_MODELS_HAS_CURL=1)
    target_link_libraries(umicom_local_models PRIVATE PkgConfig::UMICOM_MODELS_CURL)
endif()
add_executable(umicom-models "${_umi_models_root}/examples/local_models/main.c")
target_link_libraries(umicom-models PRIVATE Umicom::local_models)
foreach(_target IN ITEMS umicom_local_models umicom-models)
    set_target_properties(${_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(${_target})
    umicom_apply_sanitizers(${_target})
endforeach()
if(MINGW)
    target_link_options(umicom-models PRIVATE -static-libgcc)
endif()
if(TARGET umicom)
    target_link_libraries(umicom PRIVATE Umicom::local_models)
endif()
include("${CMAKE_CURRENT_LIST_DIR}/UmicomApplicationBranding.cmake")
umicom_apply_application_branding(TARGET umicom-models
    PRODUCT_NAME "Umicom Local Models" INTERNAL_NAME "umicom-models"
    APPLICATION_ID "org.umicom.local-models" RESOURCE_ROOT "${_umi_models_root}/resources"
    NO_RUNTIME_DEPLOYMENT)
install(TARGETS umicom_local_models EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Framework)
umicom_register_sdk_libraries(umicom_local_models)
install(TARGETS umicom-models RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Framework)
install(DIRECTORY "${_umi_models_root}/include/umicom/local_models"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom" COMPONENT Framework)
configure_file("${CMAKE_CURRENT_LIST_DIR}/UmicomLocalModelsDependencies.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/umicom-model-config/UmicomLocalModelsDependencies.cmake" @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/umicom-model-config/UmicomLocalModelsDependencies.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/UmicomFramework" COMPONENT Framework)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/umicom-model-config/UmicomLocalModelsDependencies.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/UmicomFramework" COMPONENT FrameworkSDK EXCLUDE_FROM_ALL)
install(FILES "${_umi_models_root}/docs/local-model-manager.md" "${_umi_models_root}/docs/local-model-manager.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/docs" COMPONENT Learning)
install(FILES "${_umi_models_root}/docs/learning/local-model-lifecycle.html" "${_umi_models_root}/docs/learning/local-model-lifecycle.md"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/docs/learning" COMPONENT Learning)
if(BUILD_TESTING)
    add_subdirectory("${_umi_models_root}/tests/local_models" "${CMAKE_CURRENT_BINARY_DIR}/local-model-tests")
endif()
unset(_umi_models_root)
unset(_umi_models_mode)
