#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomDeveloperProjectPlatform.cmake
#
# PURPOSE:
#   Attach the reusable project-model, project-generator, language-pack and
#   provider platform to the canonical developer library.
#
# ARCHITECTURE:
#   This file is additive. Existing project/workspace, developer-runtime,
#   language, build, toolchain and workbench implementations remain intact. The
#   new platform composes them into the next Framework-first project workflow
#   required by Studio's New Project / Open Project experiences.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

# Load the dependency only when the parent build has not already provided its target.
if(NOT TARGET umicom_developer)
    message(FATAL_ERROR
        "Developer Project Platform requires the canonical umicom_developer target")
endif()

# Application creation shares the owned scaffold service with the CLI.
target_link_libraries(umicom_developer PUBLIC Umicom::scaffold)

target_sources(umicom_developer PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/build/project_profile.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/build_plan.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/builtin_languages.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/builtin_providers.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/builtin_templates.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/detection.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/generation_plan.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/generation_request.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/generator.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/generator_create.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/generator_message.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/new_project.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/application_scaffold.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/destination.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/entry_point.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/tool_catalogue.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/language_health.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/language_pack.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/language_provider.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/language_registry.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/languages/assembly.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/languages/bash.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/languages/c23.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/languages/cmake.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/languages/cpp.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/languages/cpython.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/languages/css.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/languages/html.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/languages/java.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/languages/javascript.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/languages/json.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/languages/markdown.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/languages/php.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/languages/rust.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/languages/vhdl.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/languages/umicom_c.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/languages/sql.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/languages/typescript.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/languages/yaml.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/languages/zig.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/model.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/model_registry.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/project_workbench_platform.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/provider_registry.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/providers/bash.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/providers/cargo.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/providers/cmake.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/providers/cpython.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/providers/zig.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/render.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/run_plan.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/service.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/template.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/template_registry.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/templates/assembly_console.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/templates/bash_tool.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/templates/c23_assembly.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/templates/c23_console.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/templates/c23_gtk4.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/templates/c23_shared_library.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/templates/c23_static_library.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/templates/cmake_meta_project.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/templates/cpp_console.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/templates/cpp_gtk4.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/templates/cpython_extension.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/templates/integration_worker.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/templates/python_tool.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/templates/rust_console.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/templates/vhdl_cpu.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/templates/umicom_c_preview.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/templates/thin_desktop_application.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/templates/umicom_extension.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/templates/web_service_c23.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/templates/zig_console.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/tree.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/types.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/variable_set.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/workbench_bridge.c"
)

