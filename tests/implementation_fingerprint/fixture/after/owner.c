/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/implementation_fingerprint/fixture/after/owner.c
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

/* A stale archive cannot satisfy a consumer that calls this new entry point. */
int umi_fixture_checked_value(void)
{
    return UMI_FIXTURE_VALUE;
}
