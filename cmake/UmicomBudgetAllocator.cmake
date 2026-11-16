# Umicom Framework
# File: cmake/UmicomBudgetAllocator.cmake
#
# PURPOSE:
#   Register bounded allocator regressions and the installed beginner buffer example.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

include_guard(GLOBAL)
if(BUILD_TESTING)
    add_executable(umicom-budget-allocator-test tests/base/test_budget_allocator.c)
    add_executable(umicom-budgeted-buffer-example-test examples/budgeted_buffer/main.c)
    foreach(_target umicom-budget-allocator-test umicom-budgeted-buffer-example-test)
        target_link_libraries(${_target} PRIVATE Umicom::base)
        set_target_properties(${_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(${_target})
        umicom_apply_sanitizers(${_target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${_target})
        endif()
        if(COMMAND umicom_target_qualification_inputs)
            umicom_target_qualification_inputs(${_target} FILES
                "${CMAKE_CURRENT_SOURCE_DIR}/src/base/memory.c"
                "${CMAKE_CURRENT_SOURCE_DIR}/src/base/budget_allocator.c"
                "${CMAKE_CURRENT_SOURCE_DIR}/include/umicom/base/budget_allocator.h")
        endif()
    endforeach()
    umicom_apply_application_branding(TARGET umicom-budgeted-buffer-example-test
        PRODUCT_NAME "Umicom Bounded Buffer" INTERNAL_NAME "umicom-budgeted-buffer"
        APPLICATION_ID "org.umicom.sdk.memory.buffer"
        VERSION_MAJOR 1 VERSION_MINOR 0 VERSION_PATCH 0)
    foreach(_case IN ITEMS basic
            byte-limit
            block-limit
            maximum-blocks
            zero-bytes
            zero-limit
            zero-blocks
            too-many-blocks
            null-owner
            null-output
            occupied-output
            missing-allocate
            missing-release
            parent-no-reallocate
            create-failure
            allocation-failure
            resize-failure
            resize-grow
            resize-shrink
            resize-same
            resize-null
            resize-zero
            resize-transient-limit
            resize-foreign
            release-foreign
            release-interior
            null-release
            busy-destroy
            empty-destroy
            null-destroy
            slot-reuse
            adapter
            adapter-failure
            adapter-zero
            adapter-invalid-free
            copied-snapshot
            repeated
            two-owners
            overflow-limit
            null-snapshot
            null-adapter
            resize-occupied-output)
        add_test(NAME framework.memory.budget.${_case} COMMAND umicom-budget-allocator-test "${_case}")
        set_tests_properties(framework.memory.budget.${_case} PROPERTIES
            TIMEOUT 20 LABELS "framework;memory;ownership;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.memory.budget.${_case} TARGETS umicom-budget-allocator-test)
        endif()
    endforeach()
    add_test(NAME framework.memory.example.growth COMMAND umicom-budgeted-buffer-example-test)
    add_test(NAME framework.memory.example.refusal COMMAND umicom-budgeted-buffer-example-test --reject-only)
    set_tests_properties(framework.memory.example.growth framework.memory.example.refusal PROPERTIES TIMEOUT 10)
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(framework.memory.example.growth TARGETS umicom-budgeted-buffer-example-test)
        umicom_test_impact_owners(framework.memory.example.refusal TARGETS umicom-budgeted-buffer-example-test)
    endif()
endif()
foreach(_component Framework FrameworkSDK)
    install(FILES "${CMAKE_CURRENT_LIST_DIR}/../examples/budgeted_buffer/main.c"
        "${CMAKE_CURRENT_LIST_DIR}/../examples/budgeted_buffer/CMakeLists.txt"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/examples/budgeted_buffer"
        COMPONENT ${_component})
endforeach()
