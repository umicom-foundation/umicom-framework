/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/productisation/session.h
 *
 * PURPOSE:
 *   Join product adoption, the thin application client and runtime snapshots
 *   into one bounded command plane shared by every Umicom application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTISATION_SESSION_H
#define UMICOM_APPLICATION_PRODUCTISATION_SESSION_H

#include "umicom/application/productisation/adoption.h"
#include "umicom/application/runtime/session_snapshot.h"
#include "umicom/application/thin_client.h"

#ifdef __cplusplus
extern "C" {
#endif

/** List the bounded actions accepted by the shared product session. */
typedef enum UmiProductApplicationSessionCommandKind {
    UMI_PRODUCT_SESSION_SELECT_LAYOUT = 1,
    UMI_PRODUCT_SESSION_ACTIVATE_PANEL = 2,
    UMI_PRODUCT_SESSION_DEACTIVATE_PANEL = 3,
    UMI_PRODUCT_SESSION_SET_CONTEXT = 4,
    UMI_PRODUCT_SESSION_SET_LAYOUT_LOCKED = 5,
    UMI_PRODUCT_SESSION_REFRESH_READINESS = 6,
    UMI_PRODUCT_SESSION_SYNCHRONISE_WORKBENCH = 7
} UmiProductApplicationSessionCommandKind;

/** Describe one command without transferring ownership of any text pointer. */
typedef struct UmiProductApplicationSessionCommand {
    uint32_t structure_size;
    UmiProductApplicationSessionCommandKind kind;
    const char *target_id;
    const char *value;
    bool locked;
} UmiProductApplicationSessionCommand;

/**
 * Join a borrowed adoption contribution to one Framework thin-client runtime.
 *
 * Application repos may keep this structure as their control plane, while all
 * layout, context, readiness and workbench behaviour remains in Framework.
 */
typedef struct UmiProductApplicationSession {
    uint32_t structure_size;
    const UmiProductApplicationAdoption *adoption;
    UmiProductApplicationAdoptionSnapshot adoption_snapshot;
    UmiApplicationThinClient client;
    size_t command_count;
    size_t successful_command_count;
    size_t failed_command_count;
    UmiStatus last_status;
    uint64_t revision;
    bool initialised;
} UmiProductApplicationSession;

/** Provide a value-only status view suitable for UI panels and diagnostics. */
typedef struct UmiProductApplicationSessionSnapshot {
    uint32_t structure_size;
    char module_id[UMI_PRODUCTISATION_ID_CAPACITY];
    char application_id[UMI_PRODUCTISATION_ID_CAPACITY];
    char display_name[UMI_PRODUCTISATION_ID_CAPACITY];
    UmiApplicationSessionSnapshot workspace;
    size_t feature_count;
    size_t panel_count;
    size_t layout_count;
    size_t operation_count;
    size_t command_count;
    size_t successful_command_count;
    size_t failed_command_count;
    unsigned readiness_percent;
    UmiStatus last_status;
    uint64_t revision;
    bool workbench_bound;
    bool runnable;
    bool acceptance_ready;
} UmiProductApplicationSessionSnapshot;

/** Initialise a session from a valid contribution and canonical experience. */
UmiStatus umi_product_application_session_init(
    const UmiProductApplicationAdoption *adoption,
    UmiProductApplicationSession *out_session);
/** Attach an existing workbench so session changes can update visible panes. */
UmiStatus umi_product_application_session_bind_workbench(
    UmiProductApplicationSession *session,
    UmiUiWorkbench *workbench);
/** Validate and execute one bounded layout, panel, context or refresh command. */
UmiStatus umi_product_application_session_execute(
    UmiProductApplicationSession *session,
    const UmiProductApplicationSessionCommand *command);
/** Copy current identity, workspace, readiness and command counters to output. */
UmiStatus umi_product_application_session_snapshot(
    const UmiProductApplicationSession *session,
    UmiProductApplicationSessionSnapshot *out_snapshot);
/** Evaluate runtime capability health through the caller's optional probe. */
UmiStatus umi_product_application_session_health(
    const UmiProductApplicationSession *session,
    UmiApplicationCapabilityProbe probe,
    void *user_data,
    UmiApplicationRuntimeHealth *out_health);
/** Rebuild the session from its original contribution and clear runtime counters. */
UmiStatus umi_product_application_session_reset(
    UmiProductApplicationSession *session);

#ifdef __cplusplus
}
#endif

#endif
