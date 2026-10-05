/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/memory_inspection/test_reply.c
 * PURPOSE: Exercise strict memory reply bounds, canonical base64 and unchanged failure outputs.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/debug_runtime/memory_inspection.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                          \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *name = argv[1], *json = NULL;
    uint32_t count = 256U;
    char maximumJson[5600];
    UmiStatus expected = UMI_STATUS_PARSE_ERROR;
    if (strcmp(name, "bytes") == 0)
    {
        expected = UMI_STATUS_OK;
        count = 256U;
        json = "{\"body\":{\"address\":\"0x1000\",\"data\":\"QUJDAH+A\"}}";
    }
    else if (strcmp(name, "decimal") == 0)
    {
        expected = UMI_STATUS_OK;
        count = 1U;
        json = "{\"body\":{\"address\":\"18446744073709551616\",\"data\":\"AQ==\"}}";
    }
    else if (strcmp(name, "empty") == 0)
    {
        expected = UMI_STATUS_OK;
        count = 256U;
        json = "{\"body\":{\"address\":\"0\",\"data\":\"\"}}";
    }
    else if (strcmp(name, "no-data") == 0)
    {
        expected = UMI_STATUS_OK;
        count = 256U;
        json = "{\"body\":{\"address\":\"0x0\",\"unreadableBytes\":17}}";
    }
    else if (strcmp(name, "short") == 0)
    {
        expected = UMI_STATUS_OK;
        count = 256U;
        json = "{\"body\":{\"address\":\"0x1000\",\"data\":\"QUI=\"}}";
    }
    else if (strcmp(name, "unreadable") == 0)
    {
        expected = UMI_STATUS_OK;
        count = 1U;
        json = "{\"body\":{\"address\":\"0x1000\",\"data\":\"AQ==\",\"unreadableBytes\":9007199254740991}}";
    }
    else if (strcmp(name, "extension") == 0)
    {
        expected = UMI_STATUS_OK;
        count = 1U;
        json = "{\"body\":{\"address\":\"0x0\",\"vendor\":{\"text\":\"caf\\u00e9\"},\"data\":\"AA==\"}}";
    }
    else if (strcmp(name, "maximum") == 0)
    {
        /* Assemble the maximum response without exceeding the portable C
         * string-literal limit. Canonical base64 for zero bytes is all A's
         * followed by the two padding characters for the final single byte. */
        expected = UMI_STATUS_OK;
        count = 4096U;
        strcpy(maximumJson, "{\"body\":{\"address\":\"0x1000\",\"data\":\"");
        size_t prefix = strlen(maximumJson);
        memset(maximumJson + prefix, 'A', 5462U);
        strcpy(maximumJson + prefix + 5462U, "==\"}}");
        json = maximumJson;
    }
    else if (strcmp(name, "missing-body") == 0)
    {
        expected = UMI_STATUS_NOT_FOUND;
        count = 1U;
        json = "{}";
    }
    else if (strcmp(name, "null-body") == 0)
    {
        expected = UMI_STATUS_PARSE_ERROR;
        count = 1U;
        json = "{\"body\":null}";
    }
    else if (strcmp(name, "array-body") == 0)
    {
        expected = UMI_STATUS_PARSE_ERROR;
        count = 1U;
        json = "{\"body\":[]}";
    }
    else if (strcmp(name, "missing-address") == 0)
    {
        expected = UMI_STATUS_PARSE_ERROR;
        count = 1U;
        json = "{\"body\":{\"data\":\"AQ==\"}}";
    }
    else if (strcmp(name, "address-type") == 0)
    {
        expected = UMI_STATUS_PARSE_ERROR;
        count = 1U;
        json = "{\"body\":{\"address\":2}}";
    }
    else if (strcmp(name, "empty-address") == 0)
    {
        expected = UMI_STATUS_PARSE_ERROR;
        count = 1U;
        json = "{\"body\":{\"address\":\"\"}}";
    }
    else if (strcmp(name, "hex-prefix") == 0)
    {
        expected = UMI_STATUS_PARSE_ERROR;
        count = 1U;
        json = "{\"body\":{\"address\":\"0x\"}}";
    }
    else if (strcmp(name, "address-text") == 0)
    {
        expected = UMI_STATUS_PARSE_ERROR;
        count = 1U;
        json = "{\"body\":{\"address\":\"pointer\"}}";
    }
    else if (strcmp(name, "address-nul") == 0)
    {
        expected = UMI_STATUS_PARSE_ERROR;
        count = 1U;
        json = "{\"body\":{\"address\":\"0\\u0000\"}}";
    }
    else if (strcmp(name, "duplicate-body") == 0)
    {
        expected = UMI_STATUS_PARSE_ERROR;
        count = 1U;
        json = "{\"body\":{\"address\":\"0\"},\"body\":{\"address\":\"0\"}}";
    }
    else if (strcmp(name, "duplicate-address") == 0)
    {
        expected = UMI_STATUS_PARSE_ERROR;
        count = 1U;
        json = "{\"body\":{\"address\":\"0\",\"\\u0061ddress\":\"1\"}}";
    }
    else if (strcmp(name, "duplicate-data") == 0)
    {
        expected = UMI_STATUS_PARSE_ERROR;
        count = 1U;
        json = "{\"body\":{\"address\":\"0\",\"data\":\"AA==\",\"d\\u0061ta\":\"AA==\"}}";
    }
    else if (strcmp(name, "duplicate-unreadable") == 0)
    {
        expected = UMI_STATUS_PARSE_ERROR;
        count = 1U;
        json = "{\"body\":{\"address\":\"0\",\"unreadableBytes\":0,\"unreadableBytes\":1}}";
    }
    else if (strcmp(name, "data-type") == 0)
    {
        expected = UMI_STATUS_PARSE_ERROR;
        count = 1U;
        json = "{\"body\":{\"address\":\"0\",\"data\":null}}";
    }
    else if (strcmp(name, "padding-middle") == 0)
    {
        expected = UMI_STATUS_PARSE_ERROR;
        count = 1U;
        json = "{\"body\":{\"address\":\"0\",\"data\":\"AA==AAAA\"}}";
    }
    else if (strcmp(name, "padding-first") == 0)
    {
        expected = UMI_STATUS_PARSE_ERROR;
        count = 1U;
        json = "{\"body\":{\"address\":\"0\",\"data\":\"=AAA\"}}";
    }
    else if (strcmp(name, "padding-second") == 0)
    {
        expected = UMI_STATUS_PARSE_ERROR;
        count = 1U;
        json = "{\"body\":{\"address\":\"0\",\"data\":\"A=AA\"}}";
    }
    else if (strcmp(name, "padding-tail") == 0)
    {
        expected = UMI_STATUS_PARSE_ERROR;
        count = 1U;
        json = "{\"body\":{\"address\":\"0\",\"data\":\"AA=A\"}}";
    }
    else if (strcmp(name, "padding-bits-one") == 0)
    {
        expected = UMI_STATUS_PARSE_ERROR;
        count = 1U;
        json = "{\"body\":{\"address\":\"0\",\"data\":\"AB==\"}}";
    }
    else if (strcmp(name, "padding-bits-two") == 0)
    {
        expected = UMI_STATUS_PARSE_ERROR;
        count = 1U;
        json = "{\"body\":{\"address\":\"0\",\"data\":\"AAB=\"}}";
    }
    else if (strcmp(name, "unpadded") == 0)
    {
        expected = UMI_STATUS_PARSE_ERROR;
        count = 1U;
        json = "{\"body\":{\"address\":\"0\",\"data\":\"AQ\"}}";
    }
    else if (strcmp(name, "whitespace") == 0)
    {
        expected = UMI_STATUS_PARSE_ERROR;
        count = 1U;
        json = "{\"body\":{\"address\":\"0\",\"data\":\"AA== \"}}";
    }
    else if (strcmp(name, "alphabet") == 0)
    {
        expected = UMI_STATUS_PARSE_ERROR;
        count = 1U;
        json = "{\"body\":{\"address\":\"0\",\"data\":\"AA-_\"}}";
    }
    else if (strcmp(name, "negative") == 0)
    {
        expected = UMI_STATUS_PARSE_ERROR;
        count = 1U;
        json = "{\"body\":{\"address\":\"0\",\"unreadableBytes\":-1}}";
    }
    else if (strcmp(name, "fraction") == 0)
    {
        expected = UMI_STATUS_PARSE_ERROR;
        count = 1U;
        json = "{\"body\":{\"address\":\"0\",\"unreadableBytes\":1.5}}";
    }
    else if (strcmp(name, "exponent") == 0)
    {
        expected = UMI_STATUS_PARSE_ERROR;
        count = 1U;
        json = "{\"body\":{\"address\":\"0\",\"unreadableBytes\":1e1}}";
    }
    else if (strcmp(name, "unreadable-type") == 0)
    {
        expected = UMI_STATUS_PARSE_ERROR;
        count = 1U;
        json = "{\"body\":{\"address\":\"0\",\"unreadableBytes\":\"1\"}}";
    }
    else if (strcmp(name, "overflow") == 0)
    {
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
        count = 1U;
        json = "{\"body\":{\"address\":\"0\",\"unreadableBytes\":9007199254740992}}";
    }
    else if (strcmp(name, "trailing") == 0)
    {
        expected = UMI_STATUS_PARSE_ERROR;
        count = 1U;
        json = "{\"body\":{\"address\":\"0\"}} extra";
    }
    else if (strcmp(name, "requested-bound") == 0)
    {
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
        count = 1U;
        json = "{\"body\":{\"address\":\"0\",\"data\":\"QUJD\"}}";
    }
    else if (strcmp(name, "invalid-count") == 0)
    {
        expected = UMI_STATUS_INVALID_ARGUMENT;
        count = 0U;
        json = "{}";
    }
    else if (strcmp(name, "invalid-output") == 0)
    {
        CHECK(UmiDebugMemoryDecode("{}", 1U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        return 0;
    }
    else
        CHECK(0);
    UmiDebugMemoryBytes out, before;
    memset(&out, 0xA5, sizeof out);
    memcpy(&before, &out, sizeof before);
    CHECK(UmiDebugMemoryDecode(json, count, &out) == expected);
    if (expected != UMI_STATUS_OK)
        CHECK(memcmp(&out, &before, sizeof out) == 0);
    else
    {
        CHECK(out.count <= count && out.address[0]);
        if (strcmp(name, "bytes") == 0)
        {
            const unsigned char expectedBytes[] = {65, 66, 67, 0, 127, 128};
            CHECK(out.count == 6U && memcmp(out.bytes, expectedBytes, 6U) == 0);
        }
        if (strcmp(name, "decimal") == 0)
            CHECK(strcmp(out.address, "18446744073709551616") == 0 && out.count == 1U && out.bytes[0] == 1);
        if (strcmp(name, "empty") == 0 || strcmp(name, "no-data") == 0)
            CHECK(out.count == 0U);
        if (strcmp(name, "short") == 0)
            CHECK(out.count == 2U && out.unreadable == 0U);
        if (strcmp(name, "unreadable") == 0)
            CHECK(out.unreadable == UINT64_C(9007199254740991));
        if (strcmp(name, "maximum") == 0)
        {
            CHECK(out.count == 4096U);
            for (size_t i = 0U; i < 4096U; ++i)
                CHECK(out.bytes[i] == 0U);
        }
    }
    return 0;
}
