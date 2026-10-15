/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_compiler_diagnostic_parser.c
 *
 * PURPOSE:
 *   Implement the test compiler diagnostic parser behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "value_archive/construction_cases.h"
#include <string.h>
#include "umicom/diagnostics/compiler_parser.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted output value. */
static int UmiOutputRecordConstructionCases(void)
{
    UmiOutputRecord *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_output_record_init(value, "build", "Build", "gcc", UMI_OUTPUT_STREAM_ERROR, "C:/src/main.c:17:9: error: bad token") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->channel_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_output_record_init(value, oversized, "Build", "gcc", UMI_OUTPUT_STREAM_ERROR, "C:/src/main.c:17:9: error: bad token") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_output_record_init(value, value->channel_id, "Build", "gcc", UMI_OUTPUT_STREAM_ERROR, "C:/src/main.c:17:9: error: bad token") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->channel_id, "build") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->channel_name) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_output_record_init(value, "build", oversized, "gcc", UMI_OUTPUT_STREAM_ERROR, "C:/src/main.c:17:9: error: bad token") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_output_record_init(value, "build", value->channel_name, "gcc", UMI_OUTPUT_STREAM_ERROR, "C:/src/main.c:17:9: error: bad token") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->channel_name, "Build") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->source) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_output_record_init(value, "build", "Build", oversized, UMI_OUTPUT_STREAM_ERROR, "C:/src/main.c:17:9: error: bad token") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_output_record_init(value, "build", "Build", value->source, UMI_OUTPUT_STREAM_ERROR, "C:/src/main.c:17:9: error: bad token") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->source, "gcc") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->text) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_output_record_init(value, "build", "Build", "gcc", UMI_OUTPUT_STREAM_ERROR, oversized) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_output_record_init(value, "build", "Build", "gcc", UMI_OUTPUT_STREAM_ERROR, value->text) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->text, "C:/src/main.c:17:9: error: bad token") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void)
{
    if (UmiOutputRecordConstructionCases() != 0) return 1;

    UmiOutputRecord output;
    UmiDiagnosticSnapshot item;
    int matched = 0;
    assert(umi_output_record_init(&output, "build", "Build", "gcc",
        UMI_OUTPUT_STREAM_ERROR, "C:/src/main.c:17:9: error: bad token") == UMI_STATUS_OK);
    assert(umi_compiler_diagnostic_parse(&output, &item, &matched, NULL) == UMI_STATUS_OK);
    assert(matched && item.line == 17U && item.column == 9U);
    assert(strcmp(item.uri, "C:/src/main.c") == 0);
    assert(strcmp(item.message, "bad token") == 0);
    return 0;
}
