/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_clock.c
 *---------------------------------------------------------------------------*/
#include "umicom/platform/clock.h"

#include <assert.h>
#include <stddef.h>

int main(void)
{
    UmiClock fake = umi_clock_fake(100U);
    UmiClock system = umi_clock_system();
    uint64_t before;
    uint64_t after;

    assert(fake.state != NULL);
    assert(fake.monotonic_nanoseconds(&fake) == 100U);
    assert(umi_clock_fake_advance(&fake, 50U) == UMI_STATUS_OK);
    assert(fake.monotonic_nanoseconds(&fake) == 150U);
    assert(umi_clock_fake_set(&fake, 900U) == UMI_STATUS_OK);
    assert(fake.wall_nanoseconds(&fake) == 900U);
    umi_clock_dispose(&fake);

    before = system.monotonic_nanoseconds(&system);
    after = system.monotonic_nanoseconds(&system);
    assert(after >= before);
    umi_clock_dispose(&system);
    return 0;
}
