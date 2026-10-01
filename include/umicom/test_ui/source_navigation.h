/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_ui/source_navigation.h
 * PURPOSE: Open copied test source evidence through the shared document coordinator.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_UI_SOURCE_NAVIGATION_H
#define UMICOM_TEST_UI_SOURCE_NAVIGATION_H
#include "umicom/test_platform/source_links.h"
#include "umicom/test_platform/item.h"
#include "umicom/document/coordinator.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Explicit user navigation only. Local source paths use the existing document
 * coordinator, retaining unsaved drafts. Relative paths require a caller-chosen
 * absolute base; there is no current-directory fallback. No test is launched.
 * Columns are one-based UTF-8 byte offsets, zero means the start of the line.
 * A failed file open leaves the previous editor's cursor alone. If the file
 * opens but its line no longer exists, its tab stays open and NOT_FOUND is
 * returned. Output offsets change only on success. Overlong locations fail
 * rather than being truncated to a different destination. */
UmiStatus UmiTestSourceLinkOpen(UmiDocumentCoordinator *documents,
    const UmiTestSourceLink *link, const char *baseDirectory, size_t *outOffset);
/* Open the discovery source (for CTest this is normally its add_test location).
 * A missing source line opens line one. baseDirectory is explicit, just as for
 * failure locations; working_directory is not silently selected by this API. */
UmiStatus UmiTestItemOpenSource(UmiDocumentCoordinator *documents,
    const UmiTestPlatformItemSnapshot *item, const char *baseDirectory, size_t *outOffset);
#ifdef __cplusplus
}
#endif
#endif
