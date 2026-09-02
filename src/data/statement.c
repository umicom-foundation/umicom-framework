/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/statement.c
 *
 * PURPOSE:
 *   Implement owned SQL statement creation, cleanup and execution through the Data Server.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/statement.h"

#include <stdlib.h>
#include <string.h>

#include "include/data_internal.h"

/*
 * Initialise data statement from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_data_statement_create(const char *sql,
                                    UmiDataStatement *out_statement)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (sql == NULL || sql[0] == '\0' || out_statement == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    out_statement->sql = umi_data_strdup(sql);
    return out_statement->sql != NULL ? UMI_STATUS_OK : UMI_STATUS_OUT_OF_MEMORY;
}

/* Release or reset state held by data statement so the same storage can be reused safely. */
void umi_data_statement_dispose(UmiDataStatement *statement)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (statement == NULL) return;
    free(statement->sql);
    statement->sql = NULL;
}

/*
 * Perform data statement through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_data_statement_execute(const UmiDataStatement *statement,
                                     UmiDataServer *server)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (statement == NULL || statement->sql == NULL || server == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_data_server_execute(server, statement->sql);
}
