/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/messaging/schema.h
 *
 * PURPOSE:
 *   Declare a thread-safe schema registry that validates message kind, payload size and compatible schema versions before dispatch or persistence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_MESSAGING_SCHEMA_H
#define UMICOM_MESSAGING_SCHEMA_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/messaging/message.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiSchemaCompatibility {
    UMI_SCHEMA_EXACT = 1,
    UMI_SCHEMA_BACKWARD = 2,
    UMI_SCHEMA_FORWARD = 3,
    UMI_SCHEMA_FULL = 4
} UmiSchemaCompatibility;

typedef struct UmiSchemaDescriptor {
    uint32_t structure_size;
    const char *schema_id;
    uint32_t version;
    UmiMessageKind kind;
    size_t maximum_payload_size;
    UmiSchemaCompatibility compatibility;
} UmiSchemaDescriptor;

typedef struct UmiSchemaRegistry UmiSchemaRegistry;

UmiStatus umi_schema_registry_create(UmiSchemaRegistry **out_registry);
void umi_schema_registry_destroy(UmiSchemaRegistry *registry);
UmiStatus umi_schema_registry_register(UmiSchemaRegistry *registry,
                                       const UmiSchemaDescriptor *schema);
const UmiSchemaDescriptor *umi_schema_registry_find(
    const UmiSchemaRegistry *registry,
    const char *schema_id,
    uint32_t version);
UmiStatus umi_schema_registry_validate(const UmiSchemaRegistry *registry,
                                       const UmiMessageEnvelope *message);
size_t umi_schema_registry_count(const UmiSchemaRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
