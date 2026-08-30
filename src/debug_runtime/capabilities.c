/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/capabilities.c
 *
 * PURPOSE:
 *   Implement DAP -> Framework debugger capability negotiation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/capabilities.h"

uint64_t umi_debug_runtime_capability_bits(
    const UmiDebugRuntimeCapabilities *capabilities)
{
    uint64_t bits = 0U;

    if (capabilities == NULL) return 0U;

#define MAP(field, bit) do { if (capabilities->field) bits |= (bit); } while (0)
    MAP(supports_configuration_done, UMI_DEBUG_CAP_CONFIGURATION_DONE);
    MAP(supports_function_breakpoints, UMI_DEBUG_CAP_FUNCTION_BREAKPOINTS);
    MAP(supports_conditional_breakpoints, UMI_DEBUG_CAP_CONDITIONAL_BREAKPOINTS);
    MAP(supports_hit_conditions, UMI_DEBUG_CAP_HIT_CONDITIONS);
    MAP(supports_evaluate_for_hovers, UMI_DEBUG_CAP_EVALUATE_HOVER);
    MAP(supports_set_variable, UMI_DEBUG_CAP_SET_VARIABLE);
    MAP(supports_restart, UMI_DEBUG_CAP_RESTART);
    MAP(supports_terminate_request, UMI_DEBUG_CAP_TERMINATE);
    MAP(supports_modules_request, UMI_DEBUG_CAP_MODULES);
    MAP(supports_read_memory_request, UMI_DEBUG_CAP_READ_MEMORY);
    MAP(supports_disassemble_request, UMI_DEBUG_CAP_DISASSEMBLE);
    MAP(supports_step_back, UMI_DEBUG_CAP_STEP_BACK);
    MAP(supports_data_breakpoints, UMI_DEBUG_CAP_DATA_BREAKPOINTS);
    MAP(supports_instruction_breakpoints, UMI_DEBUG_CAP_INSTRUCTION_BREAKPOINTS);
    MAP(supports_exception_filter_options, UMI_DEBUG_CAP_EXCEPTION_FILTERS);
    MAP(supports_write_memory_request, UMI_DEBUG_CAP_WRITE_MEMORY);
    MAP(supports_set_expression, UMI_DEBUG_CAP_SET_EXPRESSION);
    MAP(supports_restart_frame, UMI_DEBUG_CAP_RESTART_FRAME);
#undef MAP

    /*
     * DAP reverseContinue is covered by the same reverse-execution family even
     * though older adapters often advertise only supportsStepBack.
     */
    if (capabilities->supports_step_back) {
        bits |= UMI_DEBUG_CAP_REVERSE_CONTINUE;
    }

    return bits;
}
