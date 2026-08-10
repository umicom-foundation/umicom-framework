/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/command_registry.h
 *
 * PURPOSE:
 *   Define one authoritative registry for commands used by menus, toolbars,
 *   keyboard shortcuts, command palettes, consoles, plug-ins, automation and
 *   AI tools without coupling command behaviour to a graphical toolkit.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_COMMAND_REGISTRY_H
#define UMICOM_RUNTIME_COMMAND_REGISTRY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_COMMAND_ID_CAPACITY 160U
#define UMI_COMMAND_TITLE_CAPACITY 160U
#define UMI_COMMAND_CATEGORY_CAPACITY 96U
#define UMI_COMMAND_DESCRIPTION_CAPACITY 384U
#define UMI_COMMAND_PERMISSION_CAPACITY 160U
#define UMI_COMMAND_REGISTRY_MAX 1024U

typedef enum UmiCommandFlags {
    UMI_COMMAND_NONE = 0U,
    UMI_COMMAND_MUTATES_STATE = 1U << 0,
    UMI_COMMAND_BACKGROUND = 1U << 1,
    UMI_COMMAND_AUDITED = 1U << 2,
    UMI_COMMAND_REQUIRES_TRUST = 1U << 3
} UmiCommandFlags;

typedef UmiStatus (*UmiRegisteredCommandHandler)(void *user_data,
                                       const char *argument,
                                       char *out_message,
                                       size_t message_capacity);
typedef int (*UmiRegisteredCommandEnabledFn)(void *user_data,
                                   const char *argument);

typedef struct UmiCommandDescriptor {
    uint32_t structure_size;
    const char *command_id;
    const char *title;
    const char *category;
    const char *description;
    const char *required_permission;
    uint32_t flags;
    UmiRegisteredCommandHandler handler;
    UmiRegisteredCommandEnabledFn enabled;
    void *user_data;
} UmiCommandDescriptor;

typedef struct UmiCommandSnapshot {
    char command_id[UMI_COMMAND_ID_CAPACITY];
    char title[UMI_COMMAND_TITLE_CAPACITY];
    char category[UMI_COMMAND_CATEGORY_CAPACITY];
    char description[UMI_COMMAND_DESCRIPTION_CAPACITY];
    char required_permission[UMI_COMMAND_PERMISSION_CAPACITY];
    uint32_t flags;
} UmiCommandSnapshot;

typedef struct UmiCommandRegistry UmiCommandRegistry;

UmiStatus umi_command_registry_create(UmiCommandRegistry **out_registry);
void umi_command_registry_destroy(UmiCommandRegistry *registry);
UmiStatus umi_command_registry_register(
    UmiCommandRegistry *registry,
    const UmiCommandDescriptor *descriptor
);
UmiStatus umi_command_registry_unregister(UmiCommandRegistry *registry,
                                          const char *command_id);
UmiStatus umi_command_registry_snapshot(
    const UmiCommandRegistry *registry,
    const char *command_id,
    UmiCommandSnapshot *out_snapshot
);
UmiStatus umi_command_registry_at(const UmiCommandRegistry *registry,
                                  size_t index,
                                  UmiCommandSnapshot *out_snapshot);
size_t umi_command_registry_count(const UmiCommandRegistry *registry);
int umi_command_registry_is_enabled(const UmiCommandRegistry *registry,
                                    const char *command_id,
                                    const char *argument);
UmiStatus umi_command_registry_execute(UmiCommandRegistry *registry,
                                       const char *command_id,
                                       const char *argument,
                                       char *out_message,
                                       size_t message_capacity);

#ifdef __cplusplus
}
#endif

#endif
