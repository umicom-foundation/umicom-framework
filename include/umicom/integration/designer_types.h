/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/designer_types.h
 *
 * PURPOSE:
 *   Publish the public designer types contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_DESIGNER_TYPES_H
#define UMICOM_INTEGRATION_DESIGNER_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#define UMI_INTEGRATION_DESIGNER_ID_CAPACITY 96U
#define UMI_INTEGRATION_DESIGNER_NAME_CAPACITY 160U
#define UMI_INTEGRATION_DESIGNER_URI_CAPACITY 384U
#define UMI_INTEGRATION_DESIGNER_TEXT_CAPACITY 512U
#define UMI_INTEGRATION_DESIGNER_PAYLOAD_CAPACITY 2048U
#define UMI_INTEGRATION_DESIGNER_MAX_PROFILES 16U
#define UMI_INTEGRATION_DESIGNER_MAX_OPERATIONS 24U
#define UMI_INTEGRATION_DESIGNER_MAX_SCHEMAS 32U
#define UMI_INTEGRATION_DESIGNER_MAX_MAPPINGS 16U
#define UMI_INTEGRATION_DESIGNER_MAX_RULES 32U
#define UMI_INTEGRATION_DESIGNER_MAX_FIELDS 32U
#define UMI_INTEGRATION_DESIGNER_MAX_WORKFLOWS 16U
#define UMI_INTEGRATION_DESIGNER_MAX_NODES 32U
#define UMI_INTEGRATION_DESIGNER_MAX_EDGES 64U
#define UMI_INTEGRATION_DESIGNER_MAX_RUNS 64U
#define UMI_INTEGRATION_DESIGNER_MAX_TRACE 64U
#define UMI_INTEGRATION_DESIGNER_MAX_ISSUES 32U

typedef enum UmiIntegrationDesignerTransport {
    UMI_INTEGRATION_DESIGNER_REST = 1,
    UMI_INTEGRATION_DESIGNER_GRAPHQL,
    UMI_INTEGRATION_DESIGNER_WEBSOCKET,
    UMI_INTEGRATION_DESIGNER_MESSAGE_BUS
} UmiIntegrationDesignerTransport;

typedef enum UmiIntegrationDesignerAuth {
    UMI_INTEGRATION_DESIGNER_AUTH_NONE = 0,
    UMI_INTEGRATION_DESIGNER_AUTH_API_KEY,
    UMI_INTEGRATION_DESIGNER_AUTH_OAUTH2,
    UMI_INTEGRATION_DESIGNER_AUTH_MTLS
} UmiIntegrationDesignerAuth;

typedef enum UmiIntegrationDesignerSchemaKind {
    UMI_INTEGRATION_DESIGNER_OPENAPI = 1,
    UMI_INTEGRATION_DESIGNER_GRAPHQL_SCHEMA,
    UMI_INTEGRATION_DESIGNER_JSON_SCHEMA,
    UMI_INTEGRATION_DESIGNER_AVRO_SCHEMA
} UmiIntegrationDesignerSchemaKind;

typedef enum UmiIntegrationDesignerTransform {
    UMI_INTEGRATION_DESIGNER_COPY = 0,
    UMI_INTEGRATION_DESIGNER_UPPERCASE,
    UMI_INTEGRATION_DESIGNER_LOWERCASE,
    UMI_INTEGRATION_DESIGNER_CONSTANT
} UmiIntegrationDesignerTransform;

typedef enum UmiIntegrationDesignerNodeKind {
    UMI_INTEGRATION_DESIGNER_TRIGGER = 1,
    UMI_INTEGRATION_DESIGNER_REQUEST,
    UMI_INTEGRATION_DESIGNER_TRANSFORM,
    UMI_INTEGRATION_DESIGNER_ROUTE,
    UMI_INTEGRATION_DESIGNER_PUBLISH,
    UMI_INTEGRATION_DESIGNER_RESPONSE,
    UMI_INTEGRATION_DESIGNER_STOP
} UmiIntegrationDesignerNodeKind;

typedef enum UmiIntegrationDesignerRunStatus {
    UMI_INTEGRATION_DESIGNER_RUN_PENDING = 0,
    UMI_INTEGRATION_DESIGNER_RUN_SUCCEEDED,
    UMI_INTEGRATION_DESIGNER_RUN_FAILED,
    UMI_INTEGRATION_DESIGNER_RUN_CANCELLED
} UmiIntegrationDesignerRunStatus;

typedef struct UmiIntegrationDesignerField {
    char key[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
    char value[UMI_INTEGRATION_DESIGNER_TEXT_CAPACITY];
} UmiIntegrationDesignerField;

typedef struct UmiIntegrationDesignerRecord {
    UmiIntegrationDesignerField fields[UMI_INTEGRATION_DESIGNER_MAX_FIELDS];
    size_t count;
} UmiIntegrationDesignerRecord;

typedef struct UmiIntegrationDesignerIssue {
    char location[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
    char message[UMI_INTEGRATION_DESIGNER_TEXT_CAPACITY];
} UmiIntegrationDesignerIssue;

typedef struct UmiIntegrationDesignerValidation {
    UmiIntegrationDesignerIssue issues[UMI_INTEGRATION_DESIGNER_MAX_ISSUES];
    size_t count;
} UmiIntegrationDesignerValidation;

UmiStatus umi_integration_designer_copy(char *destination,
                                        size_t capacity,
                                        const char *source);
const char *umi_integration_designer_transport_text(
    UmiIntegrationDesignerTransport transport);
const char *umi_integration_designer_run_status_text(
    UmiIntegrationDesignerRunStatus status);
UmiStatus umi_integration_designer_record_set(UmiIntegrationDesignerRecord *record,
                                              const char *key,
                                              const char *value);
const char *umi_integration_designer_record_get(
    const UmiIntegrationDesignerRecord *record,
    const char *key);
UmiStatus umi_integration_designer_validation_add(
    UmiIntegrationDesignerValidation *validation,
    const char *location,
    const char *message);

#endif
