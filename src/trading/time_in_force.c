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
int umi_time_in_force_valid(UmiTimeInForce tif){return tif>=UMI_TIF_DAY&&tif<=UMI_TIF_FOK;}
