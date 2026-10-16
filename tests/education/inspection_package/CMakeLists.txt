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
configure_package_config_file(
    "${FRAMEWORK_ROOT}/cmake/UmicomFrameworkConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/UmicomFrameworkConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/UmicomFramework")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/UmicomFrameworkConfig.cmake"
    "${FRAMEWORK_ROOT}/cmake/UmicomApplicationBranding.cmake"
    # The real package configuration includes module discovery even for a
    # focused export. Ship its implementation alongside the same config.
    "${FRAMEWORK_ROOT}/cmake/UmicomApplicationModules.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/UmicomFramework")
