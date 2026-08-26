/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/change_set.h
 *
 * PURPOSE:
 *   Collect changed property paths for one atomic update.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_CHANGE_SET_H
#define UMICOM_UI_REACTIVE_CHANGE_SET_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiReactiveChangeSet {
    char paths[64][UMI_UI_REACTIVE_PATH_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiUiReactiveChangeSet;
void umi_ui_reactive_change_set_init(UmiUiReactiveChangeSet *item);
int umi_ui_reactive_change_set_valid(const UmiUiReactiveChangeSet *item);
#ifdef __cplusplus
}
#endif
#endif
