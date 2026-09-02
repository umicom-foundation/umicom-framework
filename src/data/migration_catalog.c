/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/migration_catalog.c
 *
 * PURPOSE:
 *   Implement the migration catalog behavior for
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
#include "umicom/data/migration_catalog.h"
#include <string.h>
/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiDatabaseMigrationCatalog *c,uint32_t v){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL)return SIZE_MAX;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<c->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c->items[i].version==v)return i;return SIZE_MAX;}
/*
 * Initialise database migration catalog from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_database_migration_catalog_init(UmiDatabaseMigrationCatalog *c){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(c,0,sizeof(*c));c->revision=1U;return UMI_STATUS_OK;}
/*
 * Provide the database migration catalog upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_database_migration_catalog_upsert(UmiDatabaseMigrationCatalog *c,const UmiDatabaseMigrationInfo *m){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL||m==NULL||m->version==0U||m->name[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;i=find_index(c,m->version);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c->count>=UMI_DATABASE_MAX_MIGRATIONS)return UMI_STATUS_CAPACITY_EXCEEDED;i=c->count++;}c->items[i]=*m;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m->state==UMI_DATABASE_MIGRATION_APPLIED&&m->version>c->current_version)c->current_version=m->version;c->revision+=1U;return UMI_STATUS_OK;}
/*
 * Provide the database migration catalog mark applied operation used by this module and
 * its client applications.
 */
UmiStatus umi_database_migration_catalog_mark_applied(UmiDatabaseMigrationCatalog *c,uint32_t v,uint64_t at){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL)return UMI_STATUS_INVALID_ARGUMENT;i=find_index(c,v);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX)return UMI_STATUS_NOT_FOUND;c->items[i].state=UMI_DATABASE_MIGRATION_APPLIED;c->items[i].applied_at=at;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(v>c->current_version)c->current_version=v;c->revision+=1U;return UMI_STATUS_OK;}
/*
 * Provide the database migration catalog pending operation used by this module and its
 * client applications.
 */
size_t umi_database_migration_catalog_pending(const UmiDatabaseMigrationCatalog *c){size_t i,n=0U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL)return 0U;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<c->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c->items[i].state==UMI_DATABASE_MIGRATION_PENDING)n+=1U;return n;}
