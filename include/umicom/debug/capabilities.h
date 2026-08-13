/* Umicom Framework DAP capability set. Sammy Hegab, Umicom Foundation, MIT. */
#ifndef UMICOM_DEBUG_CAPABILITIES_H
#define UMICOM_DEBUG_CAPABILITIES_H
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
enum UmiDebugCapability{UMI_DEBUG_CAP_CONFIGURATION_DONE=1ULL<<0,UMI_DEBUG_CAP_FUNCTION_BREAKPOINTS=1ULL<<1,UMI_DEBUG_CAP_CONDITIONAL_BREAKPOINTS=1ULL<<2,UMI_DEBUG_CAP_HIT_CONDITIONS=1ULL<<3,UMI_DEBUG_CAP_EVALUATE_HOVER=1ULL<<4,UMI_DEBUG_CAP_SET_VARIABLE=1ULL<<5,UMI_DEBUG_CAP_RESTART=1ULL<<6,UMI_DEBUG_CAP_TERMINATE=1ULL<<7,UMI_DEBUG_CAP_MODULES=1ULL<<8,UMI_DEBUG_CAP_READ_MEMORY=1ULL<<9,UMI_DEBUG_CAP_DISASSEMBLE=1ULL<<10,UMI_DEBUG_CAP_STEP_BACK=1ULL<<11};
typedef struct UmiDebugCapabilitySet{uint64_t supported;uint64_t advertised;uint64_t required;uint64_t revision;}UmiDebugCapabilitySet;
void umi_debug_capability_set_init(UmiDebugCapabilitySet *set);
UmiStatus umi_debug_capability_set_advertise(UmiDebugCapabilitySet *set,uint64_t capabilities);
UmiStatus umi_debug_capability_set_require(UmiDebugCapabilitySet *set,uint64_t capabilities);
int umi_debug_capability_set_ready(const UmiDebugCapabilitySet *set);
int umi_debug_capability_set_has(const UmiDebugCapabilitySet *set,uint64_t capability);
#ifdef __cplusplus
}
#endif
#endif
