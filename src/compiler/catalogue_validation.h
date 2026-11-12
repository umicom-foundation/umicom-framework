/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/catalogue_validation.h
 *
 * PURPOSE:
 *   Share bounded compiler record admission before catalogue lookup and publication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_COMPILER_CATALOGUE_VALIDATION_H
#define UMICOM_COMPILER_CATALOGUE_VALIDATION_H
#include "umicom/compiler/provider.h"
#include "umicom/compiler/abi.h"

/* Scan at most the declared storage. Optional fields may be empty; this helper
 * establishes termination, not path existence or compiler support. */
static inline bool UmiCompilerCatalogueText(const char *text,size_t capacity,bool required)
{
    if (text == NULL || capacity == 0U || (required && text[0] == '\0')) return false;
    for (size_t index = 0U; index < capacity; ++index)
        if (text[index] == '\0') return true;
    return false;
}

/* Compatibility retains its existing architecture/OS/format/width policy.
 * Bound every fixed text field because selected records are returned to clients. */
static inline bool UmiCompilerCatalogueTarget(const UmiCompilerTarget *target)
{
    return target != NULL &&
        UmiCompilerCatalogueText(target->triple,sizeof(target->triple),false) &&
        UmiCompilerCatalogueText(target->architecture,sizeof(target->architecture),false) &&
        UmiCompilerCatalogueText(target->vendor,sizeof(target->vendor),false) &&
        UmiCompilerCatalogueText(target->operating_system,sizeof(target->operating_system),false) &&
        UmiCompilerCatalogueText(target->environment,sizeof(target->environment),false);
}

static inline bool UmiCompilerCatalogueAbi(const UmiCompilerAbiProfile *abi)
{
    return abi != NULL &&
        UmiCompilerCatalogueText(abi->abi_id,sizeof(abi->abi_id),false) &&
        UmiCompilerCatalogueTarget(&abi->target);
}

static inline bool UmiCompilerCatalogueProvider(const UmiCompilerProvider *provider)
{
    if (provider == NULL) return false;
    const UmiCompilerProviderDescriptor *descriptor = &provider->descriptor;
    return UmiCompilerCatalogueText(descriptor->provider_id,sizeof(descriptor->provider_id),true) &&
        UmiCompilerCatalogueText(descriptor->display_name,sizeof(descriptor->display_name),false) &&
        UmiCompilerCatalogueText(descriptor->executable,sizeof(descriptor->executable),false) &&
        UmiCompilerCatalogueText(descriptor->linker_executable,sizeof(descriptor->linker_executable),false) &&
        UmiCompilerCatalogueText(descriptor->default_target,sizeof(descriptor->default_target),false);
}
#endif
