/* Umicom Framework Tests | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "umicom/data/schema_catalog.h"
int main(void){UmiDatabaseSchemaCatalog c;UmiDatabaseTableInfo t={0},out;UmiDatabaseColumnInfo column={0};(void)strcpy(t.schema_name,"main");(void)strcpy(t.name,"orders");(void)strcpy(column.name,"order_id");(void)strcpy(column.type_name,"integer");column.primary_key=1;assert(umi_database_table_add_column(&t,&column)==UMI_STATUS_OK);assert(umi_database_schema_catalog_init(&c)==UMI_STATUS_OK);assert(umi_database_schema_catalog_upsert(&c,&t)==UMI_STATUS_OK);assert(umi_database_schema_catalog_find(&c,"main","orders",&out)==UMI_STATUS_OK);assert(out.column_count==1U&&out.columns[0].primary_key);return 0;}