# Register verification targets only when the developer has enabled testing.
if(BUILD_TESTING)
    # Define the add developer project test build helper so parent and application projects
    # apply one consistent rule.
    function(umicom_add_developer_project_test target test_name source)
        # Configure the optional target only when its feature has created it.
        if(TARGET "${target}")
            return()
        endif()

        add_executable(
            "${target}"
            "${CMAKE_CURRENT_LIST_DIR}/../${source}"
        )
        target_link_libraries("${target}" PRIVATE Umicom::Framework)

        # Use the shared build helper when it is available from the parent composition.
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings("${target}")
        endif()

        # Use the shared build helper when it is available from the parent composition.
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers("${target}")
        endif()

        add_test(NAME "${test_name}" COMMAND "${target}")
    endfunction()

    umicom_add_developer_project_test(
        umicom-developer-project-generated-profiles-test
        framework.developer_project.generated-profiles
        tests/developer_project/test_generated_profiles.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-umicom-c-preview-test
        framework.developer_project.umicom-c-preview
        tests/developer_project/test_umicom_c_preview.c
    )
    # Exercise the same generated project and language contracts consumed by Studio.
    umicom_add_developer_project_test(
        umicom-developer-project-vhdl-cpu-test
        framework.developer_project.vhdl-cpu
        tests/developer_project/test_vhdl_cpu.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-entry-point-test
        framework.developer_project.entry-point
        tests/developer_project/test_entry_point.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-destination-test
        framework.developer_project.destination
        tests/developer_project/test_destination.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-build-plan-test
        framework.developer_project.build-plan
        tests/developer_project/test_build_plan.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-builtin-languages-test
        framework.developer_project.builtin-languages
        tests/developer_project/test_builtin_languages.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-builtin-providers-test
        framework.developer_project.builtin-providers
        tests/developer_project/test_builtin_providers.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-builtin-templates-test
        framework.developer_project.builtin-templates
        tests/developer_project/test_builtin_templates.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-generation-plan-test
        framework.developer_project.generation-plan
        tests/developer_project/test_generation_plan.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-generation-request-test
        framework.developer_project.generation-request
        tests/developer_project/test_generation_request.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-generator-dry-run-test
        framework.developer_project.generator-dry-run
        tests/developer_project/test_generator_dry_run.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-language-assembly-test
        framework.developer_project.language-assembly
        tests/developer_project/test_language_assembly.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-language-bash-test
        framework.developer_project.language-bash
        tests/developer_project/test_language_bash.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-language-c23-test
        framework.developer_project.language-c23
        tests/developer_project/test_language_c23.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-language-cmake-test
        framework.developer_project.language-cmake
        tests/developer_project/test_language_cmake.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-language-cpp-test
        framework.developer_project.language-cpp
        tests/developer_project/test_language_cpp.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-language-cpython-test
        framework.developer_project.language-cpython
        tests/developer_project/test_language_cpython.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-language-css-test
        framework.developer_project.language-css
        tests/developer_project/test_language_css.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-language-health-test
        framework.developer_project.language-health
        tests/developer_project/test_language_health.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-language-html-test
        framework.developer_project.language-html
        tests/developer_project/test_language_html.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-language-java-test
        framework.developer_project.language-java
        tests/developer_project/test_language_java.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-language-javascript-test
        framework.developer_project.language-javascript
        tests/developer_project/test_language_javascript.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-language-json-test
        framework.developer_project.language-json
        tests/developer_project/test_language_json.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-language-markdown-test
        framework.developer_project.language-markdown
        tests/developer_project/test_language_markdown.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-language-php-test
        framework.developer_project.language-php
        tests/developer_project/test_language_php.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-language-rust-test
        framework.developer_project.language-rust
        tests/developer_project/test_language_rust.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-language-sql-test
        framework.developer_project.language-sql
        tests/developer_project/test_language_sql.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-language-typescript-test
        framework.developer_project.language-typescript
        tests/developer_project/test_language_typescript.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-language-yaml-test
        framework.developer_project.language-yaml
        tests/developer_project/test_language_yaml.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-language-zig-test
        framework.developer_project.language-zig
        tests/developer_project/test_language_zig.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-model-test
        framework.developer_project.model
        tests/developer_project/test_model.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-model-registry-test
        framework.developer_project.model-registry
        tests/developer_project/test_model_registry.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-project-workbench-platform-test
        framework.developer_project.project-workbench-platform
        tests/developer_project/test_project_workbench_platform.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-provider-bash-test
        framework.developer_project.provider-bash
        tests/developer_project/test_provider_bash.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-provider-cargo-test
        framework.developer_project.provider-cargo
        tests/developer_project/test_provider_cargo.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-provider-cmake-test
        framework.developer_project.provider-cmake
        tests/developer_project/test_provider_cmake.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-provider-cpython-test
        framework.developer_project.provider-cpython
        tests/developer_project/test_provider_cpython.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-provider-registry-test
        framework.developer_project.provider-registry
        tests/developer_project/test_provider_registry.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-provider-zig-test
        framework.developer_project.provider-zig
        tests/developer_project/test_provider_zig.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-render-test
        framework.developer_project.render
        tests/developer_project/test_render.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-run-plan-test
        framework.developer_project.run-plan
        tests/developer_project/test_run_plan.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-service-test
        framework.developer_project.service
        tests/developer_project/test_service.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-template-assembly-console-test
        framework.developer_project.template-assembly-console
        tests/developer_project/test_template_assembly_console.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-template-bash-tool-test
        framework.developer_project.template-bash-tool
        tests/developer_project/test_template_bash_tool.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-template-c23-assembly-test
        framework.developer_project.template-c23-assembly
        tests/developer_project/test_template_c23_assembly.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-template-c23-console-test
        framework.developer_project.template-c23-console
        tests/developer_project/test_template_c23_console.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-template-c23-gtk4-test
        framework.developer_project.template-c23-gtk4
        tests/developer_project/test_template_c23_gtk4.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-template-c23-shared-library-test
        framework.developer_project.template-c23-shared-library
        tests/developer_project/test_template_c23_shared_library.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-template-c23-static-library-test
        framework.developer_project.template-c23-static-library
        tests/developer_project/test_template_c23_static_library.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-template-cmake-meta-project-test
        framework.developer_project.template-cmake-meta-project
        tests/developer_project/test_template_cmake_meta_project.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-template-cpp-console-test
        framework.developer_project.template-cpp-console
        tests/developer_project/test_template_cpp_console.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-template-cpp-gtk4-test
        framework.developer_project.template-cpp-gtk4
        tests/developer_project/test_template_cpp_gtk4.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-template-cpython-extension-test
        framework.developer_project.template-cpython-extension
        tests/developer_project/test_template_cpython_extension.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-template-integration-worker-test
        framework.developer_project.template-integration-worker
        tests/developer_project/test_template_integration_worker.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-template-python-tool-test
        framework.developer_project.template-python-tool
        tests/developer_project/test_template_python_tool.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-template-rust-console-test
        framework.developer_project.template-rust-console
        tests/developer_project/test_template_rust_console.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-template-thin-desktop-application-test
        framework.developer_project.template-thin-desktop-application
        tests/developer_project/test_template_thin_desktop_application.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-template-umicom-extension-test
        framework.developer_project.template-umicom-extension
        tests/developer_project/test_template_umicom_extension.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-template-web-service-c23-test
        framework.developer_project.template-web-service-c23
        tests/developer_project/test_template_web_service_c23.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-template-zig-console-test
        framework.developer_project.template-zig-console
        tests/developer_project/test_template_zig_console.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-variable-set-test
        framework.developer_project.variable-set
        tests/developer_project/test_variable_set.c
    )
    umicom_add_developer_project_test(
        umicom-developer-project-workbench-bridge-test
        framework.developer_project.workbench-bridge
        tests/developer_project/test_workbench_bridge.c
    )
