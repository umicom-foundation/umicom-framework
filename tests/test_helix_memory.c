/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_helix_memory.c
 *
 * PURPOSE:
 *   Validate the AI and Helix foundation introduced by Batch 10.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This focused executable keeps one contract easy to diagnose when the larger test suite reports a failure.
 */



#include <assert.h>
#include <string.h>
#include "umicom/umicom.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiHelixMemory memory;
    umi_helix_memory_init(&memory);
    assert(umi_helix_memory_put(&memory, "build", "passed") == UMI_STATUS_OK);
    assert(strcmp(umi_helix_memory_get(&memory, "build"), "passed") == 0);
    return 0;
}
