#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomFrameworkCompletionPlatform.cmake
#
# PURPOSE:
#   Extend existing Framework developer/toolchain services with installed-tool
#   readiness and add focused acceptance coverage for capabilities that Studio
#   will consume later. No parallel language, desktop or workbench model is
#   introduced by this integration.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

if(NOT TARGET umicom_developer)
    message(FATAL_ERROR
        "UmicomFrameworkCompletionPlatform requires existing umicom_developer")
endif()

# Installed-tool readiness and capability evidence extend the existing
# developer/toolchain binding registry rather than replacing it.
target_sources(umicom_developer PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer/toolchain_readiness.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer/language_support_matrix.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer/acceptance/evidence.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer/acceptance/gate.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer/acceptance/report.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer/acceptance/toolchain_gate.c"
)

if(BUILD_TESTING AND NOT TARGET umicom-language-profile-acceptance-tests)
    add_executable(umicom-language-profile-acceptance-tests
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_profile_acceptance/test_assembly.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_profile_acceptance/test_bash.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_profile_acceptance/test_c.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_profile_acceptance/test_cmake.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_profile_acceptance/test_cpp.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_profile_acceptance/test_css.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_profile_acceptance/test_definition_catalogue.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_profile_acceptance/test_html.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_profile_acceptance/test_java.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_profile_acceptance/test_javascript.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_profile_acceptance/test_json.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_profile_acceptance/test_main.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_profile_acceptance/test_make.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_profile_acceptance/test_markdown.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_profile_acceptance/test_php.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_profile_acceptance/test_powershell.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_profile_acceptance/test_python.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_profile_acceptance/test_rust.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_profile_acceptance/test_toml.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_profile_acceptance/test_typescript.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_profile_acceptance/test_uai.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_profile_acceptance/test_umicc.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_profile_acceptance/test_yaml.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_profile_acceptance/test_zig.c"
    )
    target_link_libraries(umicom-language-profile-acceptance-tests PRIVATE
        Umicom::language)
    umicom_apply_warnings(umicom-language-profile-acceptance-tests)
    umicom_apply_sanitizers(umicom-language-profile-acceptance-tests)
    add_test(
        NAME framework.language_profile.acceptance
        COMMAND umicom-language-profile-acceptance-tests)
endif()

if(BUILD_TESTING AND NOT TARGET umicom-toolchain-binding-acceptance-tests)
    add_executable(umicom-toolchain-binding-acceptance-tests
        "${CMAKE_CURRENT_LIST_DIR}/../tests/toolchain_binding_acceptance/test_assembly_llvm.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/toolchain_binding_acceptance/test_assembly_nasm.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/toolchain_binding_acceptance/test_bash_default.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/toolchain_binding_acceptance/test_c_clang.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/toolchain_binding_acceptance/test_c_gcc.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/toolchain_binding_acceptance/test_c_msvc.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/toolchain_binding_acceptance/test_cmake_default.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/toolchain_binding_acceptance/test_cpp_clang.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/toolchain_binding_acceptance/test_cpp_gcc.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/toolchain_binding_acceptance/test_cpp_msvc.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/toolchain_binding_acceptance/test_css_web.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/toolchain_binding_acceptance/test_html_web.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/toolchain_binding_acceptance/test_java_jdk.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/toolchain_binding_acceptance/test_javascript_node.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/toolchain_binding_acceptance/test_json_default.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/toolchain_binding_acceptance/test_main.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/toolchain_binding_acceptance/test_make_default.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/toolchain_binding_acceptance/test_markdown_default.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/toolchain_binding_acceptance/test_php_default.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/toolchain_binding_acceptance/test_powershell_default.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/toolchain_binding_acceptance/test_python_cpython.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/toolchain_binding_acceptance/test_rust_cargo.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/toolchain_binding_acceptance/test_toml_default.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/toolchain_binding_acceptance/test_typescript_node.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/toolchain_binding_acceptance/test_uai_default.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/toolchain_binding_acceptance/test_umicc_default.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/toolchain_binding_acceptance/test_yaml_default.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/toolchain_binding_acceptance/test_zig_default.c"
    )
    target_link_libraries(umicom-toolchain-binding-acceptance-tests PRIVATE
        Umicom::developer)
    umicom_apply_warnings(umicom-toolchain-binding-acceptance-tests)
    umicom_apply_sanitizers(umicom-toolchain-binding-acceptance-tests)
    add_test(
        NAME framework.toolchain_binding.acceptance
        COMMAND umicom-toolchain-binding-acceptance-tests)
endif()

