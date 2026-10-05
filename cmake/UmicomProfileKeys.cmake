# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Keep verification in Security and optional native presentation in the shared
# GTK owner. Application modules only open the existing connections entry.
include_guard(GLOBAL)
set(_umicom_profile_keys_root "${CMAKE_CURRENT_LIST_DIR}/..")
if(TARGET umicom_ui_gtk4)
    target_sources(umicom_ui_gtk4 PRIVATE
        "${_umicom_profile_keys_root}/adapters/gtk4/security/profile_keys_panel_gtk4.c"
        "${_umicom_profile_keys_root}/adapters/gtk4/security/profile_keys_worker_gtk4.c"
        "${_umicom_profile_keys_root}/adapters/gtk4/security/profile_keys_window_gtk4.c")
    target_link_libraries(umicom_ui_gtk4 PUBLIC Umicom::security)
endif()
if(BUILD_TESTING)
    add_executable(umicom-profile-secrets-test "${_umicom_profile_keys_root}/tests/profile_secrets/test_profile_secrets.c")
    target_link_libraries(umicom-profile-secrets-test PRIVATE Umicom::security)
    umicom_apply_warnings(umicom-profile-secrets-test)
    umicom_apply_sanitizers(umicom-profile-secrets-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-profile-secrets-test)
    endif()
    foreach(case lifecycle denied unknown-profile reverify corrupt-profile backend-failure read-clearing malformed-result limits scope ownership invalid-construction isolation)
        add_test(NAME framework.profile_secrets.${case} COMMAND umicom-profile-secrets-test ${case})
        set_tests_properties(framework.profile_secrets.${case} PROPERTIES TIMEOUT 20
            LABELS "framework;security;profile-secrets;privacy;regression")
    endforeach()
endif()
if(BUILD_TESTING AND TARGET umicom_ui_gtk4)
    add_executable(umicom-profile-keys-gtk-test "${_umicom_profile_keys_root}/tests/profile_secrets/test_profile_keys_gtk4.c")
    target_link_libraries(umicom-profile-keys-gtk-test PRIVATE Umicom::ui_gtk4)
    umicom_apply_warnings(umicom-profile-keys-gtk-test)
    umicom_apply_sanitizers(umicom-profile-keys-gtk-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-profile-keys-gtk-test)
    endif()
    foreach(case create save-check-remove confirmation changed-intent mismatch denied oversized private retained pending-close unavailable invalid)
        add_test(NAME framework.profile_secrets.gtk4.${case} COMMAND umicom-profile-keys-gtk-test ${case})
        set_tests_properties(framework.profile_secrets.gtk4.${case} PROPERTIES TIMEOUT 30 SKIP_RETURN_CODE 77
            LABELS "framework;security;profile-secrets;gtk4;privacy;ownership;regression")
    endforeach()
endif()
install(FILES "${_umicom_profile_keys_root}/docs/learning/managing-local-provider-keys.html"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/learning COMPONENT Learning)
