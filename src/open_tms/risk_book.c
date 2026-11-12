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

#include "operational_internal.h"

/* Retained for owner review: these implementations trusted public counts,
 * inline strings and unchecked arithmetic. The active Framework boundary below
 * validates complete owners before traversal and commits results only after
 * finite calculations succeed. Existing entry points remain available. */
#if 0
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

#endif

UmiStatus umi_open_tms_risk_limit_validate(const UmiOpenTmsRiskLimit *limit)
{
    static const UmiSnapshotTextField fields[] = {
        {"id", offsetof(UmiOpenTmsRiskLimit, id), 64U, 1},
        {"scopeId", offsetof(UmiOpenTmsRiskLimit, scopeId), 64U, 1}
    };
    if (limit == NULL || UmiSnapshotValidateTextFields(limit, sizeof(*limit),
            fields, sizeof fields / sizeof fields[0], NULL) != UMI_STATUS_OK ||
        !isfinite(limit->limit) || limit->limit <= 0.0 || !isfinite(limit->utilisation) ||
        (limit->hardLimit != 0 && limit->hardLimit != 1)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
UmiStatus umi_open_tms_risk_book_validate(const UmiOpenTmsRiskBook *book)
{
    if (book == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (book->count > UMI_OPEN_TMS_RISK_LIMIT_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (book->revision == 0U) return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < book->count; ++i) {
        if (umi_open_tms_risk_limit_validate(&book->limits[i]) != UMI_STATUS_OK) return UMI_STATUS_INVALID_STATE;
        for (size_t j = 0U; j < i; ++j)
            if (strcmp(book->limits[i].id, book->limits[j].id) == 0) return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}
void umi_open_tms_risk_book_init(UmiOpenTmsRiskBook *book)
{
    if (book == NULL) return;
    memset(book, 0, sizeof(*book));
    book->revision = 1U;
}
const UmiOpenTmsRiskLimit *umi_open_tms_risk_book_find(const UmiOpenTmsRiskBook *book,
    const char *id)
{
    if (!TmsOpsQuery(id, 64U) || umi_open_tms_risk_book_validate(book) != UMI_STATUS_OK) return NULL;
    for (size_t i = 0U; i < book->count; ++i)
        if (strcmp(book->limits[i].id, id) == 0) return &book->limits[i];
    return NULL;
}
UmiStatus umi_open_tms_risk_book_upsert(UmiOpenTmsRiskBook *book, const UmiOpenTmsRiskLimit *limit)
{
    if (book != NULL && book->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = umi_open_tms_risk_book_validate(book);
    if (status == UMI_STATUS_OK) status = umi_open_tms_risk_limit_validate(limit);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0U; i < book->count; ++i) {
        if (strcmp(book->limits[i].id, limit->id) == 0) {
            if (limit->version < book->limits[i].version) return UMI_STATUS_INVALID_STATE;
            book->limits[i] = *limit;
            ++book->revision;
            return UMI_STATUS_OK;
        }
    }
    if (book->count == UMI_OPEN_TMS_RISK_LIMIT_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    book->limits[book->count++] = *limit;
    ++book->revision;
    return UMI_STATUS_OK;
}
UmiStatus umi_open_tms_risk_limit_utilisation_checked(const UmiOpenTmsRiskLimit *limit,
    double *out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_open_tms_risk_limit_validate(limit);
    if (status != UMI_STATUS_OK) return status;
    double ratio = limit->utilisation / limit->limit;
    if (!isfinite(ratio)) return UMI_STATUS_CAPACITY_EXCEEDED;
    return TmsOpsMultiply(ratio, 100.0, out);
}
UmiStatus umi_open_tms_risk_book_breach_count_checked(const UmiOpenTmsRiskBook *book,
    size_t *out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_open_tms_risk_book_validate(book);
    if (status != UMI_STATUS_OK) return status;
    size_t count = 0U;
    for (size_t i = 0U; i < book->count; ++i)
        if (book->limits[i].utilisation > book->limits[i].limit) ++count;
    *out = count;
    return UMI_STATUS_OK;
}
double umi_open_tms_risk_limit_utilisation_percent(const UmiOpenTmsRiskLimit *limit)
{
    double value;
    return umi_open_tms_risk_limit_utilisation_checked(limit, &value) == UMI_STATUS_OK ? value : 0.0;
}
size_t umi_open_tms_risk_book_breach_count(const UmiOpenTmsRiskBook *book)
{
    size_t value;
    return umi_open_tms_risk_book_breach_count_checked(book, &value) == UMI_STATUS_OK ? value : 0U;
}
