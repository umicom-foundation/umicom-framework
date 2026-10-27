/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sanitizers/main.c
 *
 * PURPOSE:
 *   Link a consumer to an instrumented library without duplicating sanitizer options.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

int UmiSanitizerFixtureValue(const char *text);

int main(void)
{
    return UmiSanitizerFixtureValue("OK");
}
