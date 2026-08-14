/* Umicom Framework | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_DATA_TRANSFER_PLAN_H
#define UMICOM_DATA_TRANSFER_PLAN_H
#include "umicom/data/explorer_types.h"
typedef struct UmiDatabaseTransferPlan { char id[UMI_DATABASE_ID_CAPACITY]; UmiDatabaseTransferDirection direction; UmiDatabaseTransferFormat format; char table_name[UMI_DATABASE_NAME_CAPACITY]; char path[UMI_DATABASE_PATH_CAPACITY]; char delimiter[8]; int include_header; int transactional; int dry_run; size_t processed_rows; size_t rejected_rows; UmiStatus status; } UmiDatabaseTransferPlan;
UmiStatus umi_database_transfer_plan_init(UmiDatabaseTransferPlan *plan,const char *id,UmiDatabaseTransferDirection direction,UmiDatabaseTransferFormat format,const char *table_name,const char *path);
UmiStatus umi_database_transfer_plan_validate(const UmiDatabaseTransferPlan *plan,char *message,size_t message_capacity);
#endif
