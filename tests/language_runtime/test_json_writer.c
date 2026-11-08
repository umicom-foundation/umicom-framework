/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_json_writer.c
 *
 * PURPOSE:
 *   Verify a focused language-runtime core contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Required fixture setup and checks also execute in optimized builds. */
#include "../support/assertions.h"
#include <assert.h>
#include <string.h>
#include "umicom/language_runtime/json_writer.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
/* The earlier single escaping check is retained for review. The active case
 * also verifies the preserved ABI delegates strict, atomic writes to protocol. */
#if 0
int main(void){char b[128];UmiLanguageRuntimeJsonWriter w;umi_language_runtime_json_writer_init(&w,b,sizeof(b));assert(umi_language_runtime_json_writer_string(&w,"a\"b\n")==UMI_STATUS_OK);assert(strcmp(b,"\"a\\\"b\\n\"")==0);return 0;}
#endif
int main(void)
{
    char bytes[128]; UmiLanguageRuntimeJsonWriter writer;
    umi_language_runtime_json_writer_init(&writer, bytes, sizeof bytes);
    assert(umi_language_runtime_json_writer_string(&writer, "a\"b\n") == UMI_STATUS_OK);
    assert(strcmp(bytes, "\"a\\\"b\\n\"") == 0);
    umi_language_runtime_json_writer_init(&writer, bytes, 4U);
    assert(umi_language_runtime_json_writer_raw(&writer, "[") == UMI_STATUS_OK);
    assert(umi_language_runtime_json_writer_string(&writer, "x") == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(strcmp(bytes, "[") == 0 && writer.length == 1U);
    assert(umi_language_runtime_json_writer_bool(&writer, 1) == UMI_STATUS_CAPACITY_EXCEEDED);
    umi_language_runtime_json_writer_init(&writer, bytes, sizeof bytes);
    assert(umi_language_runtime_json_writer_string(&writer, "\xed\xa0\x80") == UMI_STATUS_INVALID_ARGUMENT);
    assert(bytes[0] == '\0' && writer.length == 0U);
    umi_language_runtime_json_writer_init(&writer, bytes, sizeof bytes);
    assert(umi_language_runtime_json_writer_uint64(&writer, UINT64_MAX) == UMI_STATUS_OK);
    assert(strcmp(bytes, "18446744073709551615") == 0);
    umi_language_runtime_json_writer_init(&writer, bytes, sizeof bytes);
    assert(umi_language_runtime_json_writer_int64(&writer, INT64_MIN) == UMI_STATUS_OK);
    assert(strcmp(bytes, "-9223372036854775808") == 0);
    return 0;
}
