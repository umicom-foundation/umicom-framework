/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_ui/event_calendar_csv.c
 *
 * PURPOSE:
 *   Decode complete calendar snapshots atomically through the shared CSV reader.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "event_calendar_internal.h"
#include "umicom/base/csv_reader.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
typedef struct Field {
    const char *name;
    size_t offset, capacity;
} Field;
#define FIELD(member)                                                                              \
    {#member, offsetof(UmiCalendarEvent, member), sizeof(((UmiCalendarEvent *)0)->member)}
static const Field Fields[] = {
    FIELD(id),     FIELD(kind),     FIELD(date),    FIELD(time),       FIELD(timezone),
    FIELD(timing), FIELD(status),   FIELD(country), FIELD(currency),   FIELD(symbol),
    FIELD(venue),  FIELD(title),    FIELD(period),  FIELD(importance), FIELD(estimate),
    FIELD(actual), FIELD(previous), FIELD(unit),    FIELD(source),     FIELD(observed_at),
    FIELD(quality)};
#undef FIELD
static int Compare(const void *left, const void *right) {
    const UmiCalendarEvent *a = left, *b = right;
    int order = strcmp(a->date, b->date);
    if (order)
        return order;
    /* Session labels have no clock value. Retain them after exact source
     * times rather than inventing UTC instants or exchange opening times. */
    order = strcmp(a->time[0] ? a->time : "~", b->time[0] ? b->time : "~");
    if (order)
        return order;
    order = strcmp(a->source, b->source);
    return order ? order : strcmp(a->id, b->id);
}
UmiStatus UmiEventCalendarReadCsv(const void *bytes, size_t length, UmiEventCalendar **out,
                                  size_t *error_record) {
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (error_record != NULL)
        *error_record = 0U;
    if (length > UMI_EVENT_CALENDAR_MAX_BYTES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiCsvReader *reader = NULL;
    UmiStatus status = UmiCsvReaderCreate(bytes, length, &reader);
    if (status != UMI_STATUS_OK)
        return status;
    UmiEventCalendar *calendar = calloc(1U, sizeof *calendar);
    if (calendar == NULL) {
        UmiCsvReaderDestroy(reader);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    UmiCsvReadRow row = {0};
    size_t record = 1U;
    status = UmiCsvReaderNext(reader, &row);
    if (status == UMI_STATUS_NOT_FOUND)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK) {
        if (row.count != sizeof Fields / sizeof Fields[0])
            status = UMI_STATUS_PARSE_ERROR;
        else
            for (size_t i = 0U; i < row.count; ++i)
                if (strcmp(row.fields[i], Fields[i].name) != 0) {
                    status = UMI_STATUS_PARSE_ERROR;
                    break;
                }
    }
    size_t allocated = 0U;
    while (status == UMI_STATUS_OK) {
        ++record;
        status = UmiCsvReaderNext(reader, &row);
        if (status == UMI_STATUS_NOT_FOUND) {
            status = UMI_STATUS_OK;
            break;
        }
        if (status != UMI_STATUS_OK)
            break;
        if (row.count != sizeof Fields / sizeof Fields[0]) {
            status = UMI_STATUS_PARSE_ERROR;
            break;
        }
        if (calendar->count == UMI_EVENT_CALENDAR_MAX_ROWS) {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            break;
        }
        UmiCalendarEvent event = {0};
        for (size_t i = 0U; i < row.count; ++i) {
            if (row.lengths[i] >= Fields[i].capacity) {
                status = UMI_STATUS_CAPACITY_EXCEEDED;
                break;
            }
            for (size_t j = 0U; j < row.lengths[i]; ++j)
                if ((unsigned char)row.fields[i][j] < 32U ||
                    (unsigned char)row.fields[i][j] == 127U) {
                    status = UMI_STATUS_PARSE_ERROR;
                    break;
                }
            if (status != UMI_STATUS_OK)
                break;
            memcpy((char *)&event + Fields[i].offset, row.fields[i], row.lengths[i] + 1U);
        }
        if (status != UMI_STATUS_OK)
            break;
        if (!UmiEventCalendarEventValid(&event)) {
            status = UMI_STATUS_PARSE_ERROR;
            break;
        }
        for (size_t i = 0U; i < calendar->count; ++i)
            if (strcmp(calendar->rows[i].id, event.id) == 0 &&
                strcmp(calendar->rows[i].source, event.source) == 0) {
                status = UMI_STATUS_ALREADY_EXISTS;
                break;
            }
        if (status != UMI_STATUS_OK)
            break;
        if (calendar->count == allocated) {
            size_t next = allocated ? allocated * 2U : 32U;
            UmiCalendarEvent *rows = realloc(calendar->rows, next * sizeof *rows);
            if (rows == NULL) {
                status = UMI_STATUS_OUT_OF_MEMORY;
                break;
            }
            calendar->rows = rows;
            allocated = next;
        }
        calendar->rows[calendar->count++] = event;
    }
    UmiCsvReaderDestroy(reader);
    if (status != UMI_STATUS_OK) {
        if (error_record != NULL)
            *error_record = record;
        UmiEventCalendarDestroy(calendar);
        return status;
    }
    if (calendar->count > 1U)
        qsort(calendar->rows, calendar->count, sizeof *calendar->rows, Compare);
    *out = calendar;
    return UMI_STATUS_OK;
}
