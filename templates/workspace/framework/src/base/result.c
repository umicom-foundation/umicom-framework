/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: templates/workspace/framework/src/base/result.c
 *
 * PURPOSE:
 *   Implement the result behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/base/result.h"

/* Provide the result ok operation used by this module and its client applications. */
UmiResult umi_result_ok(void)
{
    UmiResult result = {UMI_STATUS_OK, "OK"};
    return result;
}

/* Provide the result error operation used by this module and its client applications. */
UmiResult umi_result_error(UmiStatus status, const char *message)
{
    UmiResult result = {status, message != 0 ? message : umi_status_text(status)};
    return result;
}

/* Provide the result is ok operation used by this module and its client applications. */
int umi_result_is_ok(UmiResult result)
{
    return result.status == UMI_STATUS_OK;
}
