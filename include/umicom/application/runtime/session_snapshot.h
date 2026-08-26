/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/runtime/session_snapshot.h
 *
 * PURPOSE:
 *   Project session state into a bounded persistence-friendly snapshot without owning Data Server storage.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_RUNTIME_SESSION_SNAPSHOT_H
#define UMICOM_APPLICATION_RUNTIME_SESSION_SNAPSHOT_H

#include "umicom/application/runtime/session.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationSessionSnapshot {
    uint32_t structure_size;
    char application_id[UMI_APPLICATION_RUNTIME_TEXT_CAPACITY];
    char layout_id[UMI_APPLICATION_RUNTIME_TEXT_CAPACITY];
    char active_panel_ids[UMI_APPLICATION_RUNTIME_MAX_PANELS]
                         [UMI_APPLICATION_RUNTIME_TEXT_CAPACITY];
    size_t active_panel_count;
    bool layout_locked;
    uint64_t revision;
} UmiApplicationSessionSnapshot;

UmiStatus umi_application_session_snapshot_capture(
    const UmiApplicationSession *session,
    UmiApplicationSessionSnapshot *out_snapshot);
UmiStatus umi_application_session_snapshot_restore(
    const UmiApplicationExperienceDefinition *experience,
    const UmiApplicationSessionSnapshot *snapshot,
    UmiApplicationSession *out_session);

#ifdef __cplusplus
}
#endif

#endif
