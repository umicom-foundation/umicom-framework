/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/optional_binding.c
 *
 * PURPOSE:
 *   Implement the optional binding behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/optional_binding.c
 *
 * PURPOSE:
 *   Resolve optional dependency presence and fallback decisions explicitly.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/optional_binding.h"


void *umi_bootstrap_optional_binding_choose(void *resolved_instance,
                                            void *fallback_instance) {
    return resolved_instance != NULL ? resolved_instance : fallback_instance;
}
