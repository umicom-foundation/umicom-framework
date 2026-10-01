/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/chart_timeframe/test_time.c
 * PURPOSE: Check fixed UTC labels across epoch, leap days and bounded output buffers.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/chart/timeframe.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); return 1; } } while (0)
int main(int argc, char **argv)
{
    CHECK(argc == 2); const char *name = argv[1], *expected = NULL; int64_t time = 0; int date = 0; size_t cap = 64;
    if (strcmp(name, "epoch") == 0) expected = "1970-01-01 00:00";
    else if (strcmp(name, "negative") == 0) { time = -1; expected = "1969-12-31 23:59"; }
    else if (strcmp(name, "leap") == 0) { time = 951782400000LL; expected = "2000-02-29 00:00"; }
    else if (strcmp(name, "date") == 0) { time = 1709164800000LL; date = 1; expected = "2024-02-29"; }
    else if (strcmp(name, "hour") == 0) { time = 129600000; expected = "1970-01-02 12:00"; }
    else if (strcmp(name, "short") == 0) cap = 16;
    else if (strcmp(name, "bounds") == 0) {
        char text[64]; CHECK(UmiChartTimeframeFormatUtc(INT64_MIN, 0, text, sizeof text) == UMI_STATUS_OK);
        CHECK(UmiChartTimeframeFormatUtc(INT64_MAX, 0, text, sizeof text) == UMI_STATUS_OK); return 0;
    } else if (strcmp(name, "catalogue") == 0) {
        uint32_t intervals[] = {0U,60000U,300000U,900000U,3600000U,14400000U,86400000U};
        for (size_t i=0; i<sizeof intervals/sizeof intervals[0]; ++i) CHECK(UmiChartTimeframeValid(intervals[i]) && strlen(UmiChartTimeframeName(intervals[i])) > 0U);
        CHECK(!UmiChartTimeframeValid(UINT32_MAX)); return 0;
    } else return 2;
    char text[64]; memset(text, '!', sizeof text); char before[64]; memcpy(before, text, sizeof text);
    UmiStatus status = UmiChartTimeframeFormatUtc(time, date, text, cap);
    if (strcmp(name, "short") == 0) CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED && memcmp(text,before,sizeof text)==0);
    else CHECK(status == UMI_STATUS_OK && strcmp(text,expected)==0);
    return 0;
}
