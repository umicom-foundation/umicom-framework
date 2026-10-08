/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/completed_fixture.h
 * PURPOSE: Provide independent completed-order wire examples and bounded fixture edits.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_COMPLETED_FIXTURE_H
#define UMICOM_TEST_COMPLETED_FIXTURE_H
#include "observation_fixture.h"
/* Names describe the fixture's wire positions, not production decoder offsets.
 * Insert optional sections explicitly at their protocol location. */
enum CompletedField
{
    CF_MESSAGE = 0,
    CF_CONTRACT = 1,
    CF_SYMBOL = 2,
    CF_SECURITY = 3,
    CF_EXPIRY = 4,
    CF_STRIKE = 5,
    CF_RIGHT = 6,
    CF_MULTIPLIER = 7,
    CF_EXCHANGE = 8,
    CF_CURRENCY = 9,
    CF_LOCAL_SYMBOL = 10,
    CF_TRADING_CLASS = 11,
    CF_ACTION = 12,
    CF_QUANTITY = 13,
    CF_TYPE = 14,
    CF_LIMIT = 15,
    CF_AUX = 16,
    CF_TIF = 17,
    CF_OCA = 18,
    CF_ACCOUNT = 19,
    CF_OPEN_CLOSE = 20,
    CF_ORIGIN = 21,
    CF_REFERENCE = 22,
    CF_PERMANENT = 23,
    CF_OUTSIDE = 24,
    CF_HIDDEN = 25,
    CF_DISCRETIONARY = 26,
    CF_GOOD_AFTER = 27,
    CF_FA_GROUP = 28,
    CF_FA_METHOD = 29,
    CF_FA_PERCENT = 30,
    CF_FA_PROFILE = 31,
    CF_MODEL = 32,
    CF_GOOD_TILL = 33,
    CF_RULE = 34,
    CF_PERCENT_OFFSET = 35,
    CF_SETTLING = 36,
    CF_SHORT_SLOT = 37,
    CF_SHORT_LOCATION = 38,
    CF_EXEMPT = 39,
    CF_BOX_START = 40,
    CF_BOX_LOWER = 41,
    CF_BOX_UPPER = 42,
    CF_STOCK_LOWER = 43,
    CF_STOCK_UPPER = 44,
    CF_DISPLAY = 45,
    CF_SWEEP = 46,
    CF_ALL_OR_NONE = 47,
    CF_MINIMUM = 48,
    CF_OCA_TYPE = 49,
    CF_TRIGGER = 50,
    CF_VOLATILITY = 51,
    CF_VOLATILITY_TYPE = 52,
    CF_NEUTRAL_TYPE = 53,
    CF_NEUTRAL_AUX = 54,
    CF_CONTINUOUS = 55,
    CF_REFERENCE_PRICE = 56,
    CF_TRAIL_STOP = 57,
    CF_TRAIL_PERCENT = 58,
    CF_COMBO_DESCRIPTION = 59,
    CF_COMBO_COUNT = 60,
    CF_LEG_PRICE_COUNT = 61,
    CF_SMART_COUNT = 62,
    CF_SCALE_INITIAL = 63,
    CF_SCALE_SUBSEQUENT = 64,
    CF_SCALE_INCREMENT = 65,
    CF_HEDGE = 66,
    CF_CLEARING_ACCOUNT = 67,
    CF_CLEARING_INTENT = 68,
    CF_NOT_HELD = 69,
    CF_NEUTRAL_PRESENT = 70,
    CF_ALGORITHM = 71,
    CF_SOLICITED = 72,
    CF_STATUS = 73,
    CF_RANDOM_SIZE = 74,
    CF_RANDOM_PRICE = 75,
    CF_CONDITIONS = 76,
    CF_STOP = 77,
    CF_OFFSET = 78,
    CF_CASH = 79,
    CF_NO_AUTO_HEDGE = 80,
    CF_OMS = 81,
    CF_AUTO_CANCEL_DATE = 82,
    CF_FILLED = 83,
    CF_REFERENCE_FUTURE = 84,
    CF_AUTO_CANCEL_PARENT = 85,
    CF_SHAREHOLDER = 86,
    CF_IMBALANCE = 87,
    CF_ROUTE_BBO = 88,
    CF_PARENT_PERMANENT = 89,
    CF_COMPLETED_TIME = 90,
    CF_COMPLETED_STATUS = 91,
    CF_MIN_TRADE = 92,
    CF_MIN_COMPETE = 93,
    CF_COMPETE_OFFSET = 94,
    CF_MID_WHOLE = 95,
    CF_MID_HALF = 96,
};
typedef struct CompletedFields
{
    const char *fields[512];
    size_t count;
} CompletedFields;
static CompletedFields CompletedExample(void)
{
    static const char *const sample[] = {
        "101",                   /* message */
        "123",                   /* contract */
        "WORKSHOP",              /* symbol */
        "STK",                   /* security */
        "",                      /* expiry */
        "0",                     /* strike */
        "",                      /* right */
        "",                      /* multiplier */
        "SMART",                 /* exchange */
        "GBP",                   /* currency */
        "WORKSHOP",              /* local symbol */
        "WORKSHOP",              /* trading class */
        "BUY",                   /* action */
        "7000",                  /* quantity */
        "LMT",                   /* type */
        "4.25",                  /* limit */
        "",                      /* aux */
        "DAY",                   /* tif */
        "",                      /* oca */
        "DU123",                 /* account */
        "O",                     /* open close */
        "0",                     /* origin */
        "review order",          /* reference */
        "800",                   /* permanent */
        "0",                     /* outside */
        "0",                     /* hidden */
        "0",                     /* discretionary */
        "",                      /* good after */
        "",                      /* fa group */
        "",                      /* fa method */
        "",                      /* fa percent */
        "",                      /* fa profile */
        "",                      /* model */
        "",                      /* good till */
        "",                      /* rule */
        "",                      /* percent offset */
        "",                      /* settling */
        "0",                     /* short slot */
        "",                      /* short location */
        "-1",                    /* exempt */
        "",                      /* box start */
        "",                      /* box lower */
        "",                      /* box upper */
        "",                      /* stock lower */
        "",                      /* stock upper */
        "0",                     /* display */
        "0",                     /* sweep */
        "1",                     /* all or none */
        "2147483647",            /* minimum */
        "0",                     /* oca type */
        "0",                     /* trigger */
        "",                      /* volatility */
        "0",                     /* volatility type */
        "",                      /* neutral type */
        "",                      /* neutral aux */
        "0",                     /* continuous */
        "0",                     /* reference price */
        "",                      /* trail stop */
        "",                      /* trail percent */
        "",                      /* combo description */
        "0",                     /* combo count */
        "0",                     /* leg price count */
        "0",                     /* smart count */
        "2147483647",            /* scale initial */
        "2147483647",            /* scale subsequent */
        "",                      /* scale increment */
        "",                      /* hedge */
        "",                      /* clearing account */
        "",                      /* clearing intent */
        "0",                     /* not held */
        "0",                     /* neutral present */
        "",                      /* algorithm */
        "0",                     /* solicited */
        "Cancelled",             /* status */
        "0",                     /* random size */
        "0",                     /* random price */
        "0",                     /* conditions */
        "",                      /* stop */
        "",                      /* offset */
        "",                      /* cash */
        "0",                     /* no auto hedge */
        "0",                     /* oms */
        "",                      /* auto cancel date */
        "10",                    /* filled */
        "0",                     /* reference future */
        "0",                     /* auto cancel parent */
        "",                      /* shareholder */
        "0",                     /* imbalance */
        "0",                     /* route bbo */
        "0",                     /* parent permanent */
        "20261007 13:25:10 UTC", /* completed time */
        "Cancelled",             /* completed status */
        "2147483647",            /* min trade */
        "2147483647",            /* min compete */
        "",                      /* compete offset */
        "",                      /* mid whole */
        "",                      /* mid half */
    };
    CompletedFields value = {0};
    memcpy(value.fields, sample, sizeof sample);
    value.count = sizeof sample / sizeof sample[0];
    return value;
}
static int CompletedInsert(CompletedFields *value, size_t at, const char *const *fields, size_t count)
{
    CHECK(at <= value->count && count <= 512U - value->count);
    memmove(value->fields + at + count, value->fields + at, (value->count - at) * sizeof value->fields[0]);
    memcpy(value->fields + at, fields, count * sizeof value->fields[0]);
    value->count += count;
    return 0;
}
static int CompletedFeed(Fixture *f, const CompletedFields *value)
{
    return Feed(f, value->fields, value->count);
}
static void CompletedFixtureReferences(void)
{
    (void)PositionFeed;
    (void)ObserveWire;
    (void)CompletedInsert;
    (void)CompletedFeed;
    (void)CompletedExample;
}
#endif
