/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ai_authorengine.c
 *
 * PURPOSE:
 *   Validate the reusable AI and Author Engine configuration foundation.
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
    UmiAiAuthorEngineConfig config = {0};
    (void)strcpy(config.executable, "uaengine");
    (void)strcpy(config.workspace, ".");
    (void)strcpy(config.provider, "local");
    assert(umi_ai_authorengine_validate(&config) == UMI_STATUS_OK);
    return 0;
}
