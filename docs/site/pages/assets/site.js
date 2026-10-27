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

/* Progressive enhancement: the teaching route stays readable without scripting.
 * Filtering only changes visibility and never stores or transmits learner input. */
{
  const learningSearch = document.getElementById("learning-search");
  const learningCount = document.getElementById("learning-count");
  if (learningSearch && learningCount) {
    const topics = Array.from(document.querySelectorAll("[data-learning-topic]"));
    const filterLearning = () => {
      const query = learningSearch.value.trim().toLocaleLowerCase("en-GB");
      let shown = 0;
      for (const topic of topics) {
        const matches = !query || topic.textContent.toLocaleLowerCase("en-GB").includes(query);
        topic.hidden = !matches;
        if (matches) shown += 1;
      }
      learningCount.textContent = shown ? `${shown} of ${topics.length} topics shown` :
        "No matching topics. Try a shorter word or clear the search.";
    };
    learningSearch.addEventListener("input", filterLearning);
    filterLearning();
  }
}
