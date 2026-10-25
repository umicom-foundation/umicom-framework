/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/editor_config_pattern.c
 *
 * PURPOSE:
 *   Match bounded project configuration patterns without recursive wildcard backtracking.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/editor_config_pattern.h"
#include "editor_config_internal.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CONFIG_PATH_MAXIMUM 2048U
typedef struct PatternWork {
  const char *path;
  size_t variants, steps, cells;
  int matched;
} PatternWork;
static size_t BoundedLength(const char *s, size_t limit) {
  size_t n = 0U;
  while (n <= limit && s[n])
    ++n;
  return n;
}
static UmiStatus PathAdmission(const char *s) {
  size_t n = BoundedLength(s, CONFIG_PATH_MAXIMUM);
  if (n > CONFIG_PATH_MAXIMUM)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  if (n == 0U || s[0] == '/' || s[n - 1U] == '/' || strchr(s, '\\') || (n > 1U && s[1] == ':'))
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = UmiEditorConfigTextValidate(s, n);
  if (status != UMI_STATUS_OK)
    return status;
  size_t start = 0U;
  for (size_t i = 0U; i <= n; ++i) {
    if (i < n && (unsigned char)s[i] < 0x20U)
      return UMI_STATUS_INVALID_ARGUMENT;
    if (i == n || s[i] == '/') {
      size_t count = i - start;
      if (count == 0U || (count == 1U && s[start] == '.') ||
          (count == 2U && s[start] == '.' && s[start + 1U] == '.'))
        return UMI_STATUS_INVALID_ARGUMENT;
      start = i + 1U;
    }
  }
  return UMI_STATUS_OK;
}
/* Matching uses two bounded rows, not recursive backtracking. The columns
 * represent Unicode scalars so ? and character classes never split UTF-8. */
