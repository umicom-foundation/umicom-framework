/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/education/archive_fixture.c
 * PURPOSE: Compile one real C23 object for the native ar inspector CLI test.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include <stdint.h>
uint32_t UmiArchiveFixtureSum(uint32_t first, uint32_t second)
{
    return first + second;
}
