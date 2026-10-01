/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/chart_appearance/test_codec.c
 * PURPOSE: Exercise strict appearance encoding, legacy classification and unchanged failure outputs.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2); const char *name = argv[1];
    UmiChartDrawingAppearance input = {0x33, 0xaa, 0xff, 25, 16}, output = {9, 8, 7, 60, 40};
    UmiChartDrawingAppearance before = output;
    char text[64] = "sentinel";
    if (strcmp(name, "encode") == 0) {
        OK(UmiChartDrawingAppearanceEncode(&input, text, sizeof text));
        CHECK(strcmp(text, "umi-drawing:1:33AAFF:25:16") == 0);
        input = (UmiChartDrawingAppearance){0, 255, 1, 80, 0};
        OK(UmiChartDrawingAppearanceEncode(&input, text, sizeof text));
        CHECK(strcmp(text, "umi-drawing:1:00FF01:80:00") == 0);
    } else if (strcmp(name, "decode") == 0) {
        OK(UmiChartDrawingAppearanceDecode("umi-drawing:1:33aAfF:25:16", 64, &output));
        CHECK(output.red == 51 && output.green == 170 && output.blue == 255 && output.width_tenths == 25 && output.fill_percent == 16);
    } else if (strcmp(name, "bounds") == 0) {
        UmiChartDrawingAppearance invalid[] = {{256,0,0,10,0},{0,256,0,10,0},{0,0,256,10,0},{0,0,0,9,0},{0,0,0,81,0},{0,0,0,10,81}};
        for (size_t i = 0; i < sizeof invalid / sizeof invalid[0]; ++i) {
            CHECK(UmiChartDrawingAppearanceEncode(&invalid[i], text, sizeof text) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(strcmp(text, "sentinel") == 0);
        }
        OK(UmiChartDrawingAppearanceFromHex("#000000", 10, 80, &output));
        CHECK(output.width_tenths == 10 && output.fill_percent == 80);
    } else if (strcmp(name, "unsupported") == 0) {
        CHECK(UmiChartDrawingAppearanceDecode("", 1, &output) == UMI_STATUS_NOT_FOUND);
        const char *legacy[] = {"red", "color=#ff0000", "umi-drawing:2:33AAFF:25:16", "umi-drawing:"};
        for (size_t i = 0; i < 4; ++i) {
            CHECK(UmiChartDrawingAppearanceDecode(legacy[i], strlen(legacy[i])+1, &output) == UMI_STATUS_UNAVAILABLE);
            CHECK(memcmp(&output, &before, sizeof output) == 0);
        }
    } else if (strcmp(name, "malformed") == 0) {
        const char *invalid[] = {"umi-drawing:1:", "umi-drawing:1:33AAGF:25:16", "umi-drawing:1:33AAFF:09:16",
            "umi-drawing:1:33AAFF:81:16", "umi-drawing:1:33AAFF:25:81", "umi-drawing:1:33AAFF:+5:16",
            "umi-drawing:1:33AAFF:25:16junk", "umi-drawing:1:33AAFF:2.5:16", "umi-drawing:1:33AAFF;25:16"};
        for (size_t i = 0; i < sizeof invalid / sizeof invalid[0]; ++i) {
            CHECK(UmiChartDrawingAppearanceDecode(invalid[i], strlen(invalid[i])+1, &output) == UMI_STATUS_PARSE_ERROR);
            CHECK(memcmp(&output, &before, sizeof output) == 0);
        }
    } else if (strcmp(name, "capacity") == 0) {
        size_t length = strlen("umi-drawing:1:33AAFF:25:16");
        CHECK(UmiChartDrawingAppearanceEncode(&input, text, length) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(strcmp(text, "sentinel") == 0);
        OK(UmiChartDrawingAppearanceEncode(&input, text, length+1));
        CHECK(UmiChartDrawingAppearanceEncode(&input, NULL, 100) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiChartDrawingAppearanceDecode(text, sizeof text, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(name, "unterminated") == 0) {
        char missing[256]; memset(missing, 'a', sizeof missing);
        CHECK(UmiChartDrawingAppearanceDecode(missing, sizeof missing, &output) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiChartDrawingAppearanceDecode(NULL, 256, &output) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiChartDrawingAppearanceDecode("", 0, &output) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&output, &before, sizeof output) == 0);
    } else if (strcmp(name, "hex") == 0) {
        const char *invalid[] = {"33AAFF", "#33AAFF0", "#GG0000", "#123", "", " #12345"};
        for (size_t i = 0; i < 6; ++i) {
            CHECK(UmiChartDrawingAppearanceFromHex(invalid[i], 25, 16, &output) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(memcmp(&output, &before, sizeof output) == 0);
        }
        OK(UmiChartDrawingAppearanceFromHex("#33aAfF", 25, 16, &output));
        CHECK(output.red == 51 && output.green == 170 && output.blue == 255);
    } else return 2;
    return 0;
}
