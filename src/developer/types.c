/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/types.c
 *
 * PURPOSE:
 *   Provide stable diagnostic text for developer-runtime operation, state and
 *   event enumerations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer/types.h"

const char *umi_developer_operation_kind_text(UmiDeveloperOperationKind kind)
{
    switch (kind) {
        case UMI_DEVELOPER_OPERATION_CUSTOM: return "custom";
        case UMI_DEVELOPER_OPERATION_CONFIGURE: return "configure";
        case UMI_DEVELOPER_OPERATION_BUILD: return "build";
        case UMI_DEVELOPER_OPERATION_TEST: return "test";
        case UMI_DEVELOPER_OPERATION_RUN: return "run";
        case UMI_DEVELOPER_OPERATION_DEBUG: return "debug";
        case UMI_DEVELOPER_OPERATION_SOURCE_CONTROL: return "source-control";
        case UMI_DEVELOPER_OPERATION_PACKAGE: return "package";
        case UMI_DEVELOPER_OPERATION_DEPLOY: return "deploy";
        case UMI_DEVELOPER_OPERATION_AI: return "ai";
        default: return "unknown";
    }
}

const char *umi_developer_operation_state_text(UmiDeveloperOperationState state)
{
    switch (state) {
        case UMI_DEVELOPER_OPERATION_QUEUED: return "queued";
        case UMI_DEVELOPER_OPERATION_RUNNING: return "running";
        case UMI_DEVELOPER_OPERATION_SUCCEEDED: return "succeeded";
        case UMI_DEVELOPER_OPERATION_FAILED: return "failed";
        case UMI_DEVELOPER_OPERATION_CANCELLED: return "cancelled";
        case UMI_DEVELOPER_OPERATION_BLOCKED: return "blocked";
        default: return "unknown";
    }
}

const char *umi_developer_event_kind_text(UmiDeveloperEventKind kind)
{
    switch (kind) {
        case UMI_DEVELOPER_EVENT_SUBMITTED: return "submitted";
        case UMI_DEVELOPER_EVENT_DEPENDENCY_ADDED: return "dependency-added";
        case UMI_DEVELOPER_EVENT_STARTED: return "started";
        case UMI_DEVELOPER_EVENT_PROGRESS: return "progress";
        case UMI_DEVELOPER_EVENT_SUCCEEDED: return "succeeded";
        case UMI_DEVELOPER_EVENT_FAILED: return "failed";
        case UMI_DEVELOPER_EVENT_CANCELLED: return "cancelled";
        case UMI_DEVELOPER_EVENT_RETRIED: return "retried";
        case UMI_DEVELOPER_EVENT_CONTEXT_CHANGED: return "context-changed";
        default: return "unknown";
    }
}
