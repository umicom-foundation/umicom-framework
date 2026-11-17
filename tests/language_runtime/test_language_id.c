/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_language_id.c
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
#include "umicom/language_runtime/language_id.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
/* Expand the retained C-only check to cover every native course identity. The former implementation is retained for owner review. */
#if 0
int main(void){char b[64];assert(umi_language_runtime_normalize_language_id("developer.language.c23",b,sizeof(b))==UMI_STATUS_OK);assert(strcmp(b,"c")==0);return 0;}

#endif
int main(void)
{
    char buffer[64];
    const char *ids[] = {"c23", "cpp", "rust", "zig", "fortran", "d", "odin", "nim"};
    const char *expected[] = {"c", "cpp", "rust", "zig", "fortran", "d", "odin", "nim"};
    for (size_t i = 0; i < sizeof ids / sizeof ids[0]; ++i) {
        char project[64];
        strcpy(project, "developer.language.");
        strcat(project, ids[i]); /* Both literals fit the fixed fixture buffer. */
        assert(umi_language_runtime_normalize_language_id(project, buffer, sizeof buffer) == UMI_STATUS_OK);
        assert(strcmp(buffer, expected[i]) == 0);
    }
    return 0;
}
