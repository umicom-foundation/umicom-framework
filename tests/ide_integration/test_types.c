/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ide_integration/test_types.c
 *
 * PURPOSE:
 *   Focused regression coverage for IDE integration types.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/ide_integration/types.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    assert(strcmp(umi_ide_domain_text(UMI_IDE_DOMAIN_TESTS), "tests") == 0);
    assert(strcmp(umi_ide_navigation_reason_text(
        UMI_IDE_NAVIGATION_DEBUG), "debug") == 0);
    assert(strcmp(umi_ide_inline_state_text(UMI_IDE_INLINE_READY), "ready") == 0);
    assert(strcmp(umi_ide_gate_state_text(UMI_IDE_GATE_BLOCK), "block") == 0);
    return 0;
}

