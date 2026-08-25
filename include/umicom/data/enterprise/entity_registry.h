/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/entity_registry.h
 *
 * PURPOSE:
 *   Store unique ORM entity descriptors for repositories and result mapping.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_ENTITY_REGISTRY_H
#define UMICOM_DATA_ENTERPRISE_ENTITY_REGISTRY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"
#include "umicom/data/enterprise/entity_descriptor.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataEntityRegistry {
    UmiDataEntityDescriptor items[UMI_DATA_ENTERPRISE_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiDataEntityRegistry;

/* Reset the bounded entity registry registry. */
void umi_data_entity_registry_init(UmiDataEntityRegistry *registry);
/* Add a unique item by stable identifier. */
UmiStatus umi_data_entity_registry_add(UmiDataEntityRegistry *registry, const UmiDataEntityDescriptor *item);
/* Find a registry item by stable identifier. */
UmiStatus umi_data_entity_registry_find(const UmiDataEntityRegistry *registry, const char *id, UmiDataEntityDescriptor *out_item);
/* Return the number of registered items. */
size_t umi_data_entity_registry_count(const UmiDataEntityRegistry *registry);

#ifdef __cplusplus
}
#endif
#endif
