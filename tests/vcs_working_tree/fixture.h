/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_working_tree/fixture.h
 * PURPOSE: Share exact binary Git fixtures and checks across status regression tests.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_VCS_WORKING_TREE_TEST_FIXTURE_H
#define UMICOM_VCS_WORKING_TREE_TEST_FIXTURE_H
#include "umicom/vcs/working_tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(condition)                                                                           \
    do                                                                                             \
    {                                                                                              \
        if (!(condition))                                                                          \
        {                                                                                          \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition);                        \
            exit(1);                                                                               \
        }                                                                                          \
    } while (0)
#define OBJECT_ID "0123456789012345678901234567890123456789"
#define OBJECT_WIDE "0123456789012345678901234567890123456789012345678901234567890123"
#define HEADERS "# branch.oid " OBJECT_ID "\0# branch.head main\0"
#define ORDINARY "1 M. N... 100644 100644 100644 " OBJECT_ID " " OBJECT_ID " "
#define CHILD "1 .M S.MU 160000 160000 160000 " OBJECT_ID " " OBJECT_ID " "
/* Tests pass explicit byte lengths: strlen would silently omit every record after the first NUL. */
static inline UmiVcsWorkingTree *ParseFixture(const char *bytes, size_t length)
{
    UmiVcsWorkingTree *tree = NULL;
    CHECK(UmiVcsWorkingTreeParse(bytes, length, &tree) == UMI_STATUS_OK);
    CHECK(tree != NULL);
    return tree;
}
#endif
