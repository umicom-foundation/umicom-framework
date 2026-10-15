/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/risk_book.c
 *
 * PURPOSE:
 *   Implement treasury limit retention, utilisation and breach counts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include "umicom/open_tms/risk_book.h"

#include <string.h>

void umi_open_tms_risk_book_init(UmiOpenTmsRiskBook *book)
{
    if (book == NULL) return;
    (void)memset(book, 0, sizeof(*book));
    book->revision = 1U;
}

const UmiOpenTmsRiskLimit *umi_open_tms_risk_book_find(
    const UmiOpenTmsRiskBook *book,
    const char *id)
{
    size_t index;

    if (book == NULL || id == NULL) return NULL;
    for (index = 0U; index < book->count; ++index) {
        if (strcmp(book->limits[index].id, id) == 0) {
            return &book->limits[index];
        }
    }
    return NULL;
}

/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
UmiStatus umi_open_tms_risk_book_upsert(
    UmiOpenTmsRiskBook *book,
    const UmiOpenTmsRiskLimit *limit)
{
    size_t index;

    if (book == NULL || limit == NULL || limit->id[0] == '\0' ||
        limit->scopeId[0] == '\0' || limit->limit <= 0.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = 0U; index < book->count; ++index) {
        if (strcmp(book->limits[index].id, limit->id) == 0) {
            if (limit->version < book->limits[index].version) {
                return UMI_STATUS_INVALID_STATE;
            }
            book->limits[index] = *limit;
            book->revision += 1U;
            return UMI_STATUS_OK;
        }
    }

    if (book->count >= UMI_OPEN_TMS_RISK_LIMIT_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    book->limits[book->count++] = *limit;
    book->revision += 1U;
    return UMI_STATUS_OK;
}
#endif

UmiStatus umi_open_tms_risk_book_upsert(
    UmiOpenTmsRiskBook *book,
    const UmiOpenTmsRiskLimit *limit)
{
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (book != NULL && book->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    size_t index;

    if (book == NULL || limit == NULL || limit->id[0] == '\0' ||
        limit->scopeId[0] == '\0' || limit->limit <= 0.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = 0U; index < book->count; ++index) {
        if (strcmp(book->limits[index].id, limit->id) == 0) {
            if (limit->version < book->limits[index].version) {
                return UMI_STATUS_INVALID_STATE;
            }
            book->limits[index] = *limit;
            book->revision += 1U;
            return UMI_STATUS_OK;
        }
    }

    if (book->count >= UMI_OPEN_TMS_RISK_LIMIT_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    book->limits[book->count++] = *limit;
    book->revision += 1U;
    return UMI_STATUS_OK;
}

double umi_open_tms_risk_limit_utilisation_percent(
    const UmiOpenTmsRiskLimit *limit)
{
    return limit != NULL && limit->limit > 0.0
        ? (limit->utilisation / limit->limit) * 100.0
        : 0.0;
}

size_t umi_open_tms_risk_book_breach_count(const UmiOpenTmsRiskBook *book)
{
    size_t index;
    size_t count = 0U;

    if (book == NULL) return 0U;
    for (index = 0U; index < book->count; ++index) {
        if (book->limits[index].utilisation > book->limits[index].limit) {
            count += 1U;
        }
    }
    return count;
}
