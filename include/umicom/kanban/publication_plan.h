/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/kanban/publication_plan.h
 *
 * PURPOSE:
 *   Keep reviewed public fields as an immutable private plan, separate from the board archive.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_KANBAN_PUBLICATION_PLAN_H
#define UMICOM_KANBAN_PUBLICATION_PLAN_H
#include "umicom/kanban/publication.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_KANBAN_PUBLIC_PLAN_MAXIMUM_BYTES 2097152U
typedef struct UmiKanbanPublicationPlan UmiKanbanPublicationPlan;
typedef struct UmiKanbanPublicationPlanInfo {
  char board_id[UMI_KANBAN_ID_SIZE];
  char title[256];
  uint64_t reviewed_revision;
  size_t card_count;
} UmiKanbanPublicationPlanInfo;
/* Capture only caller-reviewed public fields. The plan owns its strings and
 * cards; later board changes cannot change them. No private title, description,
 * developer ID or activity is copied. The stored board/card IDs are PRIVATE
 * linkage metadata: publish generated HTML, never the plan file.
 * All board access is serialized by its owner. out becomes NULL on failure. */
UmiStatus UmiKanbanPublicationPlanCreate(const UmiKanbanBoard *board, uint64_t expected_revision,
                                         const char *title, const UmiKanbanPublicCard *cards,
                                         size_t count, UmiKanbanPublicationPlan **out);
void UmiKanbanPublicationPlanDestroy(UmiKanbanPublicationPlan *plan);
UmiStatus UmiKanbanPublicationPlanRead(const UmiKanbanPublicationPlan *plan,
                                       UmiKanbanPublicationPlanInfo *out);
const UmiKanbanPublicCard *UmiKanbanPublicationPlanAt(const UmiKanbanPublicationPlan *plan,
                                                      size_t index);
/* Check identity, exact reviewed revision and active selected cards. A different
 * board with the same human title is refused. IDs/revisions are local metadata,
 * not signatures, credentials or proof of content authenticity. */
UmiStatus UmiKanbanPublicationPlanCheck(const UmiKanbanPublicationPlan *plan,
                                        const UmiKanbanBoard *board, uint64_t expected_revision);
/* Explicit owner review may adopt old public fields at the current revision.
 * Require the SAME board ID and existing active cards. This never copies newer
 * private fields, changes the old plan, generates HTML or publishes anything. */
UmiStatus UmiKanbanPublicationPlanReview(const UmiKanbanPublicationPlan *plan,
                                         const UmiKanbanBoard *board, uint64_t expected_revision,
                                         UmiKanbanPublicationPlan **out);
UmiStatus UmiKanbanPublicationPlanHtml(const UmiKanbanPublicationPlan *plan,
                                       const UmiKanbanBoard *board, uint64_t expected_revision,
                                       char *bytes, size_t capacity, size_t *out_size);
/* Complete bounded value archives with checksum, not encryption/authentication.
 * No host padding or unused string tails are stored. Short Encode preserves
 * bytes; NULL,0 measures. Decode owns an independent result or clears out.
 * Inputs and output storage must not overlap. */
UmiStatus UmiKanbanPublicationPlanEncode(const UmiKanbanPublicationPlan *plan, void *bytes,
                                         size_t capacity, size_t *out_size);
UmiStatus UmiKanbanPublicationPlanDecode(const void *bytes, size_t size,
                                         UmiKanbanPublicationPlan **out);
#ifdef __cplusplus
}
#endif
#endif
