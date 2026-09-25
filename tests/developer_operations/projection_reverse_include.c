/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_operations/projection_reverse_include.c
 *
 * PURPOSE:
 *   Keep the reversed public-header inclusion order covered by the compiler.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_operations/projection.h"
#include "umicom/developer/operation.h"
int UmiTestProjectionReverseIncludeOrder(void);
int UmiTestProjectionReverseIncludeOrder(void)
{
    UmiDeveloperOperationProjectionSnapshot projection = {0};
    UmiDeveloperOperationSnapshot executable = {0};
    executable.state = UMI_DEVELOPER_OPERATION_RUNNING;
    projection.active = 1;
    return executable.state == UMI_DEVELOPER_OPERATION_RUNNING && projection.active == 1 ? 0 : 1;
}
