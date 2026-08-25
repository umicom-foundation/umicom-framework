/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_migration_graph.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the migration graph enterprise data capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/migration_graph.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void) {
    UmiDataMigrationGraph g; UmiDataMigrationStep a,b; umi_data_migration_graph_init(&g); CHECK(umi_data_migration_step_init(&a,"a","A",1U,true,false)==UMI_STATUS_OK); CHECK(umi_data_migration_step_init(&b,"b","B",2U,true,false)==UMI_STATUS_OK); CHECK(umi_data_migration_graph_add_step(&g,&a)==UMI_STATUS_OK); CHECK(umi_data_migration_graph_add_step(&g,&b)==UMI_STATUS_OK); CHECK(umi_data_migration_graph_add_edge(&g,"a","b")==UMI_STATUS_OK); CHECK(!umi_data_migration_graph_has_cycle(&g)); CHECK(umi_data_migration_graph_add_edge(&g,"b","a")==UMI_STATUS_OK); CHECK(umi_data_migration_graph_has_cycle(&g));
    return 0;
}
