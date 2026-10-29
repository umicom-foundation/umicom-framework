# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
cmake_minimum_required(VERSION 3.24)
project(UmicomEducationInspectionPackageFixture LANGUAGES C)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Build the production owner and install its real export set into a disposable
# prefix selected by CTest. No substitute parser or hand-written imported target
# is used. The rest of Framework is intentionally outside this focused check.
include("${FRAMEWORK_ROOT}/cmake/UmicomEducationInspection.cmake")
install(EXPORT UmicomFrameworkTargets FILE UmicomFrameworkTargets.cmake
    NAMESPACE Umicom:: DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/UmicomFramework")
# The production package declares its header identity and build environment.
# Derive the same identity in this focused producer rather than substituting the
# fixture project name or a hand-maintained ABI number.
include("${FRAMEWORK_ROOT}/cmake/UmicomFrameworkIdentity.cmake")
umicom_read_framework_identity("${FRAMEWORK_ROOT}/include/umicom/base/version.h" UMICOM_FRAMEWORK_PACKAGE)
configure_package_config_file(
    "${FRAMEWORK_ROOT}/cmake/UmicomFrameworkConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/UmicomFrameworkConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/UmicomFramework")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/UmicomFrameworkConfig.cmake"
    "${FRAMEWORK_ROOT}/cmake/UmicomApplicationBranding.cmake"
    # The real package configuration includes module discovery even for a
    # focused export. Ship its implementation alongside the same config.
    "${FRAMEWORK_ROOT}/cmake/UmicomApplicationModules.cmake"
    # The production package loads installed-SDK checks unconditionally. Keep
    # their helper dependency in this isolated package too, so relocation tests
    # exercise the real configuration without requiring unrelated libraries.
    "${FRAMEWORK_ROOT}/cmake/UmicomRequireInstalledSdk.cmake"
    "${FRAMEWORK_ROOT}/cmake/UmicomRequireSdkEnvironment.cmake"
    "${FRAMEWORK_ROOT}/cmake/UmicomPublicContractFingerprint.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/UmicomFramework")

# An independently included module must load SDK registration itself and join
# the aggregate exactly once. This also holds when ordinary tests are disabled.
if(NOT TARGET umicom-sdk-libraries)
    message(FATAL_ERROR "Independent component did not provide the SDK aggregate")
endif()
get_target_property(_sdk_dependencies umicom-sdk-libraries MANUALLY_ADDED_DEPENDENCIES)
foreach(_sdk_owner IN ITEMS umicom_education_inspection)
    get_target_property(_sdk_registered "${_sdk_owner}" UMICOM_SDK_INSTALL_REGISTERED)
    if(NOT _sdk_registered OR NOT _sdk_owner IN_LIST _sdk_dependencies)
        message(FATAL_ERROR "Independent component is absent from the SDK: ${_sdk_owner}")
    endif()
endforeach()
