/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/declarative/migration.h
 *
 * PURPOSE:
 *   Upgrade declarative document version metadata through explicit migration steps.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The declarations below describe semantic application data and behaviour.
 * They deliberately avoid GUI-toolkit types so the same contract can be used
 * by GTK4, web, headless tests and future frontend adapters.
 */

#ifndef UMICOM_DECLARATIVE_MIGRATION_H
#define UMICOM_DECLARATIVE_MIGRATION_H
#include "umicom/declarative/document.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_decl_migrate_to_current(UmiDeclDocument *document,size_t *out_steps);
#ifdef __cplusplus
}
#endif
#endif
