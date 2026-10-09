/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/native_attach_plan.c
 * PURPOSE: Validate and serialize a bounded PID attachment before changing debugger registries.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/base/text.h"
#include "umicom/debug_runtime/native_attach.h"
#include "umicom/editor/text_position.h"
#include "umicom/language_runtime/json_writer.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/process_search_path.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>
UmiStatus UmiDebugNativeProcessIdRead(const char *text, uint64_t *out)
{
    if (text == NULL || out == NULL || *text == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    uint64_t value = 0U;
    /* Bound the work as well as the number: an arbitrarily long run of leading
     * zeroes is not useful input for a process selector. */
    size_t count = 0U;
    for (; *text != '\0'; ++text)
    {
        if (++count > 10U || *text < '0' || *text > '9')
            return UMI_STATUS_INVALID_ARGUMENT;
        unsigned digit = (unsigned)(*text - '0');
        if (value > ((uint64_t)INT32_MAX - digit) / 10U)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        value = value * 10U + digit;
    }
    if (value == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = value;
    return UMI_STATUS_OK;
}
static UmiStatus AttachText(const char *text, char *out, size_t capacity)
{
    if (text == NULL)
        text = "";
    UmiStatus status = umi_text_copy(out, capacity, text);
    if (status != UMI_STATUS_OK)
        return status;
    UmiEditorTextBufferView view = {0};
    view.struct_size = (uint32_t)sizeof view;
    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
    view.bytes = text;
    view.byte_count = view.capacity = strlen(text);
    UmiEditorTextPosition end;
    return UmiEditorTextViewPositionAt(&view, view.byte_count, &end);
}
UmiStatus UmiDebugNativeAttachPlanCreate(const UmiDebugNativeAttachOptions *options,
                                         UmiDebugNativeAttachPlan *out)
{
    if (options == NULL || out == NULL || options->kind == NULL ||
        options->working_directory == NULL || options->process_id == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (options->process_id > INT32_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    const char *id = strcmp(options->kind, "gdb") == 0    ? "debug.adapter.gdb-dap"
                     : strcmp(options->kind, "lldb") == 0 ? "debug.adapter.lldb-dap"
                                                          : NULL;
    if (id == NULL)
        return UMI_STATUS_NOT_IMPLEMENTED;
    const UmiDebugAdapterProfile *builtin = umi_debug_runtime_builtin_profile_find(id);
    if (builtin == NULL)
        return UMI_STATUS_NOT_FOUND;
    UmiDebugNativeAttachPlan *plan = calloc(1U, sizeof *plan);
    if (plan == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    plan->profile = *builtin;
    plan->process_id = options->process_id;
    UmiStatus status = AttachText(options->working_directory, plan->configuration.working_directory,
                                  sizeof plan->configuration.working_directory);
    if (status == UMI_STATUS_OK)
        status = AttachText(options->program, plan->configuration.program,
                            sizeof plan->configuration.program);
    if (status == UMI_STATUS_OK)
        status =
            AttachText(options->tool_directory, plan->tool_directory, sizeof plan->tool_directory);
    if (status == UMI_STATUS_OK && (!umi_fs_is_absolute(plan->configuration.working_directory) ||
                                    (plan->configuration.program[0] != '\0' &&
                                     !umi_fs_is_absolute(plan->configuration.program))))
        status = UMI_STATUS_INVALID_ARGUMENT;
    if (status == UMI_STATUS_OK &&
        (!umi_fs_is_directory(plan->configuration.working_directory) ||
         (plan->configuration.program[0] != '\0' && !umi_fs_is_file(plan->configuration.program))))
        status = UMI_STATUS_NOT_FOUND;
    if (status == UMI_STATUS_OK && options->executable != NULL && options->executable[0] != '\0')
    {
        status = AttachText(options->executable, plan->profile.executable,
                            sizeof plan->profile.executable);
        if (status == UMI_STATUS_OK && !umi_fs_is_absolute(plan->profile.executable))
            status = UMI_STATUS_INVALID_ARGUMENT;
        if (status == UMI_STATUS_OK && !umi_fs_is_file(plan->profile.executable))
            status = UMI_STATUS_NOT_FOUND;
    }
    if (status == UMI_STATUS_OK && plan->tool_directory[0] != '\0')
    {
        status = UmiProcessSearchDirectoryValidate(plan->tool_directory);
        if (status == UMI_STATUS_OK && !umi_fs_is_absolute(plan->profile.executable))
        {
            char resolved[sizeof plan->profile.executable];
            status = UmiProcessToolProgram(plan->tool_directory, plan->profile.executable, resolved,
                                           sizeof resolved);
            if (status == UMI_STATUS_OK)
                strcpy(plan->profile.executable, resolved);
        }
    }
    if (status == UMI_STATUS_OK)
    {
        strcpy(plan->configuration.id, "native.attach");
        strcpy(plan->configuration.name, "Running process attachment");
        strcpy(plan->configuration.adapter, options->kind);
        UmiLanguageRuntimeJsonWriter writer;
        umi_language_runtime_json_writer_init(&writer, plan->arguments, sizeof plan->arguments);
        umi_language_runtime_json_writer_raw(&writer, "{\"pid\":");
        umi_language_runtime_json_writer_uint64(&writer, options->process_id);
        if (plan->configuration.program[0] != '\0')
        {
            umi_language_runtime_json_writer_raw(&writer, ",\"program\":");
            umi_language_runtime_json_writer_string(&writer, plan->configuration.program);
        }
        umi_language_runtime_json_writer_raw(&writer, "}");
        status = writer.status;
    }
    if (status == UMI_STATUS_OK)
        *out = *plan;
    free(plan);
    return status;
}
