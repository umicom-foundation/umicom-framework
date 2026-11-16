/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/media_generation/comfy_numeric_input.h
 *
 * PURPOSE:
 *   Copy observed INT and FLOAT input constraints without changing the raw ComfyUI node catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_MEDIA_GENERATION_COMFY_NUMERIC_INPUT_H
#define UMICOM_MEDIA_GENERATION_COMFY_NUMERIC_INPUT_H
#include "umicom/media_generation/comfy_nodes.h"
#include <stdbool.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiComfyNumberKind
{
    UMI_COMFY_NUMBER_INTEGER = 1,
    UMI_COMFY_NUMBER_DECIMAL = 2
} UmiComfyNumberKind;
typedef enum UmiComfyNumberProperty
{
    UMI_COMFY_NUMBER_DEFAULT = 0,
    UMI_COMFY_NUMBER_MINIMUM = 1,
    UMI_COMFY_NUMBER_MAXIMUM = 2,
    UMI_COMFY_NUMBER_STEP = 3
} UmiComfyNumberProperty;
#define UMI_COMFY_NUMBER_FORCE_INPUT 1U

typedef struct UmiComfyNumericInput
{
    char name[129];
    UmiComfyNumberKind kind;
    /* A set bit (1U << UmiComfyNumberProperty) means the corresponding array
     * element was present and decoded. Zero without a bit means unknown. */
    unsigned known_values;
    unsigned known_flags, enabled_flags;
    size_t uninterpreted_options;
    bool inconsistent_values;
    union
    {
        int64_t integers[4];
        double decimals[4];
    } values;
} UmiComfyNumericInput;

/* Interpret a selected required/optional descriptor from this immutable owner.
 * Supported shape: ["INT"] or ["FLOAT"], optionally followed by an options
 * object. Observe default/min/max/step and boolean forceInput only. No default,
 * limit, step or flag is invented when absent. Other options are counted and
 * remain available through the unchanged raw InputAt API.
 *
 * INT requires exact signed-64-bit integer spelling; FLOAT uses the shared
 * finite, locale-independent, bounded double reader. Integer overflow or
 * unsupported floating magnitude/length returns CAPACITY_EXCEEDED.
 * Valid but contradictory min/max/default/step values are retained with
 * inconsistent_values=true (reversed bounds, default outside known bounds,
 * nonpositive step). This flag is an observation, not server validation.
 *
 * Other types, choices, object descriptors and extra array elements return
 * NOT_IMPLEMENTED for this profile. Malformed supported shapes/types return
 * PARSE_ERROR. Invalid arguments return INVALID_ARGUMENT; missing indices give
 * NOT_FOUND. Every failure preserves out. Success copies all values: the report
 * may outlive the catalogue. Serialize reads with catalogue destruction and do
 * not overlap out with owned storage.
 *
 * Missing flags remain unknown; forceInput=true describes a socket preference,
 * not permission to render or execute a node. Custom validation, hidden inputs,
 * unsigned 64-bit seeds, widget creation and provider execution are not supplied.
 * Inspect the catalogue review separately to retain source/connection identity. */
UmiStatus UmiComfyNodeCatalogueNumericInput(const UmiComfyNodeCatalogue *catalogue,
    size_t node_index, UmiComfyInputGroup group, size_t input_index,
    UmiComfyNumericInput *out);
#ifdef __cplusplus
}
#endif
#endif
