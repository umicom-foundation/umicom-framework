/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/order_fixture.h
 * PURPOSE: Build invented broker frames independently from the production order decoder.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_IBKR_ORDER_FIXTURE_H
#define UMICOM_TEST_IBKR_ORDER_FIXTURE_H
#include "observation_fixture.h"
/* The extension fields are deliberately opaque fixture evidence. These tests
 * qualify the documented prefix/raw-field contract, not a full Order decoder. */
static int OpenFeed(Fixture *f, const char *order, const char *client, const char *permanent, size_t changed,
                    const char *value)
{
    const char *fields[] = {"5",
                            order,
                            "123",
                            "WORKSHOP",
                            "STK",
                            "",
                            "0",
                            "",
                            "",
                            "SMART",
                            "GBP",
                            "WORKSHOP",
                            "NMS",
                            "BUY",
                            "7000",
                            "LMT",
                            "3.25",
                            "",
                            "DAY",
                            "",
                            "DU123",
                            "O",
                            "0",
                            "review",
                            client,
                            permanent,
                            "0",
                            "0",
                            "opaque-extension",
                            ""};
    if (changed < sizeof fields / sizeof fields[0])
        fields[changed] = value;
    return Feed(f, fields, sizeof fields / sizeof fields[0]);
}
static int StatusFeed(Fixture *f, const char *order, const char *client, const char *permanent,
                      const char *state, const char *filled, const char *remaining)
{
    const char *fields[] = {"3",       order, state,  filled, remaining, "3.25",
                            permanent, "0",   "3.25", client, "",        "0"};
    return Feed(f, fields, sizeof fields / sizeof fields[0]);
}
/* Reuse explicit fixture references without suppressing compiler diagnostics
 * for product code. Each executable exercises a different public contract. */
static void OrderFixtureReferences(void)
{
    (void)PositionFeed;
    (void)OpenFeed;
    (void)StatusFeed;
    (void)ObserveWire;
}
#endif
