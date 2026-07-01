# Refactor Inventory And Safety Net

Date: 2026-06-27

This is a documentation and test inventory only. It does not change dashboard runtime behavior, UI, firmware API, polling, or chart rendering.

## Current Size

- `docs/index.html`: 6,929 lines, 346,854 bytes.
- Extracted dashboard helpers in `docs/js/*.js`: 785 lines, 35,187 bytes.
- Largest remaining file by far: `docs/index.html`.

## Script Load Order

Current order in `docs/index.html`:

1. `js/experiment-catalog.js`
2. `js/app-config.js`
3. `js/device-client.js`
4. `js/storage-export.js`
5. `js/sonar-utils.js`
6. `js/chart-utils.js`
7. `js/physics-calculations.js`
8. `js/fit-utils.js`
9. `js/pendulum-utils.js`
10. Inline dashboard app script

The order matters because `app-config.js` reads `window.PalladioExperimentCatalog`, `device-client.js` reads `window.PalladioConfig`, and the inline app reads all exported globals.

## Largest Remaining Blocks

Approximate ranges in `docs/index.html` after PR #15:

| Range | Lines | Area | Notes |
| --- | ---: | --- | --- |
| 927-6457 | 5,531 | Inline dashboard app | Main app state, renderers, chart drawing, events, startup. |
| 3483-5693 | 2,211 | Chart, analysis, selection, zoom | Highest-risk area. Includes `draw()` and pointer/wheel interactions. |
| 1519-3481 | 1,963 | Experiment renderers | Hooke, Sonar, Collision, Pendulum, Heat, and helper drawing code. |
| 5694-6438 | 745 | Event listeners and startup | Tightly coupled to DOM IDs and app state. |
| 977-1291 | 315 | State, element refs, row transforms | Mixed pure and UI-bound state. |
| 1-919 | 919 | HTML shell and experiment panels | Large but mostly declarative markup. |

The `draw()` function remains in place and should stay out of the next small extraction unless a stronger characterization suite is added first.
See [`draw-contract.md`](draw-contract.md) for the current chart rendering contract and safe extraction boundaries.

## Added Safety Net

- `tests/pure-helpers.test.js` loads the extracted global helper files in dependency order and checks:
  - catalog/config helpers,
  - chart unit/format helpers,
  - weight and buoyancy calculations,
  - sonar echo-time normalization helpers,
  - pendulum period, sample, and interval helpers,
  - storage/export escaping and report generation,
  - linear, Hooke, collision, linear-system, polynomial, and auto fit helpers.
- `tests/browser-smoke.spec.js` serves `docs/` locally with mocked M5-shaped API responses and checks:
  - script load order,
  - exported globals,
  - no `console.error` or uncaught page errors,
  - basic experiment switching for Hooke, Sonar, Pendulum, and Collision.

Run locally:

```bash
npm install
npm test
```

If Chromium is not installed for Playwright yet:

```bash
npx playwright install chromium
```

## Recommended Next Extraction Candidates

Good small PR candidates:

- More pendulum pure helpers if they stay DOM-free. The period/sample/interval helpers now live in `js/pendulum-utils.js`; keep canvas geometry and drawing in `index.html` unless separately characterized.
- More sonar helpers if they stay DOM-free. `sonarRoundTripTimeMs`, `firstFiniteNumber`, and echo-time conversion rules now live in `js/sonar-utils.js`; keep canvas drawing and drag handlers in `index.html`.
- Experiment mode predicates and titles: `isHookeExperimentMode`, `isSonarExperimentMode`, `isPendulumExperimentMode`, `rightRailExperimentTitle`. These are small but currently read shared state, so prefer passing a state object in the extracted helper.
- CSV row builders for saved experiment tables. These are mostly pure formatting, but should be separated from click handlers and `storageExport.downloadCsv`.
- Table/readout formatting helpers around `compactNumber`, `resultCard`, and small summary-card builders, if they can be kept DOM-free.

Avoid for now:

- `draw()` and the chart interaction loop.
- `chartGeometry`, tooltip, selection, pan, zoom, and wheel/pinch handlers.
- Render functions that write directly to the DOM.
- Device polling, firmware API shape, and M5 request timing.
