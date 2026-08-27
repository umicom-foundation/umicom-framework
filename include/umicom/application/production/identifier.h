/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/production/identifier.h
 *
 * PURPOSE:
 *   Publish one bounded contract in the Framework-owned application production
 *   control plane without moving business logic into the Master Controller.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTION_IDENTIFIER_H
#define UMICOM_APPLICATION_PRODUCTION_IDENTIFIER_H

#include "umicom/application/production/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationProductionIdentifier {
    char value[UMI_APPLICATION_PRODUCTION_ID_CAPACITY];
} UmiApplicationProductionIdentifier;

UmiStatus umi_application_production_identifier_set(
    UmiApplicationProductionIdentifier *identifier, const char *value);
int umi_application_production_identifier_valid(
    const UmiApplicationProductionIdentifier *identifier);
int umi_application_production_identifier_equal(
    const UmiApplicationProductionIdentifier *left,
    const UmiApplicationProductionIdentifier *right);

#ifdef __cplusplus
}
#endif
#endif

