/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/observation_fixture.h
 * PURPOSE: Compare complete transmitted packets using an independently supplied field sequence.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_OBSERVATION_FIXTURE_H
#define UMICOM_TEST_OBSERVATION_FIXTURE_H
#include "fixture.h"
/* Check framing, terminators and the absence of extra fields. This fixture
 * compares observed bytes; it does not call the production packet encoder. */
static int ObserveWire(const Fixture *f, size_t offset, const char *const *expected, size_t count)
{
    CHECK(offset <= f->outSize && f->outSize - offset >= 4U);
    const unsigned char *wire = f->output + offset;
    size_t bytes = f->outSize - offset;
    size_t length = ((size_t)wire[0] << 24U) | ((size_t)wire[1] << 16U) | ((size_t)wire[2] << 8U) | wire[3];
    CHECK(length == bytes - 4U);
    size_t at = 4U;
    for (size_t i = 0U; i < count; ++i)
    {
        CHECK(at < bytes && memchr(wire + at, 0, bytes - at) != NULL);
        CHECK(strcmp((const char *)wire + at, expected[i]) == 0);
        at += strlen(expected[i]) + 1U;
    }
    CHECK(at == bytes);
    return 0;
}
#endif
