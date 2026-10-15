# Umicom Framework
# File: cmake/UmicomApplicationModuleTests.cmake
# Sammy Hegab, Umicom Foundation. MIT licence.
# Application module metadata can be checked without a display or network.
if(NOT BUILD_TESTING)
    return()
endif()
file(REAL_PATH "${CMAKE_CURRENT_LIST_DIR}/../tests/application_modules/fixtures" _module_fixtures)
add_executable(umicom-application-module-tests
    "${CMAKE_CURRENT_LIST_DIR}/../tests/application_modules/test_descriptor.c")
target_link_libraries(umicom-application-module-tests PRIVATE Umicom::runtime)
target_compile_definitions(umicom-application-module-tests PRIVATE
    UMI_MODULE_FIXTURES="${_module_fixtures}")
umicom_apply_warnings(umicom-application-module-tests)
umicom_apply_sanitizers(umicom-application-module-tests)
foreach(_case valid component console load missing limit null duplicate indent
        schema frontend frontend_indent frontend_section version list_syntax path section embedded_nul crlf)
    add_test(NAME "framework.application_module.native.${_case}"
        COMMAND umicom-application-module-tests "${_case}")
    set_tests_properties("framework.application_module.native.${_case}"
        PROPERTIES LABELS "framework;application-module;manifest;regression")
endforeach()
foreach(_case default off enabled all missing missing_all malformed missing_manifest
        duplicate_id duplicate_option duplicate_component duplicate_executable
        duplicate_path traversal device leading_digit trailing_dash path_newline quoted_path registry_crlf
        reserved duplicate_registration indent frontend
        list_syntax array_syntax oversize crlf metadata_escape module_escape
        frontend_indent frontend_section)
    add_test(NAME "framework.application_module.cmake.${_case}"
        COMMAND "${CMAKE_COMMAND}"
            "-DFRAMEWORK_SOURCE=${CMAKE_CURRENT_LIST_DIR}/.."
            "-DTEST_BINARY=${CMAKE_CURRENT_BINARY_DIR}/application-module-tests/${_case}"
            "-DCASE=${_case}"
            "-DTEST_GENERATOR=${CMAKE_GENERATOR}"
            "-DTEST_PLATFORM=${CMAKE_GENERATOR_PLATFORM}"
            "-DTEST_TOOLSET=${CMAKE_GENERATOR_TOOLSET}"
            -P "${CMAKE_CURRENT_LIST_DIR}/../tests/application_modules/check_discovery.cmake")
    set_tests_properties("framework.application_module.cmake.${_case}" PROPERTIES
        LABELS "framework;application-module;cmake;regression" TIMEOUT 60
        SKIP_REGULAR_EXPRESSION "SKIP: developer symlink creation is unavailable")
endforeach()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-application-module-tests)
endif()
if(TARGET umicom)
    add_test(NAME framework.application_module.cli.inspect
        COMMAND umicom repo inspect-module
            "${_module_fixtures}/application.umicom.yaml")
    set_tests_properties(framework.application_module.cli.inspect PROPERTIES
        PASS_REGULAR_EXPRESSION "UMICOM_APPLICATIONS_BUILD_WEATHER"
        LABELS "framework;application-module;cli;regression")
endif()

# The creation service is exercised without Git, a display or network access.
# These cases create only isolated fixtures inside their CTest binary directory.
add_executable(umicom-application-scaffold-tests
    "${CMAKE_CURRENT_LIST_DIR}/../tests/application_modules/test_scaffold.c")
target_link_libraries(umicom-application-scaffold-tests PRIVATE Umicom::scaffold)
umicom_apply_warnings(umicom-application-scaffold-tests)
umicom_apply_sanitizers(umicom-application-scaffold-tests)
add_executable(umicom-application-scaffold-io-tests
    "${CMAKE_CURRENT_LIST_DIR}/../tests/application_modules/test_scaffold_io.c")
