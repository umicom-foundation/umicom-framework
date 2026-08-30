/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/preset.h
 *
 * PURPOSE:
 *   Describe named configure, build and test presets without depending on CMake JSON internals.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BUILD_PRESET_H
#define UMICOM_BUILD_PRESET_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/build/profile.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiBuildPreset {
    char preset_id[UMI_BUILD_ID_CAPACITY];
    UmiBuildProfile profile;
    int configure_enabled;
    int build_enabled;
    int test_enabled;
    int clean_first;
} UmiBuildPreset;

void umi_build_preset_init(UmiBuildPreset *preset,
                           const UmiBuildProfile *profile);
UmiStatus umi_build_preset_validate(const UmiBuildPreset *preset,
                                    char *out_message,
                                    size_t message_capacity);
UmiStatus umi_build_preset_describe(const UmiBuildPreset *preset,
                                    char *out_text,
                                    size_t capacity);

#ifdef __cplusplus
}
#endif

#endif
