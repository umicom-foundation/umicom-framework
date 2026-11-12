/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/components/registry.h
 *
 * PURPOSE:
 *   Register named component specifications without depending on a GUI toolkit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file keeps one responsibility small and explicit. Read the public
 * structure/function declarations first, then follow the implementation in
 * the matching source file.
 */

#ifndef UMICOM_UI_COMPONENTS_REGISTRY_H
#define UMICOM_UI_COMPONENTS_REGISTRY_H
#include "umicom/ui/components/component.h"

/* This API is implemented by the C23 Framework. Keep its external symbols
 * in the C ABI when a C++ application includes this header directly. */
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_UI_COMPONENT_REGISTRY_CAPACITY 256U
/**
 * Represent the ui component registry data shared with callers of this public contract.
 */
typedef struct UmiUiComponentRegistry { UmiUiComponentSpec items[UMI_UI_COMPONENT_REGISTRY_CAPACITY]; size_t count; } UmiUiComponentRegistry;
/**
 * Initialise ui component registry from caller-provided values so later operations receive
 * a known state.
 */

/** Validate every active specification and unique ID before lookup or mutation.
 * NULL is INVALID_ARGUMENT; excessive count, invalid active values or duplicate
 * IDs are INVALID_STATE. Inactive slots are not inspected. No state is changed. */
UmiStatus umi_ui_component_registry_validate(const UmiUiComponentRegistry *registry);

/** Registry operations require complete, correctly aligned records and one
 * owner to serialize access. Add and find leave destinations unchanged on error.
 * Find's output must not overlap the registry; such overlap is INVALID_ARGUMENT.
 * Query text may borrow the separate output record and is scanned for at most
 * UMI_UI_COMPONENT_ID_CAPACITY bytes. Missing termination is CAPACITY_EXCEEDED.
 * A valid full registry still reports ALREADY_EXISTS before capacity for an
 * existing ID. Add captures a valid borrowed specification before publication. */

void umi_ui_component_registry_init(UmiUiComponentRegistry *registry);
/**
 * Add ui component registry only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_ui_component_registry_add(UmiUiComponentRegistry *registry,const UmiUiComponentSpec *spec);
/**
 * Find ui component registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ui_component_registry_find(const UmiUiComponentRegistry *registry,
                                         const char *id,
                                         UmiUiComponentSpec *out_spec);

#ifdef __cplusplus
}
#endif
#endif
