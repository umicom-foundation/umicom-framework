/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/types.h
 *
 * PURPOSE:
 *   Define bounded public types shared by the inter-application runtime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file keeps one part of the public runtime small and explicit. Product
 * code uses these contracts instead of reaching into another application's
 * private state or private headers.
 */

#ifndef UMICOM_INTEGRATION_TYPES_H
#define UMICOM_INTEGRATION_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define UMI_INTEGRATION_ID_CAPACITY 128U
#define UMI_INTEGRATION_NAME_CAPACITY 192U
#define UMI_INTEGRATION_PATH_CAPACITY 512U
#define UMI_INTEGRATION_TEXT_CAPACITY 256U
#define UMI_INTEGRATION_PAYLOAD_CAPACITY 1024U
#define UMI_INTEGRATION_MAX_APPLICATIONS 128U
#define UMI_INTEGRATION_MAX_CAPABILITIES 64U
#define UMI_INTEGRATION_MAX_DEPENDENCIES 64U
#define UMI_INTEGRATION_MAX_ROUTES 256U
#define UMI_INTEGRATION_MAX_HANDLERS 128U
#define UMI_INTEGRATION_MAX_MEMBERS 64U
#define UMI_INTEGRATION_MAX_EDGES 256U

typedef enum UmiIntegrationApplicationState {
    UMI_INTEGRATION_APP_UNKNOWN = 0,
    UMI_INTEGRATION_APP_DISCOVERED,
    UMI_INTEGRATION_APP_AVAILABLE,
    UMI_INTEGRATION_APP_STARTING,
    UMI_INTEGRATION_APP_RUNNING,
    UMI_INTEGRATION_APP_STOPPING,
    UMI_INTEGRATION_APP_STOPPED,
    UMI_INTEGRATION_APP_FAILED
} UmiIntegrationApplicationState;

typedef enum UmiIntegrationDependencyKind {
    UMI_INTEGRATION_DEPENDENCY_REQUIRED = 0,
    UMI_INTEGRATION_DEPENDENCY_OPTIONAL = 1
} UmiIntegrationDependencyKind;

typedef enum UmiIntegrationMessageKind {
    UMI_INTEGRATION_MESSAGE_COMMAND = 0,
    UMI_INTEGRATION_MESSAGE_EVENT,
    UMI_INTEGRATION_MESSAGE_QUERY,
    UMI_INTEGRATION_MESSAGE_RESPONSE
} UmiIntegrationMessageKind;

typedef enum UmiIntegrationTransport {
    UMI_INTEGRATION_TRANSPORT_IN_PROCESS = 0,
    UMI_INTEGRATION_TRANSPORT_LOCAL_IPC,
    UMI_INTEGRATION_TRANSPORT_STDIO,
    UMI_INTEGRATION_TRANSPORT_TCP,
    UMI_INTEGRATION_TRANSPORT_HTTP
} UmiIntegrationTransport;

const char *umi_integration_application_state_text(
    UmiIntegrationApplicationState state);
const char *umi_integration_message_kind_text(UmiIntegrationMessageKind kind);

#endif
