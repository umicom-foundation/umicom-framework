/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience_catalogue.c
 *
 * PURPOSE:
 *   Publish and validate the canonical application experience catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/experience_catalogue.h"

#include <string.h>

#include "umicom/application/experiences/experiences.h"

/* A getter delays catalogue construction and returns Framework-owned metadata. */
typedef const UmiApplicationExperienceDefinition *(*ExperienceGetter)(void);

/* One alias preserves a historical identifier without duplicating an experience. */
typedef struct ExperienceAlias {
    const char *legacy_application_id;
    const char *canonical_application_id;
} ExperienceAlias;

/* This is the single ordered list of application experiences exposed publicly. */
static const ExperienceGetter GETTERS[] = {
    umi_application_experience_studio,
    umi_application_experience_trader,
    umi_application_experience_ai_creator,
    umi_application_experience_bank,
    umi_application_experience_tms,
    umi_application_experience_llm,
    umi_application_experience_exchange,
    umi_application_experience_music,
    umi_application_experience_media,
    umi_application_experience_accountant,
    umi_application_experience_rag,
    umi_application_experience_desktop,
    umi_application_experience_os,
    umi_application_experience_games,
    umi_application_experience_kitchen,
    umi_application_experience_cad,
    umi_application_experience_author,
    umi_application_experience_web_studio,
    umi_application_experience_mobile_studio,
    umi_application_experience_database_studio,
    umi_application_experience_integration_studio,
    umi_application_experience_operations,
    umi_application_experience_security_centre,
    umi_application_experience_marketplace,
    umi_application_experience_education
};

/*
 * Preserve identifiers published by earlier application manifests.  Aliases
 * are resolved only at the catalogue boundary, so every downstream component
 * still receives one canonical experience definition.
 */
static const ExperienceAlias ALIASES[] = {
    { "org.umicom.music", "org.umicom.music-studio" },
    { "org.umicom.ai-creator", "org.umicom.creator" },
    { "org.umicom.desk", "org.umicom.desktop" }
};

#define COUNT_OF(values) (sizeof(values) / sizeof((values)[0]))

/* Return the fixed canonical experience count without exposing the getter array. */
size_t umi_application_experience_catalogue_count(void)
{
    return COUNT_OF(GETTERS);
}

/* Borrow one canonical definition, returning NULL instead of reading past storage. */
const UmiApplicationExperienceDefinition *
umi_application_experience_catalogue_at(size_t index)
{
    return index < COUNT_OF(GETTERS) ? GETTERS[index]() : NULL;
}

/* Resolve historical IDs once, then search only canonical immutable definitions. */
const UmiApplicationExperienceDefinition *
umi_application_experience_catalogue_find(const char *application_id)
{
    size_t index;
    const char *canonical_id = application_id;
    /* NULL cannot name either a canonical experience or a supported alias. */
    if (application_id == NULL) {
        return NULL;
    }

    /* Translate a known historical identifier before searching the catalogue. */
    for (index = 0U; index < COUNT_OF(ALIASES); ++index) {
        /* Exact alias equality selects the one canonical replacement ID. */
        if (strcmp(ALIASES[index].legacy_application_id, application_id) == 0) {
            canonical_id = ALIASES[index].canonical_application_id;
            break;
        }
    }

    /* Return the immutable canonical definition used by every thin client. */
    for (index = 0U; index < COUNT_OF(GETTERS); ++index) {
        const UmiApplicationExperienceDefinition *definition = GETTERS[index]();
        /* Skip an unavailable definition defensively instead of dereferencing it. */
        if (definition == NULL) {
            continue;
        }
        /* Exact canonical equality returns one unambiguous experience. */
        if (strcmp(definition->application_id, canonical_id) == 0) {
            return definition;
        }
    }
    return NULL;
}

/* Validate every nested contract and ensure canonical application IDs are unique. */
UmiStatus umi_application_experience_catalogue_validate(void)
{
    size_t index;
    size_t nested;
    /* Validate each definition before comparing its identity with later entries. */
    for (index = 0U; index < COUNT_OF(GETTERS); ++index) {
        const UmiApplicationExperienceDefinition *definition = GETTERS[index]();
        UmiStatus status = umi_application_experience_validate(definition);
        /* Return the precise first contract failure so diagnostics remain useful. */
        if (status != UMI_STATUS_OK) {
            return status;
        }
        /* Pairwise comparison is acceptable for the small fixed catalogue. */
        for (nested = index + 1U; nested < COUNT_OF(GETTERS); ++nested) {
            const UmiApplicationExperienceDefinition *other = GETTERS[nested]();
            /* A missing later definition is invalid even before its own iteration. */
            if (other == NULL) {
                return UMI_STATUS_INVALID_ARGUMENT;
            }
            /* Duplicate IDs would make application lookup order-dependent. */
            if (strcmp(definition->application_id,
                       other->application_id) == 0) {
                return UMI_STATUS_ALREADY_EXISTS;
            }
        }
    }
    return UMI_STATUS_OK;
}
