/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_codeguard_normalizer.c
 *
 * PURPOSE:
 *   Verify that duplicate normalisation ignores comments and formatting but
 *   preserves meaningful operators and string contents.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <stdio.h>

#include "umicom/codeguard/normalizer.h"

/*
 * Exercise write text and return a clear result when the behaviour no longer matches its
 * contract.
 */
static void write_text(const char *path, const char *text)
{
    FILE *file = fopen(path, "wb");

    assert(file != NULL);
    assert(fputs(text, file) >= 0);
    assert(fclose(file) == 0);
}

/*
 * Exercise normalized hash and return a clear result when the behaviour no longer matches
 * its contract.
 */
static uint64_t normalized_hash(const char *path)
{
    uint64_t raw_hash = 0U;
    uint64_t source_hash = 0U;
    size_t size = 0U;

    assert(umi_codeguard_hash_file(path, &raw_hash, &source_hash, &size) ==
           UMI_STATUS_OK);
    assert(raw_hash != 0U);
    assert(size > 0U);
    return source_hash;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    uint64_t first_hash;
    uint64_t second_hash;

    write_text("codeguard_normalizer_first.c",
               "int value(void) { /* explanation */ return 8 / 2; }\n");
    write_text("codeguard_normalizer_second.c",
               "// different explanation\nint value(void){return 8/2;}\n");
    first_hash = normalized_hash("codeguard_normalizer_first.c");
    second_hash = normalized_hash("codeguard_normalizer_second.c");
    assert(first_hash == second_hash);

    write_text("codeguard_normalizer_second.c",
               "int value(void){return 82;}\n");
    assert(first_hash != normalized_hash("codeguard_normalizer_second.c"));

    write_text("codeguard_normalizer_first.c",
               "const char *value(void){return \"a b\";}\n");
    write_text("codeguard_normalizer_second.c",
               "const char *value(void){return \"ab\";}\n");
    assert(normalized_hash("codeguard_normalizer_first.c") !=
           normalized_hash("codeguard_normalizer_second.c"));

    assert(remove("codeguard_normalizer_first.c") == 0);
    assert(remove("codeguard_normalizer_second.c") == 0);
    return 0;
}
