#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/test_workspace_configuration/fixture/child/CMakeLists.txt
# PURPOSE: Verify explicit test workspace configuration with isolated fixtures.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
add_test(NAME fixture.case COMMAND "${CMAKE_COMMAND}" -E true)
