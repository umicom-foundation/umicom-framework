/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/implementation_fingerprint/fixture/independent.c
 * PURPOSE: Provide an object unrelated to the changed private header.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* A source without the owner's private include must keep its existing object
 * when the owner changes. The CMake regression checks this object's timestamp. */
int umi_fixture_independent_value(void)
{
    return 7;
}
