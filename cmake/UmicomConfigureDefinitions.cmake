# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Configure-option tests inspect owned values and native controls. They do not
# configure a user project or execute its source files.
include_guard(GLOBAL)
if(NOT BUILD_TESTING)
    return()
endif()
function(umicom_configure_definitions_test target source library)
    add_executable("${target}" "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/configure_definitions/${source}")
    target_link_libraries("${target}" PRIVATE "${library}")
    set_target_properties("${target}" PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings("${target}")
    umicom_apply_sanitizers("${target}")
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target("${target}")
    endif()
endfunction()
umicom_configure_definitions_test(umicom-configure-syntax-test test_syntax.c Umicom::build)
umicom_configure_definitions_test(umicom-configure-provider-test test_provider.c Umicom::build)
umicom_configure_definitions_test(umicom-configure-storage-test test_storage.c Umicom::build)
foreach(case values invalid reserved bounds count empty)
    add_test(NAME framework.configure_definitions.syntax.${case} COMMAND umicom-configure-syntax-test "${case}")
    set_tests_properties(framework.configure_definitions.syntax.${case} PROPERTIES
        TIMEOUT 30 LABELS "framework;build;configure-definitions;regression")
endforeach()
foreach(case ordinary preset shared-preset other-phases identity)
    add_test(NAME framework.configure_definitions.provider.${case} COMMAND umicom-configure-provider-test "${case}")
    set_tests_properties(framework.configure_definitions.provider.${case} PROPERTIES
        TIMEOUT 30 LABELS "framework;build;configure-definitions;regression")
endforeach()
foreach(case migration missing damaged downgrade roundtrip)
    add_test(NAME framework.configure_definitions.storage.${case} COMMAND umicom-configure-storage-test "${case}")
    set_tests_properties(framework.configure_definitions.storage.${case} PROPERTIES
        TIMEOUT 30 LABELS "framework;build;configure-definitions;persistence")
endforeach()

# Programmatic SDK selection shares the manually entered definition grammar.
umicom_configure_definitions_test(umicom-configure-edit-test test_edit.c Umicom::build)
foreach(case replace append empty_value quoted_value untyped remove missing invalid_name
        invalid_type reserved control duplicate malformed unterminated full full_replace
        alias null token_capacity text_capacity)
    add_test(NAME framework.configure_definitions.edit.${case} COMMAND umicom-configure-edit-test "${case}")
    set_tests_properties(framework.configure_definitions.edit.${case} PROPERTIES
        TIMEOUT 30 LABELS "framework;build;configure-definitions;regression")
endforeach()

if(TARGET Umicom::ui_gtk4)
    umicom_configure_definitions_test(umicom-configure-gtk-test test_gtk4.c Umicom::ui_gtk4)
    foreach(case values invalid preset retained cancel)
        add_test(NAME framework.configure_definitions.gtk.${case} COMMAND umicom-configure-gtk-test "${case}")
        set_tests_properties(framework.configure_definitions.gtk.${case} PROPERTIES
            TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;build;configure-definitions;gtk4")
    endforeach()
endif()

umicom_configure_definitions_test(umicom-profile-initialization-test test_profile_initialization.c Umicom::build)
foreach(case alias null_id null_source null_build long_id long_source long_build)
    add_test(NAME framework.build.profile_initialization.${case} COMMAND umicom-profile-initialization-test "${case}")
    set_tests_properties(framework.build.profile_initialization.${case} PROPERTIES
        TIMEOUT 15 LABELS "framework;build;regression")
endforeach()
