/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/learning_pack.h
 *
 * PURPOSE:
 *   Discover versioned offline learning resources and verify their bytes before
 *   a reader displays them or a learner copies a practice example.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEACHER_LEARNING_PACK_H
#define UMICOM_TEACHER_LEARNING_PACK_H
#include "umicom/teacher/types.h"
#include "umicom/base/version.h"
#include "umicom/base/sha256.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_LEARNING_PACK_MANIFEST_BYTES (256U * 1024U)
#define UMI_LEARNING_PACK_MAX_RESOURCES 256U
#define UMI_LEARNING_PACK_LOCALE_CAPACITY 32U
#define UMI_LEARNING_SHELF_MAX_PACKS 32U

typedef enum UmiLearningResourceKind {
    UMI_LEARNING_RESOURCE_LESSON = 1,
    UMI_LEARNING_RESOURCE_GUIDE = 2,
    UMI_LEARNING_RESOURCE_EXAMPLE = 3,
    UMI_LEARNING_RESOURCE_IMAGE = 4
} UmiLearningResourceKind;

typedef struct UmiLearningPackInfo {
    char id[UMI_TEACHER_ID_CAPACITY];
    char title[UMI_TEACHER_TEXT_CAPACITY];
    char product[UMI_TEACHER_ID_CAPACITY];
    char locale[UMI_LEARNING_PACK_LOCALE_CAPACITY];
    uint64_t revision;
    UmiVersion minimum_version;
    UmiVersion maximum_version;
    size_t resource_count;
} UmiLearningPackInfo;

typedef struct UmiLearningPackResource {
    char id[UMI_TEACHER_ID_CAPACITY];
    char title[UMI_TEACHER_TEXT_CAPACITY];
    char topic[UMI_TEACHER_ID_CAPACITY];
    UmiLearningResourceKind kind;
    char path[UMI_TEACHER_PATH_CAPACITY];
    char sha256[UMI_SHA256_HEX_CAPACITY];
    /* Empty, or the identifier of an EXAMPLE in the same pack. This is a
     * navigation link, never authority to execute or overwrite a project. */
    char example_id[UMI_TEACHER_ID_CAPACITY];
} UmiLearningPackResource;

typedef struct UmiLearningPack UmiLearningPack;
typedef struct UmiLearningShelf UmiLearningShelf;

/* Parse schema umicom.learning.pack version 1 under fixed byte/node/depth limits.
 * Copy all metadata; JSON may be released on return. root is an explicit ordinary
 * absolute directory and is copied. No filesystem access occurs in Create.
 * Resource paths use the rooted-files portable relative-name policy. Reject
 * unknown/duplicate fields, duplicate resource IDs, invalid example references
 * and unsupported schema. host_version is the embedding product version, not
 * automatically the Framework version. Compatibility bounds are inclusive.
 * A version mismatch returns UNAVAILABLE. Success admits metadata, not file
 * integrity or authenticity. Failure clears out_pack. No partial pack is exposed.
 * Immutable packs can be read concurrently while their owner keeps them alive. */
UmiStatus UmiLearningPackCreate(const void *json, size_t length, const char *root,
    UmiVersion host_version, UmiLearningPack **out_pack);

/* Read the manifest below root using bounded rooted-file access, then Create.
 * No directory scan, working-directory fallback, download or execution occurs.
 * Filesystem calls may block; interactive clients should use a worker. */
UmiStatus UmiLearningPackLoad(const char *root, const char *manifest_path,
    UmiVersion host_version, UmiLearningPack **out_pack);
void UmiLearningPackDestroy(UmiLearningPack *pack);

/* Copy pointer-free metadata. Invalid/missing results leave output unchanged. */
UmiStatus UmiLearningPackCopyInfo(const UmiLearningPack *pack, UmiLearningPackInfo *out);
UmiStatus UmiLearningPackResourceAt(const UmiLearningPack *pack, size_t index,
    UmiLearningPackResource *out);
UmiStatus UmiLearningPackFindResource(const UmiLearningPack *pack, const char *id,
    UmiLearningPackResource *out);

/* Return exactly the bytes checked against the manifest's SHA-256, not a path
 * that a reader would reopen. maximum_bytes is a caller-chosen allocation limit.
 * Hash mismatch returns INVALID_STATE. Failure clears both outputs. Free with
 * UmiLearningPackFreeBytes. Hashes detect changes; they do not establish a signer.
 * Rooted storage rejects links/reparse points below the root; ancestors above it
 * are trusted. Reading a file grants no build/run trust. Binary data is preserved
 * with an extra NUL; the frontend owns content-type and rendering policy. */
UmiStatus UmiLearningPackReadResource(const UmiLearningPack *pack, const char *id,
    size_t maximum_bytes, unsigned char **out_bytes, size_t *out_size);
void UmiLearningPackFreeBytes(void *bytes);

/* A shelf owns up to 32 immutable packs. Serialize shelf operations, including
 * reads against removal/destruction. Adoption transfers ownership only on OK,
 * clearing *pack; duplicate pack IDs return ALREADY_EXISTS and preserve it.
 * Pack IDs must be unique even across locales/revisions. Remove before replacing
 * a pack, or prepare a separate shelf and swap it in at an application boundary.
 * The existing foundations curriculum and lesson-library APIs remain available. */
UmiStatus UmiLearningShelfCreate(UmiLearningShelf **out_shelf);
void UmiLearningShelfDestroy(UmiLearningShelf *shelf);
UmiStatus UmiLearningShelfAdopt(UmiLearningShelf *shelf, UmiLearningPack **pack);
UmiStatus UmiLearningShelfRemove(UmiLearningShelf *shelf, const char *pack_id);
/* Copy one admitted pack, including its resource metadata and root, without
 * removing it from the shelf. The caller owns the independent copy on OK;
 * failure clears *out_pack. No file is read and no new trust is granted.
 * Serialize this call against shelf mutation. The copy may subsequently be
 * transferred to a worker or reader, with no further shelf lifetime dependency. */
UmiStatus UmiLearningShelfCopyPack(const UmiLearningShelf *shelf, const char *pack_id,
    UmiLearningPack **out_pack);
size_t UmiLearningShelfCount(const UmiLearningShelf *shelf);
UmiStatus UmiLearningShelfPackAt(const UmiLearningShelf *shelf, size_t index,
    UmiLearningPackInfo *out);
/* Enumerate one admitted pack using the same copied-output contract. */
UmiStatus UmiLearningShelfResourceAt(const UmiLearningShelf *shelf, const char *pack_id,
    size_t index, UmiLearningPackResource *out);
UmiStatus UmiLearningShelfFindResource(const UmiLearningShelf *shelf, const char *pack_id,
    const char *resource_id, UmiLearningPackResource *out);
UmiStatus UmiLearningShelfReadResource(const UmiLearningShelf *shelf, const char *pack_id,
    const char *resource_id, size_t maximum_bytes, unsigned char **out_bytes, size_t *out_size);

#ifdef __cplusplus
}
#endif
#endif
