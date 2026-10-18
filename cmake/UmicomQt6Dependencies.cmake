# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Qt is required only by installations that actually export the Qt adapter.
include(CMakeFindDependencyMacro)
find_dependency(Qt6 6.5 COMPONENTS Core Gui Widgets)
