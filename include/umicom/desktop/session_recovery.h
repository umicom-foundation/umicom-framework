/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/session_recovery.h
 *
 * PURPOSE:
 *   Define portable desktop recovery journals. Framework owns checkpoints,
 *   integrity verification and restoration; applications only provide stable
 *   session identities and request lifecycle checkpoints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * List the named desktop session recovery state values accepted by this public contract.
 */
typedef enum UmiDesktopSessionRecoveryState {
    UMI_DESKTOP_SESSION_RECOVERY_IDLE = 1,
    UMI_DESKTOP_SESSION_RECOVERY_RUNNING = 2,
    UMI_DESKTOP_SESSION_RECOVERY_AVAILABLE = 3,
    UMI_DESKTOP_SESSION_RECOVERY_RECOVERED = 4,
    UMI_DESKTOP_SESSION_RECOVERY_CLEAN = 5
} UmiDesktopSessionRecoveryState;

/**
 * Represent the desktop recovery checkpoint snapshot data shared with callers of this
 * public contract.
 */
typedef struct UmiDesktopRecoveryCheckpointSnapshot {
    uint32_t structure_size;
    uint64_t sequence;
    uint64_t checksum;
    size_t encoded_length;
    char checkpoint_id[UMI_DESKTOP_ID_CAPACITY];
    char reason[UMI_DESKTOP_TITLE_CAPACITY];
    uint64_t revision;
} UmiDesktopRecoveryCheckpointSnapshot;

/**
 * Represent the desktop session recovery snapshot data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the desktop session recovery data shared with callers of this public contract.
 */
typedef struct UmiDesktopSessionRecovery UmiDesktopSessionRecovery;

/**
 * Initialise desktop session recovery from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_desktop_session_recovery_create(
    UmiDesktopRuntime *runtime,
    UmiDesktopSessionRecovery **out_recovery);
/**
 * Release or reset state held by desktop session recovery so the same storage can be
 * reused safely.
 */
void umi_desktop_session_recovery_destroy(
    UmiDesktopSessionRecovery *recovery);
/**
 * Provide the desktop session recovery begin operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_session_recovery_begin(
    UmiDesktopSessionRecovery *recovery,
    const char *session_id,
    const char *application_id);
/**
 * Provide the desktop session recovery checkpoint operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_session_recovery_checkpoint(
    UmiDesktopSessionRecovery *recovery,
    const char *checkpoint_id,
    const char *reason);
/**
 * Provide the desktop session recovery mark clean operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_session_recovery_mark_clean(
    UmiDesktopSessionRecovery *recovery);
/**
 * Provide the desktop session recovery restore operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_session_recovery_restore(
    UmiDesktopSessionRecovery *recovery,
    const char *checkpoint_id);
/**
 * Provide the desktop session recovery export latest operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_session_recovery_export_latest(
    const UmiDesktopSessionRecovery *recovery,
    char *out_text,
    size_t capacity,
    size_t *out_length);
/**
 * Provide the desktop session recovery import operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_session_recovery_import(
    UmiDesktopSessionRecovery *recovery,
    const char *text);
/**
 * Find desktop session recovery checkpoint while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_desktop_session_recovery_checkpoint_at(
    const UmiDesktopSessionRecovery *recovery,
    size_t index,
    UmiDesktopRecoveryCheckpointSnapshot *out_checkpoint);
/**
 * Provide the desktop session recovery snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_session_recovery_snapshot(
    const UmiDesktopSessionRecovery *recovery,
    UmiDesktopSessionRecoverySnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif

#endif
