/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/text_projection/test_projection.c
 * PURPOSE: Check real source identity across filtering, sorting, duplicates and failed changes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/text_projection.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "line %d: %s\n", __LINE__, #x);                                                  \
            failed = 1;                                                                                      \
            goto done;                                                                                       \
        }                                                                                                    \
    } while (0)
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *name = argv[1];
    int failed = 0;
    char first[] = "Zulu", second[] = "alpha";
    const char *rows[] = {first, second, "ALPHA", "beta", "", "caf\xc3\xa9", "CAF\xc3\x89"};
    UmiUiTextProjection *view = NULL;
    UmiUiSortFilterSnapshot filter = {0};
    filter.enabled = 1;
    filter.ascending = 1;
    size_t index = 99U;
    if (!strcmp(name, "empty"))
    {
        CHECK(UmiUiTextProjectionCreate(NULL, 0U, &view) == UMI_STATUS_OK);
        CHECK(UmiUiTextProjectionApply(view, &filter, true) == UMI_STATUS_OK);
        CHECK(UmiUiTextProjectionCount(view) == 0U);
        CHECK(UmiUiTextProjectionToSource(view, 0U, &index) == UMI_STATUS_NOT_FOUND && index == 99U);
        goto done;
    }
    if (!strcmp(name, "row-limit"))
    {
        CHECK(UmiUiTextProjectionCreate(NULL, UMI_UI_TEXT_PROJECTION_ROWS + 1U, &view) ==
              UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(view == NULL);
        goto done;
    }
    if (!strcmp(name, "text-limit"))
    {
        char *text = malloc(UMI_UI_TEXT_PROJECTION_ROW_BYTES + 1U);
        CHECK(text != NULL);
        memset(text, 'x', UMI_UI_TEXT_PROJECTION_ROW_BYTES);
        text[UMI_UI_TEXT_PROJECTION_ROW_BYTES] = '\0';
        const char *input[] = {text};
        UmiStatus status = UmiUiTextProjectionCreate(input, 1U, &view);
        free(text);
        CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED && view == NULL);
        goto done;
    }
    CHECK(UmiUiTextProjectionCreate(rows, 7U, &view) == UMI_STATUS_OK);
    if (!strcmp(name, "owned"))
    {
        strcpy(first, "gone");
        strcpy(second, "moved");
        CHECK(!strcmp(UmiUiTextProjectionText(view, 0U), "Zulu"));
        CHECK(!strcmp(UmiUiTextProjectionText(view, 1U), "alpha"));
    }
    else if (!strcmp(name, "literal"))
    {
        strcpy(filter.query, ".*");
        CHECK(UmiUiTextProjectionApply(view, &filter, false) == UMI_STATUS_OK);
        CHECK(UmiUiTextProjectionCount(view) == 0U);
    }
    else if (!strcmp(name, "filter") || !strcmp(name, "case") || !strcmp(name, "hidden"))
    {
        strcpy(filter.query, "alpha");
        filter.case_sensitive = !strcmp(name, "case");
        CHECK(UmiUiTextProjectionApply(view, &filter, false) == UMI_STATUS_OK);
        CHECK(UmiUiTextProjectionCount(view) == (filter.case_sensitive ? 1U : 2U));
        CHECK(UmiUiTextProjectionToSource(view, 0U, &index) == UMI_STATUS_OK && index == 1U);
        index = 99U;
        CHECK(UmiUiTextProjectionToView(view, 0U, &index) == UMI_STATUS_NOT_FOUND && index == 99U);
    }
    else if (!strcmp(name, "unicode"))
    {
        strcpy(filter.query, "caf\xc3\xa9");
        CHECK(UmiUiTextProjectionApply(view, &filter, false) == UMI_STATUS_OK);
        CHECK(UmiUiTextProjectionCount(view) == 1U);
        CHECK(UmiUiTextProjectionToSource(view, 0U, &index) == UMI_STATUS_OK && index == 5U);
    }
    else if (!strcmp(name, "invalid-retains"))
    {
        memset(filter.query, 'x', sizeof(filter.query));
        CHECK(UmiUiTextProjectionApply(view, &filter, true) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiUiTextProjectionCount(view) == 7U);
        CHECK(UmiUiTextProjectionToSource(view, 0U, &index) == UMI_STATUS_OK && index == 0U);
    }
    else if (!strcmp(name, "disabled"))
    {
        strcpy(filter.query, "absent");
        filter.enabled = 0;
        CHECK(UmiUiTextProjectionApply(view, &filter, false) == UMI_STATUS_OK &&
              UmiUiTextProjectionCount(view) == 7U);
    }
    else if (!strcmp(name, "ascending") || !strcmp(name, "descending") || !strcmp(name, "restore"))
    {
        filter.ascending = strcmp(name, "descending") != 0;
        CHECK(UmiUiTextProjectionApply(view, &filter, true) == UMI_STATUS_OK);
        const size_t up[] = {4U, 1U, 2U, 3U, 6U, 5U, 0U}, down[] = {0U, 5U, 6U, 3U, 1U, 2U, 4U};
        for (size_t i = 0U; i < 7U; ++i)
        {
            CHECK(UmiUiTextProjectionToSource(view, i, &index) == UMI_STATUS_OK);
            CHECK(index == (filter.ascending ? up[i] : down[i]));
        }
        if (!strcmp(name, "restore"))
        {
            CHECK(UmiUiTextProjectionApply(view, &filter, false) == UMI_STATUS_OK);
            for (size_t i = 0U; i < 7U; ++i)
                CHECK(UmiUiTextProjectionToSource(view, i, &index) == UMI_STATUS_OK && index == i);
        }
    }
    else if (!strcmp(name, "invalid-output"))
    {
        CHECK(UmiUiTextProjectionToSource(view, 7U, &index) == UMI_STATUS_NOT_FOUND && index == 99U);
        CHECK(UmiUiTextProjectionToSource(NULL, 0U, &index) == UMI_STATUS_INVALID_ARGUMENT && index == 99U);
        CHECK(UmiUiTextProjectionText(view, 7U) == NULL);
    }
    else
    {
        failed = 2;
    }
done:
    UmiUiTextProjectionDestroy(view);
    return failed;
}
