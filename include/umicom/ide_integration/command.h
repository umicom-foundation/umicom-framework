/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/command.h
 *
 * PURPOSE:
 *   Publish stable cross-domain IDE command metadata for menus, keymaps,
 *   command palettes, automation and thin application shells.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_COMMAND_H
#define UMICOM_IDE_INTEGRATION_COMMAND_H

#include "umicom/ide_integration/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiIdeCommandDescriptor {
    const char *command_id;
    const char *label;
    const char *category;
    const char *description;
    int requires_document;
    int requires_problems;
    int requires_tests;
    int requires_source_control;
    int requires_debug;
    int requires_language;
    int requires_ai;
    int requires_inline;
    int mutates_state;
} UmiIdeCommandDescriptor;

size_t umi_ide_command_count(void);
const UmiIdeCommandDescriptor *umi_ide_command_at(size_t index);
const UmiIdeCommandDescriptor *umi_ide_command_find(const char *command_id);

#ifdef __cplusplus
}
#endif
#endif
