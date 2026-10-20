/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/path_identity.h
 * PURPOSE: Compare local file identity independently of Windows separator spelling.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_PATH_IDENTITY_H
#define UMICOM_TEST_PATH_IDENTITY_H
#include <gio/gio.h>
#include <stdio.h>
/* Choosers and library history canonicalise paths. The assertion still rejects
 * a different file, relative drafts, missing values and invalid UTF-8; it does
 * not open the file or require a save target to exist. */
static inline gboolean UmiTestSameLocalPath(const char *actual, const char *expected)
{
    if (actual == NULL || expected == NULL || !g_path_is_absolute(actual) ||
        !g_path_is_absolute(expected) || !g_utf8_validate(actual, -1, NULL) ||
        !g_utf8_validate(expected, -1, NULL))
        return FALSE;
    GFile *left = g_file_new_for_path(actual);
    GFile *right = g_file_new_for_path(expected);
    gboolean same = g_file_equal(left, right);
    if (!same) (void)fprintf(stderr, "Local path mismatch: actual=[%s], expected=[%s]\n", actual, expected);
    g_object_unref(left);
    g_object_unref(right);
    return same;
}
#endif
