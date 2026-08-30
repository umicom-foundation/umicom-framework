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

typedef struct UmiDataStatement {
    char *sql;
} UmiDataStatement;

UmiStatus umi_data_statement_create(const char *sql,
                                    UmiDataStatement *out_statement);
void umi_data_statement_dispose(UmiDataStatement *statement);
UmiStatus umi_data_statement_execute(const UmiDataStatement *statement,
                                     UmiDataServer *server);

#ifdef __cplusplus
}
#endif

#endif
