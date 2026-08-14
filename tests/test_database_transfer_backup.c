/* Umicom Framework Tests | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "umicom/data/backup_catalog.h"
#include "umicom/data/transfer_plan.h"
int main(void){UmiDatabaseTransferPlan p;UmiDatabaseBackupCatalog c;UmiDatabaseBackupRecord b={0};char message[256];assert(umi_database_transfer_plan_init(&p,"export-orders",UMI_DATABASE_TRANSFER_EXPORT,UMI_DATABASE_FORMAT_CSV,"orders","orders.csv")==UMI_STATUS_OK);assert(umi_database_transfer_plan_validate(&p,message,sizeof(message))==UMI_STATUS_OK);(void)strcpy(b.id,"backup-1");(void)strcpy(b.path,"backup/studio.db");(void)strcpy(b.provider,"sqlite");assert(umi_database_backup_catalog_init(&c)==UMI_STATUS_OK);assert(umi_database_backup_catalog_add(&c,&b)==UMI_STATUS_OK);assert(umi_database_backup_catalog_mark_verified(&c,"backup-1")==UMI_STATUS_OK&&c.items[0].verified);return 0;}
