# Shared cases run against the same parser in standalone and full Framework builds.
# Sammy Hegab | Umicom Foundation | MIT
foreach(scenario mixed legacy all-passed empty truncated totals duplicate entities
    large bounds duration-overflow hostile owned invalid)
    add_test(NAME "framework.ctest.report_collection.${scenario}"
        COMMAND umicom-ctest-report-collection-checks "${scenario}")
    set_tests_properties("framework.ctest.report_collection.${scenario}" PROPERTIES TIMEOUT 60)
endforeach()
