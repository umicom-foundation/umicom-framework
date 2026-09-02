/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/commands.c
 *
 * PURPOSE:
 *   Route canonical document commands to one Framework coordinator implementation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/commands.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Provide the command message operation used by this module and its client applications. */
static void command_message(char *out_message,
                            size_t capacity,
                            const char *success,
                            UmiStatus status)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_message == NULL || capacity == 0U) return;
    (void)snprintf(out_message, capacity, "%s",
                   status == UMI_STATUS_OK ? success : umi_status_text(status));
}

/* Provide the new command operation used by this module and its client applications. */
static UmiStatus new_command(void *user_data,
                             const char *argument,
                             char *out_message,
                             size_t capacity)
{
    UmiStatus status = umi_document_coordinator_new(
        (UmiDocumentCoordinator *)user_data,
        argument != NULL && argument[0] != '\0' ? argument : NULL,
        NULL, 0U);
    command_message(out_message, capacity, "New document created", status);
    return status;
}

/* Provide the open command operation used by this module and its client applications. */
static UmiStatus open_command(void *user_data,
                              const char *argument,
                              char *out_message,
                              size_t capacity)
{
    UmiStatus status = argument != NULL && argument[0] != '\0'
        ? umi_document_coordinator_open((UmiDocumentCoordinator *)user_data,
                                        argument, NULL, 0U)
        : UMI_STATUS_INVALID_ARGUMENT;
    command_message(out_message, capacity, "Document opened", status);
    return status;
}

/* Provide the save command operation used by this module and its client applications. */
static UmiStatus save_command(void *user_data,
                              const char *argument,
                              char *out_message,
                              size_t capacity)
{
    UmiStatus status;
    (void)argument;
    status = umi_document_coordinator_save_active(
        (UmiDocumentCoordinator *)user_data);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_INVALID_STATE) {
        command_message(out_message, capacity,
                        "Use Save As for an untitled document or resolve the external-change conflict",
                        status);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        command_message(out_message, capacity, "Document saved", status);
    }
    return status;
}

/* Provide the save as command operation used by this module and its client applications. */
static UmiStatus save_as_command(void *user_data,
                                 const char *argument,
                                 char *out_message,
                                 size_t capacity)
{
    UmiStatus status = argument != NULL && argument[0] != '\0'
        ? umi_document_coordinator_save_active_as(
              (UmiDocumentCoordinator *)user_data, argument)
        : UMI_STATUS_INVALID_ARGUMENT;
    command_message(out_message, capacity, "Document saved as requested", status);
    return status;
}

/* Provide the close command operation used by this module and its client applications. */
static UmiStatus close_command(void *user_data,
                               const char *argument,
                               char *out_message,
                               size_t capacity)
{
    int force = argument != NULL && strcmp(argument, "force") == 0;
    UmiStatus status = umi_document_coordinator_close_active(
        (UmiDocumentCoordinator *)user_data, force);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_INVALID_STATE) {
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (out_message != NULL && capacity > 0U) {
            (void)snprintf(out_message, capacity,
                           "Document has unsaved changes; save it before closing");
        }
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        command_message(out_message, capacity, "Document closed", status);
    }
    return status;
}

/* Provide the undo command operation used by this module and its client applications. */
static UmiStatus undo_command(void *user_data,
                              const char *argument,
                              char *out_message,
                              size_t capacity)
{
    UmiStatus status;
    (void)argument;
    status = umi_document_coordinator_undo((UmiDocumentCoordinator *)user_data);
    command_message(out_message, capacity, "Edit undone", status);
    return status;
}

/* Provide the redo command operation used by this module and its client applications. */
static UmiStatus redo_command(void *user_data,
                              const char *argument,
                              char *out_message,
                              size_t capacity)
{
    UmiStatus status;
    (void)argument;
    status = umi_document_coordinator_redo((UmiDocumentCoordinator *)user_data);
    command_message(out_message, capacity, "Edit redone", status);
    return status;
}

/* Provide the find command operation used by this module and its client applications. */
static UmiStatus find_command(void *user_data,
                              const char *argument,
                              char *out_message,
                              size_t capacity)
{
    size_t offset = 0U;
    UmiStatus status = argument != NULL && argument[0] != '\0'
        ? umi_document_coordinator_find((UmiDocumentCoordinator *)user_data,
                                        argument, &offset)
        : UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status == UMI_STATUS_OK && out_message != NULL && capacity > 0U) {
        (void)snprintf(out_message, capacity, "Match at byte %zu", offset);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        command_message(out_message, capacity, "Text found", status);
    }
    return status;
}

