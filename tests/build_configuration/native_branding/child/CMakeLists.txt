#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/build_configuration/native_branding/child/CMakeLists.txt
#
# PURPOSE:
#   Define the child executable that receives branding from its parent.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

# Reuse the original native entry so this fixture isolates CMake ownership.
add_executable(native-child-branding ../main.c)
set_target_properties(native-child-branding PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    UMICOM_WINDOWS_RUNTIME_BOOTSTRAP_DISABLED FALSE)
