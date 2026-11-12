#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomTreeRemovalChecks.cmake
#
# PURPOSE:
#   Register native cleanup ownership and exact cache admission regressions.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

include_guard(GLOBAL)
if(BUILD_TESTING)
    add_executable(umicom-remove-tree-posix-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/platform/test_remove_tree_posix.c")
    target_link_libraries(umicom-remove-tree-posix-test PRIVATE Umicom::platform)
    add_executable(umicom-clean-cache-identity-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/toolchain_lifecycle/test_clean_cache_identity.c")
    target_link_libraries(umicom-clean-cache-identity-test PRIVATE Umicom::toolchain)
    foreach(_target umicom-remove-tree-posix-test umicom-clean-cache-identity-test)
        set_target_properties("${_target}" PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings("${_target}")
        umicom_apply_sanitizers("${_target}")
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target("${_target}")
        endif()
    endforeach()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-remove-tree-posix-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../src/platform/filesystem_remove_posix.inc"
            "${CMAKE_CURRENT_LIST_DIR}/../src/platform/filesystem.c")
        umicom_target_qualification_inputs(umicom-clean-cache-identity-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/build.c"
            "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/cmake_cache.c")
    endif()
    foreach(_case null empty root root-slashes dot parent final-dot final-parent
            too-long missing file directory symlink-file symlink-directory dangling-link
            link-cycle trailing-link backslash-name depth depth-limit read-error
            close-error unlink-error open-error relative)
        add_test(NAME "framework.filesystem.remove_tree.${_case}"
            COMMAND umicom-remove-tree-posix-test "${_case}")
        set_tests_properties("framework.filesystem.remove_tree.${_case}" PROPERTIES
            TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;filesystem;posix;ownership;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.filesystem.remove_tree.${_case}"
                TARGETS umicom-remove-tree-posix-test)
        endif()
    endforeach()
    foreach(_case valid quoted bom crlf missing-source missing-build empty-source
            empty-build wrong-source wrong-build duplicate-source duplicate-build wrong-type
            invalid-utf8 nul oversized cache-directory missing-cache preview)
        add_test(NAME "framework.toolchain.clean_cache.${_case}"
            COMMAND umicom-clean-cache-identity-test "${_case}")
        set_tests_properties("framework.toolchain.clean_cache.${_case}" PROPERTIES
            TIMEOUT 30 LABELS "framework;toolchain;filesystem;ownership;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.toolchain.clean_cache.${_case}"
                TARGETS umicom-clean-cache-identity-test)
        endif()
    endforeach()
endif()
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/owned-directory-cleanup.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/learning" COMPONENT Learning)
