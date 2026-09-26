/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/federation/types.h
 *
 * PURPOSE:
 *   Define copied workspace, launch-review and activity values, not a second process catalogue.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESKTOP_FEDERATION_TYPES_H
#define UMICOM_DESKTOP_FEDERATION_TYPES_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_DESK_FEDERATION_MAX_APPS 64U
#define UMI_DESK_FEDERATION_MAX_WORKSPACES 24U
#define UMI_DESK_FEDERATION_MAX_ACTIVITY 64U
#define UMI_DESK_FEDERATION_APP_ID_CAPACITY 160U
#define UMI_DESK_FEDERATION_WORKSPACE_ID_CAPACITY 64U
#define UMI_DESK_FEDERATION_TITLE_CAPACITY 192U
#define UMI_DESK_FEDERATION_LINK_CAPACITY 192U

typedef enum UmiDeskWorkspaceKind {
    UMI_DESK_WORKSPACE_GROUP = 1,
    UMI_DESK_WORKSPACE_SESSION = 2
} UmiDeskWorkspaceKind;
typedef struct UmiDeskFederationAppId {
    char value[UMI_DESK_FEDERATION_APP_ID_CAPACITY];
} UmiDeskFederationAppId;
typedef struct UmiDeskWorkspace {
    UmiDeskWorkspaceKind kind;
    char id[UMI_DESK_FEDERATION_WORKSPACE_ID_CAPACITY];
    char title[UMI_DESK_FEDERATION_TITLE_CAPACITY];
    UmiDeskFederationAppId members[UMI_DESK_FEDERATION_MAX_APPS];
    size_t memberCount;
} UmiDeskWorkspace;

typedef enum UmiDeskFederationAppState {
    UMI_DESK_FEDERATION_UNKNOWN = 0,
    UMI_DESK_FEDERATION_STOPPED = 1,
    UMI_DESK_FEDERATION_STARTING = 2,
    UMI_DESK_FEDERATION_RUNNING = 3,
    UMI_DESK_FEDERATION_ATTENTION = 4,
    UMI_DESK_FEDERATION_FAILED = 5,
    UMI_DESK_FEDERATION_STOPPING = 6
} UmiDeskFederationAppState;
typedef struct UmiDeskFederationApp {
    UmiDeskFederationAppId id;
    char title[UMI_DESK_FEDERATION_TITLE_CAPACITY];
    bool installed, compatible, enabled, running;
    UmiDeskFederationAppState state;
    uint64_t processToken;
    uint64_t generation;
} UmiDeskFederationApp;
typedef struct UmiDeskFederationCatalogue {
    UmiDeskFederationApp apps[UMI_DESK_FEDERATION_MAX_APPS];
    size_t count;
} UmiDeskFederationCatalogue;

typedef enum UmiDeskFederationAction {
    UMI_DESK_FEDERATION_SKIP = 0,
    UMI_DESK_FEDERATION_START = 1,
    UMI_DESK_FEDERATION_ACTIVATE = 2
} UmiDeskFederationAction;
typedef enum UmiDeskFederationReason {
    UMI_DESK_FEDERATION_READY = 0,
    UMI_DESK_FEDERATION_NOT_REGISTERED = 1,
    UMI_DESK_FEDERATION_NOT_INSTALLED = 2,
    UMI_DESK_FEDERATION_INCOMPATIBLE = 3,
    UMI_DESK_FEDERATION_DISABLED = 4,
    UMI_DESK_FEDERATION_TRANSITIONING = 5,
    UMI_DESK_FEDERATION_SELF = 6,
    UMI_DESK_FEDERATION_UNKNOWN_STATE = 7
} UmiDeskFederationReason;
typedef struct UmiDeskFederationPlanItem {
    UmiDeskFederationApp app;
    UmiDeskFederationAction action;
    UmiDeskFederationReason reason;
} UmiDeskFederationPlanItem;
typedef struct UmiDeskFederationPlan {
    uint64_t token;
    uint64_t workspaceRevision;
    UmiDeskFederationPlanItem items[UMI_DESK_FEDERATION_MAX_APPS];
    size_t count;
} UmiDeskFederationPlan;
typedef struct UmiDeskFederationResult {
    UmiDeskFederationAppId id;
    UmiDeskFederationAction action;
    UmiStatus status;
} UmiDeskFederationResult;
typedef struct UmiDeskFederationReport {
    UmiDeskFederationResult results[UMI_DESK_FEDERATION_MAX_APPS];
    size_t count, acceptedCount, failedCount, skippedCount;
} UmiDeskFederationReport;

typedef enum UmiDeskFederationActivityKind {
    UMI_DESK_ACTIVITY_OBSERVED = 1,
    UMI_DESK_ACTIVITY_REQUEST_ACCEPTED = 2,
    UMI_DESK_ACTIVITY_REQUEST_FAILED = 3,
    UMI_DESK_ACTIVITY_SKIPPED = 4
} UmiDeskFederationActivityKind;
typedef struct UmiDeskFederationActivity {
    uint64_t sequence;
    UmiDeskFederationAppId application;
    UmiDeskFederationActivityKind kind;
    UmiDeskFederationAppState state;
    UmiStatus status;
    bool read;
} UmiDeskFederationActivity;
typedef struct UmiDeskFederationSnapshot {
    size_t workspaceCount, groupCount, sessionCount;
    size_t activityCount, unreadCount;
    uint64_t revision, previewToken, droppedActivity;
    bool loaded;
} UmiDeskFederationSnapshot;

const char *UmiDeskFederationReasonText(UmiDeskFederationReason reason);
const char *UmiDeskFederationStateText(UmiDeskFederationAppState state);
const char *UmiDeskFederationActionText(UmiDeskFederationAction action);
#ifdef __cplusplus
}
#endif
#endif