/* Provide the replace command operation used by this module and its client applications. */
static UmiStatus replace_command(void *user_data,
                                 const char *argument,
                                 char *out_message,
                                 size_t capacity)
{
    const char *separator;
    char needle[UMI_UI_DESCRIPTION_CAPACITY];
    size_t needle_length;
    size_t offset = 0U;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (argument == NULL || (separator = strchr(argument, '\n')) == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    needle_length = (size_t)(separator - argument);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (needle_length == 0U || needle_length >= sizeof(needle)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memcpy(needle, argument, needle_length);
    needle[needle_length] = '\0';
    status = umi_document_coordinator_replace(
        (UmiDocumentCoordinator *)user_data, needle, separator + 1U, &offset);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status == UMI_STATUS_OK && out_message != NULL && capacity > 0U) {
        (void)snprintf(out_message, capacity, "Replacement at byte %zu", offset);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        command_message(out_message, capacity, "Text replaced", status);
    }
    return status;
}

/*
 * Provide the go to line command operation used by this module and its client
 * applications.
 */
static UmiStatus go_to_line_command(void *user_data,
                                    const char *argument,
                                    char *out_message,
                                    size_t capacity)
{
    unsigned long long line;
    char *end = NULL;
    size_t offset = 0U;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (argument == NULL || argument[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    errno = 0;
    line = strtoull(argument, &end, 10);
    /* Apply this branch only when its contract condition is satisfied. */
    if (errno != 0 || end == argument || *end != '\0' || line == 0ULL ||
        line > (unsigned long long)SIZE_MAX) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_document_coordinator_go_to_line(
        (UmiDocumentCoordinator *)user_data, (size_t)line, &offset);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status == UMI_STATUS_OK && out_message != NULL && capacity > 0U) {
        (void)snprintf(out_message, capacity, "Line %llu at byte %zu", line, offset);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        command_message(out_message, capacity, "Line selected", status);
    }
    return status;
}

/* Add document commands only after its inputs and available capacity have been checked. */
UmiStatus umi_document_commands_register(UmiCommandRegistry *registry,
                                          UmiDocumentCoordinator *coordinator)
{
    static const struct Definition {
        const char *id;
        const char *title;
        const char *description;
        UmiRegisteredCommandHandler handler;
    } DEFINITIONS[] = {
        {UMI_DOCUMENT_COMMAND_NEW, "New File", "Create an untitled working copy", new_command},
        {UMI_DOCUMENT_COMMAND_OPEN, "Open File", "Decode and open a local document", open_command},
        {UMI_DOCUMENT_COMMAND_SAVE, "Save", "Atomically save the active document", save_command},
        {UMI_DOCUMENT_COMMAND_SAVE_AS, "Save As", "Save the active document under another path", save_as_command},
        {UMI_DOCUMENT_COMMAND_CLOSE, "Close", "Close the active document safely", close_command},
        {UMI_DOCUMENT_COMMAND_UNDO, "Undo", "Undo the latest working-copy state", undo_command},
        {UMI_DOCUMENT_COMMAND_REDO, "Redo", "Redo the latest working-copy state", redo_command},
        {UMI_DOCUMENT_COMMAND_FIND, "Find", "Find text in the active document", find_command},
        {UMI_DOCUMENT_COMMAND_REPLACE, "Replace", "Replace the first matching text", replace_command},
        {UMI_DOCUMENT_COMMAND_GO_TO_LINE, "Go to Line", "Select a one-based document line", go_to_line_command}
    };
    UmiCommandDescriptor descriptor;
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || coordinator == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < sizeof(DEFINITIONS) / sizeof(DEFINITIONS[0]); ++index) {
        (void)memset(&descriptor, 0, sizeof(descriptor));
        descriptor.structure_size = sizeof(descriptor);
        descriptor.command_id = DEFINITIONS[index].id;
        descriptor.title = DEFINITIONS[index].title;
        descriptor.category = "Document";
        descriptor.description = DEFINITIONS[index].description;
        descriptor.required_permission = "document.edit";
        descriptor.flags = UMI_COMMAND_MUTATES_STATE;
        descriptor.handler = DEFINITIONS[index].handler;
        descriptor.user_data = coordinator;
        status = umi_command_registry_register(registry, &descriptor);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}
