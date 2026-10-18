# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# This fragment accompanies the separately exported native designer adapter.
include(CMakeFindDependencyMacro)
find_dependency(PkgConfig)
pkg_check_modules(UMICOM_DESIGNER_NATIVE_GTK REQUIRED IMPORTED_TARGET GLOBAL gtk4>=4.10)
