/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_instructions/test_decode.c
 * PURPOSE: Check exact instruction counts, strict fields, preserved output and offset boundaries.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../../src/debug_runtime/instruction_fields_internal.h"
#include "umicom/debug_runtime/decoders/disassembly.h"
#include "umicom/debug_runtime/instruction_inspection.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do                                                                                             \
    {                                                                                              \
        if (!(x))                                                                                  \
        {                                                                                          \
            fprintf(stderr, "%d: %s\n", __LINE__, #x);                                             \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
typedef struct DecodeCase
{
    const char *name, *body;
    uint32_t count;
    UmiStatus expected;
} DecodeCase;
static const DecodeCase cases[] = {
    {"normal",
     "{\"instructions\":[{\"address\":\"0x1000\",\"instruction\":\"nop\",\"instructionBytes\":"
     "\"90\",\"symbol\":\"entry\",\"location\":{\"path\":\"file.c\",\"sourceReference\":4},"
     "\"line\":7,\"column\":2,\"endLine\":8,\"endColumn\":3}]}",
     1, UMI_STATUS_OK},
    {"wide",
     "{\"instructions\":[{\"address\":\"0x123456789abcdef0123456789abcdef0\",\"instruction\":"
     "\"nop\"}]}",
     1, UMI_STATUS_OK},
    {"decimal", "{\"instructions\":[{\"address\":\"4096\",\"instruction\":\"nop\"}]}", 1,
     UMI_STATUS_OK},
    {"filler",
     "{\"instructions\":[{\"address\":\"0x0\",\"instruction\":\"unavailable\",\"presentationHint\":"
     "\"invalid\"}]}",
     1, UMI_STATUS_OK},
    {"inherit",
     "{\"instructions\":[{\"address\":\"0x1\",\"instruction\":\"one\",\"location\":{\"path\":\"a."
     "c\"},\"line\":5},{\"address\":\"0x2\",\"instruction\":\"two\"}]}",
     2, UMI_STATUS_OK},
    {"unicode",
     "{\"instructions\":[{\"address\":\"0x1\",\"instruction\":\"caf\\u00e9\",\"symbol\":"
     "\"\\ud83d\\ude80\"}]}",
     1, UMI_STATUS_OK},
    {"unknown-hint",
     "{\"instructions\":[{\"address\":\"0x1\",\"instruction\":\"nop\",\"presentationHint\":"
     "\"adapter-custom\"}]}",
     1, UMI_STATUS_OK},
    {"escaped-key", "{\"instructions\":[{\"addr\\u0065ss\":\"0x1\",\"instruction\":\"nop\"}]}", 1,
     UMI_STATUS_OK},
    {"max-line",
     "{\"instructions\":[{\"address\":\"0x1\",\"instruction\":\"nop\",\"line\":9007199254740991}]}",
     1, UMI_STATUS_OK},
    {"short", "{\"instructions\":[]}", 1, UMI_STATUS_PARSE_ERROR},
    {"long",
     "{\"instructions\":[{\"address\":\"0x1\",\"instruction\":\"one\"},{\"address\":\"0x2\","
     "\"instruction\":\"two\"}]}",
     1, UMI_STATUS_PARSE_ERROR},
    {"missing", "{}", 1, UMI_STATUS_PARSE_ERROR},
    {"array-type", "{\"instructions\":{}}", 1, UMI_STATUS_PARSE_ERROR},
    {"row-type", "{\"instructions\":[false]}", 1, UMI_STATUS_PARSE_ERROR},
    {"missing-address", "{\"instructions\":[{\"instruction\":\"nop\"}]}", 1,
     UMI_STATUS_PARSE_ERROR},
    {"missing-text", "{\"instructions\":[{\"address\":\"0x1\"}]}", 1, UMI_STATUS_PARSE_ERROR},
    {"text-type", "{\"instructions\":[{\"address\":\"0x1\",\"instruction\":false}]}", 1,
     UMI_STATUS_PARSE_ERROR},
    {"address-expression", "{\"instructions\":[{\"address\":\"value()\",\"instruction\":\"nop\"}]}",
     1, UMI_STATUS_PARSE_ERROR},
    {"empty-hex", "{\"instructions\":[{\"address\":\"0x\",\"instruction\":\"nop\"}]}", 1,
     UMI_STATUS_PARSE_ERROR},
    {"negative-address", "{\"instructions\":[{\"address\":\"-1\",\"instruction\":\"nop\"}]}", 1,
     UMI_STATUS_PARSE_ERROR},
    {"duplicate",
     "{\"instructions\":[{\"address\":\"0x1\",\"addr\\u0065ss\":\"0x2\",\"instruction\":\"nop\"}]}",
     1, UMI_STATUS_PARSE_ERROR},
    {"nul", "{\"instructions\":[{\"address\":\"0x1\",\"instruction\":\"a\\u0000b\"}]}", 1,
     UMI_STATUS_PARSE_ERROR},
    {"negative-line",
     "{\"instructions\":[{\"address\":\"0x1\",\"instruction\":\"nop\",\"line\":-1}]}", 1,
     UMI_STATUS_PARSE_ERROR},
    {"fraction",
     "{\"instructions\":[{\"address\":\"0x1\",\"instruction\":\"nop\",\"column\":1.5}]}", 1,
     UMI_STATUS_PARSE_ERROR},
    {"large-line",
     "{\"instructions\":[{\"address\":\"0x1\",\"instruction\":\"nop\",\"line\":9007199254740992}]}",
     1, UMI_STATUS_PARSE_ERROR},
    {"location-type",
     "{\"instructions\":[{\"address\":\"0x1\",\"instruction\":\"nop\",\"location\":false}]}", 1,
     UMI_STATUS_PARSE_ERROR},
    {"reference-range",
     "{\"instructions\":[{\"address\":\"0x1\",\"instruction\":\"nop\",\"location\":{"
     "\"sourceReference\":2147483648}}]}",
     1, UMI_STATUS_PARSE_ERROR}};
static int Decode(const char *name)
{
    const DecodeCase *test = NULL;
    for (size_t i = 0; i < sizeof cases / sizeof cases[0]; ++i)
        if (strcmp(cases[i].name, name) == 0)
            test = &cases[i];
    if (test == NULL)
        return 2;
    char json[8192];
    int size = snprintf(json, sizeof json,
                        "{\"seq\":2,\"type\":\"response\",\"request_seq\":1,\"command\":"
                        "\"disassemble\",\"success\":true,\"body\":%s}",
                        test->body);
    CHECK(size > 0 && (size_t)size < sizeof json);
    UmiDebugInstructionPage *page = malloc(sizeof *page), *before = malloc(sizeof *before);
    CHECK(page != NULL && before != NULL);
    memset(page, 0xA5, sizeof *page);
    memcpy(before, page, sizeof *page);
    UmiStatus actual = UmiDebugInstructionPageDecode(json, test->count, page);
    int failed = actual != test->expected;
    if (failed)
        fprintf(stderr, "status %d expected %d\n", (int)actual, (int)test->expected);
    if (actual != UMI_STATUS_OK)
        failed |= memcmp(page, before, sizeof *page) != 0;
    else
    {
        failed |= page->count != test->count;
        if (strcmp(name, "inherit") == 0)
            failed |= !page->items[1].has_source ||
                      strcmp(page->items[1].source.path, "a.c") != 0 || page->items[1].line_known;
        if (strcmp(name, "filler") == 0)
            failed |= strcmp(page->items[0].presentation_hint, "invalid") != 0;
        if (strcmp(name, "normal") == 0)
            failed |= page->items[0].end_line != 8U || page->items[0].end_column != 3U ||
                      !page->items[0].end_column_known;
        if (strcmp(name, "unicode") == 0)
            failed |= strcmp(page->items[0].text, "caf\xc3\xa9") != 0 ||
                      strcmp(page->items[0].symbol, "\xf0\x9f\x9a\x80") != 0;
    }
    free(page);
    free(before);
    return failed;
}
static int Limits(void)
{
    UmiDebugInstructionPage *page = malloc(sizeof *page), *before = malloc(sizeof *before);
    CHECK(page != NULL && before != NULL);
    memset(page, 0xA5, sizeof *page);
    memcpy(before, page, sizeof *page);
    char *json = malloc(16384U);
    CHECK(json != NULL);
    strcpy(json, "{\"type\":\"response\",\"command\":\"disassemble\",\"success\":true,\"body\":{"
                 "\"instructions\":[");
    for (size_t i = 0U; i < 65U; ++i)
        strcat(json, i ? ",{\"address\":\"0x1\",\"instruction\":\"nop\"}"
                       : "{\"address\":\"0x1\",\"instruction\":\"nop\"}");
    strcat(json, "]}}");
    CHECK(UmiDebugInstructionPageDecode(json, 64U, page) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(page, before, sizeof *page) == 0);
    CHECK(UmiDebugInstructionPageDecode(json, 0U, page) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDebugInstructionPageDecode(json, 65U, page) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDebugInstructionPageDecode(
              "{\"type\":\"response\",\"command\":\"disassemble\",\"success\":false}", 1U, page) ==
          UMI_STATUS_UNAVAILABLE);
    CHECK(UmiDebugInstructionPageDecode("{\"type\":\"response\",\"command\":\"evaluate\","
                                        "\"success\":true,\"body\":{\"instructions\":[]}}",
                                        1U, page) == UMI_STATUS_PARSE_ERROR);
    CHECK(memcmp(page, before, sizeof *page) == 0);
    free(json);
    free(page);
    free(before);
    return 0;
}
static int Offsets(void)
{
    const char *accepted[] = {"0", "-0", "23", "-64", "9007199254740991", "-9007199254740991"};
    const int64_t values[] = {
        0, 0, 23, -64, UMI_DEBUG_INSTRUCTION_OFFSET_LIMIT, -UMI_DEBUG_INSTRUCTION_OFFSET_LIMIT};
    for (size_t i = 0U; i < sizeof values / sizeof values[0]; ++i)
    {
        int64_t value = 99;
        CHECK(UmiDebugInstructionOffsetRead(accepted[i], &value) == UMI_STATUS_OK);
        CHECK(value == values[i]);
    }
    const char *rejected[] = {"",
                              "-",
                              "+1",
                              " 1",
                              "1 ",
                              "0x10",
                              "1e2",
                              "1.5",
                              "9007199254740992",
                              "-9007199254740992",
                              "00000000000000000"};
    for (size_t i = 0U; i < sizeof rejected / sizeof rejected[0]; ++i)
    {
        int64_t value = 99;
        CHECK(UmiDebugInstructionOffsetRead(rejected[i], &value) != UMI_STATUS_OK);
        CHECK(value == 99);
    }
    return 0;
}
static int Reference(void)
{
    char reference[128] = "unchanged";
    const char *valid =
        "{\"type\":\"response\",\"command\":\"stackTrace\",\"success\":true,\"body\":{"
        "\"stackFrames\":[{\"id\":0,\"instructionPointerReference\":\"opaque reference\"}]}}";
    CHECK(DebugInstructionReferenceDecode(valid, 0U, reference, sizeof reference) == UMI_STATUS_OK);
    CHECK(strcmp(reference, "opaque reference") == 0);
    CHECK(DebugInstructionReferenceDecode(valid, 1U, reference, sizeof reference) ==
          UMI_STATUS_INVALID_STATE);
    CHECK(DebugInstructionReferenceDecode(valid, 0U, reference, 2U) ==
          UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(strcmp(reference, "opaque reference") == 0);
    CHECK(
        DebugInstructionReferenceDecode("{\"type\":\"response\",\"command\":\"stackTrace\","
                                        "\"success\":true,\"body\":{\"stackFrames\":[{\"id\":0}]}}",
                                        0U, reference, sizeof reference) == UMI_STATUS_NOT_FOUND);
    CHECK(strcmp(reference, "opaque reference") == 0);
    return 0;
}
static int Legacy(void)
{
    UmiDebugRuntimeDisassembly *out = malloc(sizeof *out), *before = malloc(sizeof *before);
    CHECK(out != NULL && before != NULL);
    memset(out, 0xA5, sizeof *out);
    memcpy(before, out, sizeof *out);
    CHECK(umi_debug_runtime_decode_disassembly(
              "{\"type\":\"response\",\"command\":\"disassemble\",\"success\":true,\"body\":{"
              "\"instructions\":[{\"address\":\"0x1\",\"instruction\":\"nop\",\"line\":4294967296}]"
              "}}",
              out) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(out, before, sizeof *out) == 0);
    CHECK(umi_debug_runtime_decode_disassembly(
              "{\"type\":\"response\",\"command\":\"disassemble\",\"success\":true,\"body\":{"
              "\"instructions\":[{\"address\":\"0x0\",\"instruction\":\"unavailable\","
              "\"presentationHint\":\"invalid\"}]}}",
              out) == UMI_STATUS_OK);
    CHECK(out->count == 1U && strcmp(out->items[0].presentation_hint, "invalid") == 0);
    free(out);
    free(before);
    return 0;
}

static int Addresses(void)
{
    uint64_t value = 99U;
    CHECK(DebugInstructionAddressRead("010", &value) == UMI_STATUS_OK && value == 10U);
    CHECK(DebugInstructionAddressRead("0xffffffffffffffff", &value) == UMI_STATUS_OK &&
          value == UINT64_MAX);
    CHECK(DebugInstructionAddressRead("18446744073709551615", &value) == UMI_STATUS_OK &&
          value == UINT64_MAX);
    CHECK(DebugInstructionAddressRead("0x10000000000000000", &value) ==
              UMI_STATUS_CAPACITY_EXCEEDED &&
          value == UINT64_MAX);
    CHECK(DebugInstructionAddressRead("18446744073709551616", &value) ==
              UMI_STATUS_CAPACITY_EXCEEDED &&
          value == UINT64_MAX);
    CHECK(DebugInstructionAddressRead("0x", &value) == UMI_STATUS_PARSE_ERROR &&
          value == UINT64_MAX);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    if (strcmp(argv[1], "addresses") == 0)
        return Addresses();
    if (strcmp(argv[1], "limits") == 0)
        return Limits();
    if (strcmp(argv[1], "offsets") == 0)
        return Offsets();
    if (strcmp(argv[1], "reference") == 0)
        return Reference();
    if (strcmp(argv[1], "legacy") == 0)
        return Legacy();
    return Decode(argv[1]);
}
