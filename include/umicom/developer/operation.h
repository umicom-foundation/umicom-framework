/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/operation.h
 *
 * PURPOSE:
 *   Define one executable developer operation with structured program
 *   arguments, lifecycle state, retry policy, progress and result metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Arguments are stored separately instead of concatenated into one shell
 * command.  This lets the process adapter execute a program without reparsing
 * quoting rules and avoids product-specific shell-string construction.
 */
#ifndef UMICOM_DEVELOPER_OPERATION_H
#define UMICOM_DEVELOPER_OPERATION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/developer/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_OPERATION_API_VERSION 1U

/**
 * Represent the developer operation snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiDeveloperOperationSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_DEVELOPER_ID_CAPACITY];
    UmiDeveloperOperationKind kind;
    UmiDeveloperOperationState state;
    char title[UMI_DEVELOPER_TITLE_CAPACITY];
    char project_id[UMI_DEVELOPER_ID_CAPACITY];
    char configuration_id[UMI_DEVELOPER_ID_CAPACITY];
    char target_id[UMI_DEVELOPER_ID_CAPACITY];
    char program[UMI_DEVELOPER_PATH_CAPACITY];
    char arguments[UMI_DEVELOPER_MAX_ARGUMENTS][UMI_DEVELOPER_ARGUMENT_CAPACITY];
    size_t argument_count;
    char working_directory[UMI_DEVELOPER_PATH_CAPACITY];
    uint32_t timeout_ms;
    uint32_t attempt_count;
    uint32_t max_attempts;
    int exit_code;
    int has_exit_code;
    uint32_t progress_basis_points;
    char summary[UMI_DEVELOPER_SUMMARY_CAPACITY];
    uint64_t sequence;
    uint64_t revision;
} UmiDeveloperOperationSnapshot;

/**
 * Initialise developer operation from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_developer_operation_init(
    UmiDeveloperOperationSnapshot *operation,
    const char *id,
    UmiDeveloperOperationKind kind,
    const char *title);

/**
 * Provide the developer operation set program operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_operation_set_program(
    UmiDeveloperOperationSnapshot *operation,
    const char *program,
    const char *working_directory);

/**
 * Provide the developer operation add argument operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_operation_add_argument(
    UmiDeveloperOperationSnapshot *operation,
    const char *argument);

#ifdef __cplusplus
}
#endif

#endif
