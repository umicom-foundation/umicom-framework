/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_migration_plan.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the migration plan enterprise data capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/migration_plan.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiDataMigrationGraph g; UmiDataMigrationStep a,b; UmiDataMigrationPlan p; umi_data_migration_graph_init(&g); CHECK(umi_data_migration_step_init(&a,"a","A",1U,true,false)==UMI_STATUS_OK); CHECK(umi_data_migration_step_init(&b,"b","B",2U,true,false)==UMI_STATUS_OK); CHECK(umi_data_migration_graph_add_step(&g,&a)==UMI_STATUS_OK); CHECK(umi_data_migration_graph_add_step(&g,&b)==UMI_STATUS_OK); CHECK(umi_data_migration_graph_add_edge(&g,"a","b")==UMI_STATUS_OK); CHECK(umi_data_migration_plan_build(&g,&p)==UMI_STATUS_OK); CHECK(strcmp(p.step_ids[0],"a")==0);
    return 0;
}
