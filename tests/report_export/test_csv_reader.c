/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/report_export/test_csv_reader.c
 * PURPOSE: Exercise CSV boundaries, owned input and terminal parsing errors.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/base/csv_reader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(value) do { if (!(value)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); return 1; } } while (0)
static UmiStatus Open(const char *text, UmiCsvReader **out)
{
    return UmiCsvReaderCreate(text, strlen(text), out);
}
/* Invalid input is terminal, and the caller's result stays unchanged. Checking
 * this at a public boundary catches accidental publication of a partial row. */
static int Rejected(const char *text, UmiStatus expected)
{
    UmiCsvReader *reader = NULL;
    CHECK(Open(text, &reader) == UMI_STATUS_OK);
    UmiCsvReadRow row, saved;
    memset(&row, 0xa5, sizeof row);
    memcpy(&saved, &row, sizeof saved);
    CHECK(UmiCsvReaderNext(reader, &row) == expected);
    CHECK(memcmp(&row, &saved, sizeof row) == 0);
    CHECK(UmiCsvReaderNext(reader, &row) == expected);
    UmiCsvReaderDestroy(reader);
    return 0;
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    UmiCsvReader *reader = NULL;
    UmiCsvReadRow row;
    if (!strcmp(mode, "records")) {
        CHECK(Open("name,value\r\none,2\ntwo,3", &reader) == UMI_STATUS_OK);
        CHECK(UmiCsvReaderNext(reader, &row) == UMI_STATUS_OK);
        CHECK(row.record == 1U && row.count == 2U && !strcmp(row.fields[0], "name"));
        CHECK(UmiCsvReaderNext(reader, &row) == UMI_STATUS_OK && row.record == 2U);
        CHECK(!strcmp(row.fields[1], "2"));
        CHECK(UmiCsvReaderNext(reader, &row) == UMI_STATUS_OK && row.record == 3U);
        CHECK(UmiCsvReaderNext(reader, &row) == UMI_STATUS_NOT_FOUND);
    } else if (!strcmp(mode, "quotes")) {
        CHECK(Open("\"a,b\",\"said \"\"yes\"\"\",", &reader) == UMI_STATUS_OK);
        CHECK(UmiCsvReaderNext(reader, &row) == UMI_STATUS_OK && row.count == 3U);
        CHECK(!strcmp(row.fields[0], "a,b") && !strcmp(row.fields[1], "said \"yes\""));
        CHECK(row.lengths[2] == 0U);
    } else if (!strcmp(mode, "multiline")) {
        CHECK(Open("\"one\r\ntwo\nthree\",x\n", &reader) == UMI_STATUS_OK);
        CHECK(UmiCsvReaderNext(reader, &row) == UMI_STATUS_OK && row.record == 1U);
        CHECK(!strcmp(row.fields[0], "one\r\ntwo\nthree"));
        CHECK(UmiCsvReaderNext(reader, &row) == UMI_STATUS_NOT_FOUND);
    } else if (!strcmp(mode, "empty")) {
        CHECK(UmiCsvReaderCreate(NULL, 0U, &reader) == UMI_STATUS_OK);
        CHECK(UmiCsvReaderNext(reader, &row) == UMI_STATUS_NOT_FOUND);
    } else if (!strcmp(mode, "trailing")) {
        CHECK(Open(",\n\n", &reader) == UMI_STATUS_OK);
        CHECK(UmiCsvReaderNext(reader, &row) == UMI_STATUS_OK && row.count == 2U);
        CHECK(row.lengths[0] == 0U && row.lengths[1] == 0U);
        CHECK(UmiCsvReaderNext(reader, &row) == UMI_STATUS_OK && row.count == 1U);
        CHECK(row.lengths[0] == 0U);
        CHECK(UmiCsvReaderNext(reader, &row) == UMI_STATUS_NOT_FOUND);
    } else if (!strcmp(mode, "bom")) {
        CHECK(Open("\xef\xbb\xbf" "caf\xc3\xa9,ok\n", &reader) == UMI_STATUS_OK);
        CHECK(UmiCsvReaderNext(reader, &row) == UMI_STATUS_OK);
        CHECK(!strcmp(row.fields[0], "caf\xc3\xa9") && row.lengths[0] == 5U);
    } else if (!strcmp(mode, "ownership")) {
        char text[] = "retained,=1+1";
        CHECK(Open(text, &reader) == UMI_STATUS_OK);
        memset(text, 'x', sizeof text - 1U);
        CHECK(UmiCsvReaderNext(reader, &row) == UMI_STATUS_OK);
        CHECK(!strcmp(row.fields[0], "retained") && !strcmp(row.fields[1], "=1+1"));
    } else if (!strcmp(mode, "malformed")) {
        const char *bad[] = {"\"open", "\"closed\"junk", "un\"quoted", "a\rb", "\"a\rb\""};
        for (size_t i = 0U; i < sizeof bad / sizeof bad[0]; ++i)
            CHECK(Rejected(bad[i], UMI_STATUS_PARSE_ERROR) == 0);
    } else if (!strcmp(mode, "utf8")) {
        const char *bad[] = {"\xc0\xaf", "\xed\xa0\x80", "\xf4\x90\x80\x80", "\xe2\x82"};
        for (size_t i = 0U; i < sizeof bad / sizeof bad[0]; ++i)
            CHECK(Open(bad[i], &reader) == UMI_STATUS_PARSE_ERROR && reader == NULL);
    } else if (!strcmp(mode, "controls")) {
        const char nul[] = {'a', '\0', 'b'};
        CHECK(UmiCsvReaderCreate(nul, sizeof nul, &reader) == UMI_STATUS_PARSE_ERROR);
        CHECK(Open("\x01", &reader) == UMI_STATUS_PARSE_ERROR);
        CHECK(Open("\x7f", &reader) == UMI_STATUS_PARSE_ERROR);
    } else if (!strcmp(mode, "columns")) {
        char text[65];
        memset(text, ',', 64U); text[64] = '\0';
        CHECK(Rejected(text, UMI_STATUS_CAPACITY_EXCEEDED) == 0);
        text[63] = '\0';
        CHECK(Open(text, &reader) == UMI_STATUS_OK);
        CHECK(UmiCsvReaderNext(reader, &row) == UMI_STATUS_OK && row.count == 64U);
    } else if (!strcmp(mode, "field-limit")) {
        char *text = malloc(UMI_CSV_MAX_TEXT_BYTES + 2U);
        CHECK(text != NULL);
        memset(text, 'a', UMI_CSV_MAX_TEXT_BYTES + 1U);
        text[UMI_CSV_MAX_TEXT_BYTES + 1U] = '\0';
        CHECK(Rejected(text, UMI_STATUS_CAPACITY_EXCEEDED) == 0);
        text[UMI_CSV_MAX_TEXT_BYTES] = '\0';
        CHECK(Open(text, &reader) == UMI_STATUS_OK);
        CHECK(UmiCsvReaderNext(reader, &row) == UMI_STATUS_OK);
        CHECK(row.lengths[0] == UMI_CSV_MAX_TEXT_BYTES);
        free(text);
    } else if (!strcmp(mode, "byte-limit")) {
        CHECK(UmiCsvReaderCreate("x", (size_t)UMI_CSV_MAX_BYTES + 1U, &reader) ==
              UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(reader == NULL);
    } else if (!strcmp(mode, "output")) {
        CHECK(Open("one,two", &reader) == UMI_STATUS_OK);
        CHECK(UmiCsvReaderNext(reader, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCsvReaderNext(reader, &row) == UMI_STATUS_OK);
        UmiCsvReadRow saved;
        memcpy(&saved, &row, sizeof saved);
        CHECK(UmiCsvReaderNext(reader, &row) == UMI_STATUS_NOT_FOUND);
        CHECK(memcmp(&saved, &row, sizeof row) == 0);
    } else if (!strcmp(mode, "invalid")) {
        CHECK(UmiCsvReaderCreate(NULL, 1U, &reader) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCsvReaderCreate("x", 1U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCsvReaderNext(NULL, &row) == UMI_STATUS_INVALID_ARGUMENT);
        UmiCsvReaderDestroy(NULL);
    } else CHECK(0);
    UmiCsvReaderDestroy(reader);
    return 0;
}
