/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/compilation_database_internal.h
 * PURPOSE: Share the private immutable compiler-command snapshot between readers.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_COMPILATION_DATABASE_INTERNAL_H
#define UMICOM_COMPILATION_DATABASE_INTERNAL_H
#include "umicom/developer_project/compilation_database.h"
#include "umicom/language_runtime/json_tree.h"
struct UmiCompilationDatabase
{
    UmiJsonTree *tree;
    int *rows;
    size_t count;
};
UmiStatus UmiCompilationDatabaseRecord(const UmiCompilationDatabase *database, int row,
                                       UmiCompilationCommand *out);
#endif
