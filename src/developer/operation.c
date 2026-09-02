/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/operation.c
 *
 * PURPOSE:
 *   Implement safe construction helpers for structured developer operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer/operation.h"

#include <string.h>

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U) {
        memcpy(destination, source, length);
    }
    destination[length] = '\0';
    return UMI_STATUS_OK;
}

/*
 * Initialise developer operation from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_developer_operation_init(
    UmiDeveloperOperationSnapshot *operation,
    const char *id,
    UmiDeveloperOperationKind kind,
    const char *title)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (operation == NULL || id == NULL || title == NULL || id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    memset(operation, 0, sizeof(*operation));
    operation->struct_size = (uint32_t)sizeof(*operation);
    operation->api_version = UMI_DEVELOPER_OPERATION_API_VERSION;
    operation->kind = kind;
    operation->state = UMI_DEVELOPER_OPERATION_QUEUED;
    operation->max_attempts = 1U;
    operation->revision = 1U;

    status = copy_text(operation->id, sizeof(operation->id), id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    return copy_text(operation->title, sizeof(operation->title), title);
}

/*
 * Provide the developer operation set program operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_operation_set_program(
    UmiDeveloperOperationSnapshot *operation,
    const char *program,
    const char *working_directory)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (operation == NULL || program == NULL || program[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = copy_text(operation->program, sizeof(operation->program), program);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (working_directory == NULL) {
        operation->working_directory[0] = '\0';
        return UMI_STATUS_OK;
    }
    return copy_text(operation->working_directory,
                     sizeof(operation->working_directory),
                     working_directory);
}

/*
 * Provide the developer operation add argument operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_operation_add_argument(
    UmiDeveloperOperationSnapshot *operation,
    const char *argument)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (operation == NULL || argument == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (operation->argument_count >= UMI_DEVELOPER_MAX_ARGUMENTS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    status = copy_text(
        operation->arguments[operation->argument_count],
        sizeof(operation->arguments[operation->argument_count]),
        argument);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    operation->argument_count += 1U;
    return UMI_STATUS_OK;
}
