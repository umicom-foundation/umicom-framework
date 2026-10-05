# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Owned memory captures keep protocol data and native controls independent.
include_guard(GLOBAL)
target_sources(umicom_developer PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/memory_reply.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/memory_capture.c")
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/debugger-memory.html"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/learning COMPONENT Learning)

if(BUILD_TESTING)
    function(umicom_memory_cases group)
        set(target "umicom-memory-inspection-${group}-test")
        add_executable(${target} "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/memory_inspection/test_${group}.c")
        target_link_libraries(${target} PRIVATE Umicom::Framework)
        umicom_apply_warnings(${target})
        umicom_apply_sanitizers(${target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${target})
        endif()
        foreach(case IN LISTS ARGN)
            add_test(NAME framework.memory_inspection.${group}.${case} COMMAND ${target} ${case})
            set_tests_properties(framework.memory_inspection.${group}.${case} PROPERTIES
                TIMEOUT 30 LABELS "framework;debugger;memory;regression")
        endforeach()
    endfunction()
    umicom_memory_cases(reply bytes decimal empty no-data short unreadable extension maximum missing-body null-body array-body missing-address address-type empty-address hex-prefix address-text address-nul duplicate-body duplicate-address duplicate-data duplicate-unreadable data-type padding-middle padding-first padding-second padding-tail padding-bits-one padding-bits-two unpadded whitespace alphabet negative fraction exponent unreadable-type overflow trailing requested-bound invalid-count invalid-output)
    umicom_memory_cases(format ascii capacity maximum empty)
    if(TARGET umicom-dap-protocol-fixture)
        add_executable(umicom-memory-inspection-platform-test "${CMAKE_CURRENT_LIST_DIR}/../tests/memory_inspection/test_platform.c")
        target_link_libraries(umicom-memory-inspection-platform-test PRIVATE Umicom::Framework)
        umicom_apply_warnings(umicom-memory-inspection-platform-test)
        umicom_apply_sanitizers(umicom-memory-inspection-platform-test)
        add_dependencies(umicom-memory-inspection-platform-test umicom-dap-protocol-fixture)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-memory-inspection-platform-test)
        endif()
        foreach(case success offset unsupported no-reference stale wrong-owner idle bounds rejected malformed command event timeout empty unreadable retained repeat)
            add_test(NAME framework.memory_inspection.platform.${case} COMMAND umicom-memory-inspection-platform-test
                "$<TARGET_FILE:umicom-dap-protocol-fixture>" ${case})
            set_tests_properties(framework.memory_inspection.platform.${case} PROPERTIES
                TIMEOUT 30 LABELS "framework;debugger;memory;protocol-fixture;regression")
        endforeach()
    endif()
endif()
