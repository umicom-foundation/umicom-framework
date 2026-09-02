/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/explorer_types.c
 *
 * PURPOSE:
 *   Implement the explorer types behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/data/explorer_types.h"
#include <string.h>
/*
 * Provide the database copy text operation used by this module and its client
 * applications.
 */
UmiStatus umi_database_copy_text(char *d,size_t c,const char *s){size_t n;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(d==NULL||c==0U||s==NULL)return UMI_STATUS_INVALID_ARGUMENT;n=strlen(s);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(n>=c)return UMI_STATUS_CAPACITY_EXCEEDED;(void)memcpy(d,s,n+1U);return UMI_STATUS_OK;}
/*
 * Provide the database provider text operation used by this module and its client
 * applications.
 */
const char *umi_database_provider_text(UmiDatabaseProvider p){/* Select the behaviour associated with the requested command or state value. */ switch(p){case UMI_DATABASE_PROVIDER_MEMORY:return "memory";case UMI_DATABASE_PROVIDER_SQLITE:return "sqlite";case UMI_DATABASE_PROVIDER_POSTGRESQL:return "postgresql";case UMI_DATABASE_PROVIDER_ORACLE:return "oracle";case UMI_DATABASE_PROVIDER_MYSQL:return "mysql";case UMI_DATABASE_PROVIDER_MONGODB:return "mongodb";case UMI_DATABASE_PROVIDER_CUSTOM:return "custom";default:return "unknown";}}
/*
 * Provide the database query kind text operation used by this module and its client
 * applications.
 */
const char *umi_database_query_kind_text(UmiDatabaseQueryKind k){/* Select the behaviour associated with the requested command or state value. */ switch(k){case UMI_DATABASE_QUERY_READ:return "read";case UMI_DATABASE_QUERY_WRITE:return "write";case UMI_DATABASE_QUERY_SCHEMA:return "schema";case UMI_DATABASE_QUERY_TRANSACTION:return "transaction";default:return "unknown";}}
