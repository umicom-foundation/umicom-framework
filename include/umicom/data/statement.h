/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/statement.h
 *
 * PURPOSE:
 *   Declare an owned SQL statement value that can be prepared independently and executed through the Data Server boundary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_STATEMENT_H
#define UMICOM_DATA_STATEMENT_H

#include "umicom/base/status.h"
#include "umicom/data/data_server.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the data statement data shared with callers of this public contract.
 */
typedef struct UmiDataStatement {
    char *sql;
} UmiDataStatement;

/**
 * Initialise data statement from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_data_statement_create(const char *sql,
                                    UmiDataStatement *out_statement);
/**
 * Release or reset state held by data statement so the same storage can be reused safely.
 */
void umi_data_statement_dispose(UmiDataStatement *statement);
/**
 * Perform data statement through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_data_statement_execute(const UmiDataStatement *statement,
                                     UmiDataServer *server);

#ifdef __cplusplus
}
#endif

#endif
