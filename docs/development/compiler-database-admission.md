![Umicom](../../resources/brand/umicom-logo.svg)

# Own compilation records and export complete commands

A compilation database describes how source files are compiled. It does not execute those commands. Each record owns source metadata and a compiler plan. Framework plans keep the executable separate from their argument array, while the compilation-database format places the executable first in that array. The exporter now performs that conversion explicitly, following the [Clang format specification](https://clang.llvm.org/docs/JSONCompilationDatabase.html).

1. Compose a bounded entry with a unit identifier, source and known language.
2. Add it through the database service. The service validates every active stored entry before checking duplicates or capacity.
3. Keep a complete command before export: a nonempty directory, executable and at least one driver argument.
4. Export to a separate caller-owned buffer and handle the returned status.
5. Qualify any consumer of the resulting document independently. Export success is not compiler or language-server execution evidence.

## Keep stored arguments independent of the caller

Copying a plan by value copies its cached argument pointers too. Those pointers can still refer to the caller's plan even though the database has its own inline strings. Insertion now rebases the stored pointers into the receiving entry after copying admitted input. Changing or releasing the incoming plan cannot change the stored argument values.

A whole database copied by value has the same general pointer issue: rebase each copied plan with the plan argument service before consuming its cached views. Lookup returns borrowed storage and does not mutate a const owner. Keep owners alive and serialize access.

## Preserve refused work

The database rejects excessive counts, unterminated fields and malformed nested plans before lookup or mutation. A bad later record prevents an earlier match from hiding it. Insertion refuses a new row at full capacity or maximum revision without changing any bytes. The established duplicate-source result still takes precedence over those two limits.

Draft entries may have bounded empty command metadata, but export refuses them. Directory, executable and driver arguments are required to describe a command. An empty database exports an empty JSON array. The exporter retains the existing output and language fields, and emits the executable as arguments[0] followed by the plan's driver arguments.

The shared Protocol writer handles UTF-8 and JSON escaping. Export builds in temporary heap storage and replaces the output only after the complete document succeeds. Too little capacity, invalid emitted UTF-8 or malformed input preserves every previous output byte. Output must not overlap the database. A later file or pipe write has its own failure behavior.

The format permits multiple configurations for one source. This database API still rejects a duplicate source string; use separate databases for separate configurations until an explicit multi-configuration API is designed. String matching is literal and does not resolve filesystem identity.

The owner regression **framework.compiler.database_admission** covers late malformed rows, counts, revisions, capacity, caller/stored pointer independence, exact executable-first output, escaping, draft refusal and complete owner/output preservation. The existing response-file/database test now supplies an executable while retaining its earlier response checks. All these cases are authored and await execution. Previous implementations and escaping helpers remain disabled for review; no installed consumer or compiler-centre atomicity is claimed.