static int Number(const char *s, size_t n, long long *out);
static UmiStatus MatchVariant(const char *pattern, PatternWork *work) {
  if (++work->variants > 256U)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  int slash = 0, inside = 0, escaped = 0;
  for (size_t i = 0; pattern[i]; ++i) {
    char c = pattern[i];
    if (!escaped && c == '[')
      inside = 1;
    else if (!escaped && c == ']')
      inside = 0;
    if (c == '/' && !inside)
      slash = 1;
    if (c == '\\' && !escaped)
      escaped = 1;
    else
      escaped = 0;
  }
  const char *path = work->path;
  if (!slash) {
    const char *last = strrchr(path, '/');
    if (last)
      path = last + 1;
  }
  if (pattern[0] == '/')
    ++pattern;
  uint32_t chars[CONFIG_PATH_MAXIMUM + 1U];
  size_t count = 0U;
  for (size_t i = 0; path[i];)
    i += UmiEditorConfigScalar(path + i, &chars[count++]);
  unsigned char previous[CONFIG_PATH_MAXIMUM + 1U] = {0}, next[CONFIG_PATH_MAXIMUM + 1U];
  previous[0] = 1U;
  for (size_t at = 0; pattern[at];) {
    /* A fixed work budget also bounds repeated wide alternatives. Refuse
     * excessive work instead of returning a truncated or guessed match. */
    if (work->cells > 16777216U - (count + 1U))
      return UMI_STATUS_CAPACITY_EXCEEDED;
    work->cells += count + 1U;
    memset(next, 0, count + 1U);
    char token = pattern[at++];
    if (token == '*') {
      int recursive = pattern[at] == '*';
      while (pattern[at] == '*')
        ++at;
      int directory = recursive && pattern[at] == '/';
      if (directory)
        ++at;
      unsigned char reach = 0U;
      for (size_t j = 0; j <= count; ++j) {
        if (previous[j])
          reach = 1U;
        if (directory)
          next[j] = (unsigned char)(previous[j] || (reach && j > 0U && chars[j - 1U] == '/'));
        else
          next[j] = reach;
        if (!recursive && j < count && chars[j] == '/')
          reach = 0U;
      }
    } else if (token == '?') {
      for (size_t j = 0; j < count; ++j)
        if (previous[j] && chars[j] != '/')
          next[j + 1U] = 1U;
    } else if (token == '[') {
      int negate = pattern[at] == '!';
      if (negate)
        ++at;
      size_t start = at;
      while (pattern[at] && pattern[at] != ']')
        ++at;
      if (!pattern[at] || at == start)
        return UMI_STATUS_PARSE_ERROR;
      size_t end = at++;
      for (size_t j = 0; j < count; ++j)
        if (previous[j]) {
          int found = 0;
          for (size_t i = start; i < end;) {
            uint32_t c;
            if (++work->cells > 16777216U)
              return UMI_STATUS_CAPACITY_EXCEEDED;
            i += UmiEditorConfigScalar(pattern + i, &c);
            if (c == chars[j])
              found = 1;
          }
          if (found != negate)
            next[j + 1U] = 1U;
        }
    } else if (token == '{') {
      size_t close = at;
      while (pattern[close] && pattern[close] != '}')
        ++close;
      int range = 0;
      long long first = 0, last = 0;
      if (pattern[close])
        for (size_t dot = at; dot + 1U < close; ++dot)
          if (pattern[dot] == '.' && pattern[dot + 1U] == '.' &&
              Number(pattern + at, dot - at, &first) &&
              Number(pattern + dot + 2U, close - dot - 2U, &last) && first < last) {
            range = 1;
            break;
          }
      if (range) {
        at = close + 1U;
        /* Decode candidate numbers directly: leading zeroes and a
         * sign retain numeric meaning without expanding every value. */
        for (size_t j = 0; j < count; ++j)
          if (previous[j]) {
            size_t k = j;
            int negative = chars[k] == '-';
            if (chars[k] == '-' || chars[k] == '+')
              ++k;
            unsigned long long magnitude = 0U;
            for (; k < count && chars[k] >= '0' && chars[k] <= '9'; ++k) {
              if (++work->cells > 16777216U)
                return UMI_STATUS_CAPACITY_EXCEEDED;
              unsigned digit = (unsigned)(chars[k] - '0');
              if (magnitude > (2147483648ULL - digit) / 10ULL)
                break;
              magnitude = magnitude * 10ULL + digit;
              long long number = negative ? -(long long)magnitude : (long long)magnitude;
              if (number >= first && number <= last)
                next[k + 1U] = 1U;
            }
          }
      } else
        for (size_t j = 0; j < count; ++j)
          if (previous[j] && chars[j] == '{')
            next[j + 1U] = 1U;
    } else {
      size_t start = at - 1U;
      if (token == '\\') {
        if (!pattern[at])
          return UMI_STATUS_PARSE_ERROR;
        start = at;
      }
      uint32_t literal;
      at = start + UmiEditorConfigScalar(pattern + start, &literal);
      for (size_t j = 0; j < count; ++j)
        if (previous[j] && chars[j] == literal)
          next[j + 1U] = 1U;
    }
    memcpy(previous, next, count + 1U);
  }
  if (previous[count])
    work->matched = 1;
  return UMI_STATUS_OK;
}
static int Number(const char *s, size_t n, long long *out) {
  if (n == 0U || n > 12U)
    return 0;
  size_t at = s[0] == '-' ? 1U : 0U;
  if (at == n)
    return 0;
  for (size_t i = at; i < n; ++i)
    if (s[i] < '0' || s[i] > '9')
      return 0;
  char value[13];
  memcpy(value, s, n);
  value[n] = 0;
  char *end = NULL;
  errno = 0;
  long long v = strtoll(value, &end, 10);
  if (errno || end != value + n || v < (-2147483647LL - 1LL) || v > 2147483647LL)
    return 0;
  *out = v;
  return 1;
}
static UmiStatus Expand(const char *pattern, PatternWork *work, unsigned depth);
static UmiStatus Substitute(const char *pattern, size_t open, size_t close, const char *value,
                            size_t bytes, PatternWork *work, unsigned depth) {
  size_t tail = strlen(pattern + close + 1U);
  if (open + bytes + tail > UMI_EDITOR_CONFIG_MAXIMUM_PATTERN)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  char expanded[UMI_EDITOR_CONFIG_MAXIMUM_PATTERN + 1U];
  memcpy(expanded, pattern, open);
  memcpy(expanded + open, value, bytes);
  memcpy(expanded + open + bytes, pattern + close + 1U, tail + 1U);
  return Expand(expanded, work, depth + 1U);
}
static UmiStatus Expand(const char *pattern, PatternWork *work, unsigned depth) {
  if (depth > 8U || ++work->steps > 4096U)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  for (size_t open = 0; pattern[open]; ++open) {
    if (pattern[open] == '\\') {
      if (pattern[open + 1U])
        ++open;
      continue;
    }
    if (pattern[open] == '[') {
      while (pattern[open] && pattern[open] != ']')
        ++open;
      if (!pattern[open])
        return UMI_STATUS_PARSE_ERROR;
      continue;
    }
    if (pattern[open] != '{')
      continue;
    size_t close = open + 1U;
    unsigned nesting = 1U;
    int commas = 0;
    for (; pattern[close]; ++close) {
      if (pattern[close] == '\\') {
        if (pattern[close + 1U])
          ++close;
        continue;
      }
      if (pattern[close] == '[') {
        while (pattern[close] && pattern[close] != ']')
          ++close;
        if (!pattern[close])
          return UMI_STATUS_PARSE_ERROR;
        continue;
      }
      if (pattern[close] == '{')
        ++nesting;
      else if (pattern[close] == '}') {
        if (--nesting == 0U)
          break;
      } else if (pattern[close] == ',' && nesting == 1U)
        commas = 1;
    }
    if (!pattern[close])
      continue; /* An unmatched brace is a literal character. */
    if (commas) {
      size_t start = open + 1U;
      nesting = 0U;
      for (size_t end = start; end <= close; ++end) {
        if (end < close && pattern[end] == '\\') {
          if (end + 1U < close)
            ++end;
          continue;
        }
        if (end < close && pattern[end] == '[') {
          while (end < close && pattern[end] != ']')
            ++end;
          continue;
        }
        if (end < close && pattern[end] == '{')
          ++nesting;
        if (end < close && pattern[end] == '}')
          --nesting;
        if (end == close || (pattern[end] == ',' && nesting == 0U)) {
          UmiStatus status =
              Substitute(pattern, open, close, pattern + start, end - start, work, depth);
          if (status != UMI_STATUS_OK)
            return status;
          start = end + 1U;
        }
      }
      return UMI_STATUS_OK;
    }
    /* Numeric ranges are matched directly by the scalar-row engine.
     * Retain the earlier finite expansion for algorithm review; it could
     * not match leading-zero or explicitly positive candidate numbers. */
#if 0
        for(size_t dot=open+1U;dot+1U<close;++dot)if(pattern[dot]=='.'&&pattern[dot+1U]=='.') {
            long long first,last;
            if(Number(pattern+open+1U,dot-open-1U,&first)&&Number(pattern+dot+2U,close-dot-2U,&last)&&first<last) {
                if(last-first+1LL>256LL)return UMI_STATUS_CAPACITY_EXCEEDED;
                for(long long n=first;n<=last;++n) {
                    char value[24];int bytes=snprintf(value,sizeof value,"%lld",n);
                    if(bytes<0||(size_t)bytes>=sizeof value)return UMI_STATUS_INTERNAL_ERROR;
                    UmiStatus status=Substitute(pattern,open,close,value,(size_t)bytes,work,depth);
                    if(status!=UMI_STATUS_OK)return status;
                }
                return UMI_STATUS_OK;
            }
        }
#endif
  }
  return MatchVariant(pattern, work);
}
UmiStatus UmiEditorConfigPatternMatch(const char *pattern, const char *path, int *out) {
  if (pattern == NULL || path == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  size_t n = BoundedLength(pattern, UMI_EDITOR_CONFIG_MAXIMUM_PATTERN);
  if (n > UMI_EDITOR_CONFIG_MAXIMUM_PATTERN)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  if (n == 0U)
    return UMI_STATUS_PARSE_ERROR;
  UmiStatus status = UmiEditorConfigTextValidate(pattern, n);
  if (status == UMI_STATUS_OK)
    status = PathAdmission(path);
  if (status != UMI_STATUS_OK)
    return status;
  PatternWork work = {path, 0U, 0U, 0U, 0};
  status = Expand(pattern, &work, 0U);
  if (status == UMI_STATUS_OK)
    *out = work.matched;
  return status;
}
