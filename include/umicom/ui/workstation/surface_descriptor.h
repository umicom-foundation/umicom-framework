/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/surface_descriptor.h
 *
 * PURPOSE:
 *   Define reusable semantic metadata for dockable workstation surfaces across every Umicom application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_SURFACE_DESCRIPTOR_H
#define UMICOM_UI_WORKSTATION_SURFACE_DESCRIPTOR_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ws surface descriptor data shared with callers of this public contract.
 */
typedef struct UmiWsSurfaceDescriptor {
    uint32_t api_version;
    char surface_id[UMI_UI_ID_CAPACITY];
    char label[UMI_UI_TEXT_CAPACITY];
    char category[UMI_UI_TEXT_CAPACITY];
    char icon_name[UMI_UI_ICON_CAPACITY];
    UmiWsApplicationDomain domain;
    UmiWsSurfaceKind kind;
    UmiWsDockRegion default_region;
    int32_t minimum_width;
    int32_t minimum_height;
    bool closable;
    bool movable;
    bool detachable;
    bool multi_instance;
} UmiWsSurfaceDescriptor;

/* Initialise a descriptor with stable defaults suitable for a workbench catalogue. */
UmiStatus umi_ws_surface_descriptor_init(UmiWsSurfaceDescriptor *descriptor,
                                         const char *surface_id,
                                         const char *label,
                                         UmiWsApplicationDomain domain,
                                         UmiWsSurfaceKind kind);
/* Validate identity, geometry and required presentation metadata. */
UmiStatus umi_ws_surface_descriptor_validate(const UmiWsSurfaceDescriptor *descriptor);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ws_surface_descriptor_archive_encode(const UmiWsSurfaceDescriptor *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ws_surface_descriptor_archive_decode(const void *bytes, size_t byte_count,
    UmiWsSurfaceDescriptor *value);

#ifdef __cplusplus
}
#endif

#endif
