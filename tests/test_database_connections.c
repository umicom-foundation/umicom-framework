/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_database_connections.c
 *
 * PURPOSE:
 *   Implement the test database connections behavior for
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
#include "umicom/data/connection_profile.h"
int main(void){UmiDatabaseConnectionRegistry r;UmiDatabaseConnectionProfile p={0},out;(void)strcpy(p.id,"local");(void)strcpy(p.display_name,"Local SQLite");(void)strcpy(p.endpoint,"studio.db");(void)strcpy(p.secret_reference,"secret://studio/database");p.provider=UMI_DATABASE_PROVIDER_SQLITE;p.state=UMI_DATABASE_READY;assert(umi_database_connection_registry_init(&r)==UMI_STATUS_OK);assert(umi_database_connection_registry_upsert(&r,&p)==UMI_STATUS_OK);assert(umi_database_connection_registry_activate(&r,"local")==UMI_STATUS_OK);assert(umi_database_connection_registry_active(&r,&out)==UMI_STATUS_OK);assert(strcmp(out.secret_reference,"secret://studio/database")==0);return 0;}
