/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/translator.h
 *
 * PURPOSE:
 *   Translate real interaction samples into normalised Workbench Context Events using the canonical event builders.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_TRANSLATOR_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_TRANSLATOR_H

#include "umicom/workbench_context_source/definition.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_workbench_context_source_translate(
    const UmiWorkbenchContextSourceDefinition *definition,
    const UmiWorkbenchContextSourceSample *sample,
    UmiWorkbenchContextEvent *out_event);

#ifdef __cplusplus
}
#endif
#endif
