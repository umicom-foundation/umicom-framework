/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/provider.c
 *
 * PURPOSE:
 *   Validate required source-control provider operations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/vcs/provider.h"

UmiStatus umi_vcs_provider_validate(const UmiVcsProvider *provider)
{
    if (provider == NULL || provider->provider_id == NULL ||
        provider->provider_id[0] == '\0' ||
        provider->status == NULL ||
        provider->history == NULL ||
        provider->stage == NULL ||
        provider->unstage == NULL ||
        provider->commit == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}