endif()

message(STATUS
    "Umicom project models, language packs and generator platform enabled")

# Generic developer dialogs live in Framework's existing GTK adapter. Defer only
# when that optional target has not yet been declared by the parent build.
function(umicom_attach_developer_dialogs_gtk4)
    if(NOT TARGET umicom_ui_gtk4)
        return()
    endif()
    get_target_property(attached umicom_ui_gtk4 UMICOM_DEVELOPER_DIALOGS_ATTACHED)
    if(attached)
        return()
    endif()
    target_sources(umicom_ui_gtk4 PRIVATE
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../adapters/gtk4/developer_dialog_gtk4.c")
    target_link_libraries(umicom_ui_gtk4 PUBLIC Umicom::developer Umicom::scaffold)
    set_property(TARGET umicom_ui_gtk4 PROPERTY UMICOM_DEVELOPER_DIALOGS_ATTACHED TRUE)
    if(BUILD_TESTING)
        # The held dispatcher still releases onto a real GTask worker. Only the
        # native filesystem writer is injected; GTK and completion remain real.
        add_executable(umicom-project-creation-worker-gtk4-test
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/developer_project/test_project_creation_worker_gtk4.c")
        target_link_libraries(umicom-project-creation-worker-gtk4-test PRIVATE Umicom::ui_gtk4)
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings(umicom-project-creation-worker-gtk4-test)
        endif()
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers(umicom-project-creation-worker-gtk4-test)
        endif()
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-project-creation-worker-gtk4-test)
        endif()
        if(COMMAND umicom_target_qualification_inputs)
            umicom_target_qualification_inputs(umicom-project-creation-worker-gtk4-test FILES
                "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../adapters/gtk4/developer_dialog_gtk4.c"
                "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../adapters/gtk4/developer_project_creation_gtk4.inc"
                "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../adapters/gtk4/developer_application_gtk4.inc")
        endif()
        foreach(case IN ITEMS complete owned-request busy-submit busy-cancel busy-close hidden-completion owner-retired parent-retired form-retired adoption-retry adoption-destroy adoption-reentrant refused partial notify-before-start hide-before-start notify-on-result invalid-input application-admission-close application-admission-hide)
            add_test(NAME framework.ide_workflow.project_creation.gtk4.${case}
                COMMAND umicom-project-creation-worker-gtk4-test ${case})
            set_tests_properties(framework.ide_workflow.project_creation.gtk4.${case} PROPERTIES
                SKIP_RETURN_CODE 77 TIMEOUT 45 LABELS "framework;ide-workflow;gtk4;ownership;no-filesystem")
        endforeach()
        add_executable(umicom-developer-dialog-gtk4-test
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/developer_project/test_developer_dialog_gtk4.c")
        target_link_libraries(umicom-developer-dialog-gtk4-test PRIVATE Umicom::ui_gtk4)
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings(umicom-developer-dialog-gtk4-test)
        endif()
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers(umicom-developer-dialog-gtk4-test)
        endif()
        # The inventory never launches tools; GTK cases also cover edits and
        # disposal while a filesystem capture is waiting to publish its result.
        add_executable(umicom-project-tool-catalogue-gtk4-test
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/developer_project/test_tool_catalogue_gtk4.c")
        target_link_libraries(umicom-project-tool-catalogue-gtk4-test PRIVATE Umicom::ui_gtk4)
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings(umicom-project-tool-catalogue-gtk4-test)
        endif()
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers(umicom-project-tool-catalogue-gtk4-test)
        endif()
        foreach(toolCase IN ITEMS files invalid edit edit-back close hide retained notify-close notify-edit notify-edit-back)
            add_test(NAME framework.ide_workflow.tool_catalogue.gtk4.${toolCase}
                COMMAND umicom-project-tool-catalogue-gtk4-test ${toolCase})
            set_tests_properties(framework.ide_workflow.tool_catalogue.gtk4.${toolCase} PROPERTIES
                SKIP_RETURN_CODE 77 TIMEOUT 45 LABELS "framework;ide-workflow;gtk4;ownership")
        endforeach()
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-project-tool-catalogue-gtk4-test)
        endif()
        # Lifetime scenarios use the actual forms and their public callback API.
        add_executable(umicom-developer-settings-lifetime-test
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/developer_project/test_settings_lifetime_gtk4.c")
        target_link_libraries(umicom-developer-settings-lifetime-test PRIVATE Umicom::ui_gtk4)
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings(umicom-developer-settings-lifetime-test)
        endif()
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers(umicom-developer-settings-lifetime-test)
        endif()
        foreach(settingsCase IN ITEMS stage-fields mixed-fields owner-destroy parent-destroy
                retained cancel recursive retry notify-close project-owner-destroy project-retained project-cancel
                prepare-open prepare-owner-close prepare-parent-close)
            add_test(NAME framework.ide_workflow.settings.gtk4.${settingsCase}
                COMMAND umicom-developer-settings-lifetime-test ${settingsCase})
            set_tests_properties(framework.ide_workflow.settings.gtk4.${settingsCase} PROPERTIES
                SKIP_RETURN_CODE 77 TIMEOUT 60 LABELS "framework;ide-workflow;gtk4;ownership")
        endforeach()
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-developer-settings-lifetime-test)
        endif()
        # Prepare/adopt/present must tolerate native retirement at every boundary.
        add_executable(umicom-project-form-preparation-test
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/developer_project/test_project_form_preparation_gtk4.c")
        target_link_libraries(umicom-project-form-preparation-test PRIVATE Umicom::ui_gtk4)
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings(umicom-project-form-preparation-test)
        endif()
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers(umicom-project-form-preparation-test)
        endif()
        foreach(case project-open application-open project-preclosed application-preclosed project-create-close application-create-close project-title-close application-title-close project-present-owner-close application-present-owner-close project-present-parent-close application-present-parent-close project-retained application-retained project-immediate-close application-immediate-close)
            add_test(NAME framework.ide_workflow.project_form.gtk4.${case}
                COMMAND umicom-project-form-preparation-test ${case})
            set_tests_properties(framework.ide_workflow.project_form.gtk4.${case} PROPERTIES
                SKIP_RETURN_CODE 77 TIMEOUT 45 LABELS "framework;ide-workflow;gtk4;ownership")
        endforeach()
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-project-form-preparation-test)
        endif()
        # Persistent task sections retain the original form and its acceptance contract.
        add_executable(umicom-developer-settings-sections-test
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/developer_project/test_settings_sections_gtk4.c")
        target_link_libraries(umicom-developer-settings-sections-test PRIVATE Umicom::ui_gtk4)
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings(umicom-developer-settings-sections-test)
        endif()
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers(umicom-developer-settings-sections-test)
        endif()
        foreach(case IN ITEMS navigation-cycle field-map prepared same-section invalid-section invalid-output null-dialog project-form owner-close parent-close reentrant-selection retained-controls apply-drafts hidden-apply cancel-drafts independent-forms configuration-binding sidebar divider independent-scroll pinned-actions construction-close)
            add_test(NAME framework.ide_workflow.settings_sections.gtk4.${case}
                COMMAND umicom-developer-settings-sections-test ${case})
            set_tests_properties(framework.ide_workflow.settings_sections.gtk4.${case} PROPERTIES
                SKIP_RETURN_CODE 77 TIMEOUT 45 LABELS "framework;ide-workflow;gtk4;ownership;layout")
        endforeach()
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-developer-settings-sections-test)
        endif()
        # Exercise template selection and adopted profiles without executing tools.
        add_executable(umicom-simulation-starters-gtk4-test
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/developer_project/test_simulation_starters_gtk4.c")
        target_link_libraries(umicom-simulation-starters-gtk4-test PRIVATE Umicom::ui_gtk4)
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings(umicom-simulation-starters-gtk4-test)
        endif()
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers(umicom-simulation-starters-gtk4-test)
        endif()
        foreach(starterCase IN ITEMS vhdl umicom-c cancel)
            add_test(NAME framework.ide_workflow.starter.gtk4.${starterCase}
                COMMAND umicom-simulation-starters-gtk4-test ${starterCase})
            set_tests_properties(framework.ide_workflow.starter.gtk4.${starterCase} PROPERTIES
                SKIP_RETURN_CODE 77 TIMEOUT 30 LABELS "framework;ide-workflow;gtk4;generation")
        endforeach()
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-simulation-starters-gtk4-test)
        endif()
        set(dialogTestDirectory "${CMAKE_CURRENT_BINARY_DIR}/qualification/developer_dialog")
        file(MAKE_DIRECTORY "${dialogTestDirectory}")
        add_test(NAME framework.ide_workflow.developer_dialog.gtk4 COMMAND umicom-developer-dialog-gtk4-test)
        set_tests_properties(framework.ide_workflow.developer_dialog.gtk4 PROPERTIES
            WORKING_DIRECTORY "${dialogTestDirectory}" SKIP_RETURN_CODE 77
            TIMEOUT 60 LABELS "framework;ide-workflow;gtk4;acceptance")
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-developer-dialog-gtk4-test)
        endif()
    endif()
endfunction()
umicom_attach_developer_dialogs_gtk4()
if(NOT TARGET umicom_ui_gtk4)
    cmake_language(DEFER CALL umicom_attach_developer_dialogs_gtk4)
endif()

# Focused real-service qualification. Each filesystem test owns its CTest cwd.
if(BUILD_TESTING)
# The focused qualification list now includes absolute project identity when cwd is unavailable. Retain the earlier list for review.
if(FALSE)
    foreach(case IN ITEMS project_input_boundaries project_session money_boundaries save_all history_identity)
    endforeach()
endif()
    foreach(case IN ITEMS project_input_boundaries project_absolute_identity project_session money_boundaries save_all history_identity)
        set(target "umicom-project-${case}-test")
        add_executable(${target} "${CMAKE_CURRENT_LIST_DIR}/../tests/developer_project/test_${case}.c")
        target_link_libraries(${target} PRIVATE Umicom::Framework)
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings(${target})
        endif()
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers(${target})
        endif()
        set(testDirectory "${CMAKE_CURRENT_BINARY_DIR}/qualification/${case}")
        file(MAKE_DIRECTORY "${testDirectory}")
        add_test(NAME framework.ide_workflow.${case} COMMAND ${target})
        set_tests_properties(framework.ide_workflow.${case} PROPERTIES
            WORKING_DIRECTORY "${testDirectory}" TIMEOUT 30 LABELS "framework;ide-workflow;regression")
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${target})
        endif()
    endforeach()
    foreach(case IN ITEMS cancel failure)
        add_test(NAME framework.ide_workflow.project_session.${case}
            COMMAND umicom-project-project_session-test ${case})
        set_tests_properties(framework.ide_workflow.project_session.${case} PROPERTIES
            TIMEOUT 30 LABELS "framework;ide-workflow;regression")
    endforeach()