target_link_libraries(umicom-application-scaffold-io-tests PRIVATE Umicom::scaffold)
umicom_apply_warnings(umicom-application-scaffold-io-tests)
umicom_apply_sanitizers(umicom-application-scaffold-io-tests)
foreach(_case preview console apply snapshot collision collision_after null slug case
        device name identity identity_segment long_slug long_name long_id relative
        missing_parent missing_template template_identity unknown_token nul oversize write_failure close_failure)
    set(_program umicom-application-scaffold-tests)
    if(_case STREQUAL "write_failure" OR _case STREQUAL "close_failure")
        set(_program umicom-application-scaffold-io-tests)
    endif()
    add_test(NAME "framework.application_module.scaffold.${_case}"
        COMMAND "${CMAKE_COMMAND}"
            "-DTEST_PROGRAM=$<TARGET_FILE:${_program}>"
            "-DTEST_BINARY=${CMAKE_CURRENT_BINARY_DIR}/application-scaffold-tests"
            "-DTEMPLATE_SOURCE=${CMAKE_CURRENT_LIST_DIR}/../templates/application_module"
            "-DCASE=${_case}"
            -P "${CMAKE_CURRENT_LIST_DIR}/../tests/application_modules/check_scaffold.cmake")
    set_tests_properties("framework.application_module.scaffold.${_case}" PROPERTIES
        LABELS "framework;application-module;scaffold;regression" TIMEOUT 45)
endforeach()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-application-scaffold-tests)
    umicom_register_validation_target(umicom-application-scaffold-io-tests)
endif()

if(TARGET umicom)
    foreach(_case preview show show_missing unknown duplicate conflicting create discover)
        add_test(NAME "framework.application_module.creator_cli.${_case}"
            COMMAND "${CMAKE_COMMAND}"
                "-DTEST_PROGRAM=$<TARGET_FILE:umicom>"
                "-DTEST_BINARY=${CMAKE_CURRENT_BINARY_DIR}/application-scaffold-tests"
                "-DTEMPLATE_SOURCE=${CMAKE_CURRENT_LIST_DIR}/../templates/application_module"
                "-DFIXTURE_SOURCE=${CMAKE_CURRENT_LIST_DIR}/../tests/application_modules/generated_consumer"
                "-DFRAMEWORK_SOURCE=${CMAKE_CURRENT_LIST_DIR}/.."
                "-DTEST_GENERATOR=${CMAKE_GENERATOR}"
                "-DTEST_PLATFORM=${CMAKE_GENERATOR_PLATFORM}"
                "-DTEST_TOOLSET=${CMAKE_GENERATOR_TOOLSET}"
                "-DTEST_C_COMPILER=${CMAKE_C_COMPILER}"
                "-DCASE=${_case}"
                -P "${CMAKE_CURRENT_LIST_DIR}/../tests/application_modules/check_scaffold_cli.cmake")
        set_tests_properties("framework.application_module.creator_cli.${_case}" PROPERTIES
            LABELS "framework;application-module;scaffold;cli;regression" TIMEOUT 100)
    endforeach()
endif()

# Project adoption reads the same reviewed plan without creating or trusting files.
add_executable(umicom-application-project-tests
    "${CMAKE_CURRENT_LIST_DIR}/../tests/application_modules/test_project_adoption.c")
target_link_libraries(umicom-application-project-tests PRIVATE Umicom::developer)
umicom_apply_warnings(umicom-application-project-tests)
umicom_apply_sanitizers(umicom-application-project-tests)
foreach(_case desktop console null identifier_capacity)
    add_test(NAME "framework.application_module.project.${_case}"
        COMMAND "${CMAKE_COMMAND}"
            "-DTEST_PROGRAM=$<TARGET_FILE:umicom-application-project-tests>"
            "-DTEST_BINARY=${CMAKE_CURRENT_BINARY_DIR}/application-project-tests"
            "-DTEMPLATE_SOURCE=${CMAKE_CURRENT_LIST_DIR}/../templates/application_module"
            "-DCASE=${_case}"
            -P "${CMAKE_CURRENT_LIST_DIR}/../tests/application_modules/check_scaffold.cmake")
    set_tests_properties("framework.application_module.project.${_case}" PROPERTIES
        LABELS "framework;application-module;developer-project;regression" TIMEOUT 45)
