/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience_catalogue.c
 *
 * PURPOSE:
 *   Publish and validate the canonical application experience catalogue.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/experience_catalogue.h"

#include <string.h>

#include "umicom/application/experiences/experiences.h"

typedef const UmiApplicationExperienceDefinition *(*ExperienceGetter)(void);

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

#define COUNT_OF(values) (sizeof(values) / sizeof((values)[0]))

size_t umi_application_experience_catalogue_count(void)
{
    return COUNT_OF(GETTERS);
}

const UmiApplicationExperienceDefinition *
umi_application_experience_catalogue_at(size_t index)
{
    return index < COUNT_OF(GETTERS) ? GETTERS[index]() : NULL;
}

const UmiApplicationExperienceDefinition *
umi_application_experience_catalogue_find(const char *application_id)
{
    size_t index;
    if (application_id == NULL) return NULL;
    for (index = 0U; index < COUNT_OF(GETTERS); ++index) {
        const UmiApplicationExperienceDefinition *definition = GETTERS[index]();
        if (strcmp(definition->application_id, application_id) == 0)
            return definition;
    }
    return NULL;
}

UmiStatus umi_application_experience_catalogue_validate(void)
{
    size_t index;
    size_t nested;
    for (index = 0U; index < COUNT_OF(GETTERS); ++index) {
        const UmiApplicationExperienceDefinition *definition = GETTERS[index]();
        UmiStatus status = umi_application_experience_validate(definition);
        if (status != UMI_STATUS_OK) return status;
        for (nested = index + 1U; nested < COUNT_OF(GETTERS); ++nested) {
            if (strcmp(definition->application_id,
                       GETTERS[nested]()->application_id) == 0)
                return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    return UMI_STATUS_OK;
}