endif()


# Inert tool files verify location selection without trusting or executing a
# compiler. The test captures its own PATH evidence instead of requiring one.
if(BUILD_TESTING)
    add_executable(umicom-project-tool-catalogue-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/developer_project/test_tool_catalogue.c")
    target_link_libraries(umicom-project-tool-catalogue-test PRIVATE Umicom::developer)
    if(WIN32 AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_link_options(umicom-project-tool-catalogue-test PRIVATE -municode)
    endif()
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-project-tool-catalogue-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-project-tool-catalogue-test)
    endif()
    foreach(toolCase IN ITEMS selected missing automatic absolute invalid-compiler inherited snapshot boundaries invalid cancel)
        set(toolDirectory "${CMAKE_CURRENT_BINARY_DIR}/qualification/tool_catalogue/${toolCase}")
        file(MAKE_DIRECTORY "${toolDirectory}")
        add_test(NAME framework.ide_workflow.tool_catalogue.${toolCase}
            COMMAND umicom-project-tool-catalogue-test ${toolCase})
        set_tests_properties(framework.ide_workflow.tool_catalogue.${toolCase} PROPERTIES
            WORKING_DIRECTORY "${toolDirectory}" TIMEOUT 30 LABELS "framework;ide-workflow;toolchain;filesystem")
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-project-tool-catalogue-test)
    endif()
