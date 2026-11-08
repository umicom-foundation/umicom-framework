#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomBuildRecipes.cmake
#
# PURPOSE:
#   Compose source recipe admission in the developer service and register format, ownership and CLI checks.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

include_guard(GLOBAL)
get_filename_component(_recipe_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
# JSON interpretation already belongs to developer. The low-level task module
# remains independent of this higher-level metadata reader.
target_sources(umicom_developer PRIVATE
    "${_recipe_root}/src/build/recipe.c"
    "${_recipe_root}/src/build/recipe_decode.c"
    "${_recipe_root}/src/build/recipe_validate.c"
    "${_recipe_root}/src/build/recipe_file.c")
target_link_libraries(umicom_developer PRIVATE Umicom::build Umicom::repository)
if(BUILD_TESTING)
    foreach(suite fixtures boundaries cli)
        add_executable(umicom-recipe-${suite}-test "${_recipe_root}/tests/build/test_recipe_${suite}.c")
        target_link_libraries(umicom-recipe-${suite}-test PRIVATE Umicom::developer Umicom::build)
        set_target_properties(umicom-recipe-${suite}-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(umicom-recipe-${suite}-test)
        umicom_apply_sanitizers(umicom-recipe-${suite}-test)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-recipe-${suite}-test)
        endif()
    endforeach()
    set(_recipe_fixture "${_recipe_root}/tests/build/recipes")
    add_test(NAME framework.recipe.fixture.minimal COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/minimal.json" OK)
    set_tests_properties(framework.recipe.fixture.minimal PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.six-phases COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/six-phases.json" OK)
    set_tests_properties(framework.recipe.fixture.six-phases PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.reverse-order COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/reverse-order.json" OK)
    set_tests_properties(framework.recipe.fixture.reverse-order PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.diamond COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/diamond.json" OK)
    set_tests_properties(framework.recipe.fixture.diamond PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.maximum-inputs COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/maximum-inputs.json" OK)
    set_tests_properties(framework.recipe.fixture.maximum-inputs PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.maximum-stages COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/maximum-stages.json" OK)
    set_tests_properties(framework.recipe.fixture.maximum-stages PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.maximum-stage-inputs COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/maximum-stage-inputs.json" OK)
    set_tests_properties(framework.recipe.fixture.maximum-stage-inputs PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.maximum-stage-outputs COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/maximum-stage-outputs.json" OK)
    set_tests_properties(framework.recipe.fixture.maximum-stage-outputs PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.maximum-dependencies COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/maximum-dependencies.json" OK)
    set_tests_properties(framework.recipe.fixture.maximum-dependencies PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.unicode-label COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/unicode-label.json" OK)
    set_tests_properties(framework.recipe.fixture.unicode-label PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.spaces-in-path COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/spaces-in-path.json" OK)
    set_tests_properties(framework.recipe.fixture.spaces-in-path PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.escaped-key COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/escaped-key.json" OK)
    set_tests_properties(framework.recipe.fixture.escaped-key PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.maximum-size-claim COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/maximum-size-claim.json" OK)
    set_tests_properties(framework.recipe.fixture.maximum-size-claim PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.unknown-root COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/unknown-root.json" PARSE_ERROR)
    set_tests_properties(framework.recipe.fixture.unknown-root PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.unknown-toolchain COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/unknown-toolchain.json" PARSE_ERROR)
    set_tests_properties(framework.recipe.fixture.unknown-toolchain PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.unknown-input COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/unknown-input.json" PARSE_ERROR)
    set_tests_properties(framework.recipe.fixture.unknown-input PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.unknown-stage COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/unknown-stage.json" PARSE_ERROR)
    set_tests_properties(framework.recipe.fixture.unknown-stage PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.duplicate-root COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/duplicate-root.json" ALREADY_EXISTS)
    set_tests_properties(framework.recipe.fixture.duplicate-root PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.duplicate-escaped-root COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/duplicate-escaped-root.json" ALREADY_EXISTS)
    set_tests_properties(framework.recipe.fixture.duplicate-escaped-root PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.duplicate-toolchain COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/duplicate-toolchain.json" ALREADY_EXISTS)
    set_tests_properties(framework.recipe.fixture.duplicate-toolchain PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.duplicate-input-member COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/duplicate-input-member.json" ALREADY_EXISTS)
    set_tests_properties(framework.recipe.fixture.duplicate-input-member PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.duplicate-stage-member COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/duplicate-stage-member.json" ALREADY_EXISTS)
    set_tests_properties(framework.recipe.fixture.duplicate-stage-member PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.missing-root COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/missing-root.json" PARSE_ERROR)
    set_tests_properties(framework.recipe.fixture.missing-root PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.missing-toolchain COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/missing-toolchain.json" PARSE_ERROR)
    set_tests_properties(framework.recipe.fixture.missing-toolchain PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.missing-input COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/missing-input.json" PARSE_ERROR)
    set_tests_properties(framework.recipe.fixture.missing-input PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.missing-stage COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/missing-stage.json" PARSE_ERROR)
    set_tests_properties(framework.recipe.fixture.missing-stage PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.unsupported-schema COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/unsupported-schema.json" PARSE_ERROR)
    set_tests_properties(framework.recipe.fixture.unsupported-schema PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.schema-string COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/schema-string.json" PARSE_ERROR)
    set_tests_properties(framework.recipe.fixture.schema-string PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.negative-size COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/negative-size.json" PARSE_ERROR)
    set_tests_properties(framework.recipe.fixture.negative-size PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.fractional-size COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/fractional-size.json" PARSE_ERROR)
    set_tests_properties(framework.recipe.fixture.fractional-size PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.overflow-size COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/overflow-size.json" CAPACITY_EXCEEDED)
    set_tests_properties(framework.recipe.fixture.overflow-size PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.empty-inputs COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/empty-inputs.json" PARSE_ERROR)
    set_tests_properties(framework.recipe.fixture.empty-inputs PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.empty-stages COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/empty-stages.json" PARSE_ERROR)
    set_tests_properties(framework.recipe.fixture.empty-stages PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.empty-outputs COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/empty-outputs.json" PARSE_ERROR)
    set_tests_properties(framework.recipe.fixture.empty-outputs PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.non-string-reference COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/non-string-reference.json" PARSE_ERROR)
    set_tests_properties(framework.recipe.fixture.non-string-reference PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.excess-inputs COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/excess-inputs.json" CAPACITY_EXCEEDED)
    set_tests_properties(framework.recipe.fixture.excess-inputs PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.excess-stages COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/excess-stages.json" CAPACITY_EXCEEDED)
    set_tests_properties(framework.recipe.fixture.excess-stages PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.excess-stage-dependencies COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/excess-stage-dependencies.json" CAPACITY_EXCEEDED)
    set_tests_properties(framework.recipe.fixture.excess-stage-dependencies PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.excess-stage-inputs COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/excess-stage-inputs.json" CAPACITY_EXCEEDED)
    set_tests_properties(framework.recipe.fixture.excess-stage-inputs PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.excess-stage-outputs COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/excess-stage-outputs.json" CAPACITY_EXCEEDED)
    set_tests_properties(framework.recipe.fixture.excess-stage-outputs PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.long-id COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/long-id.json" CAPACITY_EXCEEDED)
    set_tests_properties(framework.recipe.fixture.long-id PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.long-path COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/long-path.json" CAPACITY_EXCEEDED)
    set_tests_properties(framework.recipe.fixture.long-path PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.invalid-id COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/invalid-id.json" INVALID_ARGUMENT)
    set_tests_properties(framework.recipe.fixture.invalid-id PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.invalid-triple COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/invalid-triple.json" INVALID_ARGUMENT)
    set_tests_properties(framework.recipe.fixture.invalid-triple PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.invalid-digest COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/invalid-digest.json" INVALID_ARGUMENT)
    set_tests_properties(framework.recipe.fixture.invalid-digest PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.duplicate-input-id COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/duplicate-input-id.json" ALREADY_EXISTS)
    set_tests_properties(framework.recipe.fixture.duplicate-input-id PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.duplicate-stage-id COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/duplicate-stage-id.json" ALREADY_EXISTS)
    set_tests_properties(framework.recipe.fixture.duplicate-stage-id PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.empty-label COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/empty-label.json" INVALID_ARGUMENT)
    set_tests_properties(framework.recipe.fixture.empty-label PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.control-label COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/control-label.json" INVALID_ARGUMENT)
    set_tests_properties(framework.recipe.fixture.control-label PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.unsupported-phase COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/unsupported-phase.json" PARSE_ERROR)
    set_tests_properties(framework.recipe.fixture.unsupported-phase PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.absolute-path COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/absolute-path.json" INVALID_ARGUMENT)
    set_tests_properties(framework.recipe.fixture.absolute-path PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.drive-path COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/drive-path.json" INVALID_ARGUMENT)
    set_tests_properties(framework.recipe.fixture.drive-path PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.parent-traversal COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/parent-traversal.json" INVALID_ARGUMENT)
    set_tests_properties(framework.recipe.fixture.parent-traversal PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.current-segment COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/current-segment.json" INVALID_ARGUMENT)
    set_tests_properties(framework.recipe.fixture.current-segment PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.backslash-path COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/backslash-path.json" INVALID_ARGUMENT)
    set_tests_properties(framework.recipe.fixture.backslash-path PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.empty-segment COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/empty-segment.json" INVALID_ARGUMENT)
    set_tests_properties(framework.recipe.fixture.empty-segment PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.trailing-dot COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/trailing-dot.json" INVALID_ARGUMENT)
    set_tests_properties(framework.recipe.fixture.trailing-dot PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.trailing-space COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/trailing-space.json" INVALID_ARGUMENT)
    set_tests_properties(framework.recipe.fixture.trailing-space PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.reserved-device COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/reserved-device.json" INVALID_ARGUMENT)
    set_tests_properties(framework.recipe.fixture.reserved-device PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.reserved-port COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/reserved-port.json" INVALID_ARGUMENT)
    set_tests_properties(framework.recipe.fixture.reserved-port PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.wildcard-path COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/wildcard-path.json" INVALID_ARGUMENT)
    set_tests_properties(framework.recipe.fixture.wildcard-path PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.case-collision COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/case-collision.json" ALREADY_EXISTS)
    set_tests_properties(framework.recipe.fixture.case-collision PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.ancestor-collision COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/ancestor-collision.json" ALREADY_EXISTS)
    set_tests_properties(framework.recipe.fixture.ancestor-collision PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.toolchain-collision COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/toolchain-collision.json" ALREADY_EXISTS)
    set_tests_properties(framework.recipe.fixture.toolchain-collision PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.output-collision COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/output-collision.json" ALREADY_EXISTS)
    set_tests_properties(framework.recipe.fixture.output-collision PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.missing-source COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/missing-source.json" NOT_FOUND)
    set_tests_properties(framework.recipe.fixture.missing-source PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.missing-dependency COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/missing-dependency.json" NOT_FOUND)
    set_tests_properties(framework.recipe.fixture.missing-dependency PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.duplicate-reference COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/duplicate-reference.json" ALREADY_EXISTS)
    set_tests_properties(framework.recipe.fixture.duplicate-reference PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.duplicate-dependency COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/duplicate-dependency.json" ALREADY_EXISTS)
    set_tests_properties(framework.recipe.fixture.duplicate-dependency PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.cycle COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/cycle.json" INVALID_STATE)
    set_tests_properties(framework.recipe.fixture.cycle PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.escaped-nul COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/escaped-nul.json" PARSE_ERROR)
    set_tests_properties(framework.recipe.fixture.escaped-nul PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    add_test(NAME framework.recipe.fixture.trailing-document COMMAND umicom-recipe-fixtures-test "${_recipe_fixture}/minimal.json" "${_recipe_fixture}/trailing-document.json" PARSE_ERROR)
    set_tests_properties(framework.recipe.fixture.trailing-document PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    set(boundary_cases null-bytes zero-bytes null-output null-info null-entry null-registry byte-limit byte-excess invalid-utf8 literal-nul file-error file-null-output)
    foreach(case IN LISTS boundary_cases)
        add_test(NAME framework.recipe.boundary.${case} COMMAND umicom-recipe-boundaries-test "${case}" "${_recipe_fixture}/minimal.json")
        set_tests_properties(framework.recipe.boundary.${case} PROPERTIES TIMEOUT 30 LABELS "framework;recipe;build;regression")
    endforeach()
    set(cli_cases valid help short-help empty negative null-vector null-item empty-item unknown-action unknown-option relative-file missing-file extra-option help-mixed)
    foreach(case IN LISTS cli_cases)
        add_test(NAME framework.recipe.cli.${case} COMMAND umicom-recipe-cli-test "${case}")
        set_tests_properties(framework.recipe.cli.${case} PROPERTIES TIMEOUT 30 LABELS "framework;recipe;cli;regression")
    endforeach()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-recipe-boundaries-test FILES "${_recipe_root}/src/build/recipe_file.c" "${_recipe_root}/src/build/recipe_internal.h")
        umicom_target_qualification_inputs(umicom-recipe-cli-test FILES "${_recipe_root}/tools/umicom/src/command_recipe.c" "${_recipe_root}/tools/umicom/src/cli.h")
        umicom_target_qualification_inputs(umicom-recipe-fixtures-test FILES
            "${_recipe_fixture}/minimal.json"
            "${_recipe_fixture}/six-phases.json"
            "${_recipe_fixture}/reverse-order.json"
            "${_recipe_fixture}/diamond.json"
            "${_recipe_fixture}/maximum-inputs.json"
            "${_recipe_fixture}/maximum-stages.json"
            "${_recipe_fixture}/maximum-stage-inputs.json"
            "${_recipe_fixture}/maximum-stage-outputs.json"
            "${_recipe_fixture}/maximum-dependencies.json"
            "${_recipe_fixture}/unicode-label.json"
            "${_recipe_fixture}/spaces-in-path.json"
            "${_recipe_fixture}/escaped-key.json"
            "${_recipe_fixture}/maximum-size-claim.json"
            "${_recipe_fixture}/unknown-root.json"
            "${_recipe_fixture}/unknown-toolchain.json"
            "${_recipe_fixture}/unknown-input.json"
            "${_recipe_fixture}/unknown-stage.json"
            "${_recipe_fixture}/duplicate-root.json"
            "${_recipe_fixture}/duplicate-escaped-root.json"
            "${_recipe_fixture}/duplicate-toolchain.json"
            "${_recipe_fixture}/duplicate-input-member.json"
            "${_recipe_fixture}/duplicate-stage-member.json"
            "${_recipe_fixture}/missing-root.json"
            "${_recipe_fixture}/missing-toolchain.json"
            "${_recipe_fixture}/missing-input.json"
            "${_recipe_fixture}/missing-stage.json"
            "${_recipe_fixture}/unsupported-schema.json"
            "${_recipe_fixture}/schema-string.json"
            "${_recipe_fixture}/negative-size.json"
            "${_recipe_fixture}/fractional-size.json"
            "${_recipe_fixture}/overflow-size.json"
            "${_recipe_fixture}/empty-inputs.json"
            "${_recipe_fixture}/empty-stages.json"
            "${_recipe_fixture}/empty-outputs.json"
            "${_recipe_fixture}/non-string-reference.json"
            "${_recipe_fixture}/excess-inputs.json"
            "${_recipe_fixture}/excess-stages.json"
            "${_recipe_fixture}/excess-stage-dependencies.json"
            "${_recipe_fixture}/excess-stage-inputs.json"
            "${_recipe_fixture}/excess-stage-outputs.json"
            "${_recipe_fixture}/long-id.json"
            "${_recipe_fixture}/long-path.json"
            "${_recipe_fixture}/invalid-id.json"
            "${_recipe_fixture}/invalid-triple.json"
            "${_recipe_fixture}/invalid-digest.json"
            "${_recipe_fixture}/duplicate-input-id.json"
            "${_recipe_fixture}/duplicate-stage-id.json"
            "${_recipe_fixture}/empty-label.json"
            "${_recipe_fixture}/control-label.json"
            "${_recipe_fixture}/unsupported-phase.json"
            "${_recipe_fixture}/absolute-path.json"
            "${_recipe_fixture}/drive-path.json"
            "${_recipe_fixture}/parent-traversal.json"
            "${_recipe_fixture}/current-segment.json"
            "${_recipe_fixture}/backslash-path.json"
            "${_recipe_fixture}/empty-segment.json"
            "${_recipe_fixture}/trailing-dot.json"
            "${_recipe_fixture}/trailing-space.json"
            "${_recipe_fixture}/reserved-device.json"
            "${_recipe_fixture}/reserved-port.json"
            "${_recipe_fixture}/wildcard-path.json"
            "${_recipe_fixture}/case-collision.json"
            "${_recipe_fixture}/ancestor-collision.json"
            "${_recipe_fixture}/toolchain-collision.json"
            "${_recipe_fixture}/output-collision.json"
            "${_recipe_fixture}/missing-source.json"
            "${_recipe_fixture}/missing-dependency.json"
            "${_recipe_fixture}/duplicate-reference.json"
            "${_recipe_fixture}/duplicate-dependency.json"
            "${_recipe_fixture}/cycle.json"
            "${_recipe_fixture}/escaped-nul.json"
            "${_recipe_fixture}/trailing-document.json")
    endif()
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(framework.recipe.fixture.minimal TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.six-phases TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.reverse-order TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.diamond TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.maximum-inputs TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.maximum-stages TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.maximum-stage-inputs TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.maximum-stage-outputs TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.maximum-dependencies TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.unicode-label TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.spaces-in-path TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.escaped-key TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.maximum-size-claim TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.unknown-root TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.unknown-toolchain TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.unknown-input TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.unknown-stage TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.duplicate-root TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.duplicate-escaped-root TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.duplicate-toolchain TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.duplicate-input-member TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.duplicate-stage-member TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.missing-root TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.missing-toolchain TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.missing-input TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.missing-stage TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.unsupported-schema TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.schema-string TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.negative-size TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.fractional-size TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.overflow-size TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.empty-inputs TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.empty-stages TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.empty-outputs TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.non-string-reference TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.excess-inputs TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.excess-stages TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.excess-stage-dependencies TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.excess-stage-inputs TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.excess-stage-outputs TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.long-id TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.long-path TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.invalid-id TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.invalid-triple TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.invalid-digest TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.duplicate-input-id TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.duplicate-stage-id TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.empty-label TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.control-label TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.unsupported-phase TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.absolute-path TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.drive-path TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.parent-traversal TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.current-segment TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.backslash-path TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.empty-segment TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.trailing-dot TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.trailing-space TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.reserved-device TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.reserved-port TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.wildcard-path TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.case-collision TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.ancestor-collision TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.toolchain-collision TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.output-collision TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.missing-source TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.missing-dependency TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.duplicate-reference TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.duplicate-dependency TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.cycle TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.escaped-nul TARGETS umicom-recipe-fixtures-test)
        umicom_test_impact_owners(framework.recipe.fixture.trailing-document TARGETS umicom-recipe-fixtures-test)
        foreach(case IN LISTS boundary_cases)
            umicom_test_impact_owners(framework.recipe.boundary.${case} TARGETS umicom-recipe-boundaries-test)
        endforeach()
        foreach(case IN LISTS cli_cases)
            umicom_test_impact_owners(framework.recipe.cli.${case} TARGETS umicom-recipe-cli-test)
        endforeach()
    endif()
    if(TARGET umicom)
        foreach(case minimal cycle unknown-stage)
            add_test(NAME framework.recipe.entry.${case} COMMAND "${CMAKE_COMMAND}" "-DTOOL=$<TARGET_FILE:umicom>" "-DFILE=${_recipe_fixture}/${case}.json" "-DCASE=${case}" -P "${_recipe_root}/tests/build/check_recipe_cli.cmake")
            set_tests_properties(framework.recipe.entry.${case} PROPERTIES TIMEOUT 30 LABELS "framework;recipe;cli;integration")
            if(COMMAND umicom_test_impact_owners)
                umicom_test_impact_owners(framework.recipe.entry.${case} TARGETS umicom)
            endif()
        endforeach()
    endif()
endif()
