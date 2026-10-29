/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/learning_help.h
 *
 * PURPOSE:
 *   Resolve explicit product help identities to copied learning-pack metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEACHER_LEARNING_HELP_H
#define UMICOM_TEACHER_LEARNING_HELP_H
#include "umicom/teacher/learning_pack.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LEARNING_HELP_MAX_ROUTES 128U
#define UMI_LEARNING_HELP_KEY_CAPACITY 128U

typedef enum UmiLearningHelpKind {
    UMI_LEARNING_HELP_COMMAND = 1,
    UMI_LEARNING_HELP_DIAGNOSTIC = 2,
    UMI_LEARNING_HELP_TOPIC = 3
} UmiLearningHelpKind;
typedef struct UmiLearningHelpRequest {
    char product[UMI_TEACHER_ID_CAPACITY];
    char locale[UMI_LEARNING_PACK_LOCALE_CAPACITY];
    UmiLearningHelpKind kind;
    char key[UMI_LEARNING_HELP_KEY_CAPACITY];
} UmiLearningHelpRequest;
typedef struct UmiLearningHelpRoute {
    UmiLearningHelpRequest request;
    char pack_id[UMI_TEACHER_ID_CAPACITY];
    char resource_id[UMI_TEACHER_ID_CAPACITY];
} UmiLearningHelpRoute;
typedef struct UmiLearningHelpSelection {
    UmiLearningHelpRoute route;
    UmiLearningPackInfo pack;
    UmiLearningPackResource resource;
} UmiLearningHelpSelection;
typedef struct UmiLearningHelpMap UmiLearningHelpMap;

/* Copy zero to 128 explicit routes into an immutable owner. Strings must be
 * terminated within their arrays and nonempty. Product, key and target IDs use
 * ASCII letters/digits followed by letters/digits, '.', '-' or '_'. Locale uses
 * nonempty alphanumeric segments separated by '-'; its ASCII case is folded.
 * Other identities compare exactly. No wildcard, URL, implicit locale fallback
 * or command execution is supported. Duplicate request identities are refused
 * with ALREADY_EXISTS, even when their targets agree. Failure clears out_map.
 * The creating application owns routing policy, not the untrusted guide text.
 * Input routes may be released on return. Concurrent reads require map lifetime
 * ownership; destruction must not race them. */
UmiStatus UmiLearningHelpMapCreate(const UmiLearningHelpRoute *routes, size_t count,
    UmiLearningHelpMap **out_map);
void UmiLearningHelpMapDestroy(UmiLearningHelpMap *map);
size_t UmiLearningHelpMapCount(const UmiLearningHelpMap *map);
UmiStatus UmiLearningHelpMapRouteAt(const UmiLearningHelpMap *map, size_t index,
    UmiLearningHelpRoute *out_route);

/* Resolve against the current shelf with no filesystem access. The selected
 * pack must match the requested locale exactly after ASCII folding; a missing
 * mapping/pack/resource returns NOT_FOUND and a locale mismatch UNAVAILABLE.
 * A help target must be a GUIDE or LESSON; other kinds return INVALID_STATE.
 * A product may deliberately route to a shared Framework pack. No matching by
 * pack product, title, substring or current working directory takes place.
 *
 * Serialize shelf changes throughout Resolve. The returned selection owns all
 * its metadata and can outlive both map and shelf. It describes the pack at
 * resolution time. Opening later must recheck pack ID, revision, resource ID,
 * digest and actual bytes, for example with UmiLearningReaderCreateFromShelf.
 * A route does not authenticate content or grant execution trust. All copy and
 * resolve refusals preserve the caller's output. */
UmiStatus UmiLearningHelpResolve(const UmiLearningHelpMap *map, const UmiLearningShelf *shelf,
    const UmiLearningHelpRequest *request, UmiLearningHelpSelection *out_selection);
#ifdef __cplusplus
}
#endif
#endif
