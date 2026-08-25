/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/application_identity.h
 *
 * PURPOSE:
 *   stable application identity, publisher and product-family metadata.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_APPLICATION_IDENTITY_H
#define UMICOM_DISTRIBUTION_RUNTIME_APPLICATION_IDENTITY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDrApplicationIdentity { char id[UMI_DR_ID_CAPACITY]; char publisher[UMI_DR_TEXT_CAPACITY]; char family[UMI_DR_ID_CAPACITY]; char product[UMI_DR_TEXT_CAPACITY]; } UmiDrApplicationIdentity;
void umi_dr_application_identity_init(UmiDrApplicationIdentity *value);
bool umi_dr_application_identity_valid(const UmiDrApplicationIdentity *value);
uint64_t umi_dr_application_identity_fingerprint(const UmiDrApplicationIdentity *value);

#ifdef __cplusplus
}
#endif
#endif
