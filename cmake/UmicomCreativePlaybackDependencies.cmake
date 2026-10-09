# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Installed only when the static creative GTK library links GStreamer.
include(CMakeFindDependencyMacro)
find_dependency(PkgConfig)
pkg_check_modules(UMICOM_CREATIVE_GSTREAMER REQUIRED IMPORTED_TARGET gstreamer-1.0)
