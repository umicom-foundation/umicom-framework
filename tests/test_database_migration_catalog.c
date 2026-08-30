/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_database_migration_catalog.c
 *
 * PURPOSE:
 *   Implement the test database migration catalog behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework Tests | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "umicom/data/migration_catalog.h"
int main(void){UmiDatabaseMigrationCatalog c;UmiDatabaseMigrationInfo m={0};m.version=1U;(void)strcpy(m.name,"Initial schema");(void)strcpy(m.checksum,"sha256:one");m.state=UMI_DATABASE_MIGRATION_PENDING;assert(umi_database_migration_catalog_init(&c)==UMI_STATUS_OK);assert(umi_database_migration_catalog_upsert(&c,&m)==UMI_STATUS_OK);assert(umi_database_migration_catalog_pending(&c)==1U);assert(umi_database_migration_catalog_mark_applied(&c,1U,100U)==UMI_STATUS_OK);assert(c.current_version==1U&&umi_database_migration_catalog_pending(&c)==0U);return 0;}