endif()

# Real CMake/CTest stages record execution independently from session counters.
if(BUILD_TESTING)
    add_executable(umicom-automatic-session-plan-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/developer_project/test_automatic_session_plan.c")
    target_link_libraries(umicom-automatic-session-plan-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-automatic-session-plan-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-automatic-session-plan-test)
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-automatic-session-plan-test)
    endif()
    get_filename_component(_automatic_tool_directory "${CMAKE_COMMAND}" DIRECTORY)
    foreach(_case bootstrap-build bootstrap-test reject-test reject-build existing-driver reject-driver disabled repeat)
        set(_directory "${CMAKE_CURRENT_BINARY_DIR}/qualification/automatic-plan/${_case}")
        file(MAKE_DIRECTORY "${_directory}")
        add_test(NAME framework.build.automatic_plan.${_case}
            COMMAND umicom-automatic-session-plan-test ${_case}
                "${_automatic_tool_directory}" "${CMAKE_GENERATOR}" "${CMAKE_MAKE_PROGRAM}")
        set_tests_properties(framework.build.automatic_plan.${_case} PROPERTIES
            WORKING_DIRECTORY "${_directory}" TIMEOUT 210
            LABELS "framework;build;workflow;regression")
    endforeach()
endif()

# Both the synchronous native command and the queued IDE session consume this
# read-only plan. Keep real process observations separate from contract checks.
if(BUILD_TESTING)
    add_executable(umicom-automatic-plan-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/developer_project/test_automatic_plan.c")
    target_link_libraries(umicom-automatic-plan-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-automatic-plan-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-automatic-plan-test)
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-automatic-plan-test)
    endif()
    set(_admission_directory "${CMAKE_CURRENT_BINARY_DIR}/qualification/automatic-admission")
    file(MAKE_DIRECTORY "${_admission_directory}")
    add_test(NAME framework.build.automatic_plan.shared-admission COMMAND umicom-automatic-plan-test)
    set_tests_properties(framework.build.automatic_plan.shared-admission PROPERTIES
        WORKING_DIRECTORY "${_admission_directory}" TIMEOUT 30
        LABELS "framework;build;workflow;regression;filesystem")
    if(UMICOM_BUILD_NATIVE_TOOL)
        foreach(_case bootstrap generic existing-driver reject-build reject-test reject-driver invalid-configuration empty-tests)
            add_test(NAME framework.build.automatic_verify.${_case}
                COMMAND "${CMAKE_COMMAND}" "-DCLI=$<TARGET_FILE:umicom>"
                    "-DTEST_ROOT=${CMAKE_CURRENT_BINARY_DIR}/qualification/automatic-verify/${_case}"
                    "-DCASE=${_case}" "-DGENERATOR=${CMAKE_GENERATOR}"
                    "-DMAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}"
                    "-DGENERATOR_PLATFORM=${CMAKE_GENERATOR_PLATFORM}"
                    "-DGENERATOR_TOOLSET=${CMAKE_GENERATOR_TOOLSET}"
                    -P "${CMAKE_CURRENT_LIST_DIR}/../tests/developer_project/check_automatic_verify.cmake")
            set_tests_properties(framework.build.automatic_verify.${_case} PROPERTIES
                TIMEOUT 270 LABELS "framework;build;workflow;regression;native-cli")
        endforeach()
    endif()
