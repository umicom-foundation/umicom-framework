# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include(CMakeFindDependencyMacro)
find_dependency(PkgConfig)
pkg_check_modules(IBKR_GTK4 REQUIRED IMPORTED_TARGET GLOBAL gtk4>=4.10)
