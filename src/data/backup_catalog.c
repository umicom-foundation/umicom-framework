/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/backup_catalog.c
 *
 * PURPOSE:
 *   Implement the backup catalog behavior for
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
#include "umicom/data/backup_catalog.h"
#include <string.h>
/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiDatabaseBackupCatalog *c,const char *id){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL||id==NULL)return SIZE_MAX;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<c->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(c->items[i].id,id)==0)return i;return SIZE_MAX;}
/*
 * Initialise database backup catalog from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_database_backup_catalog_init(UmiDatabaseBackupCatalog *c){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(c,0,sizeof(*c));c->revision=1U;return UMI_STATUS_OK;}
/*
 * Add database backup catalog only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_database_backup_catalog_add(UmiDatabaseBackupCatalog *c,const UmiDatabaseBackupRecord *r){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL||r==NULL||r->id[0]=='\0'||r->path[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;i=find_index(c,r->id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c->count>=UMI_DATABASE_MAX_BACKUPS)return UMI_STATUS_CAPACITY_EXCEEDED;i=c->count++;}c->items[i]=*r;c->revision+=1U;return UMI_STATUS_OK;}
/*
 * Provide the database backup catalog mark verified operation used by this module and its
 * client applications.
 */
UmiStatus umi_database_backup_catalog_mark_verified(UmiDatabaseBackupCatalog *c,const char *id){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;i=find_index(c,id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX)return UMI_STATUS_NOT_FOUND;c->items[i].verified=1;c->revision+=1U;return UMI_STATUS_OK;}
