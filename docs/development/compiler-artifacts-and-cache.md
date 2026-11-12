![Umicom](../../resources/brand/umicom-logo.svg)

# Retain compiler artifacts without trusting a cache hit

An artifact is a file produced by a tool, such as an object file or executable. Framework stores artifact metadata separately from its cache. Neither service reads the file to prove that it exists, is current or is safe to execute. The artifact's verified flag and the cache row's valid flag are assertions supplied by the caller.

1. Compose an artifact with a nonempty identifier and path, a named kind and a known language. UNKNOWN is permitted for language-neutral metadata.
2. Insert it with the artifact store service and inspect the returned status. Repeated identifiers return ALREADY_EXISTS.
3. Compose a cache entry with a nonzero key and a bounded nonempty artifact path.
4. Use cache_put to add or replace a key. Replacing an existing key remains possible when every slot is occupied.
5. Validate source, dependencies, compiler identity, environment and the artifact itself before deciding to reuse a cache hit.

## Bound the complete owner

The services check counts and every active record before searching. A damaged late row cannot be hidden by an earlier matching key. Even an invalidated cache row must own a terminated path. Unused storage is not interpreted.

Failed insertion and checked invalidation preserve every owner byte. Mutations refuse maximum revision instead of wrapping it. Artifact duplicate detection retains its existing precedence over capacity and revision limits. Cache replacement is a mutation and is refused at maximum revision.

Lookup remains possible at maximum revision. Cache hit and miss counters saturate independently at UINT64_MAX. Invalid input or a malformed cache records neither a hit nor a miss. Borrowed lookup pointers remain valid only while their owner stays alive and unchanged; callers serialize access.

## Distinguish request metadata

The request key now includes working directory, debug information, position independence and warnings-as-errors, as well as every target field. Each include, definition and option collection has an explicit name and count. Moving identical text between collections therefore changes its serialized identity. Order within a collection also matters.

Scalars use an explicit eight-byte little-endian encoding. Enum object size, structure padding and host byte order are not hashed. Text is literal: different spellings of the same filesystem path can have different keys. Bytes after string terminators and unused option slots do not contribute.

This changes the key algorithm. Rebuild old persisted keys when adopting it. Malformed request storage returns zero; bounded drafts can be hashed without being ready to compile. The key is non-cryptographic and collisions remain possible. It does not include file contents, compiler version or dependencies, so it must not be the only acceptance check for executable output.

## Report invalidation failures

Call umi_compiler_cache_invalidate_checked and inspect its status. Its cutoff retains the existing strict comparison: rows created before the cutoff become invalid; a row at the cutoff remains unchanged. Successful calls increment operation revision even if no rows match.

The older void invalidation function delegates to this checked service. It preserves compatibility but cannot report an invalid owner or exhausted revision. New clients should use the checked interface and keep a failed operation visible to the user.

The regression targets cover late corruption, exact text limits, oversized counts, aliasing, capacity, exhausted revision, strict cutoff, saturating telemetry and request-field distinctions. These cases are authored and await compiler and runtime qualification. No cache freshness or toolchain verification is inferred from static source review.
