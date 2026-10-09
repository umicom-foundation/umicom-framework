# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Storage, independent connections and native form lifetimes have separate cases.
include_guard(GLOBAL)
if(NOT BUILD_TESTING)
    return()
endif()
function(umicom_build_configuration_target target source library)
    add_executable("${target}" "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/build_configurations/${source}")
    target_link_libraries("${target}" PRIVATE "${library}")
    set_target_properties("${target}" PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings("${target}")
    umicom_apply_sanitizers("${target}")
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target("${target}")
    endif()
endfunction()
umicom_build_configuration_target(umicom-build-configurations-storage-test test_storage.c Umicom::build)
umicom_build_configuration_target(umicom-build-configurations-damaged-test test_damaged.c Umicom::build)
umicom_build_configuration_target(umicom-build-configurations-sqlite-test test_sqlite.c Umicom::build)
foreach(case roundtrip stale replace capacity nested names)
    add_test(NAME framework.build_configurations.storage.${case} COMMAND umicom-build-configurations-storage-test "${case}")
    set_tests_properties(framework.build_configurations.storage.${case} PROPERTIES TIMEOUT 30 LABELS "framework;build;configurations;persistence")
endforeach()
foreach(case missing-marker wrong-root bad-count bad-revision orphan-name missing-field future-entry unsupported)
    add_test(NAME framework.build_configurations.damaged.${case} COMMAND umicom-build-configurations-damaged-test "${case}")
    set_tests_properties(framework.build_configurations.damaged.${case} PROPERTIES TIMEOUT 30 LABELS "framework;build;configurations;regression")
endforeach()
foreach(case reopen rollback)
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/configuration-library-fixture-${case}")
    add_test(NAME framework.build_configurations.sqlite.${case} COMMAND umicom-build-configurations-sqlite-test "${case}")
    set_tests_properties(framework.build_configurations.sqlite.${case} PROPERTIES TIMEOUT 30 SKIP_RETURN_CODE 77
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/configuration-library-fixture-${case}" LABELS "framework;build;configurations;sqlite")
endforeach()
if(TARGET Umicom::ui_gtk4)
    umicom_build_configuration_target(umicom-build-configurations-gtk-test test_gtk4.c Umicom::ui_gtk4)
    foreach(case roundtrip replace stale wrong-root retained callback-close reentrant)
        add_test(NAME framework.build_configurations.gtk4.${case} COMMAND umicom-build-configurations-gtk-test "${case}")
        set_tests_properties(framework.build_configurations.gtk4.${case} PROPERTIES
            TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;build;configurations;gtk4;ownership")
    endforeach()
endif()

# Lifecycle mutations keep active settings separate from saved-library data.
umicom_build_configuration_target(umicom-build-configurations-lifecycle-test test_lifecycle.c Umicom::build)
foreach(case rename duplicate same stale missing nested unknown damaged overflow reuse empty first middle last)
    add_test(NAME framework.build_configurations.lifecycle.${case}
        COMMAND umicom-build-configurations-lifecycle-test "${case}")
    set_tests_properties(framework.build_configurations.lifecycle.${case} PROPERTIES
        TIMEOUT 30 LABELS "framework;build;configurations;lifecycle;regression")
endforeach()
umicom_build_configuration_target(umicom-build-configurations-lifecycle-sqlite-test test_lifecycle_sqlite.c Umicom::build)
foreach(case remove-failure rename-failure reopen)
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/configuration-lifecycle-fixture-${case}")
    add_test(NAME framework.build_configurations.lifecycle.sqlite.${case}
        COMMAND umicom-build-configurations-lifecycle-sqlite-test "${case}")
    set_tests_properties(framework.build_configurations.lifecycle.sqlite.${case} PROPERTIES
        TIMEOUT 30 SKIP_RETURN_CODE 77 WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/configuration-lifecycle-fixture-${case}"
        LABELS "framework;build;configurations;sqlite;rollback")
endforeach()
umicom_build_configuration_target(umicom-build-configurations-identity-test test_absolute_identity.c Umicom::build)
add_test(NAME framework.build_configurations.absolute_identity COMMAND umicom-build-configurations-identity-test)
set_tests_properties(framework.build_configurations.absolute_identity PROPERTIES
    TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;build;configurations;paths")
if(TARGET Umicom::ui_gtk4)
    umicom_build_configuration_target(umicom-build-configurations-lifecycle-gtk-test test_lifecycle_gtk4.c Umicom::ui_gtk4)
    foreach(case rename remove selection refresh stale retained callback-close reentrant)
        add_test(NAME framework.build_configurations.lifecycle.gtk4.${case}
            COMMAND umicom-build-configurations-lifecycle-gtk-test "${case}")
        set_tests_properties(framework.build_configurations.lifecycle.gtk4.${case} PROPERTIES
            TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;build;configurations;gtk4;ownership")
    endforeach()
endif()
