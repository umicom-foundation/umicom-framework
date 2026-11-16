# Umicom Framework
# File: cmake/UmicomOllamaPull.cmake
#
# PURPOSE:
#   Compose bounded Ollama progress parsing, native regression targets and a complete installed SDK project.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

include_guard(GLOBAL)
set(_umicom_pull_root "${CMAKE_CURRENT_LIST_DIR}/..")
target_sources(umicom_ai PRIVATE "${_umicom_pull_root}/src/ai/ollama_pull_event.c"
    "${_umicom_pull_root}/src/ai/ollama_pull_stream.c")
# The shared JSON owner and security primitive keep transport clients thin.
target_link_libraries(umicom_ai PRIVATE Umicom::protocol Umicom::security)
if(BUILD_TESTING)
    add_executable(umicom-ollama-pull-event-test "${_umicom_pull_root}/tests/ai/test_ollama_pull_event.c")
    add_executable(umicom-ollama-pull-stream-test "${_umicom_pull_root}/tests/ai/test_ollama_pull_stream.c")
    add_executable(umicom-ollama-pull-example "${_umicom_pull_root}/examples/ollama_pull/main.c")
    foreach(_target umicom-ollama-pull-event-test umicom-ollama-pull-stream-test umicom-ollama-pull-example)
        target_link_libraries(${_target} PRIVATE Umicom::ai)
        set_target_properties(${_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(${_target})
        umicom_apply_sanitizers(${_target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${_target})
        endif()
    endforeach()
    foreach(_case status success error error-precedence progress known-zero completed-only max-integer unknown unicode case-sensitive empty-object wrong-root missing-status empty-status empty-error empty-digest null-status wrong-error wrong-digest negative fraction exponent overflow counter-null beyond-total duplicate escaped-duplicate nested-duplicate trailing decoded-nul unpaired text-limit error-limit digest-limit byte-limit exact-byte-limit depth nodes embedded-nul invalid-utf8 cancel nulls ownership)
        add_test(NAME framework.ai.ollama_pull.event.${_case} COMMAND umicom-ollama-pull-event-test ${_case})
        set_tests_properties(framework.ai.ollama_pull.event.${_case} PROPERTIES TIMEOUT 30 LABELS "framework;ai;ollama;protocol")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.ai.ollama_pull.event.${_case} TARGETS umicom-ollama-pull-event-test)
        endif()
    endforeach()
    foreach(_case whole every-split bytewise crlf no-final-lf blank-lines whitespace-line missing-terminal empty provider-error after-success after-error late-malformed truncated callback-failure callback-read reentrant cancel-before cancel-callback cancel-finish ownership nulls occupied release-empty finish-twice feed-after-finish line-limit exact-line stream-limit event-limit max-events failure-latched)
        add_test(NAME framework.ai.ollama_pull.stream.${_case} COMMAND umicom-ollama-pull-stream-test ${_case})
        set_tests_properties(framework.ai.ollama_pull.stream.${_case} PROPERTIES TIMEOUT 90 LABELS "framework;ai;ollama;stream")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.ai.ollama_pull.stream.${_case} TARGETS umicom-ollama-pull-stream-test)
        endif()
    endforeach()
    add_test(NAME framework.ai.ollama_pull.example COMMAND umicom-ollama-pull-example)
    set_tests_properties(framework.ai.ollama_pull.example PROPERTIES TIMEOUT 20 LABELS "framework;ai;sdk")
    umicom_apply_application_branding(TARGET umicom-ollama-pull-example
        PRODUCT_NAME "Umicom Model Pull Progress Example" INTERNAL_NAME "umicom-ollama-pull-example"
        APPLICATION_ID "org.umicom.ollama-pull-example"
        RESOURCE_ROOT "${_umicom_pull_root}/resources" NO_RUNTIME_DEPLOYMENT)
endif()
foreach(_component Framework FrameworkSDK FrameworkDocumentation)
    set(_selection EXCLUDE_FROM_ALL)
    if(_component STREQUAL "Framework")
        set(_selection)
    endif()
    install(FILES "${_umicom_pull_root}/examples/ollama_pull/main.c"
        "${_umicom_pull_root}/examples/ollama_pull/CMakeLists.txt"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/examples/ollama_pull"
        COMPONENT "${_component}" ${_selection})
    install(FILES "${_umicom_pull_root}/docs/learning/ollama-streamed-model-progress.html"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/docs/learning"
        COMPONENT "${_component}" ${_selection})
endforeach()

# File observations compose existing platform ownership and stream validation.
# Keep the original event/stream tests and add executable routes independently.
target_sources(umicom_ai PRIVATE "${_umicom_pull_root}/src/ai/ollama_pull_file.c"
    "${_umicom_pull_root}/src/ai/ollama_pull_command.c")
include("${CMAKE_CURRENT_LIST_DIR}/UmicomOllamaPullTests.cmake")
if(BUILD_TESTING)
    add_executable(umicom-ollama-pull-file-test "${_umicom_pull_root}/tests/ai/test_ollama_pull_file.c")
    target_link_libraries(umicom-ollama-pull-file-test PRIVATE Umicom::ai)
    set_target_properties(umicom-ollama-pull-file-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-ollama-pull-file-test)
    umicom_apply_sanitizers(umicom-ollama-pull-file-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-ollama-pull-file-test)
    endif()
    foreach(mode success error incomplete malformed missing relative cancel nulls json json-error json-exact json-short json-zero json-null json-bounds)
        add_test(NAME "framework.ai.ollama_pull.file.${mode}" COMMAND "${CMAKE_COMMAND}"
            "-DHOST_EXE=$<TARGET_FILE:umicom-ollama-pull-file-test>"
            "-DAPI_EXE=$<TARGET_FILE:umicom-ollama-pull-file-test>"
            "-DTEST_ROOT=${CMAKE_CURRENT_BINARY_DIR}/tests/pull-file/${mode}"
            "-DTEST_CASE=${mode}" -P "${_umicom_pull_root}/tests/ai/check_pull_command.cmake")
        set_tests_properties("framework.ai.ollama_pull.file.${mode}" PROPERTIES TIMEOUT 80 LABELS "framework;ai;ollama;file")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.ai.ollama_pull.file.${mode}" TARGETS umicom-ollama-pull-file-test)
        endif()
    endforeach()
    umicom_add_pull_inspection_command_tests(umicom-ollama-pull-example framework.ai.ollama_pull.command)
    if(TARGET umicom)
        umicom_add_pull_inspection_command_tests(umicom framework.cli.models.pull)
    endif()
    if(TARGET umicom-models)
        umicom_add_pull_inspection_command_tests(umicom-models framework.models.pull STANDALONE_MODELS)
    endif()
endif()
foreach(_component Framework FrameworkSDK FrameworkDocumentation)
    set(_selection EXCLUDE_FROM_ALL)
    if(_component STREQUAL "Framework")
        set(_selection)
    endif()
    install(FILES "${CMAKE_CURRENT_LIST_DIR}/UmicomOllamaPullTests.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/UmicomFramework" COMPONENT "${_component}" ${_selection})
    install(FILES "${_umicom_pull_root}/tests/ai/check_pull_command.cmake"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/tests/ai" COMPONENT "${_component}" ${_selection})
endforeach()
