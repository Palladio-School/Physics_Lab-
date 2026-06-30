# Draw Contract

Date: 2026-06-30

This note documents the current responsibilities of `draw()` in `docs/index.html`.
It is a safety contract for future refactors. It does not change runtime behavior.

## Scope

`draw()` is the central renderer for the main chart canvas. It currently handles:

- clearing and repainting the main canvas,
- choosing special chart paths before the standard renderer,
- computing chart geometry,
- drawing axes, gridlines, labels, and plot frame,
- drawing selected data series,
- highlighting the active A-B selection,
- drawing area-under-curve shading for the active analysis series,
- drawing trend/fit lines,
- drawing A/B markers,
- drawing hover markers and tooltip anchors,
- delegating pendulum scrub/highlight overlays.

## Special Render Paths

`draw()` exits early for these cases:

1. Hooke split chart: `isHookeExperimentMode() && splitChartActive()`
2. Hooke force-extension chart: `isHookeExperimentMode() && hookeChartView === 'extension'`
3. Generic split chart: `splitChartActive()`

Any future extraction must preserve this order. The Hooke-specific split and
extension paths take precedence over the generic split chart.

## Inputs Read By `draw()`

The function reads global dashboard state, including:

- `canvas`, `ctx`
- active mode/experiment predicates
- `hookeChartView`
- `selection`
- `analysisCache`
- `hoverPoint`
- `timeScaleMode`
- `valueScaleMode`
- current rows through `visibleRows()`
- enabled series through `enabledSeries()`
- view bounds through `getViewBounds()`
- scaling through `yScale()`, `selectedTimeScale()`, and `selectedValueScale()`

Because these are shared globals, extracting `draw()` directly would be risky
unless the state is passed through an explicit render-state object.

## Outputs And Side Effects

`draw()` mutates:

- the canvas pixels,
- `chartGeometry`, used later by pointer, hover, pan, zoom, selection, and
  tooltip code.

It also calls:

- `drawHookeSplitChart()`
- `drawHookeDxChart()`
- `drawSplitChart()`
- `drawPendulumTimeGuides()`
- `drawDataPath()`
- `drawPendulumScrubHighlight()`
- `showTooltip()`

Future changes must treat `chartGeometry` as part of the public internal
contract of the chart interaction loop.

## Behavior To Preserve

- Empty datasets show `Αναμονή για μετρήσεις...`.
- Y-axis padding switches at narrow canvas width: left pad `64` under `520px`,
  otherwise `82`.
- Selection shading is clipped to the visible x-range.
- Area shading is drawn only for the selected analysis series when it is visible.
- Fit lines are dashed and clipped to the selected analysis interval.
- A/B markers are shown only when the active analysis series is visible.
- Hover tooltip follows the nearest visible row by time.
- Pendulum time guides and scrub highlight remain part of the main chart path.

## Safe Next Steps

Good first extractions:

1. A pure `buildChartRenderState()` helper that returns geometry, scales, rows,
   and selected series without drawing.
2. A pure `chartPointForRow()` helper that converts row values to canvas points.
3. Small draw helpers for axes/grid and selection shading after screenshot or
   pixel-level characterization exists.

Avoid first:

- extracting pointer/pinch/wheel behavior together with drawing,
- changing `chartGeometry` shape,
- changing the special-path order,
- changing scale defaults or padding while refactoring.
