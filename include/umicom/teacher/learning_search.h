/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/learning_search.h
 *
 * PURPOSE:
 *   Find copied learning-pack metadata without reading or executing resources.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEACHER_LEARNING_SEARCH_H
#define UMICOM_TEACHER_LEARNING_SEARCH_H
#include "umicom/teacher/learning_pack.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LEARNING_SEARCH_TEXT_CAPACITY 256U
#define UMI_LEARNING_SEARCH_MAX_TERMS 8U
#define UMI_LEARNING_SEARCH_MAX_RESULTS 256U

enum {
    UMI_LEARNING_MATCH_ID = 1U,
    UMI_LEARNING_MATCH_TITLE = 2U,
    UMI_LEARNING_MATCH_TOPIC = 4U,
    UMI_LEARNING_MATCH_PACK_TITLE = 8U
};
typedef struct UmiLearningSearchQuery {
    char text[UMI_LEARNING_SEARCH_TEXT_CAPACITY];
    char product[UMI_TEACHER_ID_CAPACITY];
    char locale[UMI_LEARNING_PACK_LOCALE_CAPACITY];
    /* Zero accepts every kind; otherwise use a UmiLearningResourceKind. */
    unsigned kind;
} UmiLearningSearchQuery;
typedef struct UmiLearningSearchInfo {
    size_t examined_resources;
    size_t matching_resources;
    size_t result_count;
    int truncated;
} UmiLearningSearchInfo;
typedef struct UmiLearningSearchHit {
    UmiLearningPackInfo pack;
    UmiLearningPackResource resource;
    unsigned matched_fields;
} UmiLearningSearchHit;
typedef struct UmiLearningSearch UmiLearningSearch;

/* Search current admitted shelf metadata, with no resource I/O and no stored
 * index. Empty text browses all matching resources. Split text on ASCII spaces;
 * up to eight terms must each occur in an ID, title, topic or pack title. ASCII
 * letters compare case-insensitively; other bytes compare exactly. This is not
 * Unicode normalization, stemming, regular expressions or full-text search.
 * Queries are bounded byte strings; frontends validate their text encoding.
 *
 * Empty product/locale disables that filter. Product uses exact stable identity;
 * locale compares ASCII case-insensitively with no language fallback. Pack
 * compatibility was checked when each pack was admitted. Strings must contain
 * a terminator in their arrays; ASCII controls and DEL are refused.
 *
 * maximum_results is 1..256. Scan at most the shelf's 32*256 resources. Store
 * the first matching results in shelf/manifest order; report the total matches
 * and explicit truncation. No relevance ranking or hidden file scan occurs.
 * Serialize shelf operations throughout the call. A search owns all result
 * metadata independently of the shelf. Failure clears out_search.
 *
 * A new search reflects current packs and removals. An earlier snapshot remains
 * historical copied data; rerun after shelf changes before presenting it as a
 * current catalogue. Opening a hit must resolve its pack and resource again and
 * verify its bytes. Metadata search grants no read, copy or execution trust. */
UmiStatus UmiLearningSearchCreate(const UmiLearningShelf *shelf,
    const UmiLearningSearchQuery *query, size_t maximum_results, UmiLearningSearch **out_search);
void UmiLearningSearchDestroy(UmiLearningSearch *search);
UmiStatus UmiLearningSearchCopyInfo(const UmiLearningSearch *search, UmiLearningSearchInfo *out);
UmiStatus UmiLearningSearchResultAt(const UmiLearningSearch *search, size_t index,
    UmiLearningSearchHit *out);
#ifdef __cplusplus
}
#endif
#endif
