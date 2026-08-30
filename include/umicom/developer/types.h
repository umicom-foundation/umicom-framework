/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/types.h
 *
 * PURPOSE:
 *   Define the stable, toolkit-neutral types used by the reusable developer
 *   runtime.  These types describe development operations and their lifecycle
 *   without coupling Framework applications to Studio, GTK4, CMake, Git, DAP,
 *   LSP, or any particular process-launch implementation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * A developer operation is one unit of work such as configure, build, test,
 * run, debug, source-control, package, deploy, or an application-defined task.
 * The operation lifecycle is deliberately explicit so Studio and future
 * applications can display, persist, replay and test exactly the same state.
 */
#ifndef UMICOM_DEVELOPER_TYPES_H
#define UMICOM_DEVELOPER_TYPES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_ID_CAPACITY 128U
#define UMI_DEVELOPER_TITLE_CAPACITY 256U
#define UMI_DEVELOPER_PATH_CAPACITY 1024U
#define UMI_DEVELOPER_SUMMARY_CAPACITY 1024U
#define UMI_DEVELOPER_ARGUMENT_CAPACITY 256U
#define UMI_DEVELOPER_MAX_ARGUMENTS 16U

typedef enum UmiDeveloperOperationKind {
    UMI_DEVELOPER_OPERATION_CUSTOM = 0,
    UMI_DEVELOPER_OPERATION_CONFIGURE = 1,
    UMI_DEVELOPER_OPERATION_BUILD = 2,
    UMI_DEVELOPER_OPERATION_TEST = 3,
    UMI_DEVELOPER_OPERATION_RUN = 4,
    UMI_DEVELOPER_OPERATION_DEBUG = 5,
    UMI_DEVELOPER_OPERATION_SOURCE_CONTROL = 6,
    UMI_DEVELOPER_OPERATION_PACKAGE = 7,
    UMI_DEVELOPER_OPERATION_DEPLOY = 8,
    UMI_DEVELOPER_OPERATION_AI = 9
} UmiDeveloperOperationKind;

typedef enum UmiDeveloperOperationState {
    UMI_DEVELOPER_OPERATION_QUEUED = 0,
    UMI_DEVELOPER_OPERATION_RUNNING = 1,
    UMI_DEVELOPER_OPERATION_SUCCEEDED = 2,
    UMI_DEVELOPER_OPERATION_FAILED = 3,
    UMI_DEVELOPER_OPERATION_CANCELLED = 4,
    UMI_DEVELOPER_OPERATION_BLOCKED = 5
} UmiDeveloperOperationState;

typedef enum UmiDeveloperEventKind {
    UMI_DEVELOPER_EVENT_SUBMITTED = 0,
    UMI_DEVELOPER_EVENT_DEPENDENCY_ADDED = 1,
    UMI_DEVELOPER_EVENT_STARTED = 2,
    UMI_DEVELOPER_EVENT_PROGRESS = 3,
    UMI_DEVELOPER_EVENT_SUCCEEDED = 4,
    UMI_DEVELOPER_EVENT_FAILED = 5,
    UMI_DEVELOPER_EVENT_CANCELLED = 6,
    UMI_DEVELOPER_EVENT_RETRIED = 7,
    UMI_DEVELOPER_EVENT_CONTEXT_CHANGED = 8
} UmiDeveloperEventKind;

const char *umi_developer_operation_kind_text(UmiDeveloperOperationKind kind);
const char *umi_developer_operation_state_text(UmiDeveloperOperationState state);
const char *umi_developer_event_kind_text(UmiDeveloperEventKind kind);

#ifdef __cplusplus
}
#endif

#endif
