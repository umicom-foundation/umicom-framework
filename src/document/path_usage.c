/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/path_usage.c
 *
 * PURPOSE:
 *   Check all working-copy identities without modifying drafts or tab focus.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/path_usage.h"
static int Covers(const char *parent, const char *entry) {
  return umi_path_equal(parent, entry) || umi_path_is_within(parent, entry);
}
UmiStatus UmiDocumentCoordinatorPathInUse(const UmiDocumentCoordinator *coordinator,
                                          const char *source, const char *destination,
                                          int *outInUse) {
  if (outInUse == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *outInUse = 0;
  if (coordinator == NULL || source == NULL || !umi_path_is_absolute(source) ||
      (destination != NULL && !umi_path_is_absolute(destination)))
    return UMI_STATUS_INVALID_ARGUMENT;
  char from[UMI_PATH_CAPACITY], to[UMI_PATH_CAPACITY];
  UmiStatus status = umi_path_normalise(source, from, sizeof from);
  if (status != UMI_STATUS_OK)
    return status;
  if (destination != NULL) {
    status = umi_path_normalise(destination, to, sizeof to);
    if (status != UMI_STATUS_OK)
      return status;
  }
  int inUse = 0;
  const size_t count = umi_document_coordinator_count(coordinator);
  for (size_t i = 0U; i < count; ++i) {
    UmiDocumentWorkingCopySnapshot document;
    status = umi_document_coordinator_at(coordinator, i, &document);
    if (status != UMI_STATUS_OK)
      return status;
    if (document.has_path &&
        (Covers(from, document.path) || (destination != NULL && Covers(to, document.path))))
      inUse = 1;
  }
  *outInUse = inUse;
  return UMI_STATUS_OK;
}
