/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/types.h
 *
 * PURPOSE:
 *   Define stable build phases, states, limits and owned command records shared
 *   by Framework build providers, task runners, history and thin applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BUILD_TYPES_H
#define UMICOM_BUILD_TYPES_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_BUILD_ID_CAPACITY 96U
#define UMI_BUILD_PATH_CAPACITY 2048U
#define UMI_BUILD_NAME_CAPACITY 160U
#define UMI_BUILD_COMMAND_CAPACITY 4096U
#define UMI_BUILD_ARGUMENT_CAPACITY 512U
#define UMI_BUILD_MAX_ARGUMENTS 64U
#define UMI_BUILD_OUTPUT_CAPACITY 65536U
#define UMI_BUILD_MAX_DIAGNOSTICS 256U
#define UMI_BUILD_HISTORY_MAX 128U
#define UMI_BUILD_GRAPH_MAX_NODES 64U
#define UMI_BUILD_GRAPH_MAX_DEPENDENCIES 256U
#define UMI_BUILD_ARTIFACT_CAPACITY 32U

typedef enum UmiBuildPhase {
    UMI_BUILD_PHASE_CONFIGURE = 0,
    UMI_BUILD_PHASE_BUILD = 1,
    UMI_BUILD_PHASE_TEST = 2,
    UMI_BUILD_PHASE_CLEAN = 3,
    UMI_BUILD_PHASE_RUN = 4,
    UMI_BUILD_PHASE_INSTALL = 5
} UmiBuildPhase;

typedef enum UmiBuildState {
    UMI_BUILD_STATE_CREATED = 0,
    UMI_BUILD_STATE_RUNNING = 1,
    UMI_BUILD_STATE_SUCCEEDED = 2,
    UMI_BUILD_STATE_FAILED = 3,
    UMI_BUILD_STATE_CANCELLED = 4,
    UMI_BUILD_STATE_TIMED_OUT = 5
} UmiBuildState;

/* Build graphs need two additional non-running states.  READY means that all
 * dependencies have completed successfully.  BLOCKED means a dependency ended
 * unsuccessfully, so the node must not run until that dependency is retried. */
typedef enum UmiBuildNodeState {
    UMI_BUILD_NODE_PENDING = 0,
    UMI_BUILD_NODE_READY = 1,
    UMI_BUILD_NODE_RUNNING = 2,
    UMI_BUILD_NODE_SUCCEEDED = 3,
    UMI_BUILD_NODE_FAILED = 4,
    UMI_BUILD_NODE_CANCELLED = 5,
    UMI_BUILD_NODE_TIMED_OUT = 6,
    UMI_BUILD_NODE_SKIPPED = 7,
    UMI_BUILD_NODE_BLOCKED = 8
} UmiBuildNodeState;

typedef enum UmiBuildDiagnosticSeverity {
    UMI_BUILD_DIAGNOSTIC_NOTE = 0,
    UMI_BUILD_DIAGNOSTIC_WARNING = 1,
    UMI_BUILD_DIAGNOSTIC_ERROR = 2,
    UMI_BUILD_DIAGNOSTIC_FATAL = 3
} UmiBuildDiagnosticSeverity;

/*
 * UmiBuildCommand is an owned argv command, not a shell command.
 *
 * arguments[] points into argument_storage[]. If a command is copied by value,
 * call umi_build_command_rebind() before consuming arguments[].
 */
typedef struct UmiBuildCommand {
    char program[UMI_BUILD_PATH_CAPACITY];
    char working_directory[UMI_BUILD_PATH_CAPACITY];
    char argument_storage[UMI_BUILD_MAX_ARGUMENTS][UMI_BUILD_ARGUMENT_CAPACITY];
    const char *arguments[UMI_BUILD_MAX_ARGUMENTS];
    size_t argument_count;
} UmiBuildCommand;

const char *umi_build_phase_text(UmiBuildPhase phase);
const char *umi_build_state_text(UmiBuildState state);
const char *umi_build_node_state_text(UmiBuildNodeState state);
const char *umi_build_diagnostic_severity_text(
    UmiBuildDiagnosticSeverity severity);

void umi_build_command_init(UmiBuildCommand *command, const char *program);
void umi_build_command_rebind(UmiBuildCommand *command);

int umi_build_command_add_argument(UmiBuildCommand *command,
                                   const char *argument);
int umi_build_command_set_working_directory(UmiBuildCommand *command,
                                            const char *directory);

/*
 * Parse a deterministic, shell-independent command line.
 *
 * Supported syntax:
 *   - whitespace separates tokens;
 *   - single or double quotes group a token;
 *   - backslash escapes the following character;
 *   - empty quoted arguments are preserved.
 *
 * There is deliberately no shell expansion, redirection, globbing, variable
 * interpolation, command substitution, pipe handling or statement chaining.
 */
UmiStatus umi_build_command_parse(const char *text,
                                  UmiBuildCommand *out_command,
                                  char *out_message,
                                  size_t message_capacity);

UmiStatus umi_build_command_validate(const UmiBuildCommand *command,
                                     char *out_message,
                                     size_t message_capacity);

int umi_build_command_format(const UmiBuildCommand *command,
                             char *out_text,
                             size_t capacity);

#ifdef __cplusplus
}
#endif

#endif
