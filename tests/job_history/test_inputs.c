/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/job_history/test_inputs.c
 * PURPOSE: Exercise deterministic input-set identities, bounded refusal and sealed ownership.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/base/sha256.h"
#include "umicom/data/job_inputs.h"
#include <stdio.h>
#include <stdlib.h>
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
    UmiJobInputs *left = NULL, *right = NULL;
    char a[65], b[65], digest[65];
    CHECK(UmiJobInputsCreate(8U, &left) == UMI_STATUS_OK);
    CHECK(UmiJobInputsCreate(8U, &right) == UMI_STATUS_OK);
    const char *mode = argv[1];
    if (strcmp(mode, "order") == 0)
    {
        CHECK(UmiJobInputsAddBytes(left, "source/main.c", "abc", 3U) == UMI_STATUS_OK);
        CHECK(UmiJobInputsAddBytes(left, "config", "Debug", 5U) == UMI_STATUS_OK);
        CHECK(UmiJobInputsAddBytes(right, "config", "Debug", 5U) == UMI_STATUS_OK);
        CHECK(UmiJobInputsAddBytes(right, "source/main.c", "abc", 3U) == UMI_STATUS_OK);
        CHECK(UmiJobInputsSeal(left, a) == UMI_STATUS_OK &&
              UmiJobInputsSeal(right, b) == UMI_STATUS_OK);
        CHECK(strcmp(a, b) == 0);
    }
    else if (strcmp(mode, "content") == 0 || strcmp(mode, "names") == 0)
    {
        CHECK(UmiJobInputsAddBytes(left, "notes", "abc", 3U) == UMI_STATUS_OK);
        CHECK(UmiJobInputsAddBytes(right, strcmp(mode, "names") == 0 ? "Notes" : "notes",
                                   strcmp(mode, "content") == 0 ? "abd" : "abc",
                                   3U) == UMI_STATUS_OK);
        CHECK(UmiJobInputsSeal(left, a) == UMI_STATUS_OK &&
              UmiJobInputsSeal(right, b) == UMI_STATUS_OK);
        CHECK(strcmp(a, b) != 0);
    }
    else if (strcmp(mode, "empty") == 0)
    {
        CHECK(UmiJobInputsSeal(left, a) == UMI_STATUS_OK);
        CHECK(strcmp(a, "597e4ca9b0f0254663e96837502626abf0261c55e25be06f488a000457d4254d") == 0);
        CHECK(UmiJobInputsAddBytes(right, "empty.txt", NULL, 0U) == UMI_STATUS_OK);
        CHECK(UmiJobInputsSeal(right, b) == UMI_STATUS_OK && strcmp(a, b) != 0);
    }
    else if (strcmp(mode, "owned") == 0)
    {
        char name[] = "notes", content[] = "abc";
        CHECK(UmiJobInputsAddBytes(left, name, content, 3U) == UMI_STATUS_OK);
        name[0] = 'x';
        content[0] = 'z';
        CHECK(UmiSha256Buffer("abc", 3U, digest) == UMI_STATUS_OK);
        CHECK(UmiJobInputsAddDigest(right, "notes", digest) == UMI_STATUS_OK);
        memset(digest, 'f', 64U);
        CHECK(UmiJobInputsSeal(left, a) == UMI_STATUS_OK &&
              UmiJobInputsSeal(right, b) == UMI_STATUS_OK);
        CHECK(strcmp(a, b) == 0);
    }
    else if (strcmp(mode, "duplicate") == 0)
    {
        CHECK(UmiJobInputsAddBytes(left, "notes", "abc", 3U) == UMI_STATUS_OK);
        CHECK(UmiJobInputsAddBytes(left, "notes", "different", 9U) == UMI_STATUS_ALREADY_EXISTS);
        CHECK(UmiJobInputsAddBytes(right, "notes", "abc", 3U) == UMI_STATUS_OK);
        CHECK(UmiJobInputsSeal(left, a) == UMI_STATUS_OK &&
              UmiJobInputsSeal(right, b) == UMI_STATUS_OK);
        CHECK(strcmp(a, b) == 0);
    }
    else if (strcmp(mode, "capacity") == 0)
    {
        for (unsigned i = 0U; i < 8U; ++i)
        {
            char name[24];
            (void)snprintf(name, sizeof(name), "input-%u", i);
            CHECK(UmiJobInputsAddBytes(left, name, NULL, 0U) == UMI_STATUS_OK);
            CHECK(UmiJobInputsAddBytes(right, name, NULL, 0U) == UMI_STATUS_OK);
        }
        CHECK(UmiJobInputsAddBytes(left, "extra", "x", 1U) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiJobInputsAddBytes(left, "input-0", NULL, 0U) == UMI_STATUS_ALREADY_EXISTS);
        CHECK(UmiJobInputsSeal(left, a) == UMI_STATUS_OK &&
              UmiJobInputsSeal(right, b) == UMI_STATUS_OK);
        CHECK(strcmp(a, b) == 0);
    }
    else if (strcmp(mode, "sealed") == 0)
    {
        CHECK(UmiJobInputsAddBytes(left, "notes", "abc", 3U) == UMI_STATUS_OK);
        CHECK(UmiJobInputsSeal(left, a) == UMI_STATUS_OK);
        CHECK(UmiJobInputsAddBytes(left, "more", "x", 1U) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiJobInputsSeal(left, b) == UMI_STATUS_OK && strcmp(a, b) == 0);
    }
    else if (strcmp(mode, "invalid") == 0)
    {
        char longName[UMI_JOB_INPUT_NAME_CAPACITY];
        char emptyDigest[UMI_JOB_IDENTITY_DIGEST_CAPACITY] = {0};
        memset(longName, 'x', sizeof(longName));
        CHECK(UmiJobInputsAddBytes(left, longName, NULL, 0U) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiJobInputsAddBytes(left, "bad\nname", NULL, 0U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiJobInputsAddBytes(left, "name", NULL, 1U) == UMI_STATUS_INVALID_ARGUMENT);
        /* Preserve empty-digest rejection while supplying the API's declared
         * readable capacity. The original one-byte literal call is retained for
         * review; the replacement still contains an empty, invalid digest. */
#if 0
        CHECK(UmiJobInputsAddDigest(left, "name", "") == UMI_STATUS_INVALID_ARGUMENT);
#endif
        CHECK(UmiJobInputsAddDigest(left, "name", emptyDigest) == UMI_STATUS_INVALID_ARGUMENT);
        memset(digest, 'A', 64U);
        digest[64] = '\0';
        CHECK(UmiJobInputsAddDigest(left, "name", digest) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiJobInputsSeal(left, a) == UMI_STATUS_OK &&
              UmiJobInputsSeal(right, b) == UMI_STATUS_OK);
        CHECK(strcmp(a, b) == 0);
        UmiJobInputs *invalid = left;
        CHECK(UmiJobInputsCreate(0U, &invalid) == UMI_STATUS_INVALID_ARGUMENT && invalid == NULL);
        CHECK(UmiJobInputsCreate((size_t)UMI_JOB_INPUT_MAX_CAPACITY + 1U, &invalid) ==
                  UMI_STATUS_INVALID_ARGUMENT &&
              invalid == NULL);
    }
    else if (strcmp(mode, "unicode") == 0)
    {
        CHECK(UmiJobInputsAddBytes(left, "images/caf\xc3\xa9.png", "pixels", 6U) == UMI_STATUS_OK);
        CHECK(UmiJobInputsAddBytes(right, "images/caf\xc3\xa9.png", "pixels", 6U) == UMI_STATUS_OK);
        CHECK(UmiJobInputsSeal(left, a) == UMI_STATUS_OK &&
              UmiJobInputsSeal(right, b) == UMI_STATUS_OK);
        CHECK(strcmp(a, b) == 0);
    }
    else
        return 2;
    UmiJobInputsDestroy(left);
    UmiJobInputsDestroy(right);
    return 0;
}
