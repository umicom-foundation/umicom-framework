/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/connection_profile.c
 *
 * PURPOSE:
 *   Implement the connection profile behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/data/connection_profile.h"
#include <string.h>
/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiDatabaseConnectionRegistry *r,const char *id){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||id==NULL)return SIZE_MAX;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<r->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(r->items[i].id,id)==0)return i;return SIZE_MAX;}
/*
 * Initialise database connection registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_database_connection_registry_init(UmiDatabaseConnectionRegistry *r){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(r,0,sizeof(*r));r->revision=1U;return UMI_STATUS_OK;}
/*
 * Provide the database connection registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_database_connection_registry_upsert(UmiDatabaseConnectionRegistry *r,const UmiDatabaseConnectionProfile *p){size_t i;UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||p==NULL||p->id[0]=='\0'||p->provider<UMI_DATABASE_PROVIDER_MEMORY||p->provider>UMI_DATABASE_PROVIDER_CUSTOM)return UMI_STATUS_INVALID_ARGUMENT;i=find_index(r,p->id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r->count>=UMI_DATABASE_MAX_CONNECTIONS)return UMI_STATUS_CAPACITY_EXCEEDED;i=r->count++;}r->items[i]=*p;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r->active_id[0]=='\0'){s=umi_database_copy_text(r->active_id,sizeof(r->active_id),p->id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;}r->revision+=1U;return UMI_STATUS_OK;}
/*
 * Provide the database connection registry activate operation used by this module and its
 * client applications.
 */
UmiStatus umi_database_connection_registry_activate(UmiDatabaseConnectionRegistry *r,const char *id){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(find_index(r,id)==SIZE_MAX)return UMI_STATUS_NOT_FOUND;s=umi_database_copy_text(r->active_id,sizeof(r->active_id),id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK)r->revision+=1U;return s;}
/*
 * Provide the database connection registry active operation used by this module and its
 * client applications.
 */
UmiStatus umi_database_connection_registry_active(const UmiDatabaseConnectionRegistry *r,UmiDatabaseConnectionProfile *out){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;i=find_index(r,r->active_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX)return UMI_STATUS_NOT_FOUND;*out=r->items[i];return UMI_STATUS_OK;}
