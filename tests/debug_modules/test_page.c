/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_modules/test_page.c
 * PURPOSE: Check module identity types, totals, UTF-8 and complete failure preservation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/debug_runtime/module_page.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1], *body = NULL;
    bool good = false;
    uint32_t first = 0U, requested = 2U;
    if (strcmp(mode, "numeric") == 0)
    {
        body = "{\"modules\":[{\"id\":7,\"name\":\"app\",\"path\":\"/"
               "app\",\"isOptimized\":false}],\"totalModules\":1}";
        good = true;
    }
    else if (strcmp(mode, "typed-identities") == 0)
    {
        body = "{\"modules\":[{\"id\":1,\"name\":\"number\"},{\"id\":\"1\",\"name\":\"string\"}]}";
        good = true;
    }
    else if (strcmp(mode, "unknown-total") == 0)
    {
        body = "{\"modules\":[{\"id\":\"a\",\"name\":\"A\"},{\"id\":\"b\",\"name\":\"B\"}]}";
        good = true;
    }
    else if (strcmp(mode, "first") == 0)
    {
        body = "{\"modules\":[{\"id\":\"a\",\"name\":\"A\"}],\"totalModules\":12}";
        first = 8U;
        good = true;
    }
    else if (strcmp(mode, "empty") == 0)
    {
        body = "{\"modules\":[],\"totalModules\":0}";
        good = true;
    }
    else if (strcmp(mode, "unknown-empty") == 0)
    {
        body = "{\"modules\":[]}";
        good = true;
    }
    else if (strcmp(mode, "beyond-end") == 0)
    {
        body = "{\"modules\":[],\"totalModules\":1}";
        first = 8U;
        good = true;
    }
    else if (strcmp(mode, "unicode") == 0)
    {
        body = "{\"modules\":[{\"id\":\"a\",\"name\":\"caf\\u00e9\",\"symbolFilePath\":\"/"
               "symbols\",\"isUserCode\":true}]}";
        good = true;
    }
    else if (strcmp(mode, "duplicate") == 0)
        body = "{\"modules\":[{\"id\":1,\"name\":\"A\"},{\"id\":1,\"name\":\"B\"}]}";
    else if (strcmp(mode, "missing-id") == 0)
        body = "{\"modules\":[{\"name\":\"A\"}]}";
    else if (strcmp(mode, "id-boolean") == 0)
        body = "{\"modules\":[{\"id\":true,\"name\":\"A\"}]}";
    else if (strcmp(mode, "nul") == 0)
        body = "{\"modules\":[{\"id\":1,\"name\":\"A\\u0000B\"}]}";
    else if (strcmp(mode, "boolean") == 0)
        body = "{\"modules\":[{\"id\":1,\"name\":\"A\",\"isOptimized\":\"false\"}]}";
    else if (strcmp(mode, "array") == 0)
        body = "{\"modules\":{}}";
    else if (strcmp(mode, "total-small") == 0)
        body = "{\"modules\":[{\"id\":1,\"name\":\"A\"}],\"totalModules\":0}";
    else if (strcmp(mode, "total-negative") == 0)
        body = "{\"modules\":[],\"totalModules\":-1}";
    else if (strcmp(mode, "total-large") == 0)
        body = "{\"modules\":[],\"totalModules\":9007199254740992}";
    else if (strcmp(mode, "empty-before-end") == 0)
        body = "{\"modules\":[],\"totalModules\":5}";
    else if (strcmp(mode, "capacity") == 0)
    {
        body = "{\"modules\":[{\"id\":1,\"name\":\"A\"},{\"id\":2,\"name\":\"B\"}]}";
        requested = 1U;
    }
    else if (strcmp(mode, "duplicate-key") == 0)
        body = "{\"modules\":[{\"id\":1,\"name\":\"A\",\"name\":\"B\"}]}";
    else if (strcmp(mode, "refuse") == 0 || strcmp(mode, "command") == 0)
        body = "{\"modules\":[]}";
    else
        return 2;
    UmiDebugModulePage *out = malloc(sizeof *out), *before = malloc(sizeof *before);
    if (out == NULL || before == NULL)
    {
        free(out);
        free(before);
        return 1;
    }
    memset(out, 0xA5, sizeof *out);
    *before = *out;
    char json[4096];
    snprintf(json, sizeof json,
             "{\"type\":\"response\",\"command\":\"%s\",\"success\":%s,\"body\":%s}",
             strcmp(mode, "command") == 0 ? "launch" : "modules",
             strcmp(mode, "refuse") == 0 ? "false" : "true", body);
    UmiStatus status = UmiDebugModulePageDecode(json, first, requested, out);
    bool okay =
        good ? status == UMI_STATUS_OK && out->first == first && out->requested_count == requested
             : status != UMI_STATUS_OK && memcmp(out, before, sizeof *out) == 0;
    if (okay && good)
        okay = out->session.generation == 0U && out->session.session_id[0] == '\0';
    if (okay && strcmp(mode, "numeric") == 0)
        okay = out->items[0].numeric_id && out->items[0].number == 7 &&
               out->items[0].optimized_known && !out->items[0].optimized;
    if (okay && strcmp(mode, "typed-identities") == 0)
        okay = out->items[0].numeric_id && !out->items[1].numeric_id;
    if (okay && strcmp(mode, "unknown-total") == 0)
        okay = !out->total_known && out->has_more;
    if (okay && strcmp(mode, "unknown-empty") == 0)
        okay = !out->total_known && !out->has_more;
    if (okay && strcmp(mode, "unicode") == 0)
        okay = strcmp(out->items[0].name, "caf\xc3\xa9") == 0 && out->items[0].user_code_known &&
               out->items[0].user_code;
    if (!okay)
        fprintf(stderr, "Unexpected module page for %s: %s\n", mode, umi_status_text(status));
    free(out);
    free(before);
    return okay ? 0 : 1;
}
