/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/job_history/test_sha.c
 * PURPOSE: Verify portable digest vectors and failure contracts through Base alone.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/base/sha256.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do                                                                                             \
    {                                                                                              \
        if (!(x))                                                                                  \
        {                                                                                          \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    char hex[65];
    UmiSha256 context;
    unsigned char digest[32];
    if (strcmp(argv[1], "vectors") == 0)
    {
        CHECK(UmiSha256Buffer(NULL, 0U, hex) == UMI_STATUS_OK);
        CHECK(strcmp(hex, "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855") == 0);
        CHECK(UmiSha256Buffer("abc", 3U, hex) == UMI_STATUS_OK);
        CHECK(strcmp(hex, "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad") == 0);
        const char *message = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
        for (size_t stride = 1U; stride <= 65U; ++stride)
        {
            UmiSha256Init(&context);
            for (size_t offset = 0U; offset < strlen(message);)
            {
                size_t count = strlen(message) - offset;
                if (count > stride)
                    count = stride;
                CHECK(UmiSha256Update(&context, message + offset, count) == UMI_STATUS_OK);
                offset += count;
            }
            CHECK(UmiSha256Final(&context, digest) == UMI_STATUS_OK);
            UmiSha256Hex(digest, hex);
            CHECK(strcmp(hex, "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1") ==
                  0);
        }
    }
    else if (strcmp(argv[1], "lifetime") == 0)
    {
        UmiSha256Init(&context);
        CHECK(UmiSha256Final(&context, digest) == UMI_STATUS_OK);
        memset(digest, 0x5a, sizeof(digest));
        CHECK(UmiSha256Final(&context, digest) == UMI_STATUS_INVALID_STATE);
        CHECK(digest[0] == 0x5a && digest[31] == 0x5a);
        CHECK(UmiSha256Update(&context, NULL, 0U) == UMI_STATUS_INVALID_STATE);
    }
    else if (strcmp(argv[1], "overflow") == 0)
    {
        UmiSha256Init(&context);
        context.totalBytes = UINT64_MAX / 8U;
        CHECK(UmiSha256Update(&context, "x", 1U) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(context.totalBytes == UINT64_MAX / 8U && context.used == 0U);
    }
    else
        return 2;
    return 0;
}
