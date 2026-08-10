/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/property.h
 *
 * PURPOSE:
 *   Define a bounded property bag for toolkit-neutral view models, adapters and
 *   declarative UI bindings.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiUiPropertySnapshot {
    char key[UMI_UI_PROPERTY_KEY_CAPACITY];
    UmiUiValue value;
} UmiUiPropertySnapshot;

typedef struct UmiUiPropertyBag UmiUiPropertyBag;

UmiStatus umi_ui_property_bag_create(UmiUiPropertyBag **out_bag);
void umi_ui_property_bag_destroy(UmiUiPropertyBag *bag);
UmiStatus umi_ui_property_bag_set(UmiUiPropertyBag *bag, const char *key,
                                  const UmiUiValue *value);
UmiStatus umi_ui_property_bag_get(const UmiUiPropertyBag *bag, const char *key,
                                  UmiUiValue *out_value);
UmiStatus umi_ui_property_bag_remove(UmiUiPropertyBag *bag, const char *key);
void umi_ui_property_bag_clear(UmiUiPropertyBag *bag);
size_t umi_ui_property_bag_count(const UmiUiPropertyBag *bag);
uint64_t umi_ui_property_bag_revision(const UmiUiPropertyBag *bag);
UmiStatus umi_ui_property_bag_at(const UmiUiPropertyBag *bag, size_t index,
                                 UmiUiPropertySnapshot *out_property);

#ifdef __cplusplus
}
#endif

#endif