endforeach()
add_executable(umicom-application-resource-tests
    "${CMAKE_CURRENT_LIST_DIR}/../tests/application_modules/test_resources.c")
target_link_libraries(umicom-application-resource-tests PRIVATE Umicom::platform)
umicom_apply_warnings(umicom-application-resource-tests)
umicom_apply_sanitizers(umicom-application-resource-tests)
foreach(_case installed fallback image_failure capacity null missing)
    add_test(NAME "framework.application_module.resources.${_case}"
        COMMAND umicom-application-resource-tests "${_case}")
    set_tests_properties("framework.application_module.resources.${_case}" PROPERTIES
        LABELS "framework;application-module;resources;regression" TIMEOUT 15)
endforeach()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-application-project-tests)
    umicom_register_validation_target(umicom-application-resource-tests)
endif()

# Attach optional native tests only after the shared GTK target is available.
function(umicom_attach_application_creation_tests)
    if(NOT TARGET umicom_ui_gtk4 OR TARGET umicom-application-creator-gtk4-tests)
        return()
    endif()
    add_executable(umicom-application-creator-gtk4-tests
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/application_modules/test_creator_gtk4.c")
    target_link_libraries(umicom-application-creator-gtk4-tests PRIVATE Umicom::ui_gtk4)
    umicom_apply_warnings(umicom-application-creator-gtk4-tests)
    umicom_apply_sanitizers(umicom-application-creator-gtk4-tests)
    foreach(_case preview pending_edit pending_edit_back hide retained parent_close
            edit_after create collision hide_apply hide_failure adopt_destroy adopt_retry adopt_recursive invalid_utf8 notify_edit)
        add_test(NAME "framework.application_module.creator.gtk4.${_case}"
            COMMAND "${CMAKE_COMMAND}"
                "-DTEST_PROGRAM=$<TARGET_FILE:umicom-application-creator-gtk4-tests>"
                "-DTEST_BINARY=${CMAKE_CURRENT_BINARY_DIR}/application-creator-gtk4-tests"
                "-DTEMPLATE_SOURCE=${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../templates/application_module"
                "-DCASE=${_case}"
                -P "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/application_modules/check_scaffold_gtk4.cmake")
        set_tests_properties("framework.application_module.creator.gtk4.${_case}" PROPERTIES
            LABELS "framework;application-module;gtk4;ownership;regression" TIMEOUT 50
            SKIP_REGULAR_EXPRESSION "SKIP: GTK display unavailable")
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-application-creator-gtk4-tests)
    endif()
endfunction()
umicom_attach_application_creation_tests()
if(NOT TARGET umicom_ui_gtk4)
    cmake_language(DEFER CALL umicom_attach_application_creation_tests)
endif()

# The read-only inventory uses Git's config parser, without initialising or changing a repository.
find_package(Git QUIET)
if(Git_FOUND)
    add_executable(umicom-application-catalogue-tests
        "${CMAKE_CURRENT_LIST_DIR}/../tests/application_modules/test_catalogue.c")
    target_link_libraries(umicom-application-catalogue-tests PRIVATE Umicom::scaffold)
    umicom_apply_warnings(umicom-application-catalogue-tests)
    umicom_apply_sanitizers(umicom-application-catalogue-tests)
    foreach(_case valid absent empty unregistered includes missing malformed quoted crlf
            traversal nested key_space device leading_digit trailing_dash path_newline duplicate_path duplicate_id duplicate_component
            duplicate_option duplicate_executable capacity null)
        add_test(NAME "framework.application_module.catalogue.${_case}"
            COMMAND "${CMAKE_COMMAND}"
                "-DTEST_PROGRAM=$<TARGET_FILE:umicom-application-catalogue-tests>"
                "-DTEST_BINARY=${CMAKE_CURRENT_BINARY_DIR}/application-catalogue-tests"
                "-DGIT_PROGRAM=${GIT_EXECUTABLE}" "-DCASE=${_case}"
                -P "${CMAKE_CURRENT_LIST_DIR}/../tests/application_modules/check_catalogue.cmake")
        set_tests_properties("framework.application_module.catalogue.${_case}" PROPERTIES
            LABELS "framework;application-module;repository;regression" TIMEOUT 50)
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-application-catalogue-tests)
    endif()
