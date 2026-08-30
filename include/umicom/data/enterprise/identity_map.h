/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/identity_map.h
 *
 * PURPOSE:
 *   Maintain transaction-scoped identity keys so repeated ORM materialisation resolves the same logical entity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_IDENTITY_MAP_H
#define UMICOM_DATA_ENTERPRISE_IDENTITY_MAP_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataIdentityEntry { char entity_id[UMI_DATA_ENTERPRISE_ID_CAPACITY]; char key[UMI_DATA_ENTERPRISE_ID_CAPACITY]; uint64_t object_token; } UmiDataIdentityEntry;
typedef struct UmiDataIdentityMap { UmiDataIdentityEntry items[UMI_DATA_ENTERPRISE_MAX_ITEMS]; size_t count; } UmiDataIdentityMap;
/* Reset the identity map for a new unit of work. */
void umi_data_identity_map_init(UmiDataIdentityMap *map);
/* Bind an entity/key pair to a caller-owned object token. */
UmiStatus umi_data_identity_map_bind(UmiDataIdentityMap *map,const char *entity_id,const char *key,uint64_t object_token);
/* Resolve a previously bound logical identity. */
UmiStatus umi_data_identity_map_find(const UmiDataIdentityMap *map,const char *entity_id,const char *key,uint64_t *out_token);

#ifdef __cplusplus
}
#endif
#endif
