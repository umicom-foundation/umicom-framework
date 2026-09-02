/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/property.h
 *
 * PURPOSE:
 *   Define a bounded property bag for toolkit-neutral view models, adapters and
 *   declarative UI bindings.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_PROPERTY_H
#define UMICOM_UI_PROPERTY_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/value.h"

#ifdef __cplusplus
extern "C" {
#endif


#define UMI_UI_PROPERTY_KEY_CAPACITY 128U
#define UMI_UI_PROPERTY_MAX 128U

/**
 * Represent the ui property snapshot data shared with callers of this public contract.
 */
typedef struct UmiUiPropertySnapshot {
    char key[UMI_UI_PROPERTY_KEY_CAPACITY];
    UmiUiValue value;
} UmiUiPropertySnapshot;

/**
 * Represent the ui property bag data shared with callers of this public contract.
 */
typedef struct UmiUiPropertyBag UmiUiPropertyBag;

/**
 * Initialise ui property bag from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ui_property_bag_create(UmiUiPropertyBag **out_bag);
/**
 * Release or reset state held by ui property bag so the same storage can be reused safely.
 */
void umi_ui_property_bag_destroy(UmiUiPropertyBag *bag);
/**
 * Copy ui property bag into module-owned storage so callers keep ownership of their input
 * values.
 */
UmiStatus umi_ui_property_bag_set(UmiUiPropertyBag *bag, const char *key,
                                  const UmiUiValue *value);
/**
 * Provide the ui property bag get operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_property_bag_get(const UmiUiPropertyBag *bag, const char *key,
                                  UmiUiValue *out_value);
/**
 * Remove ui property bag while keeping the remaining records in a valid and discoverable
 * state.
 */
UmiStatus umi_ui_property_bag_remove(UmiUiPropertyBag *bag, const char *key);
/**
 * Release or reset state held by ui property bag so the same storage can be reused safely.
 */
void umi_ui_property_bag_clear(UmiUiPropertyBag *bag);
/**
 * Return the number of records represented by ui property bag without changing their
 * state.
 */
size_t umi_ui_property_bag_count(const UmiUiPropertyBag *bag);
/**
 * Provide the ui property bag revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ui_property_bag_revision(const UmiUiPropertyBag *bag);
/**
 * Find ui property bag while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ui_property_bag_at(const UmiUiPropertyBag *bag, size_t index,
                                 UmiUiPropertySnapshot *out_property);

#ifdef __cplusplus
}
#endif

#endif
