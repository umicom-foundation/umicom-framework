#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomLanguageRuntimePlatform.cmake
#
# PURPOSE:
#   Attach the persistent Language Server Protocol / IntelliSense runtime to the
#   canonical developer library and register focused regression tests.
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
    message(FATAL_ERROR "Language Runtime requires the canonical umicom_developer target")
endif()
target_sources(umicom_developer PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/arguments.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/builtin_profiles.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/decoder_support.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/decoders/code_actions.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/decoders/completion.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/decoders/diagnostics.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/decoders/folding_ranges.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/decoders/hover.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/decoders/initialize.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/decoders/inlay_hints.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/decoders/locations.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/decoders/semantic_tokens.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/decoders/signature.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/decoders/symbols.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/decoders/text_edits.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/decoders/workspace_edit.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/dispatcher.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/document_sync.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/editor_bridge.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/framing.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/json.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/json_writer.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/language_id.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/memory_transport.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/message.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/pending.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/platform.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/process_stream.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/profile_health.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/profiles/bash.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/profiles/clangd.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/profiles/cmake.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/profiles/css.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/profiles/html.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/profiles/jdtls.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/profiles/json.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/profiles/marksman.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/profiles/php.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/profiles/pyright.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/profiles/rust_analyzer.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/profiles/sqls.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/profiles/typescript.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/profiles/yaml.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/profiles/zls.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/request_context.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/request_support.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/requests/code_action.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/requests/completion.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/requests/definition.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/requests/did_change.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/requests/did_close.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/requests/did_open.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/requests/did_save.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/requests/document_symbols.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/requests/exit.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/requests/folding_ranges.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/requests/formatting.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/requests/hover.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/requests/initialize.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/requests/initialized.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/requests/inlay_hints.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/requests/references.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/requests/rename.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/requests/semantic_tokens.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/requests/shutdown.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/requests/signature_help.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/requests/workspace_symbols.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/server.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/server_manager.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/service_bridge.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/transport.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/types.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/workbench_bridge.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/workspace_edit_bridge.c"
)
if(BUILD_TESTING)
    add_executable(umicom-language-runtime-arguments
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_arguments.c"
    )
    target_include_directories(umicom-language-runtime-arguments PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-arguments PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-arguments)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-arguments)
    endif()
    add_test(NAME framework.language_runtime.arguments COMMAND umicom-language-runtime-arguments)
    add_executable(umicom-language-runtime-builtin-profiles
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_builtin_profiles.c"
    )
    target_include_directories(umicom-language-runtime-builtin-profiles PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-builtin-profiles PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-builtin-profiles)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-builtin-profiles)
    endif()
    add_test(NAME framework.language_runtime.builtin-profiles COMMAND umicom-language-runtime-builtin-profiles)
    add_executable(umicom-language-runtime-decoder-code-actions
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_decoder_code_actions.c"
    )
    target_include_directories(umicom-language-runtime-decoder-code-actions PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-decoder-code-actions PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-decoder-code-actions)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-decoder-code-actions)
    endif()
    add_test(NAME framework.language_runtime.decoder-code-actions COMMAND umicom-language-runtime-decoder-code-actions)
    add_executable(umicom-language-runtime-decoder-completion
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_decoder_completion.c"
    )
    target_include_directories(umicom-language-runtime-decoder-completion PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-decoder-completion PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-decoder-completion)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-decoder-completion)
    endif()
    add_test(NAME framework.language_runtime.decoder-completion COMMAND umicom-language-runtime-decoder-completion)
    add_executable(umicom-language-runtime-decoder-diagnostics
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_decoder_diagnostics.c"
    )
    target_include_directories(umicom-language-runtime-decoder-diagnostics PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-decoder-diagnostics PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-decoder-diagnostics)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-decoder-diagnostics)
    endif()
    add_test(NAME framework.language_runtime.decoder-diagnostics COMMAND umicom-language-runtime-decoder-diagnostics)
    add_executable(umicom-language-runtime-decoder-folding-ranges
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_decoder_folding_ranges.c"
    )
    target_include_directories(umicom-language-runtime-decoder-folding-ranges PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-decoder-folding-ranges PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-decoder-folding-ranges)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-decoder-folding-ranges)
    endif()
    add_test(NAME framework.language_runtime.decoder-folding-ranges COMMAND umicom-language-runtime-decoder-folding-ranges)
    add_executable(umicom-language-runtime-decoder-hover
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_decoder_hover.c"
    )
    target_include_directories(umicom-language-runtime-decoder-hover PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-decoder-hover PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-decoder-hover)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-decoder-hover)
    endif()
    add_test(NAME framework.language_runtime.decoder-hover COMMAND umicom-language-runtime-decoder-hover)
    add_executable(umicom-language-runtime-decoder-initialize
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_decoder_initialize.c"
    )
    target_include_directories(umicom-language-runtime-decoder-initialize PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-decoder-initialize PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-decoder-initialize)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-decoder-initialize)
    endif()
    add_test(NAME framework.language_runtime.decoder-initialize COMMAND umicom-language-runtime-decoder-initialize)
    add_executable(umicom-language-runtime-decoder-inlay-hints
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_decoder_inlay_hints.c"
    )
    target_include_directories(umicom-language-runtime-decoder-inlay-hints PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-decoder-inlay-hints PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-decoder-inlay-hints)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-decoder-inlay-hints)
    endif()
    add_test(NAME framework.language_runtime.decoder-inlay-hints COMMAND umicom-language-runtime-decoder-inlay-hints)
    add_executable(umicom-language-runtime-decoder-locations
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_decoder_locations.c"
    )
    target_include_directories(umicom-language-runtime-decoder-locations PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-decoder-locations PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-decoder-locations)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-decoder-locations)
    endif()
    add_test(NAME framework.language_runtime.decoder-locations COMMAND umicom-language-runtime-decoder-locations)
    add_executable(umicom-language-runtime-decoder-semantic-tokens
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_decoder_semantic_tokens.c"
    )
    target_include_directories(umicom-language-runtime-decoder-semantic-tokens PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-decoder-semantic-tokens PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-decoder-semantic-tokens)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-decoder-semantic-tokens)
    endif()
    add_test(NAME framework.language_runtime.decoder-semantic-tokens COMMAND umicom-language-runtime-decoder-semantic-tokens)
    add_executable(umicom-language-runtime-decoder-signature
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_decoder_signature.c"
    )
    target_include_directories(umicom-language-runtime-decoder-signature PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-decoder-signature PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-decoder-signature)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-decoder-signature)
    endif()
    add_test(NAME framework.language_runtime.decoder-signature COMMAND umicom-language-runtime-decoder-signature)
    add_executable(umicom-language-runtime-decoder-symbols
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_decoder_symbols.c"
    )
    target_include_directories(umicom-language-runtime-decoder-symbols PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-decoder-symbols PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-decoder-symbols)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-decoder-symbols)
    endif()
    add_test(NAME framework.language_runtime.decoder-symbols COMMAND umicom-language-runtime-decoder-symbols)
    add_executable(umicom-language-runtime-decoder-text-edits
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_decoder_text_edits.c"
    )
    target_include_directories(umicom-language-runtime-decoder-text-edits PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-decoder-text-edits PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-decoder-text-edits)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-decoder-text-edits)
    endif()
    add_test(NAME framework.language_runtime.decoder-text-edits COMMAND umicom-language-runtime-decoder-text-edits)
    add_executable(umicom-language-runtime-decoder-workspace-edit
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_decoder_workspace_edit.c"
    )
    target_include_directories(umicom-language-runtime-decoder-workspace-edit PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-decoder-workspace-edit PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-decoder-workspace-edit)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-decoder-workspace-edit)
    endif()
    add_test(NAME framework.language_runtime.decoder-workspace-edit COMMAND umicom-language-runtime-decoder-workspace-edit)
    add_executable(umicom-language-runtime-document-sync
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_document_sync.c"
    )
    target_include_directories(umicom-language-runtime-document-sync PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-document-sync PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-document-sync)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-document-sync)
    endif()
    add_test(NAME framework.language_runtime.document-sync COMMAND umicom-language-runtime-document-sync)
    add_executable(umicom-language-runtime-editor-bridge
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_editor_bridge.c"
    )
    target_include_directories(umicom-language-runtime-editor-bridge PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-editor-bridge PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-editor-bridge)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-editor-bridge)
    endif()
    add_test(NAME framework.language_runtime.editor-bridge COMMAND umicom-language-runtime-editor-bridge)
    add_executable(umicom-language-runtime-framing
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_framing.c"
    )
    target_include_directories(umicom-language-runtime-framing PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-framing PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-framing)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-framing)
    endif()
    add_test(NAME framework.language_runtime.framing COMMAND umicom-language-runtime-framing)
    add_executable(umicom-language-runtime-json
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_json.c"
    )
    target_include_directories(umicom-language-runtime-json PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-json PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-json)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-json)
    endif()
    add_test(NAME framework.language_runtime.json COMMAND umicom-language-runtime-json)
    add_executable(umicom-language-runtime-json-writer
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_json_writer.c"
    )
    target_include_directories(umicom-language-runtime-json-writer PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-json-writer PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-json-writer)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-json-writer)
    endif()
    add_test(NAME framework.language_runtime.json-writer COMMAND umicom-language-runtime-json-writer)
    add_executable(umicom-language-runtime-language-id
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_language_id.c"
    )
    target_include_directories(umicom-language-runtime-language-id PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-language-id PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-language-id)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-language-id)
    endif()
    add_test(NAME framework.language_runtime.language-id COMMAND umicom-language-runtime-language-id)
    add_executable(umicom-language-runtime-memory-transport
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_memory_transport.c"
    )
    target_include_directories(umicom-language-runtime-memory-transport PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-memory-transport PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-memory-transport)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-memory-transport)
    endif()
    add_test(NAME framework.language_runtime.memory-transport COMMAND umicom-language-runtime-memory-transport)
    add_executable(umicom-language-runtime-message
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_message.c"
    )
    target_include_directories(umicom-language-runtime-message PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-message PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-message)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-message)
    endif()
    add_test(NAME framework.language_runtime.message COMMAND umicom-language-runtime-message)
    add_executable(umicom-language-runtime-pending
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_pending.c"
    )
    target_include_directories(umicom-language-runtime-pending PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-pending PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-pending)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-pending)
    endif()
    add_test(NAME framework.language_runtime.pending COMMAND umicom-language-runtime-pending)
    add_executable(umicom-language-runtime-platform
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_platform.c"
    )
    target_include_directories(umicom-language-runtime-platform PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-platform PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-platform)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-platform)
    endif()
    add_test(NAME framework.language_runtime.platform COMMAND umicom-language-runtime-platform)
    add_executable(umicom-language-runtime-process-stream-contract
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_process_stream_contract.c"
    )
    target_include_directories(umicom-language-runtime-process-stream-contract PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-process-stream-contract PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-process-stream-contract)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-process-stream-contract)
    endif()
    add_test(NAME framework.language_runtime.process-stream-contract COMMAND umicom-language-runtime-process-stream-contract)
    add_executable(umicom-language-runtime-profile-bash
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_profile_bash.c"
    )
    target_include_directories(umicom-language-runtime-profile-bash PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-profile-bash PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-profile-bash)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-profile-bash)
    endif()
    add_test(NAME framework.language_runtime.profile-bash COMMAND umicom-language-runtime-profile-bash)
    add_executable(umicom-language-runtime-profile-clangd
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_profile_clangd.c"
    )
    target_include_directories(umicom-language-runtime-profile-clangd PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-profile-clangd PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-profile-clangd)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-profile-clangd)
    endif()
    add_test(NAME framework.language_runtime.profile-clangd COMMAND umicom-language-runtime-profile-clangd)
    add_executable(umicom-language-runtime-profile-cmake
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_profile_cmake.c"
    )
    target_include_directories(umicom-language-runtime-profile-cmake PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-profile-cmake PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-profile-cmake)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-profile-cmake)
    endif()
    add_test(NAME framework.language_runtime.profile-cmake COMMAND umicom-language-runtime-profile-cmake)
    add_executable(umicom-language-runtime-profile-css
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_profile_css.c"
    )
    target_include_directories(umicom-language-runtime-profile-css PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-profile-css PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-profile-css)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-profile-css)
    endif()
    add_test(NAME framework.language_runtime.profile-css COMMAND umicom-language-runtime-profile-css)
    add_executable(umicom-language-runtime-profile-html
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_profile_html.c"
    )
    target_include_directories(umicom-language-runtime-profile-html PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-profile-html PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-profile-html)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-profile-html)
    endif()
    add_test(NAME framework.language_runtime.profile-html COMMAND umicom-language-runtime-profile-html)
    add_executable(umicom-language-runtime-profile-jdtls
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_profile_jdtls.c"
    )
    target_include_directories(umicom-language-runtime-profile-jdtls PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-profile-jdtls PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-profile-jdtls)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-profile-jdtls)
    endif()
    add_test(NAME framework.language_runtime.profile-jdtls COMMAND umicom-language-runtime-profile-jdtls)
    add_executable(umicom-language-runtime-profile-json
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_profile_json.c"
    )
    target_include_directories(umicom-language-runtime-profile-json PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-profile-json PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-profile-json)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-profile-json)
    endif()
    add_test(NAME framework.language_runtime.profile-json COMMAND umicom-language-runtime-profile-json)
    add_executable(umicom-language-runtime-profile-marksman
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_profile_marksman.c"
    )
    target_include_directories(umicom-language-runtime-profile-marksman PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-profile-marksman PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-profile-marksman)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-profile-marksman)
    endif()
    add_test(NAME framework.language_runtime.profile-marksman COMMAND umicom-language-runtime-profile-marksman)
    add_executable(umicom-language-runtime-profile-php
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_profile_php.c"
    )
    target_include_directories(umicom-language-runtime-profile-php PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-profile-php PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-profile-php)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-profile-php)
    endif()
    add_test(NAME framework.language_runtime.profile-php COMMAND umicom-language-runtime-profile-php)
    add_executable(umicom-language-runtime-profile-pyright
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_profile_pyright.c"
    )
    target_include_directories(umicom-language-runtime-profile-pyright PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-profile-pyright PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-profile-pyright)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-profile-pyright)
    endif()
    add_test(NAME framework.language_runtime.profile-pyright COMMAND umicom-language-runtime-profile-pyright)
    add_executable(umicom-language-runtime-profile-rust-analyzer
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_profile_rust_analyzer.c"
    )
    target_include_directories(umicom-language-runtime-profile-rust-analyzer PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-profile-rust-analyzer PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-profile-rust-analyzer)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-profile-rust-analyzer)
    endif()
    add_test(NAME framework.language_runtime.profile-rust-analyzer COMMAND umicom-language-runtime-profile-rust-analyzer)
    add_executable(umicom-language-runtime-profile-sqls
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_profile_sqls.c"
    )
    target_include_directories(umicom-language-runtime-profile-sqls PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-profile-sqls PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-profile-sqls)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-profile-sqls)
    endif()
    add_test(NAME framework.language_runtime.profile-sqls COMMAND umicom-language-runtime-profile-sqls)
    add_executable(umicom-language-runtime-profile-typescript
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_profile_typescript.c"
    )
    target_include_directories(umicom-language-runtime-profile-typescript PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-profile-typescript PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-profile-typescript)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-profile-typescript)
    endif()
    add_test(NAME framework.language_runtime.profile-typescript COMMAND umicom-language-runtime-profile-typescript)
    add_executable(umicom-language-runtime-profile-yaml
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_profile_yaml.c"
    )
    target_include_directories(umicom-language-runtime-profile-yaml PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-profile-yaml PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-profile-yaml)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-profile-yaml)
    endif()
    add_test(NAME framework.language_runtime.profile-yaml COMMAND umicom-language-runtime-profile-yaml)
    add_executable(umicom-language-runtime-profile-zls
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_profile_zls.c"
    )
    target_include_directories(umicom-language-runtime-profile-zls PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-profile-zls PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-profile-zls)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-profile-zls)
    endif()
    add_test(NAME framework.language_runtime.profile-zls COMMAND umicom-language-runtime-profile-zls)
    add_executable(umicom-language-runtime-request-code-action
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_request_code_action.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/request_test_support.c"
    )
    target_include_directories(umicom-language-runtime-request-code-action PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-request-code-action PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-request-code-action)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-request-code-action)
    endif()
    add_test(NAME framework.language_runtime.request-code-action COMMAND umicom-language-runtime-request-code-action)
    add_executable(umicom-language-runtime-request-completion
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_request_completion.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/request_test_support.c"
    )
    target_include_directories(umicom-language-runtime-request-completion PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-request-completion PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-request-completion)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-request-completion)
    endif()
    add_test(NAME framework.language_runtime.request-completion COMMAND umicom-language-runtime-request-completion)
    add_executable(umicom-language-runtime-request-definition
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_request_definition.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/request_test_support.c"
    )
    target_include_directories(umicom-language-runtime-request-definition PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-request-definition PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-request-definition)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-request-definition)
    endif()
    add_test(NAME framework.language_runtime.request-definition COMMAND umicom-language-runtime-request-definition)
    add_executable(umicom-language-runtime-request-did-change
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_request_did_change.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/request_test_support.c"
    )
    target_include_directories(umicom-language-runtime-request-did-change PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-request-did-change PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-request-did-change)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-request-did-change)
    endif()
    add_test(NAME framework.language_runtime.request-did-change COMMAND umicom-language-runtime-request-did-change)
    add_executable(umicom-language-runtime-request-did-close
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_request_did_close.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/request_test_support.c"
    )
    target_include_directories(umicom-language-runtime-request-did-close PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-request-did-close PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-request-did-close)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-request-did-close)
    endif()
    add_test(NAME framework.language_runtime.request-did-close COMMAND umicom-language-runtime-request-did-close)
    add_executable(umicom-language-runtime-request-did-open
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_request_did_open.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/request_test_support.c"
    )
    target_include_directories(umicom-language-runtime-request-did-open PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-request-did-open PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-request-did-open)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-request-did-open)
    endif()
    add_test(NAME framework.language_runtime.request-did-open COMMAND umicom-language-runtime-request-did-open)
    add_executable(umicom-language-runtime-request-did-save
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_request_did_save.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/request_test_support.c"
    )
    target_include_directories(umicom-language-runtime-request-did-save PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-request-did-save PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-request-did-save)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-request-did-save)
    endif()
    add_test(NAME framework.language_runtime.request-did-save COMMAND umicom-language-runtime-request-did-save)
    add_executable(umicom-language-runtime-request-document-symbols
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_request_document_symbols.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/request_test_support.c"
    )
    target_include_directories(umicom-language-runtime-request-document-symbols PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-request-document-symbols PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-request-document-symbols)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-request-document-symbols)
    endif()
    add_test(NAME framework.language_runtime.request-document-symbols COMMAND umicom-language-runtime-request-document-symbols)
    add_executable(umicom-language-runtime-request-exit
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_request_exit.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/request_test_support.c"
    )
    target_include_directories(umicom-language-runtime-request-exit PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-request-exit PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-request-exit)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-request-exit)
    endif()
    add_test(NAME framework.language_runtime.request-exit COMMAND umicom-language-runtime-request-exit)
    add_executable(umicom-language-runtime-request-folding-ranges
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_request_folding_ranges.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/request_test_support.c"
    )
    target_include_directories(umicom-language-runtime-request-folding-ranges PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-request-folding-ranges PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-request-folding-ranges)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-request-folding-ranges)
    endif()
    add_test(NAME framework.language_runtime.request-folding-ranges COMMAND umicom-language-runtime-request-folding-ranges)
    add_executable(umicom-language-runtime-request-formatting
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_request_formatting.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/request_test_support.c"
    )
    target_include_directories(umicom-language-runtime-request-formatting PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-request-formatting PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-request-formatting)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-request-formatting)
    endif()
    add_test(NAME framework.language_runtime.request-formatting COMMAND umicom-language-runtime-request-formatting)
    add_executable(umicom-language-runtime-request-hover
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_request_hover.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/request_test_support.c"
    )
    target_include_directories(umicom-language-runtime-request-hover PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-request-hover PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-request-hover)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-request-hover)
    endif()
    add_test(NAME framework.language_runtime.request-hover COMMAND umicom-language-runtime-request-hover)
    add_executable(umicom-language-runtime-request-initialize
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_request_initialize.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/request_test_support.c"
    )
    target_include_directories(umicom-language-runtime-request-initialize PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-request-initialize PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-request-initialize)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-request-initialize)
    endif()
    add_test(NAME framework.language_runtime.request-initialize COMMAND umicom-language-runtime-request-initialize)
    add_executable(umicom-language-runtime-request-initialized
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_request_initialized.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/request_test_support.c"
    )
    target_include_directories(umicom-language-runtime-request-initialized PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-request-initialized PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-request-initialized)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-request-initialized)
    endif()
    add_test(NAME framework.language_runtime.request-initialized COMMAND umicom-language-runtime-request-initialized)
    add_executable(umicom-language-runtime-request-inlay-hints
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_request_inlay_hints.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/request_test_support.c"
    )
    target_include_directories(umicom-language-runtime-request-inlay-hints PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-request-inlay-hints PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-request-inlay-hints)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-request-inlay-hints)
    endif()
    add_test(NAME framework.language_runtime.request-inlay-hints COMMAND umicom-language-runtime-request-inlay-hints)
    add_executable(umicom-language-runtime-request-references
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_request_references.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/request_test_support.c"
    )
    target_include_directories(umicom-language-runtime-request-references PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-request-references PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-request-references)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-request-references)
    endif()
    add_test(NAME framework.language_runtime.request-references COMMAND umicom-language-runtime-request-references)
    add_executable(umicom-language-runtime-request-rename
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_request_rename.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/request_test_support.c"
    )
    target_include_directories(umicom-language-runtime-request-rename PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-request-rename PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-request-rename)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-request-rename)
    endif()
    add_test(NAME framework.language_runtime.request-rename COMMAND umicom-language-runtime-request-rename)
    add_executable(umicom-language-runtime-request-semantic-tokens
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_request_semantic_tokens.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/request_test_support.c"
    )
    target_include_directories(umicom-language-runtime-request-semantic-tokens PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-request-semantic-tokens PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-request-semantic-tokens)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-request-semantic-tokens)
    endif()
    add_test(NAME framework.language_runtime.request-semantic-tokens COMMAND umicom-language-runtime-request-semantic-tokens)
    add_executable(umicom-language-runtime-request-shutdown
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_request_shutdown.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/request_test_support.c"
    )
    target_include_directories(umicom-language-runtime-request-shutdown PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-request-shutdown PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-request-shutdown)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-request-shutdown)
    endif()
    add_test(NAME framework.language_runtime.request-shutdown COMMAND umicom-language-runtime-request-shutdown)
    add_executable(umicom-language-runtime-request-signature-help
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_request_signature_help.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/request_test_support.c"
    )
    target_include_directories(umicom-language-runtime-request-signature-help PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-request-signature-help PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-request-signature-help)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-request-signature-help)
    endif()
    add_test(NAME framework.language_runtime.request-signature-help COMMAND umicom-language-runtime-request-signature-help)
    add_executable(umicom-language-runtime-request-workspace-symbols
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_request_workspace_symbols.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/request_test_support.c"
    )
    target_include_directories(umicom-language-runtime-request-workspace-symbols PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-request-workspace-symbols PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-request-workspace-symbols)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-request-workspace-symbols)
    endif()
    add_test(NAME framework.language_runtime.request-workspace-symbols COMMAND umicom-language-runtime-request-workspace-symbols)
    add_executable(umicom-language-runtime-server
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_server.c"
    )
    target_include_directories(umicom-language-runtime-server PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-server PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-server)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-server)
    endif()
    add_test(NAME framework.language_runtime.server COMMAND umicom-language-runtime-server)
    add_executable(umicom-language-runtime-service-bridge
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_service_bridge.c"
    )
    target_include_directories(umicom-language-runtime-service-bridge PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-service-bridge PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-service-bridge)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-service-bridge)
    endif()
    add_test(NAME framework.language_runtime.service-bridge COMMAND umicom-language-runtime-service-bridge)
    add_executable(umicom-language-runtime-workspace-edit-bridge
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_workspace_edit_bridge.c"
    )
    target_include_directories(umicom-language-runtime-workspace-edit-bridge PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime")
    target_link_libraries(umicom-language-runtime-workspace-edit-bridge PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-language-runtime-workspace-edit-bridge)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-language-runtime-workspace-edit-bridge)
    endif()
    add_test(NAME framework.language_runtime.workspace-edit-bridge COMMAND umicom-language-runtime-workspace-edit-bridge)
endif()
message(STATUS "Umicom persistent Language Server runtime and IntelliSense integration enabled")
