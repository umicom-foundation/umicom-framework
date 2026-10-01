/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/implementation_fingerprint/fixture/main.c
 * PURPOSE: Demand the new function only after the library source has been replaced.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

int umi_fixture_retained_value(void);
int umi_fixture_checked_value(void);

int main(void)
{
    /* The first build can link against the old owner. Later configurations
     * require the new public function and the current private-header value. */
#if EXPECTED_VALUE == 0
    return umi_fixture_retained_value() == 12 ? 0 : 1;
#else
    return umi_fixture_checked_value() == EXPECTED_VALUE ? 0 : 1;
#endif
}
