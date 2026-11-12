/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/time_in_force.c
 *
 * PURPOSE:
 *   Validate time-in-force values.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of time in force. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/time_in_force.h"
/* Check that time in force satisfies its contract before another service relies on it. */
int umi_time_in_force_valid(UmiTimeInForce tif){return tif>=UMI_TIF_DAY&&tif<=UMI_TIF_FOK;}


/* Enum validity describes a request vocabulary, not a provider capability.
 * These local owners retain resting requests and accept supplied executions;
 * they do not have the matching transaction required by IOC or FOK. */
UmiStatus UmiTimeInForceCheckLocalRetention(UmiTimeInForce tif)
{
    if (!umi_time_in_force_valid(tif)) return UMI_STATUS_INVALID_ARGUMENT;
    if (tif == UMI_TIF_IOC || tif == UMI_TIF_FOK) return UMI_STATUS_NOT_IMPLEMENTED;
    return UMI_STATUS_OK;
}
