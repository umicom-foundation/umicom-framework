/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/configuration_source.c
 *
 * PURPOSE:
 *   Describe trusted configuration sources and their precedence.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/configuration_source.h"


#include <string.h>
UmiStatus umi_bootstrap_configuration_source_init(
    UmiBootstrapConfigurationSource *source,
    const char *source_id,
    int32_t precedence,
    bool trusted) {
    UmiStatus status;
    if (source == NULL || !umi_bootstrap_id_valid(source_id))
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(source, 0, sizeof(*source));
    status = umi_bootstrap_copy_text(source->source_id, sizeof(source->source_id), source_id);
    if (status != UMI_STATUS_OK) return status;
    source->precedence = precedence;
    source->trusted = trusted;
    return UMI_STATUS_OK;
}
