/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_database_query_policy.c
 *
 * PURPOSE:
 *   Implement the test database query policy behavior for
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
#include "umicom/data/query_policy.h"
int main(void){UmiDatabaseQueryPolicy p=umi_database_query_policy_default();UmiDatabaseQueryDecision d;assert(umi_database_query_policy_evaluate(&p,"SELECT * FROM orders",0,&d)==UMI_STATUS_OK&&d.allowed&&d.kind==UMI_DATABASE_QUERY_READ);assert(umi_database_query_policy_evaluate(&p,"DROP TABLE orders",1,&d)==UMI_STATUS_OK&&!d.allowed&&d.approval_required);p.allow_writes=1;assert(umi_database_query_policy_evaluate(&p,"SET mode safe",0,&d)==UMI_STATUS_OK&&!d.allowed);assert(umi_database_query_policy_evaluate(&p,"SET mode safe",1,&d)==UMI_STATUS_OK&&d.allowed);return 0;}
