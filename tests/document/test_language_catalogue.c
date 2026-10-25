/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_language_catalogue.c
 *
 * PURPOSE:
 *   Check selectable syntax identities against filename inference and immutable ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/languages.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                      \
      return 1;                                                                                    \
    }                                                                                              \
  } while (0)
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const char *name = argv[1];
  const UmiDocumentLanguageChoice *choice = UmiDocumentLanguageChoiceAt(0U), *guard = choice;
  UmiDocumentLanguageIdentity identity, unchanged;
  memset(&unchanged, 0x5a, sizeof unchanged);
  identity = unchanged;
  if (!strcmp(name, "catalogue")) {
    CHECK(UmiDocumentLanguageChoiceCount() > 20U);
    for (size_t i = 0U; i < UmiDocumentLanguageChoiceCount(); ++i) {
      const UmiDocumentLanguageChoice *entry = UmiDocumentLanguageChoiceAt(i);
      CHECK(entry && entry->language_id[0] && entry->display_name[0] && entry->example_filename[0]);
      CHECK(UmiDocumentLanguageChoiceFind(entry->language_id, &choice) == UMI_STATUS_OK &&
            choice == entry);
      CHECK(UmiDocumentLanguageChoiceIdentity(entry->language_id, &identity) == UMI_STATUS_OK &&
            !strcmp(identity.language_id, entry->language_id) && identity.text);
      for (size_t j = 0U; j < i; ++j)
        CHECK(strcmp(entry->language_id, UmiDocumentLanguageChoiceAt(j)->language_id));
    }
  } else if (!strcmp(name, "unknown")) {
    CHECK(UmiDocumentLanguageChoiceFind("Python", &choice) == UMI_STATUS_NOT_FOUND &&
          choice == guard);
    CHECK(UmiDocumentLanguageChoiceIdentity("missing", &identity) == UMI_STATUS_NOT_FOUND &&
          !memcmp(&identity, &unchanged, sizeof identity));
  } else if (!strcmp(name, "bounded")) {
    char bad[UMI_DOCUMENT_LANGUAGE_CAPACITY];
    memset(bad, 'x', sizeof bad);
    CHECK(UmiDocumentLanguageChoiceFind(bad, &choice) == UMI_STATUS_CAPACITY_EXCEEDED &&
          choice == guard);
  } else if (!strcmp(name, "arguments")) {
    CHECK(UmiDocumentLanguageChoiceFind(NULL, &choice) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDocumentLanguageChoiceFind("", &choice) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDocumentLanguageChoiceFind("c", NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDocumentLanguageChoiceIdentity("c", NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDocumentLanguageChoiceAt(UmiDocumentLanguageChoiceCount()) == NULL);
  } else {
    CHECK(UmiDocumentLanguageChoiceIdentity(name, &identity) == UMI_STATUS_OK);
    CHECK(!strcmp(identity.language_id, name));
  }
  return 0;
}
