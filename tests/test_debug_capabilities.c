/* Umicom Framework debugger capability test. Sammy Hegab, Umicom Foundation, MIT. */
#include <assert.h>
#include "umicom/debug/capabilities.h"
int main(void){UmiDebugCapabilitySet s;umi_debug_capability_set_init(&s);assert(umi_debug_capability_set_require(&s,UMI_DEBUG_CAP_RESTART|UMI_DEBUG_CAP_TERMINATE)==UMI_STATUS_OK);assert(umi_debug_capability_set_advertise(&s,UMI_DEBUG_CAP_RESTART|UMI_DEBUG_CAP_TERMINATE)==UMI_STATUS_OK);assert(umi_debug_capability_set_ready(&s));return 0;}
