# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Register a child scope so the collector cannot rely on the caller's directory.
add_test(NAME child.check COMMAND "${CMAKE_COMMAND}" -E true)
umicom_test_impact_owners(child.check TARGETS helper)
set_tests_properties(child.check PROPERTIES DEPENDS cleanup)
if(CASE STREQUAL "duplicate-test")
    add_test(NAME alpha COMMAND "${CMAKE_COMMAND}" -E true)
    umicom_test_impact_owners(alpha TARGETS helper)
endif()
