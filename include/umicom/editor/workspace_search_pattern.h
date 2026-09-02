/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workspace_search_pattern.h
 *
 * PURPOSE:
 *   Define the portable search-pattern contract used by workspace search,
 *   replacement preview and transactional multi-document replacement.
 *
 *   The implementation is deliberately independent of POSIX regex, PCRE,
 *   GLib and a GUI toolkit. This allows the same compiled pattern to be used by
 *   GTK4, Qt, Wt, headless tools, remote workers and future Umicom products.
 *
 * OWNERSHIP:
 *   - Pattern objects are owned by the caller that creates them.
 *   - Request strings are copied during compilation.
 *   - Match structures are caller-owned values.
 *   - No borrowed pointer survives a public function call.
 *
 * THREADING:
 *   A compiled pattern is immutable and may be searched concurrently after a
 *   successful compile. Compile and reset must not run concurrently with find.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_WORKSPACE_SEARCH_PATTERN_H
#define UMICOM_EDITOR_WORKSPACE_SEARCH_PATTERN_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_WORKSPACE_SEARCH_PATTERN_API_VERSION 1U
#define UMI_EDITOR_WORKSPACE_SEARCH_EXPRESSION_CAPACITY 512U
#define UMI_EDITOR_WORKSPACE_SEARCH_DIAGNOSTIC_CAPACITY 256U
#define UMI_EDITOR_WORKSPACE_SEARCH_MAX_CAPTURES 9U
#define UMI_EDITOR_WORKSPACE_SEARCH_UNBOUNDED_REPEAT UINT32_MAX

/**
 * List the named editor workspace search pattern mode values accepted by this public
 * contract.
 */
typedef enum UmiEditorWorkspaceSearchPatternMode {
    UMI_EDITOR_WORKSPACE_SEARCH_PATTERN_LITERAL = 1,
    UMI_EDITOR_WORKSPACE_SEARCH_PATTERN_REGEX = 2
} UmiEditorWorkspaceSearchPatternMode;

/**
 * List the named editor workspace search case mode values accepted by this public
 * contract.
 */
typedef enum UmiEditorWorkspaceSearchCaseMode {
    UMI_EDITOR_WORKSPACE_SEARCH_CASE_INSENSITIVE = 1,
    UMI_EDITOR_WORKSPACE_SEARCH_CASE_SENSITIVE = 2,
    UMI_EDITOR_WORKSPACE_SEARCH_CASE_SMART = 3
} UmiEditorWorkspaceSearchCaseMode;

/**
 * Compile-time behaviour for one reusable search expression.
 *
 * whole_word
 *   Requires ASCII identifier boundaries around a successful match. The
 *   boundary definition is shared with the editor and treats letters, digits
 *   and underscore as word characters.
 *
 * multiline
 *   Makes ^ and $ recognise positions immediately after and before a newline,
 *   in addition to the beginning and end of the complete document.
 *
 * dot_matches_newline
 *   Allows the regex dot atom to consume a newline.
 *
 * maximum_program_instructions
 *   Protects callers from unexpectedly complex expressions. Zero selects the
 *   Framework default.
 *
 * maximum_execution_steps
 *   Bounds NFA execution for one find operation. Zero selects the Framework
 *   default. The limit prevents malformed or adversarial patterns from
 *   monopolising a worker even though the engine avoids recursive backtracking.
 */
typedef struct UmiEditorWorkspaceSearchPatternRequest {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorWorkspaceSearchPatternMode mode;
    UmiEditorWorkspaceSearchCaseMode case_mode;
    int whole_word;
    int multiline;
    int dot_matches_newline;
    size_t maximum_program_instructions;
    uint64_t maximum_execution_steps;
    char expression[UMI_EDITOR_WORKSPACE_SEARCH_EXPRESSION_CAPACITY];
} UmiEditorWorkspaceSearchPatternRequest;

/**
 * Represent the editor workspace search pattern diagnostic data shared with callers of
 * this public contract.
 */
typedef struct UmiEditorWorkspaceSearchPatternDiagnostic {
    uint32_t struct_size;
    uint32_t api_version;
    UmiStatus status;
    size_t expression_offset;
    char message[UMI_EDITOR_WORKSPACE_SEARCH_DIAGNOSTIC_CAPACITY];
} UmiEditorWorkspaceSearchPatternDiagnostic;

