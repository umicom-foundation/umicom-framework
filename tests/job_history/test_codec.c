/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/job_history/test_codec.c
 * PURPOSE: Exercise strict persisted-job decoding without accepting malformed outcomes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../../src/data/job_history_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                          \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    UmiJobHistoryEntry entry = {0}, decoded = {0}, untouched;
    entry.id = 1;
    entry.revision = 1;
    entry.state = UMI_JOB_HISTORY_PREPARED;
    entry.total_steps = 3;
    strcpy(entry.kind, "build.workflow");
    strcpy(entry.label, "Build | caf\xc3\xa9");
    char wire[UMI_JOB_HISTORY_WIRE_CAPACITY];
    CHECK(UmiJobHistoryEncode(&entry, wire, sizeof(wire)) == UMI_STATUS_OK);
    if (strcmp(argv[1], "roundtrip") == 0)
    {
        CHECK(UmiJobHistoryDecode(wire, &decoded) == UMI_STATUS_OK);
        CHECK(decoded.id == 1 && decoded.total_steps == 3 && strcmp(decoded.label, entry.label) == 0);
        entry.id = UINT64_MAX;
        entry.revision = UINT64_MAX;
        CHECK(UmiJobHistoryEncode(&entry, wire, sizeof(wire)) == UMI_STATUS_OK);
        CHECK(UmiJobHistoryDecode(wire, &decoded) == UMI_STATUS_OK && decoded.id == UINT64_MAX);
    }
    else if (strcmp(argv[1], "bounded") == 0)
    {
        memset(entry.label, 'x', sizeof(entry.label) - 1U);
        entry.label[sizeof(entry.label) - 1U] = '\0';
        memset(entry.kind, 'a', sizeof(entry.kind) - 1U);
        entry.kind[sizeof(entry.kind) - 1U] = '\0';
        CHECK(UmiJobHistoryEncode(&entry, wire, sizeof(wire)) == UMI_STATUS_OK);
        CHECK(UmiJobHistoryDecode(wire, &decoded) == UMI_STATUS_OK &&
              strcmp(entry.label, decoded.label) == 0);
        char small[4] = "old";
        CHECK(UmiJobHistoryEncode(&entry, small, sizeof(small)) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(strcmp(small, "old") == 0);
        memset(entry.kind, 'a', sizeof(entry.kind));
        CHECK(UmiJobHistoryEncode(&entry, wire, sizeof(wire)) == UMI_STATUS_INVALID_ARGUMENT);
    }
    else
    {
        const char *bad = NULL;
        if (strcmp(argv[1], "format") == 0)
            bad = "2|1|1|1|0|3|0|61|62";
        else if (strcmp(argv[1], "overflow") == 0)
            bad = "1|18446744073709551616|1|1|0|3|0|61|62";
        else if (strcmp(argv[1], "negative") == 0)
            bad = "1|-1|1|1|0|3|0|61|62";
        else if (strcmp(argv[1], "truncated") == 0)
            bad = "1|1|1|1|0|3|0|61|6";
        else if (strcmp(argv[1], "hex") == 0)
            bad = "1|1|1|1|0|3|0|61|zz";
        else if (strcmp(argv[1], "nul") == 0)
            bad = "1|1|1|1|0|3|0|61|620063";
        else if (strcmp(argv[1], "control") == 0)
            bad = "1|1|1|1|0|3|0|61|0a";
        else if (strcmp(argv[1], "identifier") == 0)
            bad = "1|1|1|1|0|3|0|2f|62";
        else if (strcmp(argv[1], "success-incomplete") == 0)
            bad = "1|1|1|3|1|3|0|61|62";
        else if (strcmp(argv[1], "failure-ok") == 0)
            bad = "1|1|1|4|1|3|0|61|62";
        else if (strcmp(argv[1], "cancel-status") == 0)
            bad = "1|1|1|5|1|3|8|61|62";
        else if (strcmp(argv[1], "zero-identity") == 0)
            bad = "1|0|1|1|0|3|0|61|62";
        else if (strcmp(argv[1], "zero-revision") == 0)
            bad = "1|1|0|1|0|3|0|61|62";
        else if (strcmp(argv[1], "invalid-state") == 0)
            bad = "1|1|1|9|0|3|0|61|62";
        else if (strcmp(argv[1], "trailing") == 0)
            bad = "1|1|1|1|0|3|0|61|62|";
        else
            return 2;
        memset(&decoded, 0x5a, sizeof(decoded));
        memcpy(&untouched, &decoded, sizeof(decoded));
        CHECK(UmiJobHistoryDecode(bad, &decoded) == UMI_STATUS_PARSE_ERROR);
        CHECK(memcmp(&decoded, &untouched, sizeof(decoded)) == 0);
    }
    return 0;
}
