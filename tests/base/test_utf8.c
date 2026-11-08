/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/base/test_utf8.c
 *
 * PURPOSE:
 *   Verify shared Unicode scalar boundaries and preserve decoder output and error-offset contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/base/utf8.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "line %d: %s\n", __LINE__, #x); return 1; } } while (0)
typedef struct Case { const char *name; unsigned char bytes[4]; size_t length; int valid; uint32_t scalar; } Case;
int main(void)
{
    static const Case cases[] = {
        {"nul", {0}, 1, 1, 0}, {"ascii-max", {0x7f}, 1, 1, 0x7f},
        {"two-min", {0xc2,0x80}, 2, 1, 0x80}, {"two-max", {0xdf,0xbf}, 2, 1, 0x7ff},
        {"three-min", {0xe0,0xa0,0x80}, 3, 1, 0x800},
        {"before-surrogate", {0xed,0x9f,0xbf}, 3, 1, 0xd7ff},
        {"after-surrogate", {0xee,0x80,0x80}, 3, 1, 0xe000},
        {"three-max", {0xef,0xbf,0xbf}, 3, 1, 0xffff},
        {"four-min", {0xf0,0x90,0x80,0x80}, 4, 1, 0x10000},
        {"four-max", {0xf4,0x8f,0xbf,0xbf}, 4, 1, 0x10ffff},
        {"continuation", {0x80}, 1, 0, 0}, {"lead-ff", {0xff}, 1, 0, 0},
        {"overlong-two", {0xc0,0x80}, 2, 0, 0},
        {"overlong-three", {0xe0,0x9f,0xbf}, 3, 0, 0},
        {"overlong-four", {0xf0,0x8f,0xbf,0xbf}, 4, 0, 0},
        {"surrogate-start", {0xed,0xa0,0x80}, 3, 0, 0},
        {"surrogate-end", {0xed,0xbf,0xbf}, 3, 0, 0},
        {"above-unicode", {0xf4,0x90,0x80,0x80}, 4, 0, 0},
        {"lead-f5", {0xf5,0x80,0x80,0x80}, 4, 0, 0},
        {"bad-continuation", {0xe1,0x41,0x80}, 3, 0, 0},
        {"truncated-two", {0xc2}, 1, 0, 0},
        {"truncated-three", {0xe1,0x80}, 2, 0, 0},
        {"truncated-four", {0xf1,0x80,0x80}, 3, 0, 0}
    };
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        const Case *c = &cases[i];
        size_t used = 99U, error = 99U; uint32_t scalar = UINT32_MAX;
        int valid = UmiUtf8DecodeScalar(c->bytes, c->length, &used, &scalar);
        if (valid != c->valid) { fprintf(stderr, "%s\n", c->name); return 1; }
        CHECK(UmiUtf8Validate(c->bytes, c->length, &error) == c->valid);
        CHECK(error == (c->valid ? c->length : 0U));
        if (valid) CHECK(used == c->length && scalar == c->scalar);
        else CHECK(used == 99U && scalar == UINT32_MAX);
        unsigned char prefixed[6] = {'a', 0};
        memcpy(prefixed + 2U, c->bytes, c->length);
        CHECK(UmiUtf8Validate(prefixed, c->length + 2U, &error) == c->valid);
        CHECK(error == (c->valid ? c->length + 2U : 2U));
    }
    size_t error = 99U, used = 99U; uint32_t scalar = UINT32_MAX;
    CHECK(UmiUtf8Validate(NULL, 0U, &error) && error == 0U);
    error = 99U;
    CHECK(!UmiUtf8Validate(NULL, 1U, &error) && error == 99U);
    CHECK(!UmiUtf8DecodeScalar(NULL, 1U, &used, &scalar));
    CHECK(!UmiUtf8DecodeScalar((const unsigned char *)"x", 0U, &used, &scalar));
    CHECK(!UmiUtf8DecodeScalar((const unsigned char *)"x", 1U, NULL, &scalar));
    CHECK(!UmiUtf8DecodeScalar((const unsigned char *)"x", 1U, &used, NULL));
    CHECK(used == 99U && scalar == UINT32_MAX);
    return 0;
}
