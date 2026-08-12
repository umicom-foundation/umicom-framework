/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/operation.c
 *
 * PURPOSE:
 *   Implement safe construction helpers for structured developer operations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer/operation.h"

#include <string.h>

static UmiStatus copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;

    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(source);
    if (length >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    if (length > 0U) {
        memcpy(destination, source, length);
    }
    destination[length] = '\0';
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_operation_init(
    UmiDeveloperOperationSnapshot *operation,
    const char *id,
    UmiDeveloperOperationKind kind,
    const char *title)
{
    UmiStatus status;

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
    if (status != UMI_STATUS_OK) {
        return status;
    }
    return copy_text(operation->title, sizeof(operation->title), title);
}

UmiStatus umi_developer_operation_set_program(
    UmiDeveloperOperationSnapshot *operation,
    const char *program,
    const char *working_directory)
{
    UmiStatus status;

    if (operation == NULL || program == NULL || program[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = copy_text(operation->program, sizeof(operation->program), program);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    if (working_directory == NULL) {
        operation->working_directory[0] = '\0';
        return UMI_STATUS_OK;
    }
    return copy_text(operation->working_directory,
                     sizeof(operation->working_directory),
                     working_directory);
}

UmiStatus umi_developer_operation_add_argument(
    UmiDeveloperOperationSnapshot *operation,
    const char *argument)
{
    UmiStatus status;

    if (operation == NULL || argument == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (operation->argument_count >= UMI_DEVELOPER_MAX_ARGUMENTS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    status = copy_text(
        operation->arguments[operation->argument_count],
        sizeof(operation->arguments[operation->argument_count]),
        argument);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    operation->argument_count += 1U;
    return UMI_STATUS_OK;
}
