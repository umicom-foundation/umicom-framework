/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_call_catalogue.c
 * PURPOSE: Check complete call metadata, caller URI ownership, source coordinates and atomic failure boundaries.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/call_catalogue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(c))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "%d: %s\n", __LINE__, #c);                                                       \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
typedef struct Case
{
    const char *name, *json;
    int direction;
    UmiStatus wanted;
    size_t count, sites;
} Case;
static const Case cases[] = {
    {"item-owned",
     "[{\"name\":\"callee\",\"kind\":12,\"uri\":\"file:///"
     "target.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":8}},"
     "\"selectionRange\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":3}},"
     "\"data\":{\"token\":[1,\"opaque\"]},\"extension\":true}]",
     0, UMI_STATUS_OK, 1U, 0U},
    {"item-null", "null", 0, UMI_STATUS_OK, 0U, 0U},
    {"item-empty", "[]", 0, UMI_STATUS_OK, 0U, 0U},
    {"item-multiple",
     "[{\"name\":\"callee\",\"kind\":12,\"uri\":\"file:///"
     "target.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":8}},"
     "\"selectionRange\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":3}},"
     "\"data\":{\"token\":[1,\"opaque\"]},\"extension\":true},{\"name\":\"callee\",\"kind\":12,\"uri\":"
     "\"file:///"
     "target.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":8}},"
     "\"selectionRange\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":3}},"
     "\"data\":{\"token\":[1,\"opaque\"]},\"extension\":true}]",
     0, UMI_STATUS_OK, 2U, 0U},
    {"item-unicode",
     "[{\"name\":\"f\\ud83d\\ude00\",\"kind\":12,\"uri\":\"file:///"
     "target.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":8}},"
     "\"selectionRange\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":3}},"
     "\"data\":{\"token\":[1,\"opaque\"]},\"extension\":true}]",
     0, UMI_STATUS_OK, 1U, 0U},
    {"item-detail",
     "[{\"name\":\"callee\",\"kind\":12,\"uri\":\"file:///"
     "target.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":8}},"
     "\"selectionRange\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":3}},"
     "\"data\":{\"token\":[1,\"opaque\"]},\"extension\":true,\"detail\":\"line\\ninfo\"}]",
     0, UMI_STATUS_OK, 1U, 0U},
    {"item-deprecated",
     "[{\"name\":\"callee\",\"kind\":12,\"uri\":\"file:///"
     "target.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":8}},"
     "\"selectionRange\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":3}},"
     "\"data\":{\"token\":[1,\"opaque\"]},\"extension\":true,\"tags\":[1,22]}]",
     0, UMI_STATUS_OK, 1U, 0U},
    {"item-unknown-kind",
     "[{\"name\":\"callee\",\"kind\":789,\"uri\":\"file:///"
     "target.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":8}},"
     "\"selectionRange\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":3}},"
     "\"data\":{\"token\":[1,\"opaque\"]},\"extension\":true}]",
     0, UMI_STATUS_OK, 1U, 0U},
    {"item-data-null",
     "[{\"name\":\"callee\",\"kind\":12,\"uri\":\"file:///"
     "target.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":8}},"
     "\"selectionRange\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":3}},"
     "\"data\":null,\"extension\":true}]",
     0, UMI_STATUS_OK, 1U, 0U},
    {"item-name-empty",
     "[{\"name\":\"\",\"kind\":12,\"uri\":\"file:///"
     "target.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":8}},"
     "\"selectionRange\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":3}},"
     "\"data\":{\"token\":[1,\"opaque\"]},\"extension\":true}]",
     0, UMI_STATUS_PARSE_ERROR, 1U, 0U},
    {"item-name-whitespace",
     "[{\"name\":\"\\u2003\\t\",\"kind\":12,\"uri\":\"file:///"
     "target.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":8}},"
     "\"selectionRange\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":3}},"
     "\"data\":{\"token\":[1,\"opaque\"]},\"extension\":true}]",
     0, UMI_STATUS_PARSE_ERROR, 1U, 0U},
    {"item-kind-zero",
     "[{\"name\":\"callee\",\"kind\":0,\"uri\":\"file:///"
     "target.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":8}},"
     "\"selectionRange\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":3}},"
     "\"data\":{\"token\":[1,\"opaque\"]},\"extension\":true}]",
     0, UMI_STATUS_PARSE_ERROR, 1U, 0U},
    {"item-kind-negative",
     "[{\"name\":\"callee\",\"kind\":-1,\"uri\":\"file:///"
     "target.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":8}},"
     "\"selectionRange\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":3}},"
     "\"data\":{\"token\":[1,\"opaque\"]},\"extension\":true}]",
     0, UMI_STATUS_PARSE_ERROR, 1U, 0U},
    {"item-kind-fraction",
     "[{\"name\":\"callee\",\"kind\":2.1,\"uri\":\"file:///"
     "target.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":8}},"
     "\"selectionRange\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":3}},"
     "\"data\":{\"token\":[1,\"opaque\"]},\"extension\":true}]",
     0, UMI_STATUS_PARSE_ERROR, 1U, 0U},
    {"item-kind-large",
     "[{\"name\":\"callee\",\"kind\":2147483648,\"uri\":\"file:///"
     "target.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":8}},"
     "\"selectionRange\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":3}},"
     "\"data\":{\"token\":[1,\"opaque\"]},\"extension\":true}]",
     0, UMI_STATUS_PARSE_ERROR, 1U, 0U},
    {"item-name-number",
     "[{\"name\":42,\"kind\":12,\"uri\":\"file:///"
     "target.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":8}},"
     "\"selectionRange\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":3}},"
     "\"data\":{\"token\":[1,\"opaque\"]},\"extension\":true}]",
     0, UMI_STATUS_PARSE_ERROR, 1U, 0U},
    {"item-detail-null",
     "[{\"name\":\"callee\",\"kind\":12,\"uri\":\"file:///"
     "target.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":8}},"
     "\"selectionRange\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":3}},"
     "\"data\":{\"token\":[1,\"opaque\"]},\"extension\":true,\"detail\":null}]",
     0, UMI_STATUS_PARSE_ERROR, 1U, 0U},
    {"item-uri-relative",
     "[{\"name\":\"callee\",\"kind\":12,\"uri\":\"file.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},"
     "\"end\":{\"line\":0,\"character\":8}},\"selectionRange\":{\"start\":{\"line\":0,\"character\":1},"
     "\"end\":{\"line\":0,\"character\":3}},\"data\":{\"token\":[1,\"opaque\"]},\"extension\":true}]",
     0, UMI_STATUS_PARSE_ERROR, 1U, 0U},
    {"item-uri-escape",
     "[{\"name\":\"callee\",\"kind\":12,\"uri\":\"file:///"
     "bad%QZ.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":8}},"
     "\"selectionRange\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":3}},"
     "\"data\":{\"token\":[1,\"opaque\"]},\"extension\":true}]",
     0, UMI_STATUS_PARSE_ERROR, 1U, 0U},
    {"item-tags-number",
     "[{\"name\":\"callee\",\"kind\":12,\"uri\":\"file:///"
     "target.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":8}},"
     "\"selectionRange\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":3}},"
     "\"data\":{\"token\":[1,\"opaque\"]},\"extension\":true,\"tags\":2}]",
     0, UMI_STATUS_PARSE_ERROR, 1U, 0U},
    {"item-tag-zero",
     "[{\"name\":\"callee\",\"kind\":12,\"uri\":\"file:///"
     "target.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":8}},"
     "\"selectionRange\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":3}},"
     "\"data\":{\"token\":[1,\"opaque\"]},\"extension\":true,\"tags\":[0]}]",
     0, UMI_STATUS_PARSE_ERROR, 1U, 0U},
    {"item-tag-string",
     "[{\"name\":\"callee\",\"kind\":12,\"uri\":\"file:///"
     "target.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":8}},"
     "\"selectionRange\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":3}},"
     "\"data\":{\"token\":[1,\"opaque\"]},\"extension\":true,\"tags\":[\"1\"]}]",
     0, UMI_STATUS_PARSE_ERROR, 1U, 0U},
    {"item-reversed",
     "[{\"name\":\"callee\",\"kind\":12,\"uri\":\"file:///"
     "target.c\",\"range\":{\"start\":{\"line\":0,\"character\":8},\"end\":{\"line\":0,\"character\":0}},"
     "\"selectionRange\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":3}},"
     "\"data\":{\"token\":[1,\"opaque\"]},\"extension\":true}]",
     0, UMI_STATUS_PARSE_ERROR, 1U, 0U},
    {"item-selection-outside",
     "[{\"name\":\"callee\",\"kind\":12,\"uri\":\"file:///"
     "target.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":8}},"
     "\"selectionRange\":{\"start\":{\"line\":0,\"character\":2},\"end\":{\"line\":0,\"character\":9}},"
     "\"data\":{\"token\":[1,\"opaque\"]},\"extension\":true}]",
     0, UMI_STATUS_PARSE_ERROR, 1U, 0U},
    {"item-negative-coordinate",
     "[{\"name\":\"callee\",\"kind\":12,\"uri\":\"file:///"
     "target.c\",\"range\":{\"start\":{\"line\":0,\"character\":-1},\"end\":{\"line\":0,\"character\":9}},"
     "\"selectionRange\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":3}},"
     "\"data\":{\"token\":[1,\"opaque\"]},\"extension\":true}]",
     0, UMI_STATUS_PARSE_ERROR, 1U, 0U},
    {"item-large-coordinate",
     "[{\"name\":\"callee\",\"kind\":12,\"uri\":\"file:///"
     "target.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":"
     "2147483648}},\"selectionRange\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,"
     "\"character\":3}},\"data\":{\"token\":[1,\"opaque\"]},\"extension\":true}]",
     0, UMI_STATUS_PARSE_ERROR, 1U, 0U},
    {"item-missing-name",
     "[{\"kind\":12,\"uri\":\"file:///"
     "target.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":8}},"
     "\"selectionRange\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":3}},"
     "\"data\":{\"token\":[1,\"opaque\"]},\"extension\":true}]",
     0, UMI_STATUS_PARSE_ERROR, 1U, 0U},
    {"item-missing-kind",
     "[{\"name\":\"callee\",\"uri\":\"file:///"
     "target.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":8}},"
     "\"selectionRange\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":3}},"
     "\"data\":{\"token\":[1,\"opaque\"]},\"extension\":true}]",
     0, UMI_STATUS_PARSE_ERROR, 1U, 0U},
    {"item-missing-uri",
     "[{\"name\":\"callee\",\"kind\":12,\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":"
     "0,\"character\":8}},\"selectionRange\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,"
     "\"character\":3}},\"data\":{\"token\":[1,\"opaque\"]},\"extension\":true}]",
     0, UMI_STATUS_PARSE_ERROR, 1U, 0U},
    {"item-missing-range",
     "[{\"name\":\"callee\",\"kind\":12,\"uri\":\"file:///"
     "target.c\",\"selectionRange\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,"
     "\"character\":3}},\"data\":{\"token\":[1,\"opaque\"]},\"extension\":true}]",
     0, UMI_STATUS_PARSE_ERROR, 1U, 0U},
    {"item-missing-selectionRange",
     "[{\"name\":\"callee\",\"kind\":12,\"uri\":\"file:///"
     "target.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":8}},"
     "\"data\":{\"token\":[1,\"opaque\"]},\"extension\":true}]",
     0, UMI_STATUS_PARSE_ERROR, 1U, 0U},
    {"item-object",
     "{\"name\":\"callee\",\"kind\":12,\"uri\":\"file:///"
     "target.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":8}},"
     "\"selectionRange\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":3}},"
     "\"data\":{\"token\":[1,\"opaque\"]},\"extension\":true}",
     0, UMI_STATUS_PARSE_ERROR, 1U, 0U},
    {"item-null-item", "[null]", 0, UMI_STATUS_PARSE_ERROR, 1U, 0U},
    {"item-atomic-second",
     "[{\"name\":\"callee\",\"kind\":12,\"uri\":\"file:///"
     "target.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":8}},"
     "\"selectionRange\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":3}},"
     "\"data\":{\"token\":[1,\"opaque\"]},\"extension\":true},{}]",
     0, UMI_STATUS_PARSE_ERROR, 1U, 0U},
    {"item-duplicate-name",
     "[{\"name\":\"callee\",\"kind\":12,\"uri\":\"file:///"
     "target.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":8}},"
     "\"selectionRange\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":3}},"
     "\"data\":{\"token\":[1,\"opaque\"]},\"extension\":true,\"name\":\"other\"}]",
     0, UMI_STATUS_ALREADY_EXISTS, 1U, 0U},

    {"edge-incoming",
     "[{\"from\":{\"name\":\"callee\",\"kind\":12,\"uri\":\"file:///"
     "target.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":8}},"
     "\"selectionRange\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":3}},"
     "\"data\":{\"token\":[1,\"opaque\"]},\"extension\":true},\"fromRanges\":[{\"start\":{\"line\":0,"
     "\"character\":1},\"end\":{\"line\":0,\"character\":2}},{\"start\":{\"line\":0,\"character\":4},\"end\":"
     "{\"line\":0,\"character\":6}}]}]",
     1, UMI_STATUS_OK, 1U, 2U},
    {"edge-outgoing",
     "[{\"to\":{\"name\":\"callee\",\"kind\":12,\"uri\":\"file:///"
     "target.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":8}},"
     "\"selectionRange\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":3}},"
     "\"data\":{\"token\":[1,\"opaque\"]},\"extension\":true},\"fromRanges\":[{\"start\":{\"line\":0,"
     "\"character\":1},\"end\":{\"line\":0,\"character\":2}},{\"start\":{\"line\":0,\"character\":4},\"end\":"
     "{\"line\":0,\"character\":6}}]}]",
     2, UMI_STATUS_OK, 1U, 2U},
    {"edge-empty", "[]", 1, UMI_STATUS_OK, 0U, 0U},
    {"edge-null", "null", 1, UMI_STATUS_OK, 0U, 0U},
    {"edge-multiple",
     "[{\"from\":{\"name\":\"callee\",\"kind\":12,\"uri\":\"file:///"
     "target.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":8}},"
     "\"selectionRange\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":3}},"
     "\"data\":{\"token\":[1,\"opaque\"]},\"extension\":true},\"fromRanges\":[{\"start\":{\"line\":0,"
     "\"character\":1},\"end\":{\"line\":0,\"character\":2}},{\"start\":{\"line\":0,\"character\":4},\"end\":"
     "{\"line\":0,\"character\":6}}]},{\"from\":{\"name\":\"callee\",\"kind\":12,\"uri\":\"file:///"
     "target.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":8}},"
     "\"selectionRange\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":3}},"
     "\"data\":{\"token\":[1,\"opaque\"]},\"extension\":true},\"fromRanges\":[{\"start\":{\"line\":0,"
     "\"character\":1},\"end\":{\"line\":0,\"character\":2}},{\"start\":{\"line\":0,\"character\":4},\"end\":"
     "{\"line\":0,\"character\":6}}]}]",
     1, UMI_STATUS_OK, 2U, 2U},
    {"edge-no-sites",
     "[{\"from\":{\"name\":\"callee\",\"kind\":12,\"uri\":\"file:///"
     "target.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":8}},"
     "\"selectionRange\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":3}},"
     "\"data\":{\"token\":[1,\"opaque\"]},\"extension\":true},\"fromRanges\":[]}]",
     1, UMI_STATUS_OK, 1U, 0U},
    {"edge-repeated-sites",
     "[{\"from\":{\"name\":\"callee\",\"kind\":12,\"uri\":\"file:///"
     "target.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":8}},"
     "\"selectionRange\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":3}},"
     "\"data\":{\"token\":[1,\"opaque\"]},\"extension\":true},\"fromRanges\":[{\"start\":{\"line\":0,"
     "\"character\":1},\"end\":{\"line\":0,\"character\":2}},{\"start\":{\"line\":0,\"character\":1},\"end\":"
     "{\"line\":0,\"character\":2}}]}]",
     1, UMI_STATUS_OK, 1U, 2U},
    {"edge-empty-site",
     "[{\"from\":{\"name\":\"callee\",\"kind\":12,\"uri\":\"file:///"
     "target.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":8}},"
     "\"selectionRange\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":3}},"
     "\"data\":{\"token\":[1,\"opaque\"]},\"extension\":true},\"fromRanges\":[{\"start\":{\"line\":0,"
     "\"character\":2},\"end\":{\"line\":0,\"character\":2}}]}]",
     1, UMI_STATUS_OK, 1U, 1U},
    {"edge-wrong-direction",
     "[{\"from\":{\"name\":\"callee\",\"kind\":12,\"uri\":\"file:///"
     "target.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":8}},"
     "\"selectionRange\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":3}},"
     "\"data\":{\"token\":[1,\"opaque\"]},\"extension\":true},\"fromRanges\":[{\"start\":{\"line\":0,"
     "\"character\":1},\"end\":{\"line\":0,\"character\":2}},{\"start\":{\"line\":0,\"character\":4},\"end\":"
     "{\"line\":0,\"character\":6}}]}]",
     2, UMI_STATUS_PARSE_ERROR, 1U, 2U},
    {"edge-null-edge", "[null]", 2, UMI_STATUS_PARSE_ERROR, 1U, 2U},
    {"edge-missing-item", "[{\"fromRanges\":[]}]", 2, UMI_STATUS_PARSE_ERROR, 1U, 2U},
    {"edge-missing-sites",
     "[{\"to\":{\"name\":\"callee\",\"kind\":12,\"uri\":\"file:///"
     "target.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":8}},"
     "\"selectionRange\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":3}},"
     "\"data\":{\"token\":[1,\"opaque\"]},\"extension\":true}}]",
     2, UMI_STATUS_PARSE_ERROR, 1U, 2U},
    {"edge-null-sites",
     "[{\"to\":{\"name\":\"callee\",\"kind\":12,\"uri\":\"file:///"
     "target.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":8}},"
     "\"selectionRange\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":3}},"
     "\"data\":{\"token\":[1,\"opaque\"]},\"extension\":true},\"fromRanges\":null}]",
     2, UMI_STATUS_PARSE_ERROR, 1U, 2U},
    {"edge-bad-site",
     "[{\"to\":{\"name\":\"callee\",\"kind\":12,\"uri\":\"file:///"
     "target.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":8}},"
     "\"selectionRange\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":3}},"
     "\"data\":{\"token\":[1,\"opaque\"]},\"extension\":true},\"fromRanges\":[{\"start\":{\"line\":0,"
     "\"character\":3},\"end\":{\"line\":0,\"character\":1}}]}]",
     2, UMI_STATUS_PARSE_ERROR, 1U, 2U},
    {"edge-bad-late-site",
     "[{\"to\":{\"name\":\"callee\",\"kind\":12,\"uri\":\"file:///"
     "target.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":8}},"
     "\"selectionRange\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":3}},"
     "\"data\":{\"token\":[1,\"opaque\"]},\"extension\":true},\"fromRanges\":[{\"start\":{\"line\":0,"
     "\"character\":0},\"end\":{\"line\":0,\"character\":3}},{}]}]",
     2, UMI_STATUS_PARSE_ERROR, 1U, 2U},
    {"edge-bad-related", "[{\"to\":{},\"fromRanges\":[]}]", 2, UMI_STATUS_PARSE_ERROR, 1U, 2U},
    {"edge-atomic-second",
     "[{\"to\":{\"name\":\"callee\",\"kind\":12,\"uri\":\"file:///"
     "target.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":8}},"
     "\"selectionRange\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":3}},"
     "\"data\":{\"token\":[1,\"opaque\"]},\"extension\":true},\"fromRanges\":[{\"start\":{\"line\":0,"
     "\"character\":1},\"end\":{\"line\":0,\"character\":2}},{\"start\":{\"line\":0,\"character\":4},\"end\":"
     "{\"line\":0,\"character\":6}}]},{}]",
     2, UMI_STATUS_PARSE_ERROR, 1U, 2U},
    {"edge-object",
     "{\"to\":{\"name\":\"callee\",\"kind\":12,\"uri\":\"file:///"
     "target.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":8}},"
     "\"selectionRange\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":3}},"
     "\"data\":{\"token\":[1,\"opaque\"]},\"extension\":true},\"fromRanges\":[{\"start\":{\"line\":0,"
     "\"character\":1},\"end\":{\"line\":0,\"character\":2}},{\"start\":{\"line\":0,\"character\":4},\"end\":"
     "{\"line\":0,\"character\":6}}]}",
     2, UMI_STATUS_PARSE_ERROR, 1U, 2U},
    {"edge-duplicate-sites",
     "[{\"from\":{\"name\":\"callee\",\"kind\":12,\"uri\":\"file:///"
     "target.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":8}},"
     "\"selectionRange\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":3}},"
     "\"data\":{\"token\":[1,\"opaque\"]},\"extension\":true},\"fromRanges\":[],\"fromRanges\":[]}]",
     1, UMI_STATUS_ALREADY_EXISTS, 1U, 0U}};
