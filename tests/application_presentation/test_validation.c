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
#include "umicom/application/presentation/presentation.h"

#include <assert.h>

int main(void)
{
    assert(umi_application_presentation_catalogues_validate() == UMI_STATUS_OK);
    return 0;
}
