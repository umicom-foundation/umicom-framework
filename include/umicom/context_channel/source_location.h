/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/source_location.h
 *
 * PURPOSE:
 *   Define the canonical source location context shared by applications and panels.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_SOURCE_LOCATION_H
#define UMICOM_CONTEXT_CHANNEL_SOURCE_LOCATION_H
#include "umicom/context_channel/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the source location context data shared with callers of this public contract.
 */
typedef struct UmiSourceLocationContext {
    uint32_t structure_size;
    char workspace_id[UMI_CONTEXT_TEXT_CAPACITY];
    char file_path[UMI_CONTEXT_PATH_CAPACITY];
    char symbol[UMI_CONTEXT_TEXT_CAPACITY];
    uint32_t line;
    uint32_t column;
    uint32_t selection_length;
    uint64_t revision;
} UmiSourceLocationContext;
/**
 * Initialise source location context from caller-provided values so later operations
 * receive a known state.
 */
void umi_source_location_context_init(UmiSourceLocationContext *context);
/**
 * Check that source location context satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_source_location_context_validate(const UmiSourceLocationContext *context);
/**
 * Copy source location context into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_source_location_context_copy(UmiSourceLocationContext *destination, const UmiSourceLocationContext *source);
/**
 * Provide the source location context set workspace id operation used by this module and
 * its client applications.
 */
UmiStatus umi_source_location_context_set_workspace_id(UmiSourceLocationContext *context, const char *value);
/**
 * Provide the source location context set file path operation used by this module and its
 * client applications.
 */
UmiStatus umi_source_location_context_set_file_path(UmiSourceLocationContext *context, const char *value);
/**
 * Provide the source location context set symbol operation used by this module and its
 * client applications.
 */
UmiStatus umi_source_location_context_set_symbol(UmiSourceLocationContext *context, const char *value);
/**
 * Provide the source location context set line operation used by this module and its
 * client applications.
 */
UmiStatus umi_source_location_context_set_line(UmiSourceLocationContext *context, uint32_t value);
/**
 * Provide the source location context set column operation used by this module and its
 * client applications.
 */
UmiStatus umi_source_location_context_set_column(UmiSourceLocationContext *context, uint32_t value);
/**
 * Provide the source location context set selection length operation used by this module
 * and its client applications.
 */
UmiStatus umi_source_location_context_set_selection_length(UmiSourceLocationContext *context, uint32_t value);
#ifdef __cplusplus
}
#endif
#endif
