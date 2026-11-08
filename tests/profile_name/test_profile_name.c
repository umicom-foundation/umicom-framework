/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/profile_name/test_profile_name.c
 *
 * PURPOSE:
 *   Exercise pure name-policy boundaries independently of account, platform and GUI services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/security/profile_name.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)

/* Rejecting an input must preserve all output bytes, including its tail. */
static int Refusal(const char *input, size_t size)
{
    char output[UMI_LOCAL_PROFILE_NAME_CAPACITY], before[sizeof(output)];
    memset(output, 0x5a, sizeof(output));
    memcpy(before, output, sizeof(output));
    CHECK(UmiLocalProfileNameBytes(input, size, output) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(output, before, sizeof(output)) == 0);
    return 0;
}
static int Alphabet(void)
{
    /* Test all byte values in both positions. The expected grammar is stated
     * as membership in the public alphabet, independently of case conversion. */
    const char first[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    const char rest[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
    for (unsigned int byte = 0U; byte <= 255U; ++byte) {
        char input[] = {'A', 'b', 'c'}, output[UMI_LOCAL_PROFILE_NAME_CAPACITY];
        for (size_t position = 0U; position < 2U; ++position) {
            input[0] = 'A'; input[1] = 'b'; input[position] = (char)byte;
            const char *alphabet = position == 0U ? first : rest;
            int accepted = byte != 0U && strchr(alphabet, (int)byte) != NULL;
            UmiStatus status = UmiLocalProfileNameBytes(input, sizeof(input), output);
            CHECK(status == (accepted ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT));
            if (!accepted) CHECK(Refusal(input, sizeof(input)) == 0);
        }
    }
    return 0;
}
static int Bounds(void)
{
    char exact[UMI_LOCAL_PROFILE_NAME_CAPACITY], output[UMI_LOCAL_PROFILE_NAME_CAPACITY];
    memset(exact, 'Z', sizeof(exact));
    CHECK(Refusal(exact, 0U) == 0);
    CHECK(Refusal(exact, 2U) == 0);
    CHECK(Refusal(exact, sizeof(exact)) == 0);
    CHECK(Refusal(exact, SIZE_MAX) == 0);
    CHECK(UmiLocalProfileNameBytes(exact, 48U, output) == UMI_STATUS_OK);
    CHECK(output[48] == '\0');
    for (size_t i = 0U; i < 48U; ++i) CHECK(output[i] == 'z');
    CHECK(UmiLocalProfileNameBytes(exact, 3U, output) == UMI_STATUS_OK);
    CHECK(strcmp(output, "zzz") == 0);
    for (size_t i = 3U; i < sizeof(output); ++i) CHECK(output[i] == '\0');
    return 0;
}
static int Overlap(void)
{
    char value[UMI_LOCAL_PROFILE_NAME_CAPACITY + 8U] = "Alice_7";
    CHECK(UmiLocalProfileNameBytes(value, 7U, value + 3U) == UMI_STATUS_OK);
    CHECK(strcmp(value + 3U, "alice_7") == 0);
    memcpy(value + 4U, "BOB-4", 5U);
    CHECK(UmiLocalProfileNameBytes(value + 4U, 5U, value) == UMI_STATUS_OK);
    CHECK(strcmp(value, "bob-4") == 0);
    CHECK(UmiLocalProfileName(value, value) == UMI_STATUS_OK);
    CHECK(strcmp(value, "bob-4") == 0);
    return 0;
}
static int Strings(void)
{
    char output[UMI_LOCAL_PROFILE_NAME_CAPACITY], before[sizeof(output)];
    CHECK(UmiLocalProfileName("AbC_12-x", output) == UMI_STATUS_OK);
    CHECK(strcmp(output, "abc_12-x") == 0);
    memset(output, 'Q', sizeof(output)); memcpy(before, output, sizeof(output));
    CHECK(UmiLocalProfileName(NULL, output) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiLocalProfileName("", output) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiLocalProfileName("ab", output) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiLocalProfileName("alice", NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(output, before, sizeof(output)) == 0);
    char bounded[UMI_LOCAL_PROFILE_NAME_CAPACITY]; memset(bounded, 'a', sizeof(bounded));
    CHECK(UmiLocalProfileName(bounded, output) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(output, before, sizeof(output)) == 0);
    return 0;
}
static int Nulls(void)
{
    CHECK(Refusal(NULL, 3U) == 0);
    CHECK(UmiLocalProfileNameBytes("abc", 3U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    const char nul[] = {'a', '\0', 'b'};
    CHECK(Refusal(nul, sizeof(nul)) == 0);
    const char unicode[] = {'A', (char)0xc3, (char)0xa9};
    CHECK(Refusal(unicode, sizeof(unicode)) == 0);
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (strcmp(argv[1], "alphabet") == 0) return Alphabet();
    if (strcmp(argv[1], "bounds") == 0) return Bounds();
    if (strcmp(argv[1], "overlap") == 0) return Overlap();
    if (strcmp(argv[1], "strings") == 0) return Strings();
    if (strcmp(argv[1], "nulls") == 0) return Nulls();
    return 2;
}
