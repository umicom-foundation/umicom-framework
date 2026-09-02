/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/context_channel/test_13_binding.c
 *
 * PURPOSE:
 *   Exercise binding boundaries and stable status behaviour.
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
#include "umicom/context_channel/binding.h"
#include "umicom/context_channel/types.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    char destination[32];
    assert(umi_context_copy_text(destination,sizeof(destination),"context") == UMI_STATUS_OK);
    assert(strcmp(destination,"context") == 0);
    assert(umi_context_text_is_valid(destination,sizeof(destination)));
    assert(umi_context_kind_text(UMI_CONTEXT_KIND_INSTRUMENT) != NULL);
    assert(umi_context_colour_text(UMI_CONTEXT_COLOUR_BLUE) != NULL);
    return 0;
}
