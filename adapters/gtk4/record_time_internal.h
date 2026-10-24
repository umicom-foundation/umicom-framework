/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Shared native provenance time formatting preserves unknown and nanoseconds. */
#ifndef UMICOM_GTK4_RECORD_TIME_INTERNAL_H
#define UMICOM_GTK4_RECORD_TIME_INTERNAL_H
#include <glib.h>
#include <inttypes.h>
#include <stdint.h>
/* Return owned UTF-8 text; no timezone or locale inference changes the instant.
 * uint64 nanoseconds fit GDateTime's supported positive Unix-second range. */
static inline char *umi_gtk_record_time(uint64_t nanoseconds) {
  if (nanoseconds == 0)
    return g_strdup("Unknown");
  GDateTime *date = g_date_time_new_from_unix_utc((gint64)(nanoseconds / 1000000000U));
  if (!date)
    return g_strdup("Unsupported timestamp");
  char *seconds = g_date_time_format(date, "%Y-%m-%d %H:%M:%S");
  char *result = seconds
                     ? g_strdup_printf("%s.%09" PRIu64 " UTC", seconds, nanoseconds % 1000000000U)
                     : g_strdup("Unsupported timestamp");
  g_free(seconds);
  g_date_time_unref(date);
  return result;
}
#endif
