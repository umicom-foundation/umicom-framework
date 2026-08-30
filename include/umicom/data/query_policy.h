/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/query_policy.h
 *
 * PURPOSE:
 *   Publish the public query policy contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_QUERY_POLICY_H
#define UMICOM_DATA_QUERY_POLICY_H
#include "umicom/data/explorer_types.h"
typedef struct UmiDatabaseQueryPolicy { int allow_reads; int allow_writes; int allow_schema_changes; int require_transaction_for_writes; size_t maximum_result_rows; size_t maximum_statement_bytes; } UmiDatabaseQueryPolicy;
typedef struct UmiDatabaseQueryDecision { UmiDatabaseQueryKind kind; int allowed; int approval_required; char reason[UMI_DATABASE_TEXT_CAPACITY]; } UmiDatabaseQueryDecision;
UmiDatabaseQueryPolicy umi_database_query_policy_default(void);
UmiDatabaseQueryKind umi_database_query_classify(const char *statement);
UmiStatus umi_database_query_policy_evaluate(const UmiDatabaseQueryPolicy *policy,const char *statement,int transaction_active,UmiDatabaseQueryDecision *out_decision);
#endif
