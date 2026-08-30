/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/productisation/command_catalogue.h
 *
 * PURPOSE:
 *   Publish stable Framework command descriptors for future Studio and Desk
 *   productisation, coverage and completion views.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTISATION_COMMAND_CATALOGUE_H
#define UMICOM_APPLICATION_PRODUCTISATION_COMMAND_CATALOGUE_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiProductisationCommandDefinition {
    const char *command_id;
    const char *title;
    const char *category;
    const char *description;
    int mutates_evidence;
} UmiProductisationCommandDefinition;

size_t umi_productisation_command_catalogue_count(void);
const UmiProductisationCommandDefinition *
umi_productisation_command_catalogue_at(size_t index);
const UmiProductisationCommandDefinition *
umi_productisation_command_catalogue_find(const char *command_id);

#ifdef __cplusplus
}
#endif

#endif
