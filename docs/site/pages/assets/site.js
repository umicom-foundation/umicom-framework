/* Filter the local source map. Nothing is sent to a server. */
"use strict";
const field = document.getElementById("source-search");
if (field) {
  const rows = Array.from(document.querySelectorAll("[data-source-row]"));
  const report = document.getElementById("source-count");
  function filterSources() {
    const query = field.value.trim().toLocaleLowerCase("en-GB");
    let count = 0;
    for (const row of rows) {
      const matches = !query || row.textContent.toLocaleLowerCase("en-GB").includes(query);
      row.hidden = !matches;
      if (matches) count += 1;
    }
    report.textContent = `${count} of ${rows.length} header entries shown`;
  }
  field.addEventListener("input", filterSources);
  filterSources();
}
