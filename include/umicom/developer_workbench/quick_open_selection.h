/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_workbench/quick_open_selection.h
 * PURPOSE: Check a complete indexed-file choice before a host opens it.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEVELOPER_WORKBENCH_QUICK_OPEN_SELECTION_H
#define UMICOM_DEVELOPER_WORKBENCH_QUICK_OPEN_SELECTION_H
#include "umicom/platform/file_index.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /** Re-read one selected row from the same index revision and query. Unlike a
 * display label, entry contains the complete indexed path. A changed root,
 * revision, row or metadata returns BUSY; nothing is opened or modified.
 * position is relative to page, and query must be the text used to read page
 * with case-insensitive matching. Keep the index alive during this call and
 * preserve its identity between capture and checking. Hosts must separately
 * check that their workspace/owner is still the one that requested the page.
 * This is an index check, not a filesystem lock: open through the document
 * owner afterwards, which handles unavailable files and existing drafts. */
    UmiStatus UmiQuickOpenCheckSelection(const UmiFileIndex *index, const char *query,
                                         const UmiFileIndexPage *page, size_t position,
                                         const UmiFileIndexEntry *entry);
#ifdef __cplusplus
}
#endif
#endif
