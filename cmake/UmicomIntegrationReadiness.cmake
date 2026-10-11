#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomIntegrationReadiness.cmake
# PURPOSE: Keep readiness consumers attached to their real implementation owner.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

# Naming the implementation owner makes the test independent of changes to the
# umbrella target. The existing public Framework dependency remains available.
if(TARGET umicom-integration-suite-runtime-tests)
    target_link_libraries(umicom-integration-suite-runtime-tests PRIVATE Umicom::integration)
endif()

if(BUILD_TESTING AND TARGET Umicom::integration)
    # This consumer deliberately links only the public integration target. Its
    # lifecycle calls require every readiness symbol from the installed owner.
    add_executable(umicom-integration-readiness-consumer
        "${CMAKE_CURRENT_LIST_DIR}/../tests/integration/test_readiness_consumer.c")
    target_link_libraries(umicom-integration-readiness-consumer PRIVATE Umicom::integration)
    set_target_properties(umicom-integration-readiness-consumer PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-integration-readiness-consumer)
    umicom_apply_sanitizers(umicom-integration-readiness-consumer)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-integration-readiness-consumer)
    endif()
    add_test(NAME framework.integration.readiness.consumer
        COMMAND umicom-integration-readiness-consumer)
    set_tests_properties(framework.integration.readiness.consumer PROPERTIES
        TIMEOUT 30 LABELS "framework;integration;readiness;link;regression")
endif()

# The ALL archive check does not run when a developer builds one executable by
# name. Make these focused builds inspect the owning archive before linking, so
# a missing definition produces a named archive report instead of many opaque
# downstream linker errors. Platforms without this inspector still link the
# same public owner and exercise the consumer normally.
if(TARGET umicom_integration-workspace-archive-symbol-check)
    foreach(_readiness_consumer IN ITEMS
            umicom-integration-suite-runtime-tests
            umicom-integration-readiness-consumer)
        if(TARGET "${_readiness_consumer}")
            add_dependencies("${_readiness_consumer}"
                umicom_integration-workspace-archive-symbol-check)
        endif()
    endforeach()
endif()

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/INTEGRATION_READINESS.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/docs")
