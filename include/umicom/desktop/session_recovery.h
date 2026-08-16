/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/session_recovery.h
 *
 * PURPOSE:
 *   Define portable desktop recovery journals. Framework owns checkpoints,
 *   integrity verification and restoration; applications only provide stable
 *   session identities and request lifecycle checkpoints.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESKTOP_SESSION_RECOVERY_H
#define UMICOM_DESKTOP_SESSION_RECOVERY_H

#include "umicom/desktop/persistence.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESKTOP_RECOVERY_CHECKPOINT_MAX 8U
#define UMI_DESKTOP_RECOVERY_JOURNAL_TEXT_CAPACITY \
    (UMI_DESKTOP_PERSISTENCE_TEXT_CAPACITY + 2048U)

typedef enum UmiDesktopSessionRecoveryState {
    UMI_DESKTOP_SESSION_RECOVERY_IDLE = 1,
    UMI_DESKTOP_SESSION_RECOVERY_RUNNING = 2,
    UMI_DESKTOP_SESSION_RECOVERY_AVAILABLE = 3,
    UMI_DESKTOP_SESSION_RECOVERY_RECOVERED = 4,
    UMI_DESKTOP_SESSION_RECOVERY_CLEAN = 5
} UmiDesktopSessionRecoveryState;

typedef struct UmiDesktopRecoveryCheckpointSnapshot {
    uint32_t structure_size;
    uint64_t sequence;
    uint64_t checksum;
    size_t encoded_length;
    char checkpoint_id[UMI_DESKTOP_ID_CAPACITY];
    char reason[UMI_DESKTOP_TITLE_CAPACITY];
    uint64_t revision;
} UmiDesktopRecoveryCheckpointSnapshot;

typedef struct UmiDesktopSessionRecoverySnapshot {
    uint32_t structure_size;
    char session_id[UMI_DESKTOP_ID_CAPACITY];
    char application_id[UMI_DESKTOP_ID_CAPACITY];
    char latest_checkpoint_id[UMI_DESKTOP_ID_CAPACITY];
    size_t checkpoint_count;
    UmiDesktopSessionRecoveryState state;
    bool clean_shutdown;
    bool recovery_available;
    uint64_t last_sequence;
    uint64_t revision;
} UmiDesktopSessionRecoverySnapshot;

typedef struct UmiDesktopSessionRecovery UmiDesktopSessionRecovery;

UmiStatus umi_desktop_session_recovery_create(
    UmiDesktopRuntime *runtime,
    UmiDesktopSessionRecovery **out_recovery);
void umi_desktop_session_recovery_destroy(
    UmiDesktopSessionRecovery *recovery);
UmiStatus umi_desktop_session_recovery_begin(
    UmiDesktopSessionRecovery *recovery,
    const char *session_id,
    const char *application_id);
UmiStatus umi_desktop_session_recovery_checkpoint(
    UmiDesktopSessionRecovery *recovery,
    const char *checkpoint_id,
    const char *reason);
UmiStatus umi_desktop_session_recovery_mark_clean(
    UmiDesktopSessionRecovery *recovery);
UmiStatus umi_desktop_session_recovery_restore(
    UmiDesktopSessionRecovery *recovery,
    const char *checkpoint_id);
UmiStatus umi_desktop_session_recovery_export_latest(
    const UmiDesktopSessionRecovery *recovery,
    char *out_text,
    size_t capacity,
    size_t *out_length);
UmiStatus umi_desktop_session_recovery_import(
    UmiDesktopSessionRecovery *recovery,
    const char *text);
UmiStatus umi_desktop_session_recovery_checkpoint_at(
    const UmiDesktopSessionRecovery *recovery,
    size_t index,
    UmiDesktopRecoveryCheckpointSnapshot *out_checkpoint);
UmiStatus umi_desktop_session_recovery_snapshot(
    const UmiDesktopSessionRecovery *recovery,
    UmiDesktopSessionRecoverySnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif

#endif
