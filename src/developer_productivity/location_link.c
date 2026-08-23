/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/location_link.c
 *
 * PURPOSE:
 *   Parse portable compiler/terminal source location links.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/location_link.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

UmiStatus umi_developer_location_link_parse(
    const char *text,
    UmiDeveloperProductivityLocation *out_location,
    int *out_matched)
{
    char path[UMI_DEVELOPER_PRODUCTIVITY_PATH_CAPACITY];
    unsigned long line_number = 0UL;
    unsigned long column_number = 0UL;
    int parsed;

    if (text == NULL || out_location == NULL || out_matched == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_matched = 0;
    (void)memset(out_location, 0, sizeof(*out_location));

    if (isalpha((unsigned char)text[0]) && text[1] == ':') {
        char drive = '\0';
        char tail[UMI_DEVELOPER_PRODUCTIVITY_PATH_CAPACITY - 2U];

        parsed = sscanf(
            text,
            "%c:%2045[^:]:%lu:%lu",
            &drive,
            tail,
            &line_number,
            &column_number);

        if (parsed >= 3) {
            (void)snprintf(path, sizeof(path), "%c:%s", drive, tail);
            parsed -= 1;
        }
    } else {
        parsed = sscanf(
            text,
            "%2047[^:]:%lu:%lu",
            path,
            &line_number,
            &column_number);
    }

    if (parsed < 2) {
        return UMI_STATUS_OK;
    }

    (void)snprintf(
        out_location->uri,
        sizeof(out_location->uri),
        "%s",
        path);
    out_location->line = (size_t)line_number;
    out_location->column = parsed >= 3 ? (size_t)column_number : 0U;
    *out_matched = 1;
    return UMI_STATUS_OK;
}
