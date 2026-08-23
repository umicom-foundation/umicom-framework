/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_productivity/test_bookmarks.c
 *
 * PURPOSE:
 *   Verify reusable source bookmarks.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/developer_productivity/bookmarks.h"

int main(void)
{
    UmiDeveloperBookmarkStore store;
    UmiDeveloperBookmark bookmark;

    umi_developer_bookmark_store_init(&store);
    (void)memset(&bookmark, 0, sizeof(bookmark));
    (void)strcpy(bookmark.bookmark_id, "bookmark.1");
    (void)strcpy(bookmark.label, "Main");
    (void)strcpy(bookmark.location.uri, "src/main.c");
    bookmark.location.line = 10U;

    assert(umi_developer_bookmark_store_upsert(
        &store, &bookmark) == UMI_STATUS_OK);
    assert(store.count == 1U);
    return 0;
}
