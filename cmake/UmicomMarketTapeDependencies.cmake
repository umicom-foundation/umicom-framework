# Umicom Foundation | Sammy Hegab | MIT
include(CMakeFindDependencyMacro)
find_dependency(PkgConfig)
pkg_check_modules(UMICOM_MARKET_TAPE_GTK4 REQUIRED IMPORTED_TARGET gtk4>=4.10)
