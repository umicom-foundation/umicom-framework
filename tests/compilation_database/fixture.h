/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compilation_database/fixture.h
 * PURPOSE: Keep compilation-database regression fixtures explicit and platform independent.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_COMPILATION_DATABASE_FIXTURE_H
#define UMICOM_COMPILATION_DATABASE_FIXTURE_H
#include "umicom/base/arguments.h"
#include "umicom/developer_project/compilation_database.h"
#include "umicom/platform/path.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(condition)                                                                           \
    do                                                                                             \
    {                                                                                              \
        if (!(condition))                                                                          \
        {                                                                                          \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition);                        \
            return EXIT_FAILURE;                                                                   \
        }                                                                                          \
    } while (0)
#ifdef _WIN32
#define DATABASE_ROOT "C:/project"
#else
#define DATABASE_ROOT "/project"
#endif
#define DATABASE_DIRECTORY DATABASE_ROOT "/build"
#define DATABASE_ROW                                                                               \
    "{\"directory\":\"" DATABASE_DIRECTORY                                                         \
    "\",\"file\":\"../src/main.c\",\"arguments\":[\"cc\",\"-std=c23\",\"-DNAME=a "                 \
    "b\",\"\",\"../src/main.c\"]}"
static inline UmiStatus ParseDatabase(const char *text, UmiCompilationDatabase **out)
{
    return UmiCompilationDatabaseCreate(text, strlen(text), NULL, out);
}
#endif
