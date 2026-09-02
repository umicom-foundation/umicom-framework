<!--
Umicom Framework
File: docs/SOURCE_PREVIEW_SERVICE.md

PURPOSE:
Explain the reusable source-preview contract, its safety boundary and the way
applications add preview providers without copying preview logic.

AUTHOR AND ORGANISATION:
Sammy Hegab
Umicom Foundation

LICENCE:
MIT
-->

# Source Preview Service

The source-preview service turns a versioned source snapshot into a bounded,
read-only preview result. It is owned by Umicom Framework so an IDE, document
tool, learning application or future designer can use the same provider
selection and safety rules.

The service receives the document path, language identifier, source text and
source revision. It selects the highest-priority compatible provider, asks that
provider to render the snapshot, then verifies that the returned result still
belongs to the requested revision. A stale or malformed result is rejected
instead of being displayed as current work.

## Built-in behavior

- HTML files produce readable document text. Script and style content is
  removed and is never executed.
- Every other source type has a plain-text fallback, making the panel useful
  for C, headers, scripts, stylesheets, configuration and documentation files.
- Input and output sizes are bounded. The result reports when output has been
  shortened.
- A provider cannot silently run source code. Providers that declare execution
  are considered only when the workspace is trusted and the caller explicitly
  allows execution.

## Adding a provider

Create a `UmiBrowserSourcePreviewProvider`, initialise it with
`umi_browser_source_preview_provider_init`, give it a stable identifier and
title, and select files by language identifier, file extension or both. The
render callback writes a `UmiBrowserSourcePreviewResult` for the exact source
revision supplied in the request.

Set `requires_trusted_workspace` when a renderer reads project resources. Set
`executes_source` when it compiles, interprets or otherwise runs document
content. These declarations are part of the enforced service contract, not
informational labels.

The application owns the provider context for as long as the provider remains
registered. Unregister the provider before releasing that context. The service
copies provider descriptors, so application code never receives a mutable
pointer into its internal catalogue.

## Presentation responsibility

The service has no dependency on a desktop toolkit. A frontend chooses how to
show the title, summary and content, and may place that view in a docked panel,
a tab, a floating window or another display. Layout and window persistence stay
separate from rendering, allowing the same preview to participate in every
Umicom workspace model.
