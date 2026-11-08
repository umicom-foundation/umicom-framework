/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_lifecycle/test_json_evidence_writer.c
 *
 * PURPOSE:
 *   Verify atomic JSON value emission, Unicode rejection and arithmetic boundary admission.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/protocol/json_writer.h"
#include "../support/assertions.h"
#include <assert.h>
#include <stdint.h>
#include <string.h>

int main(void)
{
    char text[160], before[160];
    UmiJsonWriter writer;
    UmiJsonWriterInit(NULL, NULL, 0U);
    assert(UmiJsonWriterRaw(NULL, "") == UMI_STATUS_INVALID_ARGUMENT);
    assert(UmiJsonWriterString(NULL, "") == UMI_STATUS_INVALID_ARGUMENT);
    assert(UmiJsonWriterUInt64(NULL, 0U) == UMI_STATUS_INVALID_ARGUMENT);
    assert(UmiJsonWriterInt64(NULL, 0) == UMI_STATUS_INVALID_ARGUMENT);
    assert(UmiJsonWriterBool(NULL, 0) == UMI_STATUS_INVALID_ARGUMENT);
    UmiJsonWriterInit(&writer, text, sizeof text);
    assert(UmiJsonWriterRaw(&writer, "[") == UMI_STATUS_OK);
    assert(UmiJsonWriterString(&writer, "quote\" slash\\\n\r\t\b\f") == UMI_STATUS_OK);
    assert(UmiJsonWriterRaw(&writer, ",") == UMI_STATUS_OK);
    assert(UmiJsonWriterString(&writer, "caf\xc3\xa9 \xf0\x9f\x8c\x8d") == UMI_STATUS_OK);
    assert(UmiJsonWriterRaw(&writer, "]") == UMI_STATUS_OK);
    assert(strcmp(text, "[\"quote\\\" slash\\\\\\n\\r\\t\\u0008\\u000c\",\"caf\xc3\xa9 \xf0\x9f\x8c\x8d\"]") == 0);
    assert(writer.length == strlen(text));
    UmiJsonWriterInit(&writer, text, sizeof text);
    assert(UmiJsonWriterInt64(&writer, INT64_MIN) == UMI_STATUS_OK);
    assert(UmiJsonWriterRaw(&writer, ",") == UMI_STATUS_OK);
    assert(UmiJsonWriterUInt64(&writer, UINT64_MAX) == UMI_STATUS_OK);
    assert(UmiJsonWriterRaw(&writer, ",") == UMI_STATUS_OK);
    assert(UmiJsonWriterBool(&writer, -1) == UMI_STATUS_OK);
    assert(UmiJsonWriterRaw(&writer, ",") == UMI_STATUS_OK);
    assert(UmiJsonWriterBool(&writer, 0) == UMI_STATUS_OK);
    assert(strcmp(text, "-9223372036854775808,18446744073709551615,true,false") == 0);
    /* Every undersized capacity preserves a previously accepted prefix. */
    const char *value = "a\"b\n";
    const char *complete = "[\"a\\\"b\\n\"";
    for (size_t capacity = 2U; capacity <= strlen(complete); ++capacity) {
        memset(text, 0x5a, sizeof text);
        UmiJsonWriterInit(&writer, text, capacity);
        assert(UmiJsonWriterRaw(&writer, "[") == UMI_STATUS_OK);
        memcpy(before, text, sizeof text);
        assert(UmiJsonWriterString(&writer, value) == UMI_STATUS_CAPACITY_EXCEEDED);
        assert(memcmp(before, text, sizeof text) == 0 && writer.length == 1U);
        assert(UmiJsonWriterRaw(&writer, "x") == UMI_STATUS_CAPACITY_EXCEEDED);
        assert(memcmp(before, text, sizeof text) == 0);
    }
    UmiJsonWriterInit(&writer, text, strlen(complete) + 1U);
    assert(UmiJsonWriterRaw(&writer, "[") == UMI_STATUS_OK);
    assert(UmiJsonWriterString(&writer, value) == UMI_STATUS_OK);
    assert(strcmp(text, complete) == 0);
    /* Corrupted state must be rejected before pointer arithmetic. This models
     * the former addition wrapping to a small value, without huge allocation. */
    UmiJsonWriterInit(&writer, text, sizeof text);
    writer.length = SIZE_MAX - 1U;
    assert(UmiJsonWriterRaw(&writer, "abcd") == UMI_STATUS_INVALID_ARGUMENT);
    assert(text[0] == '\0');
    const char *bad[] = {"\xc0\xaf", "\xed\xa0\x80", "\xf4\x90\x80\x80", "\xe2\x82", "\x80"};
    for (size_t i = 0U; i < sizeof bad / sizeof bad[0]; ++i) {
        UmiJsonWriterInit(&writer, text, sizeof text);
        assert(UmiJsonWriterRaw(&writer, "[") == UMI_STATUS_OK);
        assert(UmiJsonWriterString(&writer, bad[i]) == UMI_STATUS_INVALID_ARGUMENT);
        assert(strcmp(text, "[") == 0 && writer.length == 1U);
        assert(UmiJsonWriterString(&writer, "good") == UMI_STATUS_INVALID_ARGUMENT);
        UmiJsonWriterInit(&writer, text, sizeof text);
        assert(UmiJsonWriterRaw(&writer, bad[i]) == UMI_STATUS_INVALID_ARGUMENT);
        assert(text[0] == '\0');
    }
    UmiJsonWriterInit(&writer, text, sizeof text);
    assert(UmiJsonWriterString(&writer, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    assert(UmiJsonWriterBool(&writer, 1) == UMI_STATUS_INVALID_ARGUMENT);
    UmiJsonWriterInit(&writer, NULL, 1U);
    assert(UmiJsonWriterRaw(&writer, "x") == UMI_STATUS_INVALID_ARGUMENT);
    UmiJsonWriterInit(&writer, text, 0U);
    assert(UmiJsonWriterString(&writer, "") == UMI_STATUS_INVALID_ARGUMENT);
    UmiJsonWriterInit(&writer, text, 3U);
    assert(UmiJsonWriterString(&writer, "") == UMI_STATUS_OK && !strcmp(text, "\"\""));
    UmiJsonWriterInit(&writer, text, 1U);
    assert(UmiJsonWriterRaw(&writer, "") == UMI_STATUS_OK);
    assert(UmiJsonWriterRaw(&writer, "x") == UMI_STATUS_CAPACITY_EXCEEDED);
    return 0;
}
