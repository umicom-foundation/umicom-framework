/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sanitizers/value.c
 *
 * PURPOSE:
 *   Provide an instrumented memory access for the static-library consumer link check.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

int UmiSanitizerFixtureValue(const char *text)
{
    return text[0] == 'O' ? 0 : 1;
}
