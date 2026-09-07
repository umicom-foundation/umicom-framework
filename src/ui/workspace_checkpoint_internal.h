/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workspace_checkpoint_internal.h
 * PURPOSE: Share strict checkpoint validation and storage evidence between
 * active-layout and named-library services without another public policy.
 * AUTHOR: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_WORKSPACE_CHECKPOINT_INTERNAL_H
#define UMICOM_UI_WORKSPACE_CHECKPOINT_INTERNAL_H
#include "umicom/ui/workspace_checkpoint.h"
#define UMI_UI_CHECKPOINT_AGGREGATE_CAPACITY 160U

/* Private bridges retain the established active-checkpoint implementation. */
UmiStatus umi_ui_checkpoint_internal_library_keys(
    const UmiUiWorkspaceCheckpointScope *scope, char *primary, char *backup);
void umi_ui_checkpoint_internal_report_init(
    UmiDataServer *server, UmiUiWorkspaceCheckpointReport *report);
UmiStatus umi_ui_checkpoint_internal_validate_host(
    const UmiUiWorkspaceCustomisation *model);
UmiStatus umi_ui_checkpoint_internal_validate_record(
    const UmiUiWorkspaceCheckpointScope *scope,
    const UmiUiWorkspaceCustomisation *model,
    const UmiUiLayoutPersistenceRecord *record);
UmiStatus umi_ui_checkpoint_internal_read_revision(
    UmiDataServer *server, const char *primary,
    UmiUiWorkspaceCheckpointReport *report);
UmiStatus umi_ui_checkpoint_internal_finish_transaction(
    UmiDataServer *server, UmiStatus status);
#endif
