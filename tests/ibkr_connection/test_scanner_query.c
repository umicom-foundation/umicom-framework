/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_scanner_query.c
 * PURPOSE: Reject malformed scanner thresholds, dates and filter serialization.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "discovery_fixture.h"
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    DiscoveryReferences();
    (void)New;
    (void)Delete;
    (void)Connect;
    const char *mode = argv[1];
    UmiIbkrScannerQuery q = ScanQuery();
    UmiStatus expected = UMI_STATUS_INVALID_ARGUMENT;
    if (!strcmp(mode, "valid"))
        expected = UMI_STATUS_OK;
    else if (!strcmp(mode, "null"))
    {
        CHECK(UmiIbkrScannerQueryValidate(NULL) == expected);
        return 0;
    }
    else if (!strcmp(mode, "rows-zero"))
        q.numberOfRows = 0U;
    else if (!strcmp(mode, "rows-overflow"))
        q.numberOfRows = 51U;
    else if (!strcmp(mode, "instrument"))
        q.instrument[0] = 0;
    else if (!strcmp(mode, "location"))
        q.locationCode[0] = 0;
    else if (!strcmp(mode, "scan"))
        q.scanCode[0] = 0;
    else if (!strcmp(mode, "unterminated"))
        memset(q.scanCode, 'x', sizeof q.scanCode);
    else if (!strcmp(mode, "control"))
        strcpy(q.instrument, "STK\n");
    else if (!strcmp(mode, "negative-price"))
        strcpy(q.abovePrice, "-1");
    else if (!strcmp(mode, "price-range"))
    {
        strcpy(q.abovePrice, "10");
        strcpy(q.belowPrice, "9");
    }
    else if (!strcmp(mode, "cap-range"))
    {
        strcpy(q.marketCapAbove, "10");
        strcpy(q.marketCapBelow, "9");
    }
    else if (!strcmp(mode, "coupon-range"))
    {
        strcpy(q.couponRateAbove, "10");
        strcpy(q.couponRateBelow, "9");
    }
    else if (!strcmp(mode, "precision"))
        strcpy(q.abovePrice, "0.12345678901");
    else if (!strcmp(mode, "nan"))
        strcpy(q.abovePrice, "NaN");
    else if (!strcmp(mode, "fraction-volume"))
        strcpy(q.aboveVolume, "1.5");
    else if (!strcmp(mode, "volume-overflow"))
        strcpy(q.averageOptionVolumeAbove, "2147483648");
    else if (!strcmp(mode, "leap"))
    {
        strcpy(q.maturityDateAbove, "20280229");
        expected = UMI_STATUS_OK;
    }
    else if (!strcmp(mode, "bad-leap"))
        strcpy(q.maturityDateAbove, "20260229");
    else if (!strcmp(mode, "date-range"))
    {
        strcpy(q.maturityDateAbove, "20271231");
        strcpy(q.maturityDateBelow, "20260101");
    }
    else if (!strcmp(mode, "filter-count"))
        q.filterCount = 17U;
    else if (!strcmp(mode, "filter-injection"))
    {
        q.filterCount = 1U;
        strcpy(q.filters[0].tag, "x;y");
        strcpy(q.filters[0].value, "1");
    }
    else if (!strcmp(mode, "filter-value"))
    {
        q.filterCount = 1U;
        strcpy(q.filters[0].tag, "x");
        strcpy(q.filters[0].value, "a=b");
    }
    else if (!strcmp(mode, "filter-duplicate"))
    {
        q.filterCount = 2U;
        strcpy(q.filters[0].tag, "x");
        strcpy(q.filters[0].value, "1");
        q.filters[1] = q.filters[0];
    }
    else if (!strcmp(mode, "filter-capacity"))
    {
        q.filterCount = 8U;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
        for (size_t i = 0; i < q.filterCount; ++i)
        {
            (void)snprintf(q.filters[i].tag, 64U, "tag%zu", i);
            memset(q.filters[i].value, 'x', 95U);
        }
    }
    else
        return 2;
    CHECK(UmiIbkrScannerQueryValidate(&q) == expected);
    return 0;
}
