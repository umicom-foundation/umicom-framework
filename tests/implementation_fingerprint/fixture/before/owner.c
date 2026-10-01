/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/implementation_fingerprint/fixture/before/owner.c
 * PURPOSE: Provide a small real archive whose contents change between configurations.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "private.h"

/* This existing entry point stays available when the owner gains another. */
int umi_fixture_retained_value(void)
{
    return 12;
}
