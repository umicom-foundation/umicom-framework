# Umicom Framework
# File: cmake/UmicomStudioZen.cmake
# PURPOSE: Register shared Zen transition regression checks.
# AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
# LICENCE: MIT
if(BUILD_TESTING)
    add_executable(umicom-studio-runtime-workspace-zen-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_workspace_zen.c")
    target_link_libraries(umicom-studio-runtime-workspace-zen-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-workspace-zen-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-workspace-zen-test)
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-studio-runtime-workspace-zen-test)
    endif()
    foreach(case roundtrip direct-zen missing-return intervening-selection busy-enter busy-exit
        missing-zen missing-default revision-limit bad-active bad-return bad-record duplicate empty count null)
        add_test(NAME framework.studio_runtime.workspace-zen.${case}
            COMMAND umicom-studio-runtime-workspace-zen-test ${case})
        set_tests_properties(framework.studio_runtime.workspace-zen.${case}
            PROPERTIES LABELS "framework;studio;layout;zen;regression")
    endforeach()
endif()