endif()

# Recipe admission and candidate publication use no filesystem or GUI actions.
# Keep the real source allocation seam visible to source-identity qualification.
if(BUILD_TESTING)
    add_executable(umicom-project-template-admission-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/developer_project/test_template_admission.c")
    target_link_libraries(umicom-project-template-admission-test PRIVATE Umicom::Framework)
    set_target_properties(umicom-project-template-admission-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-project-template-admission-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-project-template-admission-test)
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-project-template-admission-test)
    endif()
    foreach(_case IN ITEMS valid later-null-path later-empty-path later-null-content late-invalid-before-duplicate duplicate absolute backslash drive traversal null-language-array null-later-language empty-later-language language-count empty-primary file-count null-files api kind build-system empty-content)
        add_test(NAME framework.developer_project.template-admission.${_case}
            COMMAND umicom-project-template-admission-test ${_case})
        set_tests_properties(framework.developer_project.template-admission.${_case} PROPERTIES
            TIMEOUT 20 LABELS "framework;project;regression;no-filesystem")
    endforeach()
    add_executable(umicom-project-variable-admission-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/developer_project/test_variable_admission.c")
    target_link_libraries(umicom-project-variable-admission-test PRIVATE Umicom::Framework)
    set_target_properties(umicom-project-variable-admission-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-project-variable-admission-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-project-variable-admission-test)
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-project-variable-admission-test)
    endif()
    foreach(_case IN ITEMS empty set replace empty-value alias-name alias-value long-name long-value long-new-value count unterminated-name unterminated-value late-invalid duplicate empty-name full-append full-replace revision lookup-missing render-invalid)
        add_test(NAME framework.developer_project.variable-admission.${_case}
            COMMAND umicom-project-variable-admission-test ${_case})
        set_tests_properties(framework.developer_project.variable-admission.${_case} PROPERTIES
            TIMEOUT 20 LABELS "framework;project;regression;no-filesystem")
    endforeach()
    add_executable(umicom-project-generation-transaction-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/developer_project/test_generation_transaction.c")
    target_link_libraries(umicom-project-generation-transaction-test PRIVATE Umicom::Framework)
    set_target_properties(umicom-project-generation-transaction-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-project-generation-transaction-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-project-generation-transaction-test)
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-project-generation-transaction-test)
    endif()
    foreach(_case IN ITEMS replace late-syntax late-missing late-content-capacity late-path-capacity root-capacity id-capacity rendered-duplicate rendered-traversal rendered-drive invalid-variable-count invalid-variable-value later-null-path empty-root null-output aliased-root aliased-content empty-content maximum-files template-refusal)
        add_test(NAME framework.developer_project.generation-transaction.${_case}
            COMMAND umicom-project-generation-transaction-test ${_case})
        set_tests_properties(framework.developer_project.generation-transaction.${_case} PROPERTIES
            TIMEOUT 20 LABELS "framework;project;regression;no-filesystem")
    endforeach()
    add_executable(umicom-project-generation-allocation-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/developer_project/test_generation_allocation.c")
    target_link_libraries(umicom-project-generation-allocation-test PRIVATE Umicom::Framework)
    set_target_properties(umicom-project-generation-allocation-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-project-generation-allocation-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-project-generation-allocation-test)
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-project-generation-allocation-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-project-generation-allocation-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/generation_plan.c")
    endif()
    add_test(NAME framework.developer_project.generation-allocation COMMAND umicom-project-generation-allocation-test)
    set_tests_properties(framework.developer_project.generation-allocation PROPERTIES
        TIMEOUT 20 LABELS "framework;project;regression;allocation;no-filesystem")
endif()

# Rendering contracts run without GTK initialization or project-file writes.
if(BUILD_TESTING)
    add_executable(umicom-gtk-starter-branding-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/developer_project/test_gtk_starter_branding.c")
    target_link_libraries(umicom-gtk-starter-branding-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-gtk-starter-branding-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-gtk-starter-branding-test)
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-gtk-starter-branding-test)
    endif()
    foreach(_starter IN ITEMS c23 cpp thin)
        add_test(NAME framework.developer_project.gtk-branding.${_starter}
            COMMAND umicom-gtk-starter-branding-test ${_starter})
        set_tests_properties(framework.developer_project.gtk-branding.${_starter} PROPERTIES
            TIMEOUT 20 LABELS "framework;project;branding;regression;no-filesystem")
    endforeach()
endif()

# The command adapter reuses the real Framework service. Fixtures preserve
# previous run directories and never configure/build/launch generated products.
if(BUILD_TESTING)
    add_executable(umicom-project-cli-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/developer_project/test_project_cli.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tools/umicom/src/command_project.c")
    target_link_libraries(umicom-project-cli-test PRIVATE Umicom::Framework)
    add_executable(umicom-project-service-allocation-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/developer_project/test_service_allocation.c")
    target_link_libraries(umicom-project-service-allocation-test PRIVATE Umicom::Framework)
    foreach(_target IN ITEMS umicom-project-cli-test umicom-project-service-allocation-test)
        set_target_properties(${_target} PROPERTIES
            C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings(${_target})
        endif()
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers(${_target})
        endif()
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${_target})
        endif()
    endforeach()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-project-service-allocation-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/service.c")
    endif()
    foreach(_case IN ITEMS help templates templates-extra plan plan-files plan-c23 plan-cpp plan-thin unknown-option duplicate-option missing-value missing-option missing-apply plan-apply relative-parent escape-folder reserved-folder unknown-template oversized-name create existing-directory existing-file null-vector empty-argument unknown-command)
        add_test(NAME framework.developer_project.cli.${_case}
            COMMAND umicom-project-cli-test ${_case}
                "${CMAKE_CURRENT_BINARY_DIR}/qualification/project-command/${_case}")
        set_tests_properties(framework.developer_project.cli.${_case} PROPERTIES
            TIMEOUT 30 LABELS "framework;project;native-cli;filesystem;regression")
    endforeach()
    add_test(NAME framework.developer_project.service-allocation
        COMMAND umicom-project-service-allocation-test
            "${CMAKE_CURRENT_BINARY_DIR}/qualification/project-service-allocation/preview")
    set_tests_properties(framework.developer_project.service-allocation PROPERTIES
        TIMEOUT 30 LABELS "framework;project;allocation;regression")
    if(UMICOM_BUILD_NATIVE_TOOL)
        add_test(NAME framework.developer_project.native-route-help COMMAND "$<TARGET_FILE:umicom>" project --help)
        add_test(NAME framework.developer_project.native-route-templates COMMAND "$<TARGET_FILE:umicom>" project templates)
        set_tests_properties(framework.developer_project.native-route-help
            framework.developer_project.native-route-templates PROPERTIES
            TIMEOUT 30 LABELS "framework;project;native-cli;regression")
    endif()
endif()

# Generation admission must finish before any filesystem effect or probe.
if(BUILD_TESTING)
    add_executable(umicom-project-path-conflicts-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/developer_project/test_path_conflicts.c")
    target_link_libraries(umicom-project-path-conflicts-test PRIVATE Umicom::developer)
    umicom_apply_warnings(umicom-project-path-conflicts-test)
    umicom_apply_sanitizers(umicom-project-path-conflicts-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-project-path-conflicts-test)
    endif()
    foreach(case valid ascii-case separators first-parent second-parent siblings later-traversal git reserved duplicate-before-invalid trailing-dot trailing-space empty-component late-join-capacity null-request missing-plan unterminated)
        add_test(NAME framework.developer_project.path-conflicts.${case}
            COMMAND umicom-project-path-conflicts-test ${case})
        set_tests_properties(framework.developer_project.path-conflicts.${case}
            PROPERTIES TIMEOUT 10 LABELS "framework;developer_project;paths;regression")
    endforeach()
endif()

# Fresh-project effects and state publication have separate native and injected-boundary cases.
if(BUILD_TESTING)
    add_executable(umicom-project-generator-create-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/developer_project/test_generator_create.c")
    target_link_libraries(umicom-project-generator-create-test PRIVATE Umicom::developer)
    set_target_properties(umicom-project-generator-create-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-project-generator-create-test)
    umicom_apply_sanitizers(umicom-project-generator-create-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-project-generator-create-test)
    endif()
    foreach(case complete repeat empty-content separators unicode existing-directory existing-file missing-parent parent-link root-link dry-existing dry-missing-parent invalid-late overwrite null-request)
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/generator-create/${case}")
        add_test(NAME framework.developer_project.generator-create.${case}
            COMMAND umicom-project-generator-create-test ${case})
        set_tests_properties(framework.developer_project.generator-create.${case} PROPERTIES
            TIMEOUT 30 LABELS "framework;developer_project;regression;filesystem"
            SKIP_RETURN_CODE 77 WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/generator-create/${case}"
        )
    endforeach()
    add_executable(umicom-project-generator-create-effects-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/developer_project/test_generator_create_effects.c")
    target_link_libraries(umicom-project-generator-create-effects-test PRIVATE Umicom::developer)
    set_target_properties(umicom-project-generator-create-effects-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-project-generator-create-effects-test)
    umicom_apply_sanitizers(umicom-project-generator-create-effects-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-project-generator-create-effects-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-project-generator-create-effects-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/generator_create.c")
    endif()
    foreach(case complete root-refused root-partial ensure-refused ensure-partial first-file-refused first-file-partial second-file-refused second-file-partial late-admission dry-no-effects null-reset overwrite-no-effects)
        add_test(NAME framework.developer_project.generator-create-effects.${case}
            COMMAND umicom-project-generator-create-effects-test ${case})
        set_tests_properties(framework.developer_project.generator-create-effects.${case} PROPERTIES
            TIMEOUT 30 LABELS "framework;developer_project;regression;no-filesystem"
        )
    endforeach()
    add_executable(umicom-new-project-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/developer_project/test_new_project.c")
    target_link_libraries(umicom-new-project-test PRIVATE Umicom::developer)
    set_target_properties(umicom-new-project-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-new-project-test)
    umicom_apply_sanitizers(umicom-new-project-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-new-project-test)
    endif()
    foreach(case complete repeat dry-run dry-existing existing-directory existing-file missing-parent parent-link invalid-request unknown-template overwrite null-output null-service)
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/new-project/${case}")
        add_test(NAME framework.developer_project.new-project.${case}
            COMMAND umicom-new-project-test ${case})
        set_tests_properties(framework.developer_project.new-project.${case} PROPERTIES
            TIMEOUT 30 LABELS "framework;developer_project;regression;filesystem"
            SKIP_RETURN_CODE 77 WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/new-project/${case}"
        )
    endforeach()
    add_executable(umicom-new-project-preflight-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/developer_project/test_new_project_preflight.c")
    target_link_libraries(umicom-new-project-preflight-test PRIVATE Umicom::developer)
    set_target_properties(umicom-new-project-preflight-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-new-project-preflight-test)
    umicom_apply_sanitizers(umicom-new-project-preflight-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-new-project-preflight-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-new-project-preflight-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/service.c")
    endif()
    foreach(case profile-refusal writer-refusal complete dry-run full-registry full-registry-upsert revision-exhausted relative-root overwrite invalid-root)
        add_test(NAME framework.developer_project.new-project-preflight.${case}
            COMMAND umicom-new-project-preflight-test ${case})
        set_tests_properties(framework.developer_project.new-project-preflight.${case} PROPERTIES
            TIMEOUT 30 LABELS "framework;developer_project;regression;no-filesystem"
        )
    endforeach()
endif()

# Pure creation-result text is shared by the CLI and native project form.
if(BUILD_TESTING)
    add_executable(umicom-project-creation-message-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/developer_project/test_creation_message.c")
    target_link_libraries(umicom-project-creation-message-test PRIVATE Umicom::developer)
    umicom_apply_warnings(umicom-project-creation-message-test)
    umicom_apply_sanitizers(umicom-project-creation-message-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-project-creation-message-test)
    endif()
    foreach(case success preview preview-refusal existing partial-directory partial-file partial-collision refused unknown-progress capacity exact-capacity null-output zero-capacity report-preserved large-counts skipped-report invalid-preview invalid-flag bytes-without-file)
        add_test(NAME framework.developer_project.creation-message.${case}
            COMMAND umicom-project-creation-message-test ${case})
        set_tests_properties(framework.developer_project.creation-message.${case} PROPERTIES
            TIMEOUT 10 LABELS "framework;developer_project;diagnostics;regression;no-filesystem")
    endforeach()
endif()