if(BUILD_TESTING AND NOT TARGET umicom-language-support-acceptance-tests)
    add_executable(umicom-language-support-acceptance-tests
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_support_acceptance/test_all_tools.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_support_acceptance/test_assembly_x86.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_support_acceptance/test_custom_missing_binding.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_support_acceptance/test_document.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_support_acceptance/test_find_missing.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_support_acceptance/test_generic_c.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_support_acceptance/test_main.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_support_acceptance/test_no_tools.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_support_acceptance/test_partial_tools.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_support_acceptance/test_python.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_support_acceptance/test_typescript.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_support_acceptance/test_windows_c.c"
    )
    target_link_libraries(umicom-language-support-acceptance-tests PRIVATE
        Umicom::developer)
    umicom_apply_warnings(umicom-language-support-acceptance-tests)
    umicom_apply_sanitizers(umicom-language-support-acceptance-tests)
    add_test(
        NAME framework.language_support.acceptance
        COMMAND umicom-language-support-acceptance-tests)
endif()

if(BUILD_TESTING AND NOT TARGET umicom-desktop-monitor-acceptance-tests)
    add_executable(umicom-desktop-monitor-acceptance-tests
        "${CMAKE_CURRENT_LIST_DIR}/../tests/desktop_monitor_acceptance/test_add_primary.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/desktop_monitor_acceptance/test_capacity.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/desktop_monitor_acceptance/test_constrain_disabled.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/desktop_monitor_acceptance/test_constrain_negative.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/desktop_monitor_acceptance/test_constrain_oversized.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/desktop_monitor_acceptance/test_constrain_right_bottom.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/desktop_monitor_acceptance/test_duplicate_add.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/desktop_monitor_acceptance/test_find.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/desktop_monitor_acceptance/test_invalid_refresh.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/desktop_monitor_acceptance/test_invalid_scale.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/desktop_monitor_acceptance/test_main.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/desktop_monitor_acceptance/test_negative_origin.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/desktop_monitor_acceptance/test_remove_missing.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/desktop_monitor_acceptance/test_remove_primary.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/desktop_monitor_acceptance/test_set_primary.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/desktop_monitor_acceptance/test_update_geometry.c"
    )
    target_link_libraries(umicom-desktop-monitor-acceptance-tests PRIVATE
        Umicom::desktop)
    umicom_apply_warnings(umicom-desktop-monitor-acceptance-tests)
    umicom_apply_sanitizers(umicom-desktop-monitor-acceptance-tests)
    add_test(
        NAME framework.desktop_monitor.acceptance
        COMMAND umicom-desktop-monitor-acceptance-tests)
endif()

if(BUILD_TESTING AND NOT TARGET umicom-application-experience-acceptance-tests)
    add_executable(umicom-application-experience-acceptance-tests
        "${CMAKE_CURRENT_LIST_DIR}/../tests/application_experience_acceptance/test_accountant.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/application_experience_acceptance/test_ai_creator.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/application_experience_acceptance/test_author.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/application_experience_acceptance/test_bank.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/application_experience_acceptance/test_cad.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/application_experience_acceptance/test_database_studio.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/application_experience_acceptance/test_desktop.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/application_experience_acceptance/test_education.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/application_experience_acceptance/test_exchange.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/application_experience_acceptance/test_games.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/application_experience_acceptance/test_integration_studio.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/application_experience_acceptance/test_kitchen.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/application_experience_acceptance/test_llm.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/application_experience_acceptance/test_main.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/application_experience_acceptance/test_marketplace.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/application_experience_acceptance/test_media.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/application_experience_acceptance/test_mobile_studio.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/application_experience_acceptance/test_music.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/application_experience_acceptance/test_operations.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/application_experience_acceptance/test_os.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/application_experience_acceptance/test_rag.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/application_experience_acceptance/test_security_centre.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/application_experience_acceptance/test_studio.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/application_experience_acceptance/test_tms.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/application_experience_acceptance/test_trader.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/application_experience_acceptance/test_web_studio.c"
    )
    target_link_libraries(umicom-application-experience-acceptance-tests PRIVATE
        Umicom::application)
    umicom_apply_warnings(umicom-application-experience-acceptance-tests)
    umicom_apply_sanitizers(umicom-application-experience-acceptance-tests)
    add_test(
        NAME framework.application_experience.acceptance
        COMMAND umicom-application-experience-acceptance-tests)
endif()

if(BUILD_TESTING AND NOT TARGET umicom-developer-acceptance-tests)
    add_executable(umicom-developer-acceptance-tests
        "${CMAKE_CURRENT_LIST_DIR}/../tests/developer_acceptance/test_evidence.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/developer_acceptance/test_evidence_replace.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/developer_acceptance/test_gate_blocked.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/developer_acceptance/test_gate_fail.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/developer_acceptance/test_gate_missing.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/developer_acceptance/test_gate_pass.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/developer_acceptance/test_main.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/developer_acceptance/test_report.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/developer_acceptance/test_report_empty.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/developer_acceptance/test_toolchain_gate_blocked.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/developer_acceptance/test_toolchain_gate_pass.c"
    )
    target_link_libraries(umicom-developer-acceptance-tests PRIVATE
        Umicom::developer)
    umicom_apply_warnings(umicom-developer-acceptance-tests)
    umicom_apply_sanitizers(umicom-developer-acceptance-tests)
    add_test(
        NAME framework.developer.acceptance
        COMMAND umicom-developer-acceptance-tests)
endif()
