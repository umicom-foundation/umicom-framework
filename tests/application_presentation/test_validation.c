/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_presentation/test_validation.c
 *
 * PURPOSE:
 *   Verify the complete application presentation catalogue is unique,
 *   structurally valid and projectable.
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
#include "umicom/application/presentation/presentation.h"

#include <assert.h>

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    assert(umi_application_presentation_catalogues_validate() == UMI_STATUS_OK);
    return 0;
}
