/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_path_root_containment.c
 * PURPOSE: Verify root-directory containment without admitting sibling-prefix paths.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/path.h"
#include <stdio.h>
#define CHECK(value)                                                                               \
    do                                                                                             \
    {                                                                                              \
        if (!(value))                                                                              \
        {                                                                                          \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value);                            \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
int main(void)
{
    CHECK(umi_path_is_within("/", "/source/main.c"));
    CHECK(umi_path_is_within("/", "/"));
    CHECK(umi_path_is_within("/source", "/source/main.c"));
    CHECK(!umi_path_is_within("/source", "/source-extra/main.c"));
    CHECK(!umi_path_is_within("/source", "/source/../elsewhere"));
    CHECK(umi_path_is_within("C:/", "C:/source/main.c"));
    CHECK(!umi_path_is_within("C:/", "D:/source/main.c"));
    CHECK(!umi_path_is_within("C:/source", "C:/source-extra/main.c"));
    return 0;
}
