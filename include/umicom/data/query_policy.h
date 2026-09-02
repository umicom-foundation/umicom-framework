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
/**
 * Represent the database query policy data shared with callers of this public contract.
 */
typedef struct UmiDatabaseQueryPolicy { int allow_reads; int allow_writes; int allow_schema_changes; int require_transaction_for_writes; size_t maximum_result_rows; size_t maximum_statement_bytes; } UmiDatabaseQueryPolicy;
/**
 * Represent the database query decision data shared with callers of this public contract.
 */
typedef struct UmiDatabaseQueryDecision { UmiDatabaseQueryKind kind; int allowed; int approval_required; char reason[UMI_DATABASE_TEXT_CAPACITY]; } UmiDatabaseQueryDecision;
/**
 * Provide the database query policy default operation used by this module and its client
 * applications.
 */
UmiDatabaseQueryPolicy umi_database_query_policy_default(void);
/**
 * Provide the database query classify operation used by this module and its client
 * applications.
 */
UmiDatabaseQueryKind umi_database_query_classify(const char *statement);
/**
 * Provide the database query policy evaluate operation used by this module and its client
 * applications.
 */
UmiStatus umi_database_query_policy_evaluate(const UmiDatabaseQueryPolicy *policy,const char *statement,int transaction_active,UmiDatabaseQueryDecision *out_decision);
#endif
