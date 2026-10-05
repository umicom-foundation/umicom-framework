/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/setup_document.c
 * PURPOSE: Serialize complete desired settings with strict bounded JSON parsing.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/debug/setup_document.h"
#include "umicom/language_runtime/json_tree.h"
#include "umicom/language_runtime/json_writer.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>
void UmiDebugSetupFreeBytes(char *bytes) { free(bytes); }
UmiStatus UmiDebugSetupEncode(const UmiDebugSetup *setup, char **out, size_t *outSize)
{
    if (out != NULL)
        *out = NULL;
    if (outSize != NULL)
        *outSize = 0U;
    if (out == NULL || outSize == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiDebugSetupSummary summary;
    UmiStatus status = UmiDebugSetupInspect(setup, &summary);
    if (status != UMI_STATUS_OK)
        return status;
    char *bytes = malloc(UMI_DEBUG_SETUP_DOCUMENT_LIMIT);
    if (bytes == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiLanguageRuntimeJsonWriter writer;
    umi_language_runtime_json_writer_init(&writer, bytes, UMI_DEBUG_SETUP_DOCUMENT_LIMIT);
    (void)umi_language_runtime_json_writer_raw(&writer, "{\"format\":\"umicom.debug-setup\",\"title\":");
    (void)umi_language_runtime_json_writer_string(&writer, summary.title);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"breakpoints\":[");
    for (size_t i = 0U; status == UMI_STATUS_OK && i < summary.breakpoints; ++i)
    {
        UmiDebugSetupBreakpoint item;
        status = UmiDebugSetupBreakpointAt(setup, i, &item);
        if (status != UMI_STATUS_OK)
            break;
        (void)umi_language_runtime_json_writer_raw(&writer, i == 0U ? "{\"source\":" : ",{\"source\":");
        (void)umi_language_runtime_json_writer_string(&writer, item.source);
        (void)umi_language_runtime_json_writer_raw(&writer, ",\"line\":");
        (void)umi_language_runtime_json_writer_uint64(&writer, item.line);
        (void)umi_language_runtime_json_writer_raw(&writer, ",\"column\":");
        (void)umi_language_runtime_json_writer_uint64(&writer, item.column);
        (void)umi_language_runtime_json_writer_raw(&writer, ",\"enabled\":");
        (void)umi_language_runtime_json_writer_bool(&writer, item.enabled);
        (void)umi_language_runtime_json_writer_raw(&writer, ",\"condition\":");
        (void)umi_language_runtime_json_writer_string(&writer, item.condition);
        (void)umi_language_runtime_json_writer_raw(&writer, ",\"logMessage\":");
        (void)umi_language_runtime_json_writer_string(&writer, item.logMessage);
        (void)umi_language_runtime_json_writer_raw(&writer, "}");
    }
    (void)umi_language_runtime_json_writer_raw(&writer, "],\"watches\":[");
    for (size_t i = 0U; status == UMI_STATUS_OK && i < summary.watches; ++i)
    {
        UmiDebugSetupWatch item;
        status = UmiDebugSetupWatchAt(setup, i, &item);
        if (status != UMI_STATUS_OK)
            break;
        (void)umi_language_runtime_json_writer_raw(&writer,
                                                   i == 0U ? "{\"expression\":" : ",{\"expression\":");
        (void)umi_language_runtime_json_writer_string(&writer, item.expression);
        (void)umi_language_runtime_json_writer_raw(&writer, ",\"enabled\":");
        (void)umi_language_runtime_json_writer_bool(&writer, item.enabled);
        (void)umi_language_runtime_json_writer_raw(&writer, "}");
    }
    (void)umi_language_runtime_json_writer_raw(&writer, "]}\n");
    if (status == UMI_STATUS_OK)
        status = writer.status;
    if (status == UMI_STATUS_OK)
    {
        *out = bytes;
        *outSize = writer.length;
    }
    else
        free(bytes);
    return status;
}
/* Requiring each known member once plus an exact field count rejects unknown
 * keys as well as duplicates, including keys spelled with JSON escapes. */
static UmiStatus Members(const UmiJsonTree *tree, int object, const char *const *names, size_t count,
                         int *nodes)
{
    if (UmiJsonTreeKind(tree, object) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT ||
        UmiJsonTreeCount(tree, object) != count)
        return UMI_STATUS_PARSE_ERROR;
    for (size_t i = 0U; i < count; ++i)
    {
        UmiStatus status = UmiJsonTreeMember(tree, object, names[i], &nodes[i]);
        if (status != UMI_STATUS_OK)
            return status;
    }
    return UMI_STATUS_OK;
}
static UmiStatus Breakpoint(const UmiJsonTree *tree, int node, UmiDebugSetup *setup)
{
    const char *names[] = {"source", "line", "column", "enabled", "condition", "logMessage"};
    int nodes[6];
    UmiStatus status = Members(tree, node, names, 6U, nodes);
    UmiDebugSetupBreakpoint item = {0};
    int64_t line = 0, column = 0;
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeText(tree, nodes[0], item.source, sizeof item.source);
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeInteger(tree, nodes[1], &line);
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeInteger(tree, nodes[2], &column);
    if (status == UMI_STATUS_OK && (line < 1 || line > INT32_MAX || column < 0 || column > INT32_MAX))
        status = UMI_STATUS_INVALID_ARGUMENT;
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeBoolean(tree, nodes[3], &item.enabled);
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeText(tree, nodes[4], item.condition, sizeof item.condition);
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeText(tree, nodes[5], item.logMessage, sizeof item.logMessage);
    if (status == UMI_STATUS_OK)
    {
        item.line = (uint32_t)line;
        item.column = (uint32_t)column;
        status = UmiDebugSetupAddBreakpoint(setup, &item);
    }
    return status;
}
static UmiStatus Watch(const UmiJsonTree *tree, int node, UmiDebugSetup *setup)
{
    const char *names[] = {"expression", "enabled"};
    int nodes[2];
    UmiDebugSetupWatch item = {0};
    UmiStatus status = Members(tree, node, names, 2U, nodes);
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeText(tree, nodes[0], item.expression, sizeof item.expression);
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeBoolean(tree, nodes[1], &item.enabled);
    return status == UMI_STATUS_OK ? UmiDebugSetupAddWatch(setup, &item) : status;
}
UmiStatus UmiDebugSetupDecode(const void *bytes, size_t size, UmiDebugSetup **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    UmiJsonTreeLimits limits = {UMI_DEBUG_SETUP_DOCUMENT_LIMIT, 4096U, 6U};
    UmiJsonTree *tree = NULL;
    UmiStatus status = UmiJsonTreeCreate(bytes, size, &limits, NULL, &tree);
    const char *names[] = {"format", "title", "breakpoints", "watches"};
    int nodes[4];
    char format[64], title[256];
    UmiDebugSetup *setup = NULL;
    if (status == UMI_STATUS_OK)
        status = Members(tree, 0, names, 4U, nodes);
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeText(tree, nodes[0], format, sizeof format);
    if (status == UMI_STATUS_OK && strcmp(format, "umicom.debug-setup") != 0)
        status = UMI_STATUS_NOT_IMPLEMENTED;
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeText(tree, nodes[1], title, sizeof title);
    if (status == UMI_STATUS_OK)
        status = UmiDebugSetupCreate(title, &setup);
    for (size_t i = 2U; status == UMI_STATUS_OK && i < 4U; ++i)
    {
        if (UmiJsonTreeKind(tree, nodes[i]) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
        {
            status = UMI_STATUS_PARSE_ERROR;
            break;
        }
        if (UmiJsonTreeCount(tree, nodes[i]) > UMI_DEBUG_SETUP_CAPACITY)
        {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            break;
        }
        for (int node = UmiJsonTreeFirst(tree, nodes[i]); status == UMI_STATUS_OK && node >= 0;
             node = UmiJsonTreeNext(tree, node))
            status = i == 2U ? Breakpoint(tree, node, setup) : Watch(tree, node, setup);
    }
    UmiJsonTreeDestroy(tree);
    if (status == UMI_STATUS_OK)
        *out = setup;
    else
        UmiDebugSetupDestroy(setup);
    return status;
}