#define ITEM                                                                                                 \
    "{\"name\":\"callee\",\"kind\":12,\"uri\":\"file:///"                                                    \
    "target.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":8}},"    \
    "\"selectionRange\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":3}},"      \
    "\"data\":{\"token\":[1,\"opaque\"]},\"extension\":true}"
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    UmiLanguageCallItemCatalogue *roots = NULL;
    UmiLanguageCallEdgeCatalogue *edges = NULL;
    char root_uri[] = "file:///caller.c";
    UmiLanguageCallItem root = {.location = {.uri = root_uri}};
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(mode, cases[i].name) == 0)
        {
            const Case *test = &cases[i];
            UmiStatus status;
            size_t length = strlen(test->json);
            char *owned = malloc(length + 1U);
            CHECK(owned != NULL);
            memcpy(owned, test->json, length + 1U);
            if (test->direction == 0)
                status = UmiLanguageCallItemCatalogueCreate(owned, length, NULL, &roots);
            else
            {
                char *response = malloc(length + 64U);
                CHECK(response != NULL);
                (void)snprintf(response, length + 64U, "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":%s}", owned);
                status = UmiLanguageCallEdgeCatalogueReadResponse(response, strlen(response), 7U, &root,
                                                                  (UmiLanguageCallDirection)test->direction,
                                                                  NULL, &edges);
                root_uri[0] = 'X'; /* The edge catalogue owns its independent caller URI. */
                memset(response, 'x', strlen(response));
                free(response);
            }
            memset(owned, 'x', length);
            free(owned);
            CHECK(status == test->wanted);
            if (status == UMI_STATUS_OK && test->direction == 0)
            {
                CHECK(roots != NULL && UmiLanguageCallItemCatalogueCount(roots) == test->count);
                if (test->count != 0U)
                {
                    UmiLanguageCallItem value;
                    const char *json = NULL;
                    size_t bytes = 0U;
                    CHECK(UmiLanguageCallItemCatalogueAt(roots, 0U, &value) == UMI_STATUS_OK);
                    CHECK(UmiLanguageCallItemCatalogueItemJson(roots, 0U, &json, &bytes) == UMI_STATUS_OK);
                    CHECK(json[0] == '{' && bytes != 0U && value.name[0] != '\0');
                    CHECK(strcmp(value.location.uri, "file:///target.c") == 0 && value.has_data);
                    if (strcmp(mode, "item-owned") == 0)
                        CHECK(strstr(json, "opaque") != NULL && strstr(json, "extension") != NULL);
                    if (strcmp(mode, "item-deprecated") == 0)
                        CHECK(value.deprecated);
                    if (strcmp(mode, "item-unknown-kind") == 0)
                        CHECK(value.kind == 789);
                    if (strcmp(mode, "item-unicode") == 0)
                        CHECK(strcmp(value.name, "f\xf0\x9f\x98\x80") == 0);
                    if (strcmp(mode, "item-detail") == 0)
                        CHECK(strcmp(value.detail, "line\ninfo") == 0);
                }
            }
            else if (status == UMI_STATUS_OK)
            {
                CHECK(edges != NULL && UmiLanguageCallEdgeCatalogueCount(edges) == test->count);
                if (test->count != 0U)
                {
                    UmiLanguageCallEdge edge;
                    UmiLanguageSourceLocation location;
                    CHECK(UmiLanguageCallEdgeCatalogueAt(edges, 0U, &edge) == UMI_STATUS_OK &&
                          edge.call_sites == test->sites);
                    CHECK(UmiLanguageCallEdgeCatalogueLocation(edges, 0U, 0U, &location) == UMI_STATUS_OK);
                    CHECK(strcmp(location.uri, "file:///target.c") == 0 &&
                          location.selection.start.utf16_column == 1U);
                    for (size_t j = 1U; j <= edge.call_sites; ++j)
                    {
                        CHECK(UmiLanguageCallEdgeCatalogueLocation(edges, 0U, j, &location) == UMI_STATUS_OK);
                        CHECK(strcmp(location.uri,
                                     test->direction == 1 ? "file:///target.c" : "file:///caller.c") == 0);
                    }
                    CHECK(UmiLanguageCallEdgeCatalogueLocation(edges, 0U, edge.call_sites + 1U, &location) ==
                          UMI_STATUS_NOT_FOUND);
                }
            }
            else
                CHECK(roots == NULL && edges == NULL);
            UmiLanguageCallItemCatalogueDestroy(roots);
            UmiLanguageCallEdgeCatalogueDestroy(edges);
            return 0;
        }
    if (strcmp(mode, "item-long-name") == 0)
    {
        char json[4608];
        const char *prefix = "[{\"name\":\"";
        size_t used = strlen(prefix);
        memcpy(json, prefix, used);
        memset(json + used, 'x', 4097U);
        used += 4097U;
        const char *tail = "\",\"kind\":12,\"uri\":\"file:///"
                           "target.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                           "\"character\":1}},"
                           "\"selectionRange\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                           "\"character\":1}}}]";
        CHECK(used + strlen(tail) < sizeof(json));
        memcpy(json + used, tail, strlen(tail) + 1U);
        used += strlen(tail);
        CHECK(UmiLanguageCallItemCatalogueCreate(json, used, NULL, &roots) == UMI_STATUS_CAPACITY_EXCEEDED &&
              roots == NULL);
        return 0;
    }
    const char *response = "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":[" ITEM "]}";
    if (strcmp(mode, "cancelled") == 0)
    {
        UmiCancellationToken *cancel = NULL;
        CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
        umi_cancellation_token_request(cancel);
        CHECK(UmiLanguageCallItemCatalogueReadResponse(response, strlen(response), 7U, cancel, &roots) ==
                  UMI_STATUS_CANCELLED &&
              roots == NULL);
        CHECK(UmiLanguageCallEdgeCatalogueReadResponse(response, strlen(response), 7U, &root,
                                                       UMI_LANGUAGE_CALL_OUTGOING, cancel,
                                                       &edges) == UMI_STATUS_CANCELLED &&
              edges == NULL);
        umi_cancellation_token_destroy(cancel);
        return 0;
    }
    if (strcmp(mode, "wrong-id") == 0)
    {
        CHECK(UmiLanguageCallItemCatalogueReadResponse(response, strlen(response), 8U, NULL, &roots) ==
                  UMI_STATUS_NOT_FOUND &&
              roots == NULL);
        return 0;
    }
    if (strcmp(mode, "error") == 0)
    {
        response = "{\"jsonrpc\":\"2.0\",\"id\":7,\"error\":{\"code\":-1,\"message\":\"unavailable\"}}";
        CHECK(UmiLanguageCallItemCatalogueReadResponse(response, strlen(response), 7U, NULL, &roots) ==
                  UMI_STATUS_UNAVAILABLE &&
              roots == NULL);
        return 0;
    }
    if (strcmp(mode, "null-output") == 0)
    {
        CHECK(UmiLanguageCallItemCatalogueCreate("[]", 2U, NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLanguageCallItemCatalogueReadResponse(response, strlen(response), 7U, NULL, NULL) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLanguageCallEdgeCatalogueReadResponse(response, strlen(response), 7U, &root,
                                                       UMI_LANGUAGE_CALL_INCOMING, NULL,
                                                       NULL) == UMI_STATUS_INVALID_ARGUMENT);
        return 0;
    }
    if (strcmp(mode, "null-root") == 0 || strcmp(mode, "null-uri") == 0 || strcmp(mode, "bad-direction") == 0)
    {
        if (strcmp(mode, "null-uri") == 0)
            root.location.uri = NULL;
        CHECK(
            UmiLanguageCallEdgeCatalogueReadResponse(
                response, strlen(response), 7U, strcmp(mode, "null-root") == 0 ? NULL : &root,
                strcmp(mode, "bad-direction") == 0 ? (UmiLanguageCallDirection)9 : UMI_LANGUAGE_CALL_INCOMING,
                NULL, &edges) == UMI_STATUS_INVALID_ARGUMENT &&
            edges == NULL);
        return 0;
    }
    if (strcmp(mode, "invalid-index") == 0)
    {
        CHECK(UmiLanguageCallItemCatalogueReadResponse(response, strlen(response), 7U, NULL, &roots) ==
              UMI_STATUS_OK);
        UmiLanguageCallItem sentinel = {.kind = 987};
        const char *span = "sentinel";
        size_t length = 42U;
        CHECK(UmiLanguageCallItemCatalogueAt(roots, SIZE_MAX, &sentinel) == UMI_STATUS_NOT_FOUND &&
              sentinel.kind == 987);
        CHECK(UmiLanguageCallItemCatalogueItemJson(roots, SIZE_MAX, &span, &length) == UMI_STATUS_NOT_FOUND &&
              length == 42U && strcmp(span, "sentinel") == 0);
        UmiLanguageCallItemCatalogueDestroy(roots);
        return 0;
    }
    const char *capacity_cases[] = {"items-many", "items-over", "sites-many", "sites-over"};
    int known = 0;
    for (size_t i = 0U; i < sizeof(capacity_cases) / sizeof(capacity_cases[0]); ++i)
        if (strcmp(mode, capacity_cases[i]) == 0)
            known = 1;
    CHECK(known);
    int item_case = strncmp(mode, "items-", 6U) == 0;
    CHECK(item_case || strncmp(mode, "sites-", 6U) == 0);
    CHECK(strstr(mode, "many") != NULL || strstr(mode, "over") != NULL);
    size_t count = strstr(mode, "over") != NULL ? (item_case ? 4097U : 16385U) : 1000U;
    const char *element =
        item_case ? ITEM : "{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":0}}";
    size_t capacity = count * (strlen(element) + 1U) + 2048U;
    char *json = malloc(capacity);
    CHECK(json != NULL);
    size_t used = (size_t)snprintf(
        json, capacity,
        item_case ? "[" : "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":[{\"to\":" ITEM ",\"fromRanges\":[");
    for (size_t i = 0U; i < count; ++i)
    {
        if (i != 0U)
            json[used++] = ',';
        memcpy(json + used, element, strlen(element));
        used += strlen(element);
    }
    const char *end = item_case ? "]" : "]}]}";
    memcpy(json + used, end, strlen(end) + 1U);
    used += strlen(end);
    UmiStatus status = item_case ? UmiLanguageCallItemCatalogueCreate(json, used, NULL, &roots)
                                 : UmiLanguageCallEdgeCatalogueReadResponse(
                                       json, used, 7U, &root, UMI_LANGUAGE_CALL_OUTGOING, NULL, &edges);
    /* Parser and JSON size bounds can precede semantic item/site bounds. Both
     * are whole-result refusals; the smaller native budgets are tested apart. */
    if (strstr(mode, "over") != NULL)
        CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED && roots == NULL && edges == NULL);
    else
    {
        CHECK(status == UMI_STATUS_OK);
        if (item_case)
            CHECK(UmiLanguageCallItemCatalogueCount(roots) == 1000U);
        else
        {
            UmiLanguageCallEdge edge;
            CHECK(UmiLanguageCallEdgeCatalogueAt(edges, 0U, &edge) == UMI_STATUS_OK &&
                  edge.call_sites == 1000U);
        }
        UmiLanguageCallItemCatalogueDestroy(roots);
        UmiLanguageCallEdgeCatalogueDestroy(edges);
    }

    free(json);
    return 0;
}
