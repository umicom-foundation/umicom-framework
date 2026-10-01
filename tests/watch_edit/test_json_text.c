/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/watch_edit/test_json_text.c
 * PURPOSE: Verify complete UTF-8 token decoding, paired escapes and unchanged failure outputs.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/json_text.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); return 1; } } while (0)
int main(int argc, char **argv)
{
    CHECK(argc == 2); const char *name = argv[1], *text = NULL, *expected = NULL; size_t capacity = 64U;
    UmiStatus status = UMI_STATUS_PARSE_ERROR;
    if (strcmp(name, "ascii") == 0) { text = "a\\nb\\\\c"; expected = "a\nb\\c"; status = UMI_STATUS_OK; }
    else if (strcmp(name, "raw-utf8") == 0) { text = "caf\xc3\xa9 \xf0\x9f\x9a\x80"; expected = text; status = UMI_STATUS_OK; }
    else if (strcmp(name, "pair") == 0) { text = "\\ud83d\\ude80"; expected = "\xf0\x9f\x9a\x80"; status = UMI_STATUS_OK; }
    else if (strcmp(name, "escaped-slash") == 0) { text = "\\\\u0000"; expected = "\\u0000"; status = UMI_STATUS_OK; }
    else if (strcmp(name, "nul") == 0) text = "a\\u0000b";
    else if (strcmp(name, "low") == 0) text = "\\udfff";
    else if (strcmp(name, "high") == 0) text = "\\ud800";
    else if (strcmp(name, "overlong") == 0) text = "\xc0\x80";
    else if (strcmp(name, "continuation") == 0) text = "\xe2\x28\xa1";
    else if (strcmp(name, "range") == 0) text = "\xf4\x90\x80\x80";
    else if (strcmp(name, "capacity") == 0) { text = "\\ud83d\\ude80"; capacity = 4U; status = UMI_STATUS_CAPACITY_EXCEEDED; }
    else if (strcmp(name, "bad-pair") == 0) text = "\\ud800\\u0061";
    else if (strcmp(name, "raw-control") == 0) text = "a\nb";
    else if (strcmp(name, "invalid-token") == 0) { text = "abc"; status = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(name, "empty") == 0) { text = ""; expected = ""; capacity = 1U; status = UMI_STATUS_OK; }
    else return 2;
    /* A manually bounded string token isolates text decoding from tokenization.
     * The same token layout is produced by the runtime JSON parser. */
    UmiLanguageRuntimeJsonDocument *doc = calloc(1U, sizeof *doc); CHECK(doc != NULL);
    doc->json = text; doc->token_count = 1U; doc->tokens[0].type = UMI_LANGUAGE_RUNTIME_JSON_STRING;
    doc->tokens[0].start = 0; doc->tokens[0].end = (int)strlen(text);
    if (strcmp(name, "invalid-token") == 0) doc->tokens[0].end = 1000;
    char output[64]; memset(output, '!', sizeof output); char before[64]; memcpy(before, output, sizeof before);
    UmiStatus actual = UmiLanguageRuntimeJsonText(doc, 0, output, capacity); free(doc);
    CHECK(actual == status);
    if (status == UMI_STATUS_OK) CHECK(strcmp(output, expected) == 0);
    else CHECK(memcmp(output, before, sizeof output) == 0);
    return 0;
}