endif()

# Copied descriptors need validation independently of parsing: native forms and
# plugin boundaries can modify fields after a manifest was originally read.
add_executable(umicom-application-descriptor-tests
    "${CMAKE_CURRENT_LIST_DIR}/../tests/application_modules/test_descriptor_validation.c")
target_link_libraries(umicom-application-descriptor-tests PRIVATE Umicom::runtime)
umicom_apply_warnings(umicom-application-descriptor-tests)
umicom_apply_sanitizers(umicom-application-descriptor-tests)
foreach(_case valid null unterminated stale_option empty_identity path component option_case)
    add_test(NAME "framework.application_module.descriptor.${_case}"
        COMMAND umicom-application-descriptor-tests "${_case}")
    set_tests_properties("framework.application_module.descriptor.${_case}" PROPERTIES
        LABELS "framework;application-module;validation;regression" TIMEOUT 15)
endforeach()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-application-descriptor-tests)
endif()
if(Git_FOUND)
    add_executable(umicom-application-registration-tests
        "${CMAKE_CURRENT_LIST_DIR}/../tests/application_modules/test_registration.c")
    target_link_libraries(umicom-application-registration-tests PRIVATE Umicom::scaffold)
    umicom_apply_warnings(umicom-application-registration-tests)
    umicom_apply_sanitizers(umicom-application-registration-tests)
    foreach(_case ready empty path id option component executable unavailable malformed
            capacity traversal nested uppercase invalid_descriptor null)
        add_test(NAME "framework.application_module.registration.${_case}"
            COMMAND "${CMAKE_COMMAND}"
                "-DTEST_PROGRAM=$<TARGET_FILE:umicom-application-registration-tests>"
                "-DTEST_BINARY=${CMAKE_CURRENT_BINARY_DIR}/application-registration-tests"
                "-DGIT_PROGRAM=${GIT_EXECUTABLE}" "-DCASE=${_case}"
                -P "${CMAKE_CURRENT_LIST_DIR}/../tests/application_modules/check_registration.cmake")
        set_tests_properties("framework.application_module.registration.${_case}" PROPERTIES
            LABELS "framework;application-module;repository;regression" TIMEOUT 50)
    endforeach()
    if(TARGET umicom)
        foreach(_case ready empty id option component executable unavailable malformed)
            add_test(NAME "framework.application_module.registration_cli.${_case}"
                COMMAND "${CMAKE_COMMAND}"
                    "-DTEST_PROGRAM=$<TARGET_FILE:umicom-application-registration-tests>"
                    "-DCLI_PROGRAM=$<TARGET_FILE:umicom>"
                    "-DTEST_BINARY=${CMAKE_CURRENT_BINARY_DIR}/application-registration-cli-tests"
                    "-DGIT_PROGRAM=${GIT_EXECUTABLE}" "-DCASE=${_case}"
                    -P "${CMAKE_CURRENT_LIST_DIR}/../tests/application_modules/check_registration.cmake")
            set_tests_properties("framework.application_module.registration_cli.${_case}" PROPERTIES
                LABELS "framework;application-module;repository;cli;regression" TIMEOUT 50)
        endforeach()
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-application-registration-tests)
    endif()
endif()
