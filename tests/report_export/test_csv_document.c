/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/report_export/test_csv_document.c
 * PURPOSE: Verify atomic CSV ownership, limits, UTF-8 and spreadsheet text treatment.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/base/csv_document.h"
#include <float.h>
#include <locale.h>
#include <math.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); exit(1); } } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    UmiCsvDocument *doc = NULL;
    OK(UmiCsvDocumentCreate(UMI_CSV_MAX_BYTES, &doc));
    if (strcmp(argv[1], "escaping") == 0) {
        char owned[] = "a,\"b\"\r\nc";
        UmiCsvCell cells[] = {UmiCsvText(owned), UmiCsvText("caf\xc3\xa9 \xf0\x9f\x9a\x80"), UmiCsvTextSpan(NULL, 0)};
        OK(UmiCsvDocumentAppendRow(doc, cells, 3));
        memset(owned, 'x', strlen(owned));
        CHECK(strcmp(UmiCsvDocumentData(doc), "\"a,\"\"b\"\"\r\nc\",\"caf\xc3\xa9 \xf0\x9f\x9a\x80\",\"\"\r\n") == 0);
        CHECK(UmiCsvDocumentRows(doc) == 1);
    } else if (strcmp(argv[1], "formula") == 0) {
        UmiCsvCell cells[] = {UmiCsvText("=SUM(1,2)"), UmiCsvText("  +1"), UmiCsvText("-12"),
            UmiCsvText("@command"), UmiCsvText("\ttext"), UmiCsvText("\xef\xbb\xbf=1"), UmiCsvSigned(-12)};
        OK(UmiCsvDocumentAppendRow(doc, cells, 7));
        CHECK(strcmp(UmiCsvDocumentData(doc), "\"'=SUM(1,2)\",\"'  +1\",\"'-12\",\"'@command\",\"'\ttext\",\"'\xef\xbb\xbf=1\",\"-12\"\r\n") == 0);
    } else if (strcmp(argv[1], "numbers") == 0) {
        UmiCsvCell cells[] = {UmiCsvSigned(INT64_MIN), UmiCsvUnsigned(UINT64_MAX), UmiCsvReal(1.25), UmiCsvReal(-0.5)};
        OK(UmiCsvDocumentAppendRow(doc, cells, 4));
        CHECK(strcmp(UmiCsvDocumentData(doc), "\"-9223372036854775808\",\"18446744073709551615\",\"1.25\",\"-0.5\"\r\n") == 0);
        size_t bytes = UmiCsvDocumentBytes(doc);
        cells[3] = UmiCsvReal(NAN);
        CHECK(UmiCsvDocumentAppendRow(doc, cells, 4) == UMI_STATUS_INVALID_ARGUMENT);
        cells[3] = UmiCsvReal(INFINITY);
        CHECK(UmiCsvDocumentAppendRow(doc, cells, 4) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCsvDocumentBytes(doc) == bytes && UmiCsvDocumentRows(doc) == 1);
    } else if (strcmp(argv[1], "capacity") == 0) {
        UmiCsvDocumentDestroy(doc); OK(UmiCsvDocumentCreate(6, &doc));
        UmiCsvCell cell = UmiCsvText("ab");
        CHECK(UmiCsvDocumentAppendRow(doc, &cell, 1) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiCsvDocumentRows(doc) == 0 && UmiCsvDocumentBytes(doc) == 0);
        cell = UmiCsvText("a"); OK(UmiCsvDocumentAppendRow(doc, &cell, 1));
        CHECK(strcmp(UmiCsvDocumentData(doc), "\"a\"\r\n") == 0);
        CHECK(UmiCsvDocumentAppendRow(doc, &cell, 1) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiCsvDocumentBytes(doc) == 5);
    } else if (strcmp(argv[1], "invalid-text") == 0) {
        static const struct { const char *text; size_t length; } invalid[] = {
            {"a\0b",3}, {"\xc0\xaf",2}, {"\xed\xa0\x80",3}, {"\xf4\x90\x80\x80",4},
            {"\xe2\x82",2}, {"\x80",1}, {"\x1b[31m",5}, {"\x7f",1}, {NULL,1}};
        for (size_t i = 0; i < sizeof(invalid)/sizeof(invalid[0]); ++i) {
            UmiCsvCell cell = UmiCsvTextSpan(invalid[i].text, invalid[i].length);
            CHECK(UmiCsvDocumentAppendRow(doc, &cell, 1) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiCsvDocumentRows(doc) == 0 && UmiCsvDocumentBytes(doc) == 0);
        }
        UmiCsvCell cell = UmiCsvTextSpan("", UMI_CSV_MAX_TEXT_BYTES + 1U);
        CHECK(UmiCsvDocumentAppendRow(doc, &cell, 1) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(argv[1], "copy-alias") == 0) {
        UmiCsvCell cell = UmiCsvText("one"); OK(UmiCsvDocumentAppendRow(doc, &cell, 1));
        cell = UmiCsvTextSpan(UmiCsvDocumentData(doc), UmiCsvDocumentBytes(doc));
        OK(UmiCsvDocumentAppendRow(doc, &cell, 1));
        CHECK(strcmp(UmiCsvDocumentData(doc), "\"one\"\r\n\"\"\"one\"\"\r\n\"\r\n") == 0);
        size_t required = 0; OK(UmiCsvDocumentCopy(doc, NULL, 0, &required));
        char *output = malloc(required); CHECK(output != NULL); memset(output, 'z', required);
        CHECK(UmiCsvDocumentCopy(doc, output, required - 1U, NULL) == UMI_STATUS_CAPACITY_EXCEEDED);
        for (size_t i = 0; i < required; ++i) CHECK(output[i] == 'z');
        OK(UmiCsvDocumentCopy(doc, output, required, NULL));
        UmiCsvDocumentDestroy(doc); doc = NULL;
        CHECK(strlen(output) + 1U == required && strstr(output, "one") != NULL); free(output);
    } else if (strcmp(argv[1], "columns") == 0) {
        UmiCsvCell cells[] = {UmiCsvText("a"), UmiCsvText("b")};
        CHECK(UmiCsvDocumentAppendRow(doc, cells, 0) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCsvDocumentAppendRow(doc, cells, UMI_CSV_MAX_COLUMNS + 1U) == UMI_STATUS_INVALID_ARGUMENT);
        OK(UmiCsvDocumentAppendRow(doc, cells, 2));
        CHECK(UmiCsvDocumentAppendRow(doc, cells, 1) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCsvDocumentRows(doc) == 1);
    } else if (strcmp(argv[1], "locale") == 0) {
        const char *locales[] = {"de_DE.UTF-8", "de_DE.utf8", "German_Germany.1252", "de-DE"};
        int found = 0;
        for (size_t i = 0; i < sizeof(locales)/sizeof(locales[0]); ++i)
            if (setlocale(LC_NUMERIC, locales[i]) != NULL && strcmp(localeconv()->decimal_point, ",") == 0) { found = 1; break; }
        if (!found) { fprintf(stderr, "No comma-decimal locale is installed for this locale-specific case.\n"); UmiCsvDocumentDestroy(doc); return 77; }
        UmiCsvCell cell = UmiCsvReal(1.25); OK(UmiCsvDocumentAppendRow(doc, &cell, 1));
        CHECK(strcmp(UmiCsvDocumentData(doc), "\"1.25\"\r\n") == 0);
    } else {
        CHECK(strcmp(argv[1], "invalid") == 0);
        UmiCsvDocument *bad = doc;
        CHECK(UmiCsvDocumentCreate(0, &bad) == UMI_STATUS_INVALID_ARGUMENT && bad == NULL);
        CHECK(UmiCsvDocumentCreate(UMI_CSV_MAX_BYTES + 1U, &bad) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCsvDocumentCreate(1, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCsvDocumentData(NULL) == NULL && UmiCsvDocumentRows(NULL) == 0);
        CHECK(UmiCsvDocumentCopy(doc, NULL, 1, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        UmiCsvCell cell = UmiCsvText("valid"); cell.kind = (UmiCsvKind)99;
        CHECK(UmiCsvDocumentAppendRow(doc, &cell, 1) == UMI_STATUS_INVALID_ARGUMENT);
    }
    UmiCsvDocumentDestroy(doc); return 0;
}
