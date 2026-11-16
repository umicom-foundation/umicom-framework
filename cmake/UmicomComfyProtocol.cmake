# Umicom Framework
# File: cmake/UmicomComfyProtocol.cmake
#
# PURPOSE:
# Export native ComfyUI text-event contracts and qualify their task ownership without transport effects.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
include_guard(GLOBAL)
include(GNUInstallDirs)
include("${CMAKE_CURRENT_LIST_DIR}/UmicomSdkComponent.cmake")
set(_umicom_comfy_root "${CMAKE_CURRENT_LIST_DIR}/..")
add_library(umicom_comfy_protocol STATIC "${_umicom_comfy_root}/src/media_generation/comfy_events.c")
add_library(Umicom::comfy_protocol ALIAS umicom_comfy_protocol)
set_target_properties(umicom_comfy_protocol PROPERTIES EXPORT_NAME comfy_protocol C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_include_directories(umicom_comfy_protocol PUBLIC
    $<BUILD_INTERFACE:${_umicom_comfy_root}/include> $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(umicom_comfy_protocol PUBLIC Umicom::provider_connections PRIVATE Umicom::protocol)
umicom_apply_warnings(umicom_comfy_protocol)
umicom_apply_sanitizers(umicom_comfy_protocol)
install(TARGETS umicom_comfy_protocol EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT Framework)
umicom_register_sdk_libraries(umicom_comfy_protocol)
if(BUILD_TESTING)
    add_executable(umicom-comfy-event-test "${_umicom_comfy_root}/tests/media_generation/test_comfy_events.c")
    add_executable(umicom-comfy-example-test "${_umicom_comfy_root}/examples/comfy_observations/main.c")
    foreach(_target umicom-comfy-event-test umicom-comfy-example-test)
        target_link_libraries(${_target} PRIVATE Umicom::comfy_protocol)
        set_target_properties(${_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(${_target})
        umicom_apply_sanitizers(${_target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${_target})
        endif()
    endforeach()
    foreach(_case start progress queue executing executing-null ui-output cached success error interrupted unknown duplicate negative over-progress wrong-node missing-prompt null-data long-prompt cancel nulls task-lifecycle task-wrong-id task-stale task-provider task-operation task-unconfirmed task-ui task-global task-terminal task-cancel)
        add_test(NAME framework.media.comfy.${_case} COMMAND umicom-comfy-event-test ${_case})
        set_tests_properties(framework.media.comfy.${_case} PROPERTIES TIMEOUT 20 LABELS "framework;media;comfy;ownership")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.media.comfy.${_case} TARGETS umicom-comfy-event-test)
        endif()
    endforeach()
    add_test(NAME framework.media.comfy.sdk-example COMMAND umicom-comfy-example-test)
    set_tests_properties(framework.media.comfy.sdk-example PROPERTIES TIMEOUT 20 LABELS "framework;media;comfy;sdk")
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomApplicationBranding.cmake")
    umicom_apply_application_branding(TARGET umicom-comfy-example-test
        PRODUCT_NAME "Umicom ComfyUI Observation Example" INTERNAL_NAME "umicom-comfy-example-test"
        APPLICATION_ID "org.umicom.comfy-observation-example"
        RESOURCE_ROOT "${_umicom_comfy_root}/resources" NO_RUNTIME_DEPLOYMENT)
endif()
foreach(_component Framework FrameworkSDK FrameworkDocumentation)
    set(_selection EXCLUDE_FROM_ALL)
    if(_component STREQUAL "Framework")
        set(_selection)
    endif()
    install(FILES "${_umicom_comfy_root}/examples/comfy_observations/main.c"
        "${_umicom_comfy_root}/examples/comfy_observations/CMakeLists.txt"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/examples/comfy_observations"
        COMPONENT "${_component}" ${_selection})
endforeach()

# Submission/history reuse the protocol owner and the existing task state service.
target_sources(umicom_comfy_protocol PRIVATE
    "${_umicom_comfy_root}/src/media_generation/comfy_response.c"
    "${_umicom_comfy_root}/src/media_generation/comfy_submission.c"
    "${_umicom_comfy_root}/src/media_generation/comfy_history.c")
target_link_libraries(umicom_comfy_protocol PRIVATE Umicom::security)
if(BUILD_TESTING)
    add_executable(umicom-comfy-submission-test "${_umicom_comfy_root}/tests/media_generation/test_comfy_submission.c")
    add_executable(umicom-comfy-history-test "${_umicom_comfy_root}/tests/media_generation/test_comfy_history.c")
    add_executable(umicom-comfy-recovery-example-test "${_umicom_comfy_root}/examples/comfy_recovery/main.c")
    foreach(_target umicom-comfy-submission-test umicom-comfy-history-test umicom-comfy-recovery-example-test)
        target_link_libraries(${_target} PRIVATE Umicom::comfy_protocol)
        set_target_properties(${_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(${_target})
        umicom_apply_sanitizers(${_target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${_target})
        endif()
    endforeach()
    foreach(_case accepted fractional negative partial null-error unknown missing-id empty-id long-id missing-number wrong-number max-number long-number missing-node-errors wrong-node-errors error duplicate escaped-duplicate trailing embedded-nul root http cancel nulls task-confirm task-stale task-provider task-operation task-not-begun task-repeat task-different task-running task-invalid-id boolean-number null-number)
        add_test(NAME framework.media.comfy.submission.${_case} COMMAND umicom-comfy-submission-test ${_case})
        set_tests_properties(framework.media.comfy.submission.${_case} PROPERTIES TIMEOUT 20 LABELS "framework;media;comfy;recovery")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.media.comfy.submission.${_case} TARGETS umicom-comfy-submission-test)
        endif()
    endforeach()
    foreach(_case success-empty image source-order unicode duplicate-image opaque-path missing null-status missing-status incomplete failed contradiction unknown-state wrong-completed missing-completed missing-outputs wrong-outputs wrong-node wrong-images wrong-image empty-name missing-subfolder unknown-folder empty-node long-node max-name long-name unknown-output entry-kind root duplicate other-duplicate trailing nulls cancel max-images too-many-images ownership task-success task-failed task-stale task-provider task-operation task-unconfirmed task-other task-unknown task-terminal)
        add_test(NAME framework.media.comfy.history.${_case} COMMAND umicom-comfy-history-test ${_case})
        set_tests_properties(framework.media.comfy.history.${_case} PROPERTIES TIMEOUT 20 LABELS "framework;media;comfy;recovery")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.media.comfy.history.${_case} TARGETS umicom-comfy-history-test)
        endif()
    endforeach()
    add_test(NAME framework.media.comfy.recovery-sdk-example COMMAND umicom-comfy-recovery-example-test)
    set_tests_properties(framework.media.comfy.recovery-sdk-example PROPERTIES TIMEOUT 20 LABELS "framework;media;comfy;sdk")
    umicom_apply_application_branding(TARGET umicom-comfy-recovery-example-test
        PRODUCT_NAME "Umicom ComfyUI Recovery Example" INTERNAL_NAME "umicom-comfy-recovery-example-test"
        APPLICATION_ID "org.umicom.comfy-recovery-example"
        RESOURCE_ROOT "${_umicom_comfy_root}/resources" NO_RUNTIME_DEPLOYMENT)
endif()
foreach(_component Framework FrameworkSDK FrameworkDocumentation)
    set(_selection EXCLUDE_FROM_ALL)
    if(_component STREQUAL "Framework")
        set(_selection)
    endif()
    install(FILES "${_umicom_comfy_root}/examples/comfy_recovery/main.c"
        "${_umicom_comfy_root}/examples/comfy_recovery/CMakeLists.txt"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/examples/comfy_recovery"
        COMPONENT "${_component}" ${_selection})
endforeach()

# Immutable request preparation shares protocol parsing, settings and task state.
target_sources(umicom_comfy_protocol PRIVATE "${_umicom_comfy_root}/src/media_generation/comfy_prompt.c")
if(BUILD_TESTING)
    add_executable(umicom-comfy-prompt-test "${_umicom_comfy_root}/tests/media_generation/test_comfy_prompt.c")
    add_executable(umicom-comfy-prompt-example-test "${_umicom_comfy_root}/examples/comfy_prompt/main.c")
    foreach(_target umicom-comfy-prompt-test umicom-comfy-prompt-example-test)
        target_link_libraries(${_target} PRIVATE Umicom::comfy_protocol)
        set_target_properties(${_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(${_target})
        umicom_apply_sanitizers(${_target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${_target})
        endif()
    endforeach()
    foreach(_case body digest unicode escaped-client custom-values source-spelling ownership max-client empty-client unterminated-client invalid-client-utf8 null-draft null-output null-connection zero-revision disabled provider remote path trailing-slash secret bad-port empty array node-kind missing-type empty-type type-kind long-type missing-inputs inputs-kind empty-node-id long-node-id duplicate escaped-duplicate nested-duplicate malformed trailing embedded-nul max-bytes too-many-bytes max-nodes too-many-nodes cancel occupied review-refusal same-connection changed-label changed-timeout changed-endpoint changed-revision prepare-task task-empty-scope task-long-scope task-invalid-scope task-null-scope task-null-output confirm-history)
        add_test(NAME framework.media.comfy.prompt.${_case} COMMAND umicom-comfy-prompt-test ${_case})
        set_tests_properties(framework.media.comfy.prompt.${_case} PROPERTIES TIMEOUT 20 LABELS "framework;media;comfy;request-ownership")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.media.comfy.prompt.${_case} TARGETS umicom-comfy-prompt-test)
        endif()
    endforeach()
    add_test(NAME framework.media.comfy.prompt-sdk-example COMMAND umicom-comfy-prompt-example-test)
    set_tests_properties(framework.media.comfy.prompt-sdk-example PROPERTIES TIMEOUT 20 LABELS "framework;media;comfy;sdk")
    umicom_apply_application_branding(TARGET umicom-comfy-prompt-example-test
        PRODUCT_NAME "Umicom ComfyUI Prompt Example" INTERNAL_NAME "umicom-comfy-prompt-example-test"
        APPLICATION_ID "org.umicom.comfy-prompt-example"
        RESOURCE_ROOT "${_umicom_comfy_root}/resources" NO_RUNTIME_DEPLOYMENT)
endif()
foreach(_component Framework FrameworkSDK FrameworkDocumentation)
    set(_selection EXCLUDE_FROM_ALL)
    if(_component STREQUAL "Framework")
        set(_selection)
    endif()
    install(FILES "${_umicom_comfy_root}/examples/comfy_prompt/main.c"
        "${_umicom_comfy_root}/examples/comfy_prompt/CMakeLists.txt"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/examples/comfy_prompt"
        COMPONENT "${_component}" ${_selection})
endforeach()

# Node metadata stays in the same protocol owner as immutable prompts and recovery.
target_sources(umicom_comfy_protocol PRIVATE "${_umicom_comfy_root}/src/media_generation/comfy_nodes.c")
if(BUILD_TESTING)
    add_executable(umicom-comfy-nodes-test "${_umicom_comfy_root}/tests/media_generation/test_comfy_nodes.c")
    add_executable(umicom-comfy-nodes-example-test "${_umicom_comfy_root}/examples/comfy_nodes/main.c")
    foreach(_target umicom-comfy-nodes-test umicom-comfy-nodes-example-test)
        target_link_libraries(${_target} PRIVATE Umicom::comfy_protocol)
        set_target_properties(${_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(${_target})
        umicom_apply_sanitizers(${_target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${_target})
        endif()
    endforeach()
    foreach(_case empty minimal opaque unicode root node-kind empty-class name-mismatch name-kind missing-input input-kind required-kind optional-kind empty-input cross-group missing-output output-kind output-type empty-output display-kind category-kind flag-kind duplicate escaped-duplicate nested-duplicate malformed trailing metadata source-order schema-span input-span output-types ownership flags-unknown max-classes too-many-classes max-inputs too-many-inputs max-outputs too-many-outputs max-class long-class max-metadata long-metadata long-input long-output max-bytes too-many-bytes cancel occupied zero-revision disabled provider remote path secret same-connection changed-connection changed-revision accessor-refusal find-refusal nulls)
        add_test(NAME framework.media.comfy.nodes.${_case} COMMAND umicom-comfy-nodes-test ${_case})
        set_tests_properties(framework.media.comfy.nodes.${_case} PROPERTIES TIMEOUT 30 LABELS "framework;media;comfy;discovery")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.media.comfy.nodes.${_case} TARGETS umicom-comfy-nodes-test)
        endif()
    endforeach()
    add_test(NAME framework.media.comfy.nodes-sdk-example COMMAND umicom-comfy-nodes-example-test)
    set_tests_properties(framework.media.comfy.nodes-sdk-example PROPERTIES TIMEOUT 20 LABELS "framework;media;comfy;sdk")
    umicom_apply_application_branding(TARGET umicom-comfy-nodes-example-test
        PRODUCT_NAME "Umicom ComfyUI Node Catalogue Example" INTERNAL_NAME "umicom-comfy-nodes-example-test"
        APPLICATION_ID "org.umicom.comfy-nodes-example"
        RESOURCE_ROOT "${_umicom_comfy_root}/resources" NO_RUNTIME_DEPLOYMENT)
endif()
foreach(_component Framework FrameworkSDK FrameworkDocumentation)
    set(_selection EXCLUDE_FROM_ALL)
    if(_component STREQUAL "Framework")
        set(_selection)
    endif()
    install(FILES "${_umicom_comfy_root}/examples/comfy_nodes/main.c"
        "${_umicom_comfy_root}/examples/comfy_nodes/CMakeLists.txt"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/examples/comfy_nodes"
        COMPONENT "${_component}" ${_selection})
endforeach()

# Preflight compares immutable observations in the existing protocol owner.
target_sources(umicom_comfy_protocol PRIVATE "${_umicom_comfy_root}/src/media_generation/comfy_preflight.c")
if(BUILD_TESTING)
    add_executable(umicom-comfy-preflight-test "${_umicom_comfy_root}/tests/media_generation/test_comfy_preflight.c")
    add_executable(umicom-comfy-preflight-example-test "${_umicom_comfy_root}/examples/comfy_preflight/main.c")
    foreach(_target umicom-comfy-preflight-test umicom-comfy-preflight-example-test)
        target_link_libraries(${_target} PRIVATE Umicom::comfy_protocol)
        set_target_properties(${_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(${_target})
        umicom_apply_sanitizers(${_target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${_target})
        endif()
    endforeach()
    foreach(_case complete missing-class empty-catalogue missing-input default-not-supplied present-null opaque-value optional-absent hidden-absent extra-input value-types-not-checked links-not-checked cycles-not-checked flags-unknown flags-false all-flags output-unconnected unicode escaped-key all-nodes-inspected multiple-missing opaque-model-name diagnostic-limit diagnostic-truncation maximum-nodes maximum-bytes source-order workflow-span catalogue-spelling owner-lifetime copied-summary occupied accessor-refusal nulls observation-refusal current-revision zero-revision catalogue-revision catalogue-settings current-settings disabled cancel byte-identities)
        add_test(NAME framework.media.comfy.preflight.${_case} COMMAND umicom-comfy-preflight-test ${_case})
        set_tests_properties(framework.media.comfy.preflight.${_case} PROPERTIES TIMEOUT 40 LABELS "framework;media;comfy;preflight")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.media.comfy.preflight.${_case} TARGETS umicom-comfy-preflight-test)
        endif()
    endforeach()
    add_test(NAME framework.media.comfy.preflight-sdk-example COMMAND umicom-comfy-preflight-example-test)
    set_tests_properties(framework.media.comfy.preflight-sdk-example PROPERTIES TIMEOUT 20 LABELS "framework;media;comfy;sdk")
    umicom_apply_application_branding(TARGET umicom-comfy-preflight-example-test
        PRODUCT_NAME "Umicom ComfyUI Preflight Example" INTERNAL_NAME "umicom-comfy-preflight-example-test"
        APPLICATION_ID "org.umicom.comfy-preflight-example"
        RESOURCE_ROOT "${_umicom_comfy_root}/resources" NO_RUNTIME_DEPLOYMENT)
endif()
foreach(_component Framework FrameworkSDK FrameworkDocumentation)
    set(_selection EXCLUDE_FROM_ALL)
    if(_component STREQUAL "Framework")
        set(_selection)
    endif()
    install(FILES "${_umicom_comfy_root}/examples/comfy_preflight/main.c"
        "${_umicom_comfy_root}/examples/comfy_preflight/CMakeLists.txt"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/examples/comfy_preflight"
        COMPONENT "${_component}" ${_selection})
endforeach()

# Filesystem admission and presentation belong to Framework, shared by the CLI
# and application consoles. This adds no transport or provider activation.
target_sources(umicom_comfy_protocol PRIVATE
    "${_umicom_comfy_root}/src/media_generation/comfy_inspection.c"
    "${_umicom_comfy_root}/src/media_generation/comfy_inspection_json.c"
    "${_umicom_comfy_root}/src/media_generation/comfy_inspection_command.c")
target_link_libraries(umicom_comfy_protocol PRIVATE Umicom::platform)
include("${CMAKE_CURRENT_LIST_DIR}/UmicomComfyInspectionTests.cmake")
if(TARGET umicom)
    target_link_libraries(umicom PRIVATE Umicom::comfy_protocol)
    if(BUILD_TESTING)
        umicom_add_comfy_inspection_command_tests(umicom framework.media.comfy.command)
    endif()
endif()
if(BUILD_TESTING)
    add_executable(umicom-comfy-inspection-test "${_umicom_comfy_root}/tests/media_generation/test_comfy_inspection.c")
    target_link_libraries(umicom-comfy-inspection-test PRIVATE Umicom::comfy_protocol Umicom::platform Umicom::protocol)
    set_target_properties(umicom-comfy-inspection-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-comfy-inspection-test)
    umicom_apply_sanitizers(umicom-comfy-inspection-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-comfy-inspection-test)
    endif()
    foreach(_case complete missing missing-class disabled malformed-document malformed-workflow malformed-catalogue document-limit workflow-limit catalogue-limit missing-file directory unicode-path relative-before-read null-path client-empty client-unterminated client-utf8 cancel occupied null-output json-escaping json-exact json-refusal json-max-revision json-digests json-private-values json-truncation json-null)
        add_test(NAME framework.media.comfy.inspection.${_case} COMMAND "${CMAKE_COMMAND}"
            "-DTEST_EXE=$<TARGET_FILE:umicom-comfy-inspection-test>"
            "-DTEST_ROOT=${CMAKE_CURRENT_BINARY_DIR}/tests/comfy-inspection/${_case}"
            "-DTEST_CASE=${_case}" -P "${_umicom_comfy_root}/tests/media_generation/check_comfy_inspection.cmake")
        set_tests_properties(framework.media.comfy.inspection.${_case} PROPERTIES TIMEOUT 45 LABELS "framework;media;comfy;offline")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.media.comfy.inspection.${_case} TARGETS umicom-comfy-inspection-test)
        endif()
    endforeach()
endif()
foreach(_component Framework FrameworkSDK FrameworkDocumentation)
    set(_selection EXCLUDE_FROM_ALL)
    if(_component STREQUAL "Framework")
        set(_selection)
    endif()
    install(FILES "${CMAKE_CURRENT_LIST_DIR}/UmicomComfyInspectionTests.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/UmicomFramework" COMPONENT "${_component}" ${_selection})
    install(DIRECTORY "${_umicom_comfy_root}/examples/comfy_inspection"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/examples" COMPONENT "${_component}" ${_selection})
    install(FILES "${_umicom_comfy_root}/tests/media_generation/comfy_inspection_fixture.cmake"
        "${_umicom_comfy_root}/tests/media_generation/check_comfy_inspection_command.cmake"
        "${_umicom_comfy_root}/tests/media_generation/check_comfy_reference_command.cmake"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/tests/media_generation" COMPONENT "${_component}" ${_selection})
endforeach()

# Selection owns copied node metadata; graphical hosts can reuse it without
# inventing another parser or retaining response-backed schema pointers.
target_sources(umicom_comfy_protocol PRIVATE "${_umicom_comfy_root}/src/media_generation/comfy_palette.c")
if(BUILD_TESTING)
    add_executable(umicom-comfy-palette-test "${_umicom_comfy_root}/tests/media_generation/test_comfy_palette.c")
    add_executable(umicom-comfy-palette-example-test "${_umicom_comfy_root}/examples/comfy_palette/main.c")
    foreach(_target umicom-comfy-palette-test umicom-comfy-palette-example-test)
        target_link_libraries(${_target} PRIVATE Umicom::comfy_protocol)
        set_target_properties(${_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(${_target})
        umicom_apply_sanitizers(${_target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${_target})
        endif()
    endforeach()
    foreach(_case source class display category-order search-display search-class search-category search-miss category-tree category-leaf category-case category-prefix known-true known-false output-false combined-flags all-true all-false unicode unicode-case empty maximum unterminated-search unterminated-category invalid-search invalid-category leading-slash trailing-slash double-slash overlap-flags unknown-flags bad-order cancel occupied nulls copy owner-lifetime refusal same-catalogue changed-revision changed-settings changed-bytes)
        add_test(NAME framework.media.comfy.palette.${_case} COMMAND umicom-comfy-palette-test ${_case})
        set_tests_properties(framework.media.comfy.palette.${_case} PROPERTIES TIMEOUT 40 LABELS "framework;comfy;palette;ownership")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.media.comfy.palette.${_case} TARGETS umicom-comfy-palette-test)
        endif()
    endforeach()
    add_test(NAME framework.media.comfy.palette-sdk-example COMMAND umicom-comfy-palette-example-test)
    set_tests_properties(framework.media.comfy.palette-sdk-example PROPERTIES TIMEOUT 20 LABELS "framework;comfy;sdk")
    umicom_apply_application_branding(TARGET umicom-comfy-palette-example-test
        PRODUCT_NAME "Umicom ComfyUI Palette Example" INTERNAL_NAME "umicom-comfy-palette-example-test"
        APPLICATION_ID "org.umicom.comfy-palette-example"
        RESOURCE_ROOT "${_umicom_comfy_root}/resources" NO_RUNTIME_DEPLOYMENT)
endif()
foreach(_component Framework FrameworkSDK FrameworkDocumentation)
    set(_selection EXCLUDE_FROM_ALL)
    if(_component STREQUAL "Framework")
        set(_selection)
    endif()
    install(FILES "${_umicom_comfy_root}/examples/comfy_palette/main.c"
        "${_umicom_comfy_root}/examples/comfy_palette/CMakeLists.txt"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/examples/comfy_palette"
        COMPONENT "${_component}" ${_selection})
endforeach()

# Reference inventory shares prepared-workflow and catalogue ownership.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomComfyReferences.cmake")

# Share file-backed reference pagination without adding application-specific parsers.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomComfyReferenceInspection.cmake")

# Queue metadata shares admission and connection services with other Comfy
# readers. No application owns another queue parser or provider-control path.
target_sources(umicom_comfy_protocol PRIVATE "${_umicom_comfy_root}/src/media_generation/comfy_queue.c")
if(BUILD_TESTING)
    add_executable(umicom-comfy-queue-test "${_umicom_comfy_root}/tests/media_generation/test_comfy_queue.c")
    add_executable(umicom-comfy-queue-example-test "${_umicom_comfy_root}/examples/comfy_queue/main.c")
    foreach(_target umicom-comfy-queue-test umicom-comfy-queue-example-test)
        target_link_libraries(${_target} PRIVATE Umicom::comfy_protocol)
        set_target_properties(${_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(${_target})
        umicom_apply_sanitizers(${_target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${_target})
        endif()
    endforeach()
    foreach(mode complete empty front fraction exponent large-integer large-exponent number-boundary number-limit id-boundary id-limit unicode missing-running missing-pending wrong-root wrong-list row-type row-short number-string number-bool number-null empty-id id-type prompt-type extra-type outputs-type output-nonstring empty-output trailing decoded-nul unpaired extensions duplicate-id escaped-duplicate cross-duplicate case-sensitive duplicate-key nested-duplicate invalid-utf8 byte-limit exact-byte-limit node-limit depth-limit entry-limit maximum-entries connection-scope revision occupied nulls cancel ownership find-bounds output-preserved identity)
        add_test(NAME framework.media.comfy.queue.${mode} COMMAND umicom-comfy-queue-test ${mode})
        set_tests_properties(framework.media.comfy.queue.${mode} PROPERTIES TIMEOUT 60 LABELS "framework;media;comfy;queue;offline")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.media.comfy.queue.${mode} TARGETS umicom-comfy-queue-test)
        endif()
    endforeach()
    add_test(NAME framework.media.comfy.queue.example COMMAND umicom-comfy-queue-example-test)
    set_tests_properties(framework.media.comfy.queue.example PROPERTIES TIMEOUT 20 LABELS "framework;media;comfy;sdk")
    umicom_apply_application_branding(TARGET umicom-comfy-queue-example-test
        PRODUCT_NAME "Umicom Queue Observation Example" INTERNAL_NAME "umicom-comfy-queue-example"
        APPLICATION_ID "org.umicom.comfy-queue-example" RESOURCE_ROOT "${_umicom_comfy_root}/resources" NO_RUNTIME_DEPLOYMENT)
endif()
foreach(_component Framework FrameworkSDK FrameworkDocumentation)
    set(_selection EXCLUDE_FROM_ALL)
    if(_component STREQUAL "Framework")
        set(_selection)
    endif()
    install(DIRECTORY "${_umicom_comfy_root}/examples/comfy_queue"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/examples" COMPONENT "${_component}" ${_selection})
endforeach()


# Captured queue file admission and paged presentation stay in Framework.
target_sources(umicom_comfy_protocol PRIVATE
    "${_umicom_comfy_root}/src/media_generation/comfy_queue_file.c"
    "${_umicom_comfy_root}/src/media_generation/comfy_queue_json.c"
    "${_umicom_comfy_root}/src/media_generation/comfy_queue_command.c")
if(BUILD_TESTING)
    add_executable(umicom-comfy-queue-file-test "${_umicom_comfy_root}/tests/media_generation/test_comfy_queue_file.c")
    add_executable(umicom-comfy-queue-json-test "${_umicom_comfy_root}/tests/media_generation/test_comfy_queue_json.c")
    foreach(_target umicom-comfy-queue-file-test umicom-comfy-queue-json-test)
        target_link_libraries(${_target} PRIVATE Umicom::comfy_protocol Umicom::platform Umicom::protocol)
        set_target_properties(${_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(${_target})
        umicom_apply_sanitizers(${_target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${_target})
        endif()
        if(COMMAND umicom_target_qualification_inputs)
            umicom_target_qualification_inputs(${_target} FILES
                "${_umicom_comfy_root}/src/media_generation/comfy_queue.c"
                "${_umicom_comfy_root}/src/media_generation/comfy_queue_file.c"
                "${_umicom_comfy_root}/src/media_generation/comfy_queue_json.c"
                "${_umicom_comfy_root}/src/provider_connections/document_file.c")
        endif()
    endforeach()
    foreach(mode complete empty disabled malformed-document malformed-queue missing-file directory unicode-path relative-before-read null-document null-queue cancel occupied null-output queue-limit document-limit)
        add_test(NAME framework.media.comfy.queue_file.${mode} COMMAND "${CMAKE_COMMAND}"
            "-DTEST_EXE=$<TARGET_FILE:umicom-comfy-queue-file-test>"
            "-DTEST_ROOT=${CMAKE_CURRENT_BINARY_DIR}/tests/comfy-queue-file/${mode}"
            "-DTEST_CASE=${mode}" -P "${_umicom_comfy_root}/tests/media_generation/check_comfy_queue_file.cmake")
        set_tests_properties(framework.media.comfy.queue_file.${mode} PROPERTIES TIMEOUT 45 LABELS "framework;media;comfy;queue;native-file")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.media.comfy.queue_file.${mode} TARGETS umicom-comfy-queue-file-test)
        endif()
    endforeach()
    foreach(mode summary first-page second-page last-page end-page empty beyond zero-limit large-limit null-owner null-out null-length exact-capacity small-capacity zero-capacity escaping number-string max-revision digests private-values maximum-page)
        add_test(NAME framework.media.comfy.queue_json.${mode} COMMAND umicom-comfy-queue-json-test "${mode}")
        set_tests_properties(framework.media.comfy.queue_json.${mode} PROPERTIES TIMEOUT 30 LABELS "framework;media;comfy;queue;json")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.media.comfy.queue_json.${mode} TARGETS umicom-comfy-queue-json-test)
        endif()
    endforeach()
    umicom_add_comfy_queue_command_tests(umicom-comfy-queue-example-test framework.media.comfy.queue_sdk)
    if(TARGET umicom)
        umicom_add_comfy_queue_command_tests(umicom framework.media.comfy.queue_command)
    endif()
endif()
foreach(_component Framework FrameworkSDK FrameworkDocumentation)
    set(_selection EXCLUDE_FROM_ALL)
    if(_component STREQUAL "Framework")
        set(_selection)
    endif()
    install(FILES
        "${_umicom_comfy_root}/tests/media_generation/comfy_queue_fixture.cmake"
        "${_umicom_comfy_root}/tests/media_generation/check_comfy_queue_command.cmake"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/tests/media_generation"
        COMPONENT "${_component}" ${_selection})
endforeach()

# System/device reporting shares protocol and connection ownership with existing readers.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomComfySystem.cmake")


# Typed input metadata remains part of the existing immutable catalogue owner.
if(COMMAND umicom_release_inventory_own_headers)
    umicom_release_inventory_own_headers(umicom_comfy_protocol
        include/umicom/media_generation/comfy_numeric_input.h)
endif()
if(BUILD_TESTING)
    add_executable(umicom-comfy-numeric-input-test
        "${_umicom_comfy_root}/tests/media_generation/test_comfy_numeric_input.c")
    add_executable(umicom-comfy-numeric-input-example-test
        "${_umicom_comfy_root}/examples/comfy_nodes/numeric.c")
    foreach(_target umicom-comfy-numeric-input-test umicom-comfy-numeric-input-example-test)
        target_link_libraries(${_target} PRIVATE Umicom::comfy_protocol)
        set_target_properties(${_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(${_target})
        umicom_apply_sanitizers(${_target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${_target})
        endif()
        if(COMMAND umicom_target_qualification_inputs)
            umicom_target_qualification_inputs(${_target} FILES
                "${_umicom_comfy_root}/src/media_generation/comfy_nodes.c"
                "${_umicom_comfy_root}/src/language_runtime/json_tree.c"
                "${_umicom_comfy_root}/src/base/ascii_number.c")
        endif()
    endforeach()
    foreach(mode integer-complete decimal-complete integer-minimal decimal-minimal known-zero negative-zero exponent missing-default integer-adjacent integer-below integer-above integer-reversed integer-zero-step integer-negative-step decimal-below decimal-above decimal-reversed decimal-zero-step decimal-negative-step force-false unknown-options optional-group report-lifetime integer-fraction integer-exponent numeric-null numeric-boolean numeric-string flag-number options-array empty-array type-null type-number integer-overflow decimal-overflow decimal-underflow string-type boolean-type tensor-type choice-type object-schema extra-elements null-catalogue null-output invalid-group missing-node missing-input missing-group)
        add_test(NAME framework.media.comfy.numeric_input.${mode}
            COMMAND umicom-comfy-numeric-input-test "${mode}")
        set_tests_properties(framework.media.comfy.numeric_input.${mode} PROPERTIES
            TIMEOUT 30 LABELS "framework;media;comfy;numeric;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.media.comfy.numeric_input.${mode}
                TARGETS umicom-comfy-numeric-input-test)
        endif()
    endforeach()
    add_test(NAME framework.media.comfy.numeric_input.sdk-example
        COMMAND umicom-comfy-numeric-input-example-test)
    set_tests_properties(framework.media.comfy.numeric_input.sdk-example PROPERTIES
        TIMEOUT 20 LABELS "framework;media;comfy;sdk")
    umicom_apply_application_branding(TARGET umicom-comfy-numeric-input-example-test
        PRODUCT_NAME "Umicom Numeric Node Example" INTERNAL_NAME "umicom-comfy-numeric-input-example"
        APPLICATION_ID "org.umicom.comfy-numeric-input-example"
        RESOURCE_ROOT "${_umicom_comfy_root}/resources" NO_RUNTIME_DEPLOYMENT)
endif()
foreach(_component Framework FrameworkSDK FrameworkDocumentation)
    set(_selection EXCLUDE_FROM_ALL)
    if(_component STREQUAL "Framework")
        set(_selection)
    endif()
    install(FILES "${_umicom_comfy_root}/examples/comfy_nodes/numeric.c"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/examples/comfy_nodes"
        COMPONENT "${_component}" ${_selection})
endforeach()


# String-choice metadata is read from the existing immutable catalogue owner.
if(COMMAND umicom_release_inventory_own_headers)
    umicom_release_inventory_own_headers(umicom_comfy_protocol
        include/umicom/media_generation/comfy_choice_input.h)
endif()
if(BUILD_TESTING)
    add_executable(umicom-comfy-choice-input-test
        "${_umicom_comfy_root}/tests/media_generation/test_comfy_choice_input.c")
    add_executable(umicom-comfy-choice-input-example-test
        "${_umicom_comfy_root}/examples/comfy_nodes/choices.c")
    foreach(_target umicom-comfy-choice-input-test umicom-comfy-choice-input-example-test)
        target_link_libraries(${_target} PRIVATE Umicom::comfy_protocol)
        set_target_properties(${_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(${_target})
        umicom_apply_sanitizers(${_target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${_target})
        endif()
        if(COMMAND umicom_target_qualification_inputs)
            umicom_target_qualification_inputs(${_target} FILES
                "${_umicom_comfy_root}/src/media_generation/comfy_nodes.c"
                "${_umicom_comfy_root}/src/language_runtime/json_tree.c"
                "${_umicom_comfy_root}/src/language_runtime/json_text.c")
        endif()
    endforeach()
    foreach(mode one ordered empty-list empty-value duplicates escaped-duplicate unicode escaped-controls markup-text opaque-options empty-options optional-group copy-lifetime late-number boolean-choice null-choice object-choice array-choice named-combo numeric-type string-type custom-object extra-elements empty-descriptor null-type number-type object-type array-options null-options exact-capacity small-capacity zero-capacity null-text null-report null-catalogue invalid-group missing-node missing-input missing-group missing-choice long-choice many-choices)
        add_test(NAME framework.media.comfy.choice_input.${mode}
            COMMAND umicom-comfy-choice-input-test "${mode}")
        set_tests_properties(framework.media.comfy.choice_input.${mode} PROPERTIES
            TIMEOUT 30 LABELS "framework;media;comfy;choices;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.media.comfy.choice_input.${mode}
                TARGETS umicom-comfy-choice-input-test)
        endif()
    endforeach()
    add_test(NAME framework.media.comfy.choice_input.sdk-example COMMAND umicom-comfy-choice-input-example-test)
    set_tests_properties(framework.media.comfy.choice_input.sdk-example PROPERTIES
        TIMEOUT 20 LABELS "framework;media;comfy;sdk")
    umicom_apply_application_branding(TARGET umicom-comfy-choice-input-example-test
        PRODUCT_NAME "Umicom Choice Node Example" INTERNAL_NAME "umicom-comfy-choice-input-example"
        APPLICATION_ID "org.umicom.comfy-choice-input-example"
        RESOURCE_ROOT "${_umicom_comfy_root}/resources" NO_RUNTIME_DEPLOYMENT)
endif()
foreach(_component Framework FrameworkSDK FrameworkDocumentation)
    set(_selection EXCLUDE_FROM_ALL)
    if(_component STREQUAL "Framework")
        set(_selection)
    endif()
    install(FILES "${_umicom_comfy_root}/examples/comfy_nodes/choices.c"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/examples/comfy_nodes"
        COMPONENT "${_component}" ${_selection})
endforeach()


# Text and Boolean metadata share the existing immutable catalogue owner.
if(COMMAND umicom_release_inventory_own_headers)
    umicom_release_inventory_own_headers(umicom_comfy_protocol
        include/umicom/media_generation/comfy_literal_input.h)
endif()
if(BUILD_TESTING)
    add_executable(umicom-comfy-literal-input-test
        "${_umicom_comfy_root}/tests/media_generation/test_comfy_literal_input.c")
    add_executable(umicom-comfy-literal-input-example-test
        "${_umicom_comfy_root}/examples/comfy_nodes/literals.c")
    foreach(_target umicom-comfy-literal-input-test umicom-comfy-literal-input-example-test)
        target_link_libraries(${_target} PRIVATE Umicom::comfy_protocol)
        set_target_properties(${_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(${_target})
        umicom_apply_sanitizers(${_target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${_target})
        endif()
        if(COMMAND umicom_target_qualification_inputs)
            umicom_target_qualification_inputs(${_target} FILES
                "${_umicom_comfy_root}/src/media_generation/comfy_nodes.c"
                "${_umicom_comfy_root}/src/language_runtime/json_tree.c"
                "${_umicom_comfy_root}/src/language_runtime/json_text.c")
        endif()
    endforeach()
    foreach(mode string-minimal boolean-minimal string-empty boolean-false boolean-true string-flags flags-false boolean-force boolean-opaque-string-flags unknown-options missing-default-flags unicode escaped-controls template-text string-null string-number string-bool bool-null bool-number bool-string force-number multiline-string dynamic-null late-invalid-flag options-array options-null empty-descriptor null-type number-type object-type numeric-type image-type choice-type custom-object extra-elements optional-group copy-lifetime exact-capacity small-capacity zero-capacity null-text null-report null-catalogue invalid-group missing-node missing-input missing-group long-default)
        add_test(NAME framework.media.comfy.literal_input.${mode}
            COMMAND umicom-comfy-literal-input-test "${mode}")
        set_tests_properties(framework.media.comfy.literal_input.${mode} PROPERTIES
            TIMEOUT 30 LABELS "framework;media;comfy;literal;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.media.comfy.literal_input.${mode}
                TARGETS umicom-comfy-literal-input-test)
        endif()
    endforeach()
    add_test(NAME framework.media.comfy.literal_input.sdk-example COMMAND umicom-comfy-literal-input-example-test)
    set_tests_properties(framework.media.comfy.literal_input.sdk-example PROPERTIES
        TIMEOUT 20 LABELS "framework;media;comfy;sdk")
    umicom_apply_application_branding(TARGET umicom-comfy-literal-input-example-test
        PRODUCT_NAME "Umicom Literal Node Example" INTERNAL_NAME "umicom-comfy-literal-input-example"
        APPLICATION_ID "org.umicom.comfy-literal-input-example"
        RESOURCE_ROOT "${_umicom_comfy_root}/resources" NO_RUNTIME_DEPLOYMENT)
endif()
foreach(_component Framework FrameworkSDK FrameworkDocumentation)
    set(_selection EXCLUDE_FROM_ALL)
    if(_component STREQUAL "Framework")
        set(_selection)
    endif()
    install(FILES "${_umicom_comfy_root}/examples/comfy_nodes/literals.c"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/examples/comfy_nodes"
        COMPONENT "${_component}" ${_selection})
endforeach()

# Captured node browsing composes the immutable catalogue and selector.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomComfyNodePages.cmake")
