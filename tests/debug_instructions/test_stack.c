/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_instructions/test_stack.c
 * PURPOSE: Verify stack positions and caller-owned output survive malformed debugger replies.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/debug_runtime/decoders/stack_trace.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct StackCase
{
    const char *name, *body;
    UmiStatus expected;
} StackCase;
static const StackCase cases[] = {
    {"zero", "{\"stackFrames\":[{\"id\":0,\"name\":\"main\",\"line\":0,\"column\":0}]}",
     UMI_STATUS_OK},
    {"ordered",
     "{\"stackFrames\":[{\"id\":19,\"name\":\"inner\"},{\"id\":2,\"name\":\"outer\"}],"
     "\"totalFrames\":9}",
     UMI_STATUS_OK},
    {"source",
     "{\"stackFrames\":[{\"id\":0,\"name\":\"caf\\u00e9\",\"source\":{\"name\":\"generated\","
     "\"path\":\"/"
     "not-opened.c\",\"sourceReference\":42},\"instructionPointerReference\":\"opaque\","
     "\"presentationHint\":\"normal\"}]}",
     UMI_STATUS_OK},
    {"empty", "{\"stackFrames\":[]}", UMI_STATUS_OK},
    {"partial",
     "{\"stackFrames\":[{\"id\":1,\"name\":\"valid\"},{\"id\":-1,\"name\":\"invalid\"}]}",
     UMI_STATUS_PARSE_ERROR},
    {"duplicate-id", "{\"stackFrames\":[{\"id\":1,\"name\":\"a\"},{\"id\":1,\"name\":\"b\"}]}",
     UMI_STATUS_PARSE_ERROR},
    {"duplicate-key", "{\"stackFrames\":[{\"id\":1,\"i\\u0064\":2,\"name\":\"main\"}]}",
     UMI_STATUS_PARSE_ERROR},
    {"id-type", "{\"stackFrames\":[{\"id\":true,\"name\":\"main\"}]}", UMI_STATUS_PARSE_ERROR},
    {"id-range", "{\"stackFrames\":[{\"id\":2147483648,\"name\":\"main\"}]}",
     UMI_STATUS_PARSE_ERROR},
    {"name-type", "{\"stackFrames\":[{\"id\":1,\"name\":false}]}", UMI_STATUS_PARSE_ERROR},
    {"coordinate-range", "{\"stackFrames\":[{\"id\":1,\"name\":\"main\",\"line\":4294967296}]}",
     UMI_STATUS_CAPACITY_EXCEEDED},
    {"coordinate-negative", "{\"stackFrames\":[{\"id\":1,\"name\":\"main\",\"column\":-1}]}",
     UMI_STATUS_PARSE_ERROR},
    {"source-type", "{\"stackFrames\":[{\"id\":1,\"name\":\"main\",\"source\":false}]}",
     UMI_STATUS_PARSE_ERROR},
    {"source-reference",
     "{\"stackFrames\":[{\"id\":1,\"name\":\"main\",\"source\":{\"sourceReference\":-1}}]}",
     UMI_STATUS_PARSE_ERROR},
    {"reference-type",
     "{\"stackFrames\":[{\"id\":1,\"name\":\"main\",\"instructionPointerReference\":0}]}",
     UMI_STATUS_PARSE_ERROR},
    {"total-type", "{\"stackFrames\":[],\"totalFrames\":false}", UMI_STATUS_PARSE_ERROR},
    {"total-short",
     "{\"stackFrames\":[{\"id\":1,\"name\":\"a\"},{\"id\":2,\"name\":\"b\"}],\"totalFrames\":1}",
     UMI_STATUS_PARSE_ERROR},
    /* A remote stack count can exceed signed identifiers without allocating
     * that many rows. Only the returned page consumes bounded local storage. */
    {"total-wide", "{\"stackFrames\":[],\"totalFrames\":4294967295}", UMI_STATUS_OK},
    {"total-range", "{\"stackFrames\":[],\"totalFrames\":4294967296}", UMI_STATUS_PARSE_ERROR},
    {"array-type", "{\"stackFrames\":{}}", UMI_STATUS_PARSE_ERROR}};
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const StackCase *test = NULL;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
        if (strcmp(argv[1], cases[i].name) == 0)
            test = &cases[i];
    bool oversized = strcmp(argv[1], "capacity") == 0;
    if (test == NULL && !oversized)
        return 2;
    UmiDebugRuntimeStackTrace *out = malloc(sizeof *out), *before = malloc(sizeof *before);
    char *json = malloc(65536U);
    if (out == NULL || before == NULL || json == NULL)
    {
        free(out);
        free(before);
        free(json);
        return 1;
    }
    memset(out, 0xA5, sizeof *out);
    memcpy(before, out, sizeof *out);
    if (oversized)
    {
        strcpy(json, "{\"type\":\"response\",\"command\":\"stackTrace\",\"success\":true,\"body\":{"
                     "\"stackFrames\":[");
        /* Empty objects keep this capacity check below the JSON token limit.
         * Collection bounds must reject before per-row decoding begins. */
        for (size_t i = 0U; i < UMI_DEBUG_RUNTIME_MAX_ITEMS + 1U; ++i)
            strcat(json, i ? ",{}" : "{}");
        strcat(json, "]}}");
    }
    else
        snprintf(json, 65536U,
                 "{\"type\":\"response\",\"command\":\"stackTrace\",\"success\":true,\"body\":%s}",
                 test->body);
    UmiStatus status = umi_debug_runtime_decode_stack_trace(json, out);
    UmiStatus expected = oversized ? UMI_STATUS_CAPACITY_EXCEEDED : test->expected;
    int failed = status != expected;
    if (status != UMI_STATUS_OK)
        failed |= memcmp(out, before, sizeof *out) != 0;
    else if (strcmp(argv[1], "zero") == 0)
        failed |= out->count != 1U || out->items[0].id != 0U;
    else if (strcmp(argv[1], "ordered") == 0)
        failed |= out->count != 2U || out->items[0].id != 19U || out->items[1].id != 2U ||
                  out->total_frames != 9U;
    else if (strcmp(argv[1], "source") == 0)
        failed |= out->items[0].source.source_reference != 42U ||
                  strcmp(out->items[0].instruction_pointer, "opaque") != 0;
    else if (strcmp(argv[1], "empty") == 0)
        failed |= out->count != 0U || out->total_frames != 0U;
    if (strcmp(argv[1], "total-wide") == 0 && status == UMI_STATUS_OK)
        failed |= out->count != 0U || out->total_frames != UINT32_MAX;
    if (failed)
        fprintf(stderr, "case %s: status %d expected %d, output preservation and fields checked\n",
                argv[1], (int)status, (int)expected);
    free(out);
    free(before);
    free(json);
    return failed;
}
