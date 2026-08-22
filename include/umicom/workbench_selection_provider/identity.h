/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/identity.h
 *
 * PURPOSE:
 *   Construct stable bounded selection identifiers from provider/model identity and revisions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_IDENTITY_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_IDENTITY_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_workbench_selection_provider_make_id(
    char *out_id,
    size_t capacity,
    const char *prefix,
    const char *subject_id,
    uint64_t revision);
UmiStatus umi_workbench_selection_provider_make_indexed_id(
    char *out_id,
    size_t capacity,
    const char *prefix,
    size_t index,
    uint64_t revision);

#ifdef __cplusplus
}
#endif
#endif
