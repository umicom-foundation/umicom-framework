/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/binding_catalogue.h
 *
 * PURPOSE:
 *   Store a bounded catalogue of binding identifiers and revisions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_BINDING_CATALOGUE_H
#define UMICOM_UI_REACTIVE_BINDING_CATALOGUE_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive binding catalogue data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveBindingCatalogue {
    char ids[UMI_UI_REACTIVE_MAX_ITEMS][UMI_UI_REACTIVE_ID_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiUiReactiveBindingCatalogue;
/**
 * Initialise ui reactive binding catalogue from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_binding_catalogue_init(UmiUiReactiveBindingCatalogue *item);
/**
 * Add ui reactive binding catalogue only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_ui_reactive_binding_catalogue_add(UmiUiReactiveBindingCatalogue *item,const char *id);
/**
 * Provide the ui reactive binding catalogue contains operation used by this module and its
 * client applications.
 */
int umi_ui_reactive_binding_catalogue_contains(const UmiUiReactiveBindingCatalogue *item,const char *id);
#ifdef __cplusplus
}
#endif
#endif
