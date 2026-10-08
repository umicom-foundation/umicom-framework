/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/realtime_csv_fixture.h
 * PURPOSE: Inspect exported business columns independently from the production CSV writer.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_REALTIME_CSV_FIXTURE_H
#define UMICOM_TEST_REALTIME_CSV_FIXTURE_H
#include "realtime_fixture.h"
/* Read a quoted CSV cell independently of the production writer. Tests check
 * named business columns, including escaping, rather than only searching bytes. */
static int Cell(const UmiCsvDocument *document, size_t wantedRow, size_t wantedColumn, char *out,
                size_t capacity)
{
    const char *p = UmiCsvDocumentData(document);
    size_t row = 0U, column = 0U;
    CHECK(p && capacity);
    while (*p)
    {
        CHECK(*p++ == '"');
        size_t size = 0U;
        bool selected = row == wantedRow && column == wantedColumn;
        bool closed = false;
        while (*p)
        {
            char value = *p++;
            if (value == '"')
            {
                if (*p == '"')
                    ++p;
                else
                {
                    closed = true;
                    break;
                }
            }
            if (selected)
            {
                CHECK(size + 1U < capacity);
                out[size++] = value;
            }
        }
        CHECK(closed);
        if (selected)
        {
            out[size] = '\0';
            return 0;
        }
        if (*p == ',')
        {
            ++p;
            ++column;
        }
        else
        {
            CHECK(p[0] == '\r' && p[1] == '\n');
            p += 2;
            ++row;
            column = 0U;
        }
    }
    return 1;
}

static int NamedCell(const UmiCsvDocument *document, size_t row, const char *name, char *out, size_t capacity)
{
    char heading[128];
    for (size_t column = 0U; column < 40U; ++column)
    {
        CHECK(Cell(document, 0U, column, heading, sizeof heading) == 0);
        if (!strcmp(heading, name))
            return Cell(document, row, column, out, capacity);
    }
    return 1;
}

#endif