/**
 * Represent the editor workspace search capture data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorWorkspaceSearchCapture {
    uint64_t start_byte_offset;
    uint64_t end_byte_offset;
    int matched;
} UmiEditorWorkspaceSearchCapture;

/**
 * Represent the editor workspace search pattern match data shared with callers of this
 * public contract.
 */
typedef struct UmiEditorWorkspaceSearchPatternMatch {
    uint32_t struct_size;
    uint32_t api_version;
    uint64_t start_byte_offset;
    uint64_t end_byte_offset;
    size_t capture_count;
    UmiEditorWorkspaceSearchCapture
        captures[UMI_EDITOR_WORKSPACE_SEARCH_MAX_CAPTURES];
    uint64_t execution_steps;
    int empty;
} UmiEditorWorkspaceSearchPatternMatch;

/**
 * Represent the editor workspace search pattern snapshot data shared with callers of this
 * public contract.
 */
typedef struct UmiEditorWorkspaceSearchPatternSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorWorkspaceSearchPatternMode mode;
    UmiEditorWorkspaceSearchCaseMode case_mode;
    size_t expression_length;
    size_t instruction_count;
    size_t capture_count;
    uint64_t revision;
    uint64_t maximum_execution_steps;
    int whole_word;
    int multiline;
    int dot_matches_newline;
    int compiled;
} UmiEditorWorkspaceSearchPatternSnapshot;

/**
 * Represent the editor workspace search pattern data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorWorkspaceSearchPattern
    UmiEditorWorkspaceSearchPattern;

/**
 * Initialise editor workspace search pattern request from caller-provided values so later
 * operations receive a known state.
 */
void umi_editor_workspace_search_pattern_request_init(
    UmiEditorWorkspaceSearchPatternRequest *request,
    const char *expression);

/**
 * Initialise editor workspace search pattern from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_workspace_search_pattern_create(
    UmiEditorWorkspaceSearchPattern **out_pattern);
/**
 * Release or reset state held by editor workspace search pattern so the same storage can
 * be reused safely.
 */
void umi_editor_workspace_search_pattern_destroy(
    UmiEditorWorkspaceSearchPattern *pattern);

/**
 * Provide the editor workspace search pattern compile operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_workspace_search_pattern_compile(
    UmiEditorWorkspaceSearchPattern *pattern,
    const UmiEditorWorkspaceSearchPatternRequest *request,
    UmiEditorWorkspaceSearchPatternDiagnostic *out_diagnostic);

/**
 * Release or reset state held by editor workspace search pattern so the same storage can
 * be reused safely.
 */
UmiStatus umi_editor_workspace_search_pattern_reset(
    UmiEditorWorkspaceSearchPattern *pattern);

/**
 * Find the earliest leftmost match at or after start_byte_offset. At one start
 * position, ordered NFA priority preserves leftmost-first alternation and the
 * requested greedy or lazy quantifier semantics.
 *
 * The returned end offset is exclusive. UMI_STATUS_NOT_FOUND is normal and
 * leaves out_match zero-initialised. Empty matches are permitted by the regex
 * language; callers performing repeated search must advance by at least one
 * byte after an empty match to guarantee progress.
 */
UmiStatus umi_editor_workspace_search_pattern_find(
    const UmiEditorWorkspaceSearchPattern *pattern,
    const char *text,
    size_t text_length,
    size_t start_byte_offset,
    UmiEditorWorkspaceSearchPatternMatch *out_match);

/**
 * Provide the editor workspace search pattern snapshot operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_workspace_search_pattern_snapshot(
    const UmiEditorWorkspaceSearchPattern *pattern,
    UmiEditorWorkspaceSearchPatternSnapshot *out_snapshot);

/**
 * Provide the editor workspace search pattern revision operation used by this module and
 * its client applications.
 */
uint64_t umi_editor_workspace_search_pattern_revision(
    const UmiEditorWorkspaceSearchPattern *pattern);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_WORKSPACE_SEARCH_PATTERN_H */
