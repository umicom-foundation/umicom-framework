/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/identity_map.c
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
#include "umicom/data/enterprise/identity_map.h"
#include <string.h>

/* Identity maps are intentionally scoped and allocation-free. */
void umi_data_identity_map_init(UmiDataIdentityMap *map){if(map!=NULL)(void)memset(map,0,sizeof(*map));}
/* Rebinding the same logical identity to a different object is rejected. */
UmiStatus umi_data_identity_map_bind(UmiDataIdentityMap *map,const char *entity_id,const char *key,uint64_t object_token){size_t i;UmiStatus s;if(map==NULL||entity_id==NULL||key==NULL||object_token==0U)return UMI_STATUS_INVALID_ARGUMENT;for(i=0;i<map->count;++i)if(strcmp(map->items[i].entity_id,entity_id)==0&&strcmp(map->items[i].key,key)==0)return map->items[i].object_token==object_token?UMI_STATUS_ALREADY_EXISTS:UMI_STATUS_INVALID_STATE;if(map->count>=UMI_DATA_ENTERPRISE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;s=umi_data_enterprise_copy_text(map->items[map->count].entity_id,sizeof(map->items[map->count].entity_id),entity_id);if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(map->items[map->count].key,sizeof(map->items[map->count].key),key);if(s!=UMI_STATUS_OK)return s;map->items[map->count].object_token=object_token;map->count++;return UMI_STATUS_OK;}
/* Lookup never dereferences the object token; ownership remains with the caller. */
UmiStatus umi_data_identity_map_find(const UmiDataIdentityMap *map,const char *entity_id,const char *key,uint64_t *out_token){size_t i;if(map==NULL||entity_id==NULL||key==NULL||out_token==NULL)return UMI_STATUS_INVALID_ARGUMENT;for(i=0;i<map->count;++i)if(strcmp(map->items[i].entity_id,entity_id)==0&&strcmp(map->items[i].key,key)==0){*out_token=map->items[i].object_token;return UMI_STATUS_OK;}return UMI_STATUS_NOT_FOUND;}
