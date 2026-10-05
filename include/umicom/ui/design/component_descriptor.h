/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/component_descriptor.h
 *
 * PURPOSE:
 *   Describe a semantic component using canonical Umicom component kinds plus design-system metadata.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_UI_DESIGN_COMPONENT_DESCRIPTOR_H
#define INCLUDE_UMICOM_UI_DESIGN_COMPONENT_DESCRIPTOR_H

#include "umicom/ui/design/types.h"
#include "umicom/base/value_archive.h"
#include "umicom/ui/design/semantic_role.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/ui/design/semantic_role.h"
/**
 * Represent the design component descriptor data shared with callers of this public
 * contract.
 */
typedef struct UmiDesignComponentDescriptor { char id[UMI_DESIGN_ID_CAPACITY]; char display_name[UMI_DESIGN_NAME_CAPACITY]; UmiUiComponentKind kind; UmiDesignSemanticRole default_role; uint32_t capability_flags; int interactive; } UmiDesignComponentDescriptor;
/* Initialise a semantic descriptor that augments an existing canonical component kind. */
UmiStatus umi_design_component_descriptor_init(UmiDesignComponentDescriptor *descriptor, const char *id, const char *display_name, UmiUiComponentKind kind, UmiDesignSemanticRole default_role, int interactive);
/* Return one when the descriptor can participate in the component catalogue. */
int umi_design_component_descriptor_valid(const UmiDesignComponentDescriptor *descriptor);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_design_component_descriptor_archive_encode(const UmiDesignComponentDescriptor *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_design_component_descriptor_archive_decode(const void *bytes, size_t byte_count,
    UmiDesignComponentDescriptor *value);

#ifdef __cplusplus
}
#endif

#endif
