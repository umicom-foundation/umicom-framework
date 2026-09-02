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

/**
 * List the named integration designer transport values accepted by this public contract.
 */
typedef enum UmiIntegrationDesignerTransport {
    UMI_INTEGRATION_DESIGNER_REST = 1,
    UMI_INTEGRATION_DESIGNER_GRAPHQL,
    UMI_INTEGRATION_DESIGNER_WEBSOCKET,
    UMI_INTEGRATION_DESIGNER_MESSAGE_BUS
} UmiIntegrationDesignerTransport;

/**
 * List the named integration designer auth values accepted by this public contract.
 */
typedef enum UmiIntegrationDesignerAuth {
    UMI_INTEGRATION_DESIGNER_AUTH_NONE = 0,
    UMI_INTEGRATION_DESIGNER_AUTH_API_KEY,
    UMI_INTEGRATION_DESIGNER_AUTH_OAUTH2,
    UMI_INTEGRATION_DESIGNER_AUTH_MTLS
} UmiIntegrationDesignerAuth;

/**
 * List the named integration designer schema kind values accepted by this public contract.
 */
typedef enum UmiIntegrationDesignerSchemaKind {
    UMI_INTEGRATION_DESIGNER_OPENAPI = 1,
    UMI_INTEGRATION_DESIGNER_GRAPHQL_SCHEMA,
    UMI_INTEGRATION_DESIGNER_JSON_SCHEMA,
    UMI_INTEGRATION_DESIGNER_AVRO_SCHEMA
} UmiIntegrationDesignerSchemaKind;

/**
 * List the named integration designer transform values accepted by this public contract.
 */
typedef enum UmiIntegrationDesignerTransform {
    UMI_INTEGRATION_DESIGNER_COPY = 0,
    UMI_INTEGRATION_DESIGNER_UPPERCASE,
    UMI_INTEGRATION_DESIGNER_LOWERCASE,
    UMI_INTEGRATION_DESIGNER_CONSTANT
} UmiIntegrationDesignerTransform;

/**
 * List the named integration designer node kind values accepted by this public contract.
 */
typedef enum UmiIntegrationDesignerNodeKind {
    UMI_INTEGRATION_DESIGNER_TRIGGER = 1,
    UMI_INTEGRATION_DESIGNER_REQUEST,
    UMI_INTEGRATION_DESIGNER_TRANSFORM,
    UMI_INTEGRATION_DESIGNER_ROUTE,
    UMI_INTEGRATION_DESIGNER_PUBLISH,
    UMI_INTEGRATION_DESIGNER_RESPONSE,
    UMI_INTEGRATION_DESIGNER_STOP
} UmiIntegrationDesignerNodeKind;

/**
 * List the named integration designer run status values accepted by this public contract.
 */
typedef enum UmiIntegrationDesignerRunStatus {
    UMI_INTEGRATION_DESIGNER_RUN_PENDING = 0,
    UMI_INTEGRATION_DESIGNER_RUN_SUCCEEDED,
    UMI_INTEGRATION_DESIGNER_RUN_FAILED,
    UMI_INTEGRATION_DESIGNER_RUN_CANCELLED
} UmiIntegrationDesignerRunStatus;

/**
 * Represent the integration designer field data shared with callers of this public
 * contract.
 */
typedef struct UmiIntegrationDesignerField {
    char key[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
    char value[UMI_INTEGRATION_DESIGNER_TEXT_CAPACITY];
} UmiIntegrationDesignerField;

/**
 * Represent the integration designer record data shared with callers of this public
 * contract.
 */
typedef struct UmiIntegrationDesignerRecord {
    UmiIntegrationDesignerField fields[UMI_INTEGRATION_DESIGNER_MAX_FIELDS];
    size_t count;
} UmiIntegrationDesignerRecord;

/**
 * Represent the integration designer issue data shared with callers of this public
 * contract.
 */
typedef struct UmiIntegrationDesignerIssue {
    char location[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
    char message[UMI_INTEGRATION_DESIGNER_TEXT_CAPACITY];
} UmiIntegrationDesignerIssue;

/**
 * Represent the integration designer validation data shared with callers of this public
 * contract.
 */
typedef struct UmiIntegrationDesignerValidation {
    UmiIntegrationDesignerIssue issues[UMI_INTEGRATION_DESIGNER_MAX_ISSUES];
    size_t count;
} UmiIntegrationDesignerValidation;

/**
 * Copy integration designer into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_integration_designer_copy(char *destination,
                                        size_t capacity,
                                        const char *source);
/**
 * Provide the integration designer transport text operation used by this module and its
 * client applications.
 */
const char *umi_integration_designer_transport_text(
    UmiIntegrationDesignerTransport transport);
/**
 * Provide the integration designer run status text operation used by this module and its
 * client applications.
 */
const char *umi_integration_designer_run_status_text(
    UmiIntegrationDesignerRunStatus status);
/**
 * Copy integration designer record into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_integration_designer_record_set(UmiIntegrationDesignerRecord *record,
                                              const char *key,
                                              const char *value);
/**
 * Provide the integration designer record get operation used by this module and its client
 * applications.
 */
const char *umi_integration_designer_record_get(
    const UmiIntegrationDesignerRecord *record,
    const char *key);
/**
 * Add integration designer validation only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_integration_designer_validation_add(
    UmiIntegrationDesignerValidation *validation,
    const char *location,
    const char *message);

#endif
