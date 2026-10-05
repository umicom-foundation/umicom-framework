/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/source_workspace_fixture.h
 * PURPOSE: Share real document owners for dependency capture and complete source-review regressions.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_SOURCE_WORKSPACE_FIXTURE_H
#define UMICOM_TEST_SOURCE_WORKSPACE_FIXTURE_H
#include "replacement_fixture.h"
#include "umicom/document/source_workspace.h"
static inline int SourceFixtureAdd(ReplacementFixture *first, ReplacementFixture *added, const char *name,
                                   const char *text)
{
    *added = *first;
    added->plan = NULL;
    added->second = NULL;
    CHECK(umi_document_coordinator_new(first->documents, name, added->viewId, sizeof(added->viewId)) ==
          UMI_STATUS_OK);
    UmiDocumentWorkingCopySnapshot snapshot;
    CHECK(umi_document_coordinator_active_snapshot(first->documents, &snapshot) == UMI_STATUS_OK);
    added->id = snapshot.document_id;
    CHECK(Draft(added, text) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_sync_active(first->documents) == UMI_STATUS_OK);
    return 0;
}
static inline int SourceFixtureView(ReplacementFixture *fixture, UmiUiDocumentViewSnapshot *out_view)
{
    CHECK(umi_ui_document_view_model_find(umi_ui_workbench_documents(fixture->workbench), fixture->viewId,
                                          out_view) == UMI_STATUS_OK);
    return 0;
}
static inline int SourceFixturePut(ReplacementFixture *fixture, const UmiUiDocumentViewSnapshot *view)
{
    CHECK(umi_ui_document_view_model_upsert(umi_ui_workbench_documents(fixture->workbench), view) ==
          UMI_STATUS_OK);
    return 0;
}
#endif
