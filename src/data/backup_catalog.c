/* Umicom Framework | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/data/backup_catalog.h"
#include <string.h>
static size_t find_index(const UmiDatabaseBackupCatalog *c,const char *id){size_t i;if(c==NULL||id==NULL)return SIZE_MAX;for(i=0U;i<c->count;++i)if(strcmp(c->items[i].id,id)==0)return i;return SIZE_MAX;}
UmiStatus umi_database_backup_catalog_init(UmiDatabaseBackupCatalog *c){if(c==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(c,0,sizeof(*c));c->revision=1U;return UMI_STATUS_OK;}
UmiStatus umi_database_backup_catalog_add(UmiDatabaseBackupCatalog *c,const UmiDatabaseBackupRecord *r){size_t i;if(c==NULL||r==NULL||r->id[0]=='\0'||r->path[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;i=find_index(c,r->id);if(i==SIZE_MAX){if(c->count>=UMI_DATABASE_MAX_BACKUPS)return UMI_STATUS_CAPACITY_EXCEEDED;i=c->count++;}c->items[i]=*r;c->revision+=1U;return UMI_STATUS_OK;}
UmiStatus umi_database_backup_catalog_mark_verified(UmiDatabaseBackupCatalog *c,const char *id){size_t i;if(c==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;i=find_index(c,id);if(i==SIZE_MAX)return UMI_STATUS_NOT_FOUND;c->items[i].verified=1;c->revision+=1U;return UMI_STATUS_OK;}
