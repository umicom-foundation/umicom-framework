/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_integer_bounds.c
 * PURPOSE: Check exact JSON integer limits and unchanged output on invalid token data.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/json.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct IntegerCase
{
    const char *name, *text;
    int valid;
    int64_t expected;
} IntegerCase;
static const IntegerCase cases[] = {{"zero", "0", 1, 0},
                                    {"negative-zero", "-0", 1, 0},
                                    {"positive", "42", 1, 42},
                                    {"negative", "-42", 1, -42},
                                    {"signed-max", "9223372036854775807", 1, INT64_MAX},
                                    {"signed-min", "-9223372036854775808", 1, INT64_MIN},
                                    {"positive-overflow", "9223372036854775808", 0, 0},
                                    {"negative-overflow", "-9223372036854775809", 0, 0},
                                    {"unsigned-max", "18446744073709551615", 0, 0},
                                    {"huge", "9999999999999999999999999999999999999999", 0, 0},
                                    {"fraction", "1.0", 0, 0},
                                    {"exponent", "1e3", 0, 0},
                                    {"boolean", "true", 0, 0},
                                    {"null", "null", 0, 0},
                                    {"plus", "+1", 0, 0},
                                    {"leading-zero", "01", 0, 0},
                                    {"negative-leading-zero", "-01", 0, 0},
                                    {"space", " 1", 0, 0},
                                    {"suffix", "1 ", 0, 0},
                                    {"empty", "", 0, 0},
                                    {"minus", "-", 0, 0}};
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    UmiLanguageRuntimeJsonDocument *document = calloc(1U, sizeof *document);
    if (document == NULL)
        return 1;
    int result = 2;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
    {
        if (strcmp(cases[i].name, argv[1]) != 0)
            continue;
        /* Construct one primitive token deliberately: the value reader must
         * defend its own integer contract, including noncanonical spellings. */
        document->json = cases[i].text;
        document->token_count = 1U;
        document->tokens[0].type = UMI_LANGUAGE_RUNTIME_JSON_PRIMITIVE;
        document->tokens[0].start = 0;
        document->tokens[0].end = (int)strlen(cases[i].text);
        int64_t value = 1234567;
        UmiStatus status = umi_language_runtime_json_int64(document, 0, &value);
        result = cases[i].valid ? (status == UMI_STATUS_OK && value == cases[i].expected ? 0 : 1)
                                : (status == UMI_STATUS_PARSE_ERROR && value == 1234567 ? 0 : 1);
        if (result)
            fprintf(stderr, "%s: status=%s value=%lld\n", cases[i].name, umi_status_text(status),
                    (long long)value);
        break;
    }
    if (strcmp(argv[1], "token-bounds") == 0)
    {
        document->json = "1";
        document->token_count = 1U;
        document->tokens[0].type = UMI_LANGUAGE_RUNTIME_JSON_PRIMITIVE;
        document->tokens[0].start = -1;
        document->tokens[0].end = 1;
        int64_t value = 77;
        result =
            umi_language_runtime_json_int64(document, 0, &value) == UMI_STATUS_INVALID_ARGUMENT &&
                    value == 77
                ? 0
                : 1;
    }
    free(document);
    return result;
}
