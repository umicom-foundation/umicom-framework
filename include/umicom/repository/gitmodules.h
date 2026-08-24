/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/gitmodules.h
 *
 * PURPOSE:
 *   Parse .gitmodules text into the canonical submodule inventory.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable repository-control capability. Applications
 *   remain thin consumers and must not duplicate this policy or state model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_REPOSITORY_GITMODULES_H
#define UMICOM_REPOSITORY_GITMODULES_H
#include "umicom/repository/inventory.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_repository_gitmodules_parse(
    const char *text,
    UmiRepositoryInventory *out_inventory);
#ifdef __cplusplus
}
#endif
#endif
