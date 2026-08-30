/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/prudential_entity.h
 *
 * PURPOSE:
 *   Represent a regulated legal entity and its prudential reporting perimeter.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_PRUDENTIAL_ENTITY_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_PRUDENTIAL_ENTITY_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPrudentialEntity {
    char entity_id[UMI_PRU_ID_CAPACITY];
    char name[UMI_PRU_NAME_CAPACITY];
    char jurisdiction[UMI_PRU_ID_CAPACITY];
} UmiPrudentialEntity;

/* Initialise a regulated prudential entity with bounded identifiers. */
UmiStatus umi_pru_prudential_entity_init(UmiPrudentialEntity *record, const char *entity_id, const char *display_name, const char *jurisdiction);

#ifdef __cplusplus
}
#endif

#endif
