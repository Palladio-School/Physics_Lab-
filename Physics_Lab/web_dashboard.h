const char DASHBOARD_HTML[] PROGMEM = R"rawliteral(
<!doctype html>
<html lang="el">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Palladio Physics Lab</title>
<style>
:root {
  color-scheme: light;
  --blue: #2563eb;
  --blue-dark: #173f8a;
  --blue-soft: #eaf1ff;
  --ink: #142033;
  --muted: #64748b;
  --line: #dbe3ed;
  --grid: #dce4eb;
  --grid-minor: #eef2f5;
  --panel: rgba(255, 255, 255, 0.96);
  --page: #f3f6fa;
  --green: #2d9d57;
  --red: #d63c3c;
  --orange: #e58a16;
  --shadow: 0 10px 30px rgba(37, 55, 84, 0.08);
  --shadow-soft: 0 2px 8px rgba(37, 55, 84, 0.07);
}
* { box-sizing: border-box; }
body {
  margin: 0;
  min-width: 320px;
  background:
    radial-gradient(circle at 15% -10%, rgba(37, 99, 235, 0.10), transparent 30rem),
    var(--page);
  color: var(--ink);
  font-family: Inter, system-ui, -apple-system, BlinkMacSystemFont, "Segoe UI", sans-serif;
  letter-spacing: -0.01em;
}
button, select, input { font: inherit; }
button {
  min-height: 40px;
  border: 1px solid #cbd6e2;
  border-radius: 9px;
  background: #fff;
  color: var(--ink);
  padding: 0 14px;
  font-weight: 700;
  cursor: pointer;
  transition: background 140ms ease, border-color 140ms ease, color 140ms ease, box-shadow 140ms ease;
}
button:hover {
  border-color: #aebed0;
  background: #f8fafc;
  box-shadow: var(--shadow-soft);
}
button:focus-visible, select:focus-visible {
  outline: 3px solid rgba(23, 105, 170, 0.25);
  outline-offset: 2px;
}
.topbar {
  min-height: 72px;
  padding: 12px clamp(16px, 3vw, 38px);
  background: linear-gradient(120deg, #102a5a, #174b9a 62%, #2563eb);
  color: #fff;
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 20px;
}
.brand h1 {
  margin: 0;
  font-size: clamp(21px, 2.2vw, 30px);
  line-height: 1.15;
  letter-spacing: -0.035em;
}
.brand span {
  display: block;
  margin-top: 3px;
  color: #cfe4f5;
  font-size: 13px;
}
.connection {
  padding: 8px 12px;
  border: 1px solid rgba(255,255,255,0.18);
  border-radius: 999px;
  background: rgba(255,255,255,0.10);
  backdrop-filter: blur(8px);
  display: flex;
  align-items: center;
  gap: 9px;
  font-size: 13px;
  font-weight: 700;
  white-space: nowrap;
}
.dot {
  width: 10px;
  height: 10px;
  border: 2px solid rgba(255,255,255,0.65);
  border-radius: 50%;
  background: #59d37d;
}
.dot.offline { background: #ef6b6b; }
.modebar {
  padding: 7px clamp(12px, 3vw, 32px);
  background: rgba(255,255,255,0.92);
  border-bottom: 1px solid var(--line);
  display: flex;
  gap: 2px;
  overflow-x: auto;
}
.modebar button {
  min-height: 40px;
  border: 0;
  border-radius: 9px;
  color: #435363;
  padding: 0 18px;
  white-space: nowrap;
}
.modebar button.active {
  background: #173f8a;
  color: #fff;
  box-shadow: 0 5px 14px rgba(23, 63, 138, 0.22);
}
.workspace {
  width: min(1680px, 100%);
  margin: 0 auto;
  padding: 18px clamp(10px, 2vw, 26px) 28px;
  display: grid;
  gap: 16px;
}
.instrument-bar,
.readouts,
.chart-panel,
.table-panel {
  background: var(--panel);
  border: 1px solid var(--line);
  border-radius: 14px;
  box-shadow: var(--shadow);
}
.instrument-bar {
  min-height: 64px;
  padding: 10px 12px;
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 10px;
  flex-wrap: wrap;
}
.controls, .settings {
  display: flex;
  align-items: center;
  gap: 8px;
  flex-wrap: wrap;
}
.primary {
  border-color: var(--blue);
  background: var(--blue);
  color: #fff;
  box-shadow: 0 5px 14px rgba(37, 99, 235, 0.22);
}
.primary:hover { background: var(--blue-dark); }
.calibrate-button {
  border-color: #c9d8f1;
  background: #f4f7ff;
  color: var(--blue-dark);
}
.calibrate-button.busy {
  opacity: 0.65;
  pointer-events: none;
}
.recording::before {
  content: "";
  display: inline-block;
  width: 9px;
  height: 9px;
  margin-right: 8px;
  border-radius: 50%;
  background: #fff;
}
.field {
  display: flex;
  align-items: center;
  gap: 7px;
  color: var(--muted);
  font-size: 13px;
  font-weight: 700;
}
select {
  min-height: 38px;
  border: 1px solid #cbd6e2;
  border-radius: 9px;
  background: #fff;
  color: var(--ink);
  padding: 0 30px 0 10px;
}
.check {
  min-height: 38px;
  padding: 0 10px;
  border: 1px solid #c7d1da;
  border-radius: 9px;
  display: flex;
  align-items: center;
  gap: 7px;
  color: #435363;
  font-size: 13px;
  font-weight: 700;
}
.check input { accent-color: var(--blue); }
.readouts {
  padding: 12px;
  display: grid;
  grid-template-columns: repeat(4, minmax(130px, 1fr));
  gap: 9px;
}
.readout {
  position: relative;
  min-height: 80px;
  padding: 11px 14px;
  border: 1px solid #e4eaf1;
  border-radius: 11px;
  background: linear-gradient(145deg, #ffffff, #f7f9fc);
  overflow: hidden;
}
.readout::before {
  content: "";
  position: absolute;
  inset: 0 auto 0 0;
  width: 4px;
  background: var(--series-color, var(--blue));
}
.readout span {
  display: block;
  color: var(--muted);
  font-size: 12px;
  font-weight: 800;
  letter-spacing: 0.03em;
  text-transform: uppercase;
}
.readout strong {
  display: block;
  margin-top: 2px;
  font-size: clamp(22px, 2.4vw, 31px);
  line-height: 1.15;
  font-variant-numeric: tabular-nums;
}
.readout small {
  margin-left: 5px;
  color: var(--muted);
  font-size: 13px;
  font-weight: 700;
}
.chart-panel { overflow: hidden; }
.chart-head {
  min-height: 72px;
  padding: 12px 16px;
  border-bottom: 1px solid var(--line);
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 14px;
  flex-wrap: wrap;
}
.title h2 {
  margin: 0;
  font-size: clamp(20px, 2vw, 27px);
}
.chart-actions {
  margin-left: auto;
  display: flex;
  align-items: center;
  justify-content: flex-end;
  gap: 9px;
  flex-wrap: wrap;
}
.tool-group {
  padding: 3px;
  border: 1px solid #d4dee8;
  border-radius: 11px;
  background: #f3f6fa;
  display: inline-flex;
  gap: 3px;
}
.tool-group button {
  min-width: 40px;
  min-height: 34px;
  border: 0;
  border-radius: 8px;
  padding: 0 10px;
  background: transparent;
  color: #536477;
  box-shadow: none;
  font-size: 13px;
}
.tool-group button:hover { background: #fff; }
.tool-group button.active {
  background: #fff;
  color: var(--blue-dark);
  box-shadow: 0 2px 7px rgba(37, 55, 84, 0.12);
}
.zoom-status {
  min-height: 34px;
  padding: 0 10px;
  border: 1px solid #d4dee8;
  border-radius: 9px;
  background: #fff;
  color: var(--muted);
  display: inline-flex;
  align-items: center;
  font-size: 12px;
  font-weight: 800;
  font-variant-numeric: tabular-nums;
}
.title p {
  margin: 3px 0 0;
  color: var(--muted);
  font-size: 13px;
}
.legend {
  flex: 1 0 100%;
  padding-top: 9px;
  border-top: 1px solid #e8edf3;
  display: flex;
  align-items: center;
  justify-content: flex-start;
  gap: 7px;
  flex-wrap: wrap;
}
.legend button {
  min-height: 34px;
  padding: 0 10px;
  color: #536474;
  font-size: 13px;
}
.legend button.enabled {
  border-color: var(--series-color);
  background: color-mix(in srgb, var(--series-color) 9%, white);
  color: var(--ink);
}
.swatch {
  display: inline-block;
  width: 20px;
  height: 4px;
  margin-right: 7px;
  border-radius: 2px;
  background: var(--series-color);
  vertical-align: middle;
}
.plot-wrap {
  position: relative;
  height: clamp(360px, 56vh, 620px);
  min-height: 360px;
  background: linear-gradient(#fff, #fcfdff);
}
canvas {
  width: 100%;
  height: 100%;
  display: block;
  touch-action: none;
}
.tooltip {
  position: absolute;
  z-index: 2;
  min-width: 145px;
  padding: 8px 10px;
  border: 1px solid #9eacb9;
  border-radius: 10px;
  background: rgba(255,255,255,0.96);
  box-shadow: var(--shadow);
  color: var(--ink);
  font-size: 12px;
  line-height: 1.55;
  pointer-events: none;
}
.tooltip[hidden] { display: none; }
.tooltip strong { display: block; border-bottom: 1px solid #d9e1e7; }
.chart-foot {
  min-height: 42px;
  padding: 8px 16px;
  border-top: 1px solid var(--line);
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 10px;
  color: var(--muted);
  font-size: 13px;
}
.chart-foot strong { color: var(--ink); }
.analysis-panel {
  background: var(--panel);
  border: 1px solid var(--line);
  border-radius: 14px;
  box-shadow: var(--shadow);
  overflow: hidden;
}
.analysis-head {
  padding: 12px 16px;
  border-bottom: 1px solid var(--line);
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 12px;
  flex-wrap: wrap;
}
.analysis-head h3 {
  margin: 0;
  font-size: 18px;
}
.analysis-head p {
  margin: 3px 0 0;
  color: var(--muted);
  font-size: 13px;
}
.analysis-tools {
  padding: 10px 16px;
  border-bottom: 1px solid #e2e8ed;
  background: #f7f9fb;
  display: flex;
  align-items: center;
  gap: 10px;
  flex-wrap: wrap;
}
.analysis-tools .field { min-height: 40px; }
.selection-status {
  padding: 7px 10px;
  border-radius: 999px;
  background: var(--blue-soft);
  color: var(--blue-dark);
  font-size: 13px;
  font-weight: 800;
}
.results-grid {
  padding: 12px;
  display: grid;
  grid-template-columns: repeat(4, minmax(150px, 1fr));
  gap: 9px;
}
.result-card {
  min-height: 76px;
  padding: 10px 12px;
  border: 1px solid #d9e1e7;
  border-radius: 11px;
  background: linear-gradient(145deg, #fff, #fafcff);
}
.result-card.wide { grid-column: span 2; }
.result-card span {
  display: block;
  color: var(--muted);
  font-size: 11px;
  font-weight: 800;
  letter-spacing: 0.03em;
  text-transform: uppercase;
}
.result-card strong {
  display: block;
  margin-top: 4px;
  color: var(--ink);
  font-size: 19px;
  line-height: 1.25;
  font-variant-numeric: tabular-nums;
  overflow-wrap: anywhere;
}
.result-card small {
  display: block;
  margin-top: 3px;
  color: var(--muted);
  font-size: 12px;
}
.result-example {
  margin-top: 9px;
  padding-top: 8px;
  border-top: 1px solid #e7edf3;
  color: #53677f;
  font-size: 11px;
  line-height: 1.42;
}
.result-example b {
  color: var(--blue-dark);
  font-weight: 900;
}
.analysis-empty {
  grid-column: 1 / -1;
  padding: 22px 16px;
  color: var(--muted);
  text-align: center;
}
.table-panel { overflow-x: auto; }
.table-title {
  padding: 10px 14px;
  border-bottom: 1px solid var(--line);
  color: #435363;
  font-weight: 800;
}
table {
  width: 100%;
  border-collapse: collapse;
  font-size: 13px;
  font-variant-numeric: tabular-nums;
}
th, td {
  padding: 9px 13px;
  border-bottom: 1px solid #e4e9ed;
  text-align: right;
  white-space: nowrap;
}
th:first-child, td:first-child { text-align: left; }
th {
  background: #f1f5f8;
  color: #4f6070;
  font-weight: 800;
}
tbody tr:last-child td { border-bottom: 0; }
@media (max-width: 820px) {
  .topbar { align-items: flex-start; flex-direction: column; }
  .readouts { grid-template-columns: repeat(2, minmax(130px, 1fr)); }
  .instrument-bar { align-items: stretch; }
  .controls, .settings { width: 100%; }
  .controls button { flex: 1; }
  .chart-head { align-items: flex-start; flex-direction: column; }
  .legend { justify-content: flex-start; }
  .plot-wrap { height: 52vh; }
  .results-grid { grid-template-columns: repeat(2, minmax(140px, 1fr)); }
}
@media (max-width: 520px) {
  .connection { white-space: normal; }
  .modebar button { padding: 0 14px; }
  .readouts { grid-template-columns: 1fr 1fr; }
  .settings .field { flex: 1; }
  .settings select { flex: 1; }
  .plot-wrap { min-height: 330px; }
  .chart-foot { align-items: flex-start; flex-direction: column; }
  .analysis-tools { align-items: stretch; flex-direction: column; }
  .analysis-tools .field { width: 100%; justify-content: space-between; }
  .analysis-tools select { flex: 1; }
  .results-grid { grid-template-columns: 1fr 1fr; }
  .result-card.wide { grid-column: span 2; }
}

/* Application workspace layout */
.app-shell {
  min-height: 100vh;
  display: grid;
  grid-template-columns: 286px minmax(0, 1fr);
}
.lab-sidebar {
  position: sticky;
  top: 0;
  z-index: 3;
  height: 100vh;
  padding: 22px 16px 16px;
  background:
    radial-gradient(circle at 20% 0%, rgba(67, 118, 255, 0.26), transparent 18rem),
    #101a2b;
  color: #fff;
  display: flex;
  flex-direction: column;
  gap: 18px;
  overflow-y: auto;
}
.topbar {
  min-height: 0;
  padding: 0 6px;
  background: none;
  display: block;
}
.brand-mark {
  width: 62px;
  height: 62px;
  margin-bottom: 13px;
  border: 2px solid rgba(255,255,255,0.22);
  border-radius: 50%;
  background: #fff;
  box-shadow: 0 10px 28px rgba(12, 27, 56, 0.38);
  display: block;
  object-fit: cover;
}
.brand h1 {
  font-size: 20px;
  line-height: 1.2;
}
.brand span {
  margin-top: 6px;
  color: #91a3bb;
  font-size: 12px;
  line-height: 1.45;
}
.side-label {
  padding: 0 7px;
  color: #72849c;
  font-size: 10px;
  font-weight: 900;
  letter-spacing: 0.12em;
  text-transform: uppercase;
}
.modebar {
  padding: 0;
  border: 0;
  background: none;
  display: grid;
  gap: 6px;
  overflow: visible;
}
.modebar button {
  min-height: 44px;
  border: 1px solid transparent;
  border-radius: 10px;
  background: transparent;
  color: #aebbd0;
  padding: 0 12px;
  text-align: left;
}
.modebar button::before {
  content: "";
  display: inline-block;
  width: 7px;
  height: 7px;
  margin-right: 10px;
  border-radius: 50%;
  background: #52637b;
  vertical-align: 1px;
}
.modebar button:hover {
  border-color: rgba(255,255,255,0.08);
  background: rgba(255,255,255,0.05);
  color: #fff;
  box-shadow: none;
}
.modebar button.active {
  border-color: rgba(96,165,250,0.32);
  background: rgba(37,99,235,0.18);
  color: #fff;
  box-shadow: inset 3px 0 0 #60a5fa;
}
.modebar button.active::before {
  background: #60a5fa;
  box-shadow: 0 0 0 4px rgba(96,165,250,0.13);
}
.lab-sidebar .readouts {
  padding: 0;
  border: 0;
  border-radius: 0;
  background: none;
  box-shadow: none;
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 7px;
}
.lab-sidebar .readout {
  min-width: 0;
  min-height: 72px;
  padding: 10px 11px;
  border: 1px solid rgba(255,255,255,0.08);
  border-radius: 10px;
  background: rgba(255,255,255,0.045);
}
.lab-sidebar .readout::before {
  inset: auto 10px 0;
  width: auto;
  height: 3px;
  border-radius: 3px 3px 0 0;
}
.lab-sidebar .readout span {
  color: #8191a8;
  font-size: 9px;
}
.lab-sidebar .readout strong {
  margin-top: 7px;
  color: #fff;
  font-size: 20px;
}
.lab-sidebar .readout small {
  color: #8495ac;
  font-size: 10px;
}
.connection {
  margin-top: auto;
  padding: 10px 12px;
  border-color: rgba(255,255,255,0.08);
  border-radius: 10px;
  background: rgba(255,255,255,0.04);
  color: #a8b7c9;
  font-size: 11px;
  white-space: normal;
}
.workspace {
  width: 100%;
  margin: 0;
  padding: 22px clamp(16px, 2.4vw, 34px) 34px;
  gap: 18px;
  align-content: start;
}
.workspace-heading {
  display: flex;
  align-items: flex-end;
  justify-content: space-between;
  gap: 16px;
}
.workspace-heading h2 {
  margin: 0;
  font-size: clamp(25px, 3vw, 38px);
  letter-spacing: -0.045em;
}
.workspace-heading p {
  margin: 6px 0 0;
  color: var(--muted);
  font-size: 13px;
}
.live-chip {
  padding: 8px 11px;
  border: 1px solid #cfe7d7;
  border-radius: 999px;
  background: #edf9f1;
  color: #237642;
  font-size: 12px;
  font-weight: 900;
  white-space: nowrap;
}
.live-chip.paused {
  border-color: #f0d9ad;
  background: #fff7e8;
  color: #9a6414;
}
.toast {
  position: fixed;
  z-index: 20;
  right: 22px;
  bottom: 22px;
  max-width: min(360px, calc(100vw - 28px));
  padding: 12px 15px;
  border: 1px solid #cfe2d5;
  border-radius: 11px;
  background: #f0faf3;
  box-shadow: 0 14px 36px rgba(31, 58, 42, 0.18);
  color: #27623a;
  font-size: 13px;
  font-weight: 800;
}
.toast.error {
  border-color: #efcaca;
  background: #fff2f2;
  color: #9b3030;
}
.toast[hidden] { display: none; }
.instrument-bar {
  min-height: 58px;
  padding: 8px;
  border-radius: 12px;
  box-shadow: var(--shadow-soft);
}
.chart-panel {
  border-radius: 16px;
  box-shadow: 0 18px 50px rgba(35, 52, 80, 0.10);
}
.chart-head {
  padding: 16px 18px 10px;
  border-bottom: 0;
}
.chart-head .title h2 { font-size: 20px; }
.chart-head .title p { font-size: 12px; }
.legend {
  margin-top: 2px;
  padding-top: 10px;
}
.plot-wrap {
  height: clamp(400px, 58vh, 680px);
  margin: 0 10px;
  border: 1px solid #e5eaf0;
  border-radius: 12px;
  overflow: hidden;
}
.chart-foot { border-top: 0; }
.analysis-panel {
  border-radius: 16px;
}
.analysis-head { padding: 16px 18px; }
.analysis-tools { padding: 11px 18px; }
.results-grid {
  grid-template-columns: repeat(3, minmax(150px, 1fr));
  padding: 14px;
  gap: 10px;
}
.result-card {
  min-height: 88px;
  padding: 12px 14px;
}
.table-panel {
  border-radius: 14px;
  overflow: hidden;
}
.table-panel summary {
  padding: 13px 16px;
  cursor: pointer;
  color: #41536a;
  font-weight: 800;
  list-style-position: inside;
}
.table-panel[open] summary { border-bottom: 1px solid var(--line); }
.table-title { display: none; }

@media (max-width: 1020px) {
  .app-shell { grid-template-columns: 230px minmax(0, 1fr); }
  .lab-sidebar { padding-inline: 12px; }
  .lab-sidebar .readouts { grid-template-columns: 1fr; }
  .plot-wrap { height: 54vh; }
}
@media (max-width: 760px) {
  .app-shell { display: block; }
  .lab-sidebar {
    position: static;
    width: 100%;
    height: auto;
    padding: 15px;
    gap: 13px;
    overflow: visible;
  }
  .topbar {
    display: flex;
    align-items: center;
    gap: 12px;
  }
  .brand-mark {
    flex: 0 0 46px;
    width: 46px;
    height: 46px;
    margin: 0;
  }
  .brand span { display: none; }
  .modebar {
    display: flex;
    overflow-x: auto;
  }
  .modebar button {
    flex: 0 0 auto;
    min-height: 40px;
  }
  .lab-sidebar .side-label,
  .lab-sidebar .readouts { display: none; }
  .connection {
    position: absolute;
    top: 16px;
    right: 15px;
    width: auto;
    max-width: 48%;
    margin: 0;
  }
  .workspace { padding: 18px 10px 28px; }
  .workspace-heading { padding: 0 4px; }
  .instrument-bar { align-items: stretch; }
  .controls, .settings { width: 100%; }
  .controls button { flex: 1; }
  .chart-actions {
    width: 100%;
    margin-left: 0;
    justify-content: flex-start;
  }
  .results-grid { grid-template-columns: repeat(2, minmax(140px, 1fr)); }
}
@media (max-width: 520px) {
  .connection { display: none; }
  .workspace-heading { align-items: flex-start; flex-direction: column; }
  .workspace-heading h2 { font-size: 28px; }
  .plot-wrap { height: 52vh; min-height: 350px; margin: 0 6px; }
  .tool-group { max-width: 100%; }
  .tool-group button { padding-inline: 8px; }
  .zoom-status { display: none; }
  .results-grid { grid-template-columns: 1fr; }
  .result-card.wide { grid-column: span 1; }
}
</style>
</head>
<body>
<div class="app-shell">
  <aside class="lab-sidebar">
    <header class="topbar">
      <img class="brand-mark" src="/logo.png" alt="Λογότυπο Palladio">
      <div class="brand">
        <h1>Palladio Lab</h1>
        <span>Ζωντανό εργαστήριο φυσικών επιστημών</span>
      </div>
    </header>

    <div class="side-label">Αισθητήρες</div>
    <nav class="modebar" aria-label="Επιλογή αισθητήρα">
      <button data-mode="0">Επιτάχυνση</button>
      <button data-mode="1">Γυροσκόπιο</button>
      <button data-mode="2">Δύναμη</button>
      <button data-mode="4">Υπερηχητική κίνηση</button>
    </nav>

    <div class="side-label">Ζωντανές τιμές</div>
    <section class="readouts" id="readouts" aria-label="Τρέχουσες τιμές"></section>

    <div class="connection">
      <span class="dot" id="statusDot"></span>
      <span id="network">Σύνδεση με το M5Stick...</span>
    </div>
  </aside>

  <main class="workspace">
    <header class="workspace-heading">
      <div>
        <h2>Πείραμα σε εξέλιξη</h2>
        <p>Καταγραφή, οπτικοποίηση και ανάλυση δεδομένων σε πραγματικό χρόνο</p>
      </div>
      <span class="live-chip" id="sessionStatus">● LIVE DATA</span>
    </header>

    <section class="instrument-bar" aria-label="Εργαλεία καταγραφής">
      <div class="controls">
        <button id="toggle" class="primary recording">Παύση</button>
        <button id="clear">Νέα μέτρηση</button>
        <button id="export">Εξαγωγή CSV</button>
        <button id="calibrate" class="calibrate-button">Μηδενισμός accel</button>
      </div>
      <div class="settings">
        <label class="field">
          Δειγματοληψία
          <select id="sampleRate">
            <option value="5">5 Hz</option>
            <option value="10" selected>10 Hz</option>
            <option value="20">20 Hz</option>
          </select>
        </label>
        <label class="field">
          Παράθυρο
          <select id="timeWindow">
            <option value="10">10 s</option>
            <option value="30" selected>30 s</option>
            <option value="60">60 s</option>
          </select>
        </label>
        <label class="check">
          <input id="autoScale" type="checkbox" checked>
          Αυτόματη κλίμακα
        </label>
      </div>
    </section>

    <section class="chart-panel">
      <div class="chart-head">
        <div class="title">
          <h2 id="chartTitle">Επιτάχυνση</h2>
          <p id="chartSubtitle">Επιτάχυνση ως προς τον χρόνο</p>
        </div>
        <div class="chart-actions">
          <div class="tool-group" aria-label="Λειτουργία δείκτη">
            <button data-chart-tool="inspect" title="Ανάγνωση τιμών">Δείκτης</button>
            <button class="active" data-chart-tool="select" title="Επιλογή διαστήματος">Επιλογή</button>
            <button data-chart-tool="pan" title="Μετακίνηση γραφήματος">Μετακίνηση</button>
          </div>
          <div class="tool-group" aria-label="Μεγέθυνση γραφήματος">
            <button id="zoomOut" title="Σμίκρυνση">−</button>
            <button id="zoomIn" title="Μεγέθυνση">+</button>
            <button id="zoomReset" title="Επιστροφή στη ζωντανή προβολή">Live</button>
          </div>
          <span class="zoom-status" id="zoomStatus">30 s · Live</span>
        </div>
        <div class="legend" id="legend" aria-label="Καμπύλες γραφήματος"></div>
      </div>
      <div class="plot-wrap" id="plotWrap">
        <canvas id="chart" aria-label="Γράφημα μετρήσεων"></canvas>
        <div class="tooltip" id="tooltip" hidden></div>
      </div>
      <div class="chart-foot">
        <span><strong id="sampleCount">0</strong> δείγματα</span>
        <span id="chartHint">Σύρε πάνω στη γραφική για επιλογή διαστήματος. Ροδέλα ή pinch για zoom.</span>
      </div>
    </section>

    <section class="analysis-panel">
      <div class="analysis-head">
        <div>
          <h3>Ανάλυση γραφικής παράστασης</h3>
          <p>Επίλεξε ένα διάστημα για fit, ολοκλήρωση και στατιστικά.</p>
        </div>
        <span class="selection-status" id="selectionStatus">Ολόκληρο το ορατό διάστημα</span>
      </div>
      <div class="analysis-tools">
        <label class="field">
          Μέγεθος
          <select id="analysisSeries"></select>
        </label>
        <label class="field">
          Προσαρμογή
          <select id="fitType">
            <option value="linear">Γραμμική y = αt + β</option>
            <option value="quadratic">Παραβολική y = αt² + βt + γ</option>
            <option value="best">Καλύτερη από γραμμική / παραβολική</option>
            <option value="none">Χωρίς καμπύλη fit</option>
          </select>
        </label>
        <button id="clearSelection">Καθαρισμός επιλογής</button>
      </div>
      <div class="results-grid" id="analysisResults">
        <div class="analysis-empty">Αναμονή για αρκετά δείγματα...</div>
      </div>
    </section>

    <details class="table-panel">
      <summary>Τελευταίες αριθμητικές μετρήσεις</summary>
      <div class="table-title">Τελευταίες μετρήσεις</div>
      <table>
        <thead id="tableHead"></thead>
        <tbody id="tableBody"></tbody>
      </table>
    </details>
  </main>
</div>
<div class="toast" id="toast" hidden></div>

<script>
const maxSamples = 1200;
let running = true;
let rows = [];
let activeMode = 0;
let startTime = performance.now();
let timeWindow = 30;
let autoScale = true;
let hoverPoint = null;
let chartGeometry = null;
let lastData = {};
let selection = null;
let selecting = false;
let selectionAnchor = 0;
let analysisSeriesIndex = 0;
let fitType = 'linear';
let analysisCache = null;
let customView = null;
let chartTool = 'select';
let panning = false;
let panStartX = 0;
let panStartView = null;
const activePointers = new Map();
let pinchState = null;
let lastSampleId = -1;
let sampleEpochMs = null;
let toastTimer = null;

const canvas = document.getElementById('chart');
const ctx = canvas.getContext('2d');
const tooltip = document.getElementById('tooltip');
const statusDot = document.getElementById('statusDot');
const modeButtons = [...document.querySelectorAll('[data-mode]')];

const modes = {
  0: {
    title: 'Επιτάχυνση',
    subtitle: 'Συνιστώσες επιτάχυνσης ως προς τον χρόνο',
    yLabel: 'Επιτάχυνση (m/s²)',
    symmetric: true,
    minimumSpan: 24,
    series: [
      { key: 'ax', label: 'Άξονας X', short: 'X', unit: 'm/s²', color: '#d63c3c', enabled: true },
      { key: 'ay', label: 'Άξονας Y', short: 'Y', unit: 'm/s²', color: '#2d9d57', enabled: true },
      { key: 'az', label: 'Άξονας Z', short: 'Z', unit: 'm/s²', color: '#1769aa', enabled: true }
    ]
  },
  1: {
    title: 'Γωνιακή ταχύτητα',
    subtitle: 'Γωνιακή ταχύτητα ως προς τον χρόνο',
    yLabel: 'Γωνιακή ταχύτητα (°/s)',
    symmetric: true,
    minimumSpan: 40,
    series: [
      { key: 'gx', label: 'Άξονας X', short: 'Gx', unit: '°/s', color: '#d63c3c', enabled: true },
      { key: 'gy', label: 'Άξονας Y', short: 'Gy', unit: '°/s', color: '#2d9d57', enabled: true },
      { key: 'gz', label: 'Άξονας Z', short: 'Gz', unit: '°/s', color: '#1769aa', enabled: true }
    ]
  },
  2: {
    title: 'Δύναμη',
    subtitle: 'Δύναμη από την κυψέλη φορτίου ως προς τον χρόνο',
    yLabel: 'Δύναμη (N)',
    symmetric: false,
    minimumSpan: 10,
    series: [
      { key: 'force', label: 'Δύναμη', short: 'F', unit: 'N', color: '#e58a16', enabled: true }
    ]
  },
  4: {
    title: 'Υπερηχητική κίνηση',
    subtitle: 'Επίλεξε ένα φυσικό μέγεθος για σωστή κλίμακα και μονάδα',
    yLabel: '',
    symmetric: false,
    minimumSpan: 1,
    series: [
      { key: 'distance', label: 'Απόσταση', short: 'd', unit: 'm', color: '#7b4db7', enabled: true },
      { key: 'velocity', label: 'Ταχύτητα', short: 'υ', unit: 'm/s', color: '#1769aa', enabled: false },
      { key: 'acceleration', label: 'Επιτάχυνση', short: 'α', unit: 'm/s²', color: '#2d9d57', enabled: false }
    ]
  }
};

function currentMode() {
  return modes[activeMode] || modes[0];
}

function enabledSeries() {
  const enabled = currentMode().series.filter(series => series.enabled);
  return enabled.length ? enabled : [currentMode().series[0]];
}

function formatValue(value, span) {
  const magnitude = Math.abs(value);
  if (magnitude >= 100 || span >= 100) return value.toFixed(0);
  if (magnitude >= 10 || span >= 10) return value.toFixed(1);
  return value.toFixed(2);
}

function resetMeasurement() {
  rows = [];
  startTime = performance.now();
  lastSampleId = -1;
  sampleEpochMs = null;
  hoverPoint = null;
  selection = null;
  selecting = false;
  analysisCache = null;
  customView = null;
  panning = false;
  activePointers.clear();
  pinchState = null;
  tooltip.hidden = true;
  updateTable();
  updateAnalysis();
  updateZoomStatus();
  draw();
}

function setMode(mode) {
  fetch('/set-mode?mode=' + mode).catch(() => {});
  activeMode = mode;
  resetMeasurement();
  updateModeUi();
  updateReadouts(lastData);
}

function updateModeUi() {
  const meta = currentMode();
  document.getElementById('chartTitle').textContent = meta.title;
  document.getElementById('chartSubtitle').textContent = meta.subtitle;
  modeButtons.forEach(button => {
    button.classList.toggle('active', Number(button.dataset.mode) === activeMode);
  });
  document.getElementById('tableHead').innerHTML =
    '<tr><th>Χρόνος (s)</th>' +
    meta.series.map(series => '<th>' + series.label + ' (' + series.unit + ')</th>').join('') +
    '</tr>';
  analysisSeriesIndex = Math.max(0, meta.series.findIndex(series => series.enabled));
  updateCalibrationUi();
  renderAnalysisSeries();
  renderLegend();
  updateTable();
  updateAnalysis();
  draw();
}

function calibrationForMode() {
  const calibrations = {
    0: { target: 'accel', label: 'Μηδενισμός accel', success: 'Το επιταχυνσιόμετρο μηδενίστηκε.' },
    1: { target: 'gyro', label: 'Μηδενισμός gyro', success: 'Το γυροσκόπιο μηδενίστηκε.' },
    2: { target: 'force', label: 'Tare δύναμης', success: 'Η κυψέλη φορτίου μηδενίστηκε.' },
    4: { target: 'distance', label: 'Θέσε απόσταση 0', success: 'Η τρέχουσα θέση ορίστηκε ως μηδέν.' }
  };
  return calibrations[activeMode] || null;
}

function updateCalibrationUi() {
  const calibration = calibrationForMode();
  const button = document.getElementById('calibrate');
  button.disabled = !calibration;
  button.textContent = calibration ? calibration.label : 'Βαθμονόμηση';
}

function showToast(message, error) {
  const toast = document.getElementById('toast');
  clearTimeout(toastTimer);
  toast.textContent = message;
  toast.classList.toggle('error', Boolean(error));
  toast.hidden = false;
  toastTimer = setTimeout(() => {
    toast.hidden = true;
  }, 3200);
}

function renderAnalysisSeries() {
  const meta = currentMode();
  const select = document.getElementById('analysisSeries');
  select.innerHTML = meta.series.map((series, index) => {
    return '<option value="' + index + '">' + series.label + ' (' + series.unit + ')</option>';
  }).join('');
  select.value = String(analysisSeriesIndex);
}

function renderLegend() {
  const meta = currentMode();
  document.getElementById('legend').innerHTML = meta.series.map((series, index) => {
    const activeClass = series.enabled ? 'enabled' : '';
    return '<button class="' + activeClass + '" data-series="' + index +
      '" style="--series-color:' + series.color + '">' +
      '<span class="swatch"></span>' + series.label + '</button>';
  }).join('');

  document.querySelectorAll('[data-series]').forEach(button => {
    button.addEventListener('click', () => toggleSeries(Number(button.dataset.series)));
  });
}

function toggleSeries(index) {
  const meta = currentMode();
  const selected = meta.series[index];
  if (activeMode === 4) {
    meta.series.forEach((series, seriesIndex) => {
      series.enabled = seriesIndex === index;
    });
  } else {
    const enabledCount = meta.series.filter(series => series.enabled).length;
    if (selected.enabled && enabledCount === 1) return;
    selected.enabled = !selected.enabled;
  }
  renderLegend();
  updateReadouts(lastData);
  updateAnalysis();
  draw();
}

function updateReadouts(data) {
  const meta = currentMode();
  const cards = meta.series.map(series => {
    const value = Number(data[series.key] || 0);
    return '<div class="readout" style="--series-color:' + series.color + '">' +
      '<span>' + series.label + '</span>' +
      '<strong>' + value.toFixed(2) + '<small>' + series.unit + '</small></strong>' +
      '</div>';
  });
  cards.push(
    '<div class="readout" style="--series-color:#607080">' +
    '<span>Μπαταρία</span><strong>' + Number(data.battery || 0).toFixed(2) +
    '<small>V</small></strong></div>'
  );
  document.getElementById('readouts').innerHTML = cards.join('');
}

function addRow(data) {
  lastData = data;
  const mode = Number(data.mode);
  if (mode !== activeMode && modes[mode]) {
    activeMode = mode;
    resetMeasurement();
    updateModeUi();
  }
  updateReadouts(data);
  if (data.sampleRate) {
    document.getElementById('sampleRate').value = String(data.sampleRate);
  }
  if (!running || !modes[activeMode]) return;

  const sampleId = Number(data.sampleId);
  const sampleMs = Number(data.sampleMs);
  if (!Number.isFinite(sampleId) || sampleId === lastSampleId) return;
  lastSampleId = sampleId;
  if (sampleEpochMs === null || sampleMs < sampleEpochMs) sampleEpochMs = sampleMs;

  const meta = currentMode();
  rows.push({
    time: (sampleMs - sampleEpochMs) / 1000,
    values: meta.series.map(series => Number(data[series.key] || 0))
  });
  if (rows.length > maxSamples) rows.shift();
  updateTable();
  updateAnalysis();
  draw();
}

function updateTable() {
  const meta = currentMode();
  const latest = rows.slice(-6).reverse();
  document.getElementById('sampleCount').textContent = rows.length;
  document.getElementById('tableBody').innerHTML = latest.map(row => {
    return '<tr><td>' + row.time.toFixed(2) + '</td>' +
      row.values.map(value => '<td>' + value.toFixed(3) + '</td>').join('') +
      '</tr>';
  }).join('');
}

function visibleRows() {
  if (!rows.length) return [];
  const view = getViewBounds();
  return rows.filter(row => row.time >= view.start && row.time <= view.end);
}

function dataBounds() {
  const first = rows.length ? rows[0].time : 0;
  const latest = rows.length ? rows[rows.length - 1].time : 0;
  return {
    start: Math.min(0, first),
    end: Math.max(timeWindow, latest),
    span: Math.max(timeWindow, latest - Math.min(0, first))
  };
}

function getViewBounds() {
  if (customView) return { start: customView.start, end: customView.end };
  const latest = rows.length ? rows[rows.length - 1].time : 0;
  const end = Math.max(timeWindow, latest);
  return { start: Math.max(0, end - timeWindow), end };
}

function clampView(start, end) {
  const bounds = dataBounds();
  const requestedSpan = Math.max(0.5, end - start);
  const maxSpan = Math.max(timeWindow, bounds.end - bounds.start);
  const span = Math.min(requestedSpan, maxSpan);
  let nextStart = start;
  let nextEnd = nextStart + span;
  if (nextStart < bounds.start) {
    nextStart = bounds.start;
    nextEnd = nextStart + span;
  }
  if (nextEnd > bounds.end) {
    nextEnd = bounds.end;
    nextStart = Math.max(bounds.start, nextEnd - span);
  }
  return { start: nextStart, end: nextEnd };
}

function setCustomView(start, end) {
  customView = clampView(start, end);
  hoverPoint = null;
  tooltip.hidden = true;
  updateAnalysis();
  updateZoomStatus();
  draw();
}

function zoomAt(centerTime, factor) {
  const view = getViewBounds();
  const span = view.end - view.start;
  const nextSpan = Math.max(0.5, span * factor);
  const ratio = span > 0 ? (centerTime - view.start) / span : 0.5;
  const start = centerTime - nextSpan * ratio;
  setRunning(false);
  setCustomView(start, start + nextSpan);
}

function resetZoom(resumeLive) {
  customView = null;
  selection = null;
  hoverPoint = null;
  tooltip.hidden = true;
  if (resumeLive) setRunning(true);
  updateAnalysis();
  updateZoomStatus();
  draw();
}

function updateZoomStatus() {
  const view = getViewBounds();
  const span = Math.max(0, view.end - view.start);
  document.getElementById('zoomStatus').textContent =
    compactNumber(span) + ' s' + (customView ? ' · Zoom' : ' · Live');
}

function analysisRows() {
  const source = selection ? rows : visibleRows();
  if (!selection) return source;
  const start = Math.min(selection.start, selection.end);
  const end = Math.max(selection.start, selection.end);
  return source.filter(row => row.time >= start && row.time <= end);
}

function solveLinearSystem(matrix, vector) {
  const size = vector.length;
  const augmented = matrix.map((row, index) => [...row, vector[index]]);
  for (let column = 0; column < size; column++) {
    let pivot = column;
    for (let row = column + 1; row < size; row++) {
      if (Math.abs(augmented[row][column]) > Math.abs(augmented[pivot][column])) pivot = row;
    }
    if (Math.abs(augmented[pivot][column]) < 1e-12) return null;
    [augmented[column], augmented[pivot]] = [augmented[pivot], augmented[column]];
    const divisor = augmented[column][column];
    for (let item = column; item <= size; item++) augmented[column][item] /= divisor;
    for (let row = 0; row < size; row++) {
      if (row === column) continue;
      const factor = augmented[row][column];
      for (let item = column; item <= size; item++) {
        augmented[row][item] -= factor * augmented[column][item];
      }
    }
  }
  return augmented.map(row => row[size]);
}

function polynomialFit(points, degree) {
  if (points.length < degree + 1) return null;
  const origin = points.reduce((sum, point) => sum + point.x, 0) / points.length;
  const shifted = points.map(point => ({ x: point.x - origin, y: point.y }));
  const size = degree + 1;
  const matrix = Array.from({ length: size }, () => Array(size).fill(0));
  const vector = Array(size).fill(0);

  shifted.forEach(point => {
    for (let row = 0; row < size; row++) {
      vector[row] += point.y * Math.pow(point.x, row);
      for (let column = 0; column < size; column++) {
        matrix[row][column] += Math.pow(point.x, row + column);
      }
    }
  });

  const coefficients = solveLinearSystem(matrix, vector);
  if (!coefficients) return null;
  const predict = time => {
    const shiftedTime = time - origin;
    return coefficients.reduce((sum, coefficient, power) => {
      return sum + coefficient * Math.pow(shiftedTime, power);
    }, 0);
  };
  const mean = points.reduce((sum, point) => sum + point.y, 0) / points.length;
  const total = points.reduce((sum, point) => sum + Math.pow(point.y - mean, 2), 0);
  const residual = points.reduce((sum, point) => sum + Math.pow(point.y - predict(point.x), 2), 0);
  const r2 = total < 1e-12 ? (residual < 1e-12 ? 1 : 0) : 1 - residual / total;
  const predictors = degree;
  const adjustedR2 = points.length > predictors + 1
    ? 1 - (1 - r2) * (points.length - 1) / (points.length - predictors - 1)
    : -Infinity;
  return {
    degree,
    coefficients,
    origin,
    predict,
    r2,
    adjustedR2,
    rmse: Math.sqrt(residual / points.length)
  };
}

function chooseFit(points) {
  if (fitType === 'none') return null;
  const linear = polynomialFit(points, 1);
  if (fitType === 'linear') return linear;
  const quadratic = polynomialFit(points, 2);
  if (fitType === 'quadratic') return quadratic;
  if (!quadratic) return linear;
  if (!linear) return quadratic;
  return quadratic.adjustedR2 > linear.adjustedR2 + 0.002 ? quadratic : linear;
}

function compactNumber(value) {
  if (!isFinite(value)) return '—';
  const magnitude = Math.abs(value);
  if (magnitude !== 0 && (magnitude >= 10000 || magnitude < 0.001)) return value.toExponential(3);
  const digits = magnitude >= 100 ? 1 : (magnitude >= 10 ? 2 : 3);
  return value.toFixed(digits).replace(/\.?0+$/, '');
}

function fitEquation(fit) {
  if (!fit) return 'Δεν εφαρμόστηκε';
  const c = fit.coefficients;
  const tau = 'τ = t − ' + compactNumber(fit.origin) + ' s';
  if (fit.degree === 1) {
    return 'y = ' + compactNumber(c[1]) + 'τ ' +
      (c[0] < 0 ? '− ' : '+ ') + compactNumber(Math.abs(c[0])) + '  (' + tau + ')';
  }
  return 'y = ' + compactNumber(c[2]) + 'τ² ' +
    (c[1] < 0 ? '− ' : '+ ') + compactNumber(Math.abs(c[1])) + 'τ ' +
    (c[0] < 0 ? '− ' : '+ ') + compactNumber(Math.abs(c[0])) + '  (' + tau + ')';
}

function areaUnit(series) {
  if (series.unit === 'm/s²') return 'm/s';
  if (series.unit === 'm/s') return 'm';
  if (series.unit === '°/s') return '°';
  if (series.unit === 'N') return 'N·s';
  return series.unit + '·s';
}

function slopeUnit(series) {
  if (series.unit === 'm') return 'm/s';
  if (series.unit === 'm/s') return 'm/s²';
  if (series.unit === 'm/s²') return 'm/s³';
  if (series.unit === '°/s') return '°/s²';
  return series.unit + '/s';
}

function areaMeaning(series) {
  if (series.key === 'force') return 'Ώθηση';
  if (series.key === 'ax' || series.key === 'ay' || series.key === 'az' ||
      series.key === 'acceleration') return 'Μεταβολή ταχύτητας';
  if (series.key === 'gx' || series.key === 'gy' || series.key === 'gz') return 'Γωνία περιστροφής';
  if (series.key === 'velocity') return 'Μετατόπιση';
  return 'Προσημασμένο εμβαδό';
}

function slopeMeaning(series) {
  if (series.key === 'distance') return 'Μέση ταχύτητα (fit)';
  if (series.key === 'velocity') return 'Μέση επιτάχυνση (fit)';
  if (series.key === 'force') return 'Ρυθμός μεταβολής δύναμης';
  return 'Κλίση γραμμικού fit';
}

function analysisExamples(series) {
  const examples = {
    markerA: 'Τοποθέτησε το A στην αρχή του φαινομένου που εξετάζεις.',
    markerB: 'Τοποθέτησε το B στο τέλος του ίδιου φαινομένου.',
    duration: 'Μέτρησε τη διάρκεια μιας κρούσης, ταλάντωσης ή μετακίνησης.',
    delta: 'Σύγκρινε την τελική με την αρχική τιμή του μεγέθους.',
    mean: 'Βρες μια αντιπροσωπευτική τιμή όταν οι μετρήσεις έχουν μικρές διακυμάνσεις.',
    stdDev: 'Μικρή σ σημαίνει σταθερές μετρήσεις· μεγάλη σ δείχνει διασπορά ή θόρυβο.',
    minMax: 'Εντόπισε τις ακραίες τιμές, όπως τη μέγιστη δύναμη σε μια κρούση.',
    absoluteArea: 'Χρήσιμο όταν θέλεις τη συνολική δράση χωρίς να αλληλοαναιρούνται θετικές και αρνητικές περιοχές.',
    fit: 'Έλεγξε αν το θεωρητικό μοντέλο περιγράφει τα δεδομένα: R² κοντά στο 1 σημαίνει καλύτερη συμφωνία.'
  };

  if (series.key === 'distance') {
    examples.slope = 'Η κλίση σε γράφημα θέσης–χρόνου δίνει τη μέση ταχύτητα.';
    examples.area = 'Το εμβαδό θέσης–χρόνου δεν έχει συνήθη άμεση φυσική ερμηνεία.';
  } else if (series.key === 'velocity') {
    examples.slope = 'Η κλίση σε γράφημα ταχύτητας–χρόνου δίνει τη μέση επιτάχυνση.';
    examples.area = 'Το εμβαδό κάτω από υ–t δίνει τη μετατόπιση.';
  } else if (series.key === 'force') {
    examples.slope = 'Δείχνει πόσο γρήγορα αυξάνεται ή μειώνεται η δύναμη.';
    examples.area = 'Το εμβαδό κάτω από F–t δίνει την ώθηση σε μια κρούση.';
  } else if (series.key === 'gx' || series.key === 'gy' || series.key === 'gz') {
    examples.slope = 'Δείχνει τον ρυθμό μεταβολής της γωνιακής ταχύτητας.';
    examples.area = 'Το εμβαδό κάτω από ω–t δίνει τη γωνία περιστροφής.';
  } else {
    examples.slope = 'Δείχνει πόσο γρήγορα μεταβάλλεται η επιτάχυνση στο διάστημα A–B.';
    examples.area = 'Το εμβαδό κάτω από a–t δίνει τη μεταβολή της ταχύτητας.';
  }
  return examples;
}

function calculateAnalysis() {
  const meta = currentMode();
  const series = meta.series[analysisSeriesIndex] || meta.series[0];
  const index = meta.series.indexOf(series);
  const points = analysisRows()
    .map(row => ({ x: row.time, y: row.values[index] }))
    .filter(point => isFinite(point.x) && isFinite(point.y));
  if (points.length < 2) return { series, points };

  const duration = points[points.length - 1].x - points[0].x;
  const pointA = points[0];
  const pointB = points[points.length - 1];
  const mean = points.reduce((sum, point) => sum + point.y, 0) / points.length;
  const variance = points.length > 1
    ? points.reduce((sum, point) => sum + Math.pow(point.y - mean, 2), 0) / (points.length - 1)
    : 0;
  let signedArea = 0;
  let absoluteArea = 0;
  for (let index = 1; index < points.length; index++) {
    const first = points[index - 1];
    const second = points[index];
    const dt = second.x - first.x;
    signedArea += (first.y + second.y) * dt / 2;
    if (first.y * second.y < 0) {
      absoluteArea += dt * (first.y * first.y + second.y * second.y) /
        (2 * (Math.abs(first.y) + Math.abs(second.y)));
    } else {
      absoluteArea += Math.abs((first.y + second.y) * dt / 2);
    }
  }

  const linear = polynomialFit(points, 1);
  return {
    series,
    points,
    pointA,
    pointB,
    duration,
    mean,
    stdDev: Math.sqrt(variance),
    min: Math.min(...points.map(point => point.y)),
    max: Math.max(...points.map(point => point.y)),
    delta: points[points.length - 1].y - points[0].y,
    abSlope: duration > 0 ? (pointB.y - pointA.y) / duration : 0,
    signedArea,
    absoluteArea,
    linear,
    fit: chooseFit(points)
  };
}

function resultCard(label, value, detail, example, wide) {
  return '<div class="result-card' + (wide ? ' wide' : '') + '">' +
    '<span>' + label + '</span><strong>' + value + '</strong>' +
    (detail ? '<small>' + detail + '</small>' : '') +
    (example ? '<div class="result-example"><b>Παράδειγμα:</b> ' + example + '</div>' : '') +
    '</div>';
}

function updateAnalysis() {
  analysisCache = calculateAnalysis();
  const status = document.getElementById('selectionStatus');
  if (selection) {
    const start = Math.min(selection.start, selection.end);
    const end = Math.max(selection.start, selection.end);
    status.textContent = compactNumber(start) + ' s έως ' + compactNumber(end) + ' s';
  } else {
    status.textContent = 'Ολόκληρο το ορατό διάστημα';
  }

  const container = document.getElementById('analysisResults');
  if (!analysisCache || analysisCache.points.length < 2) {
    container.innerHTML = '<div class="analysis-empty">Αναμονή για τουλάχιστον δύο δείγματα...</div>';
    return;
  }

  const result = analysisCache;
  const series = result.series;
  const examples = analysisExamples(series);
  const fitName = result.fit
    ? (result.fit.degree === 2 ? 'Παραβολικό fit' : 'Γραμμικό fit')
    : 'Fit';
  container.innerHTML =
    resultCard('Δείκτης A',
      't = ' + compactNumber(result.pointA.x) + ' s',
      'y = ' + compactNumber(result.pointA.y) + ' ' + series.unit,
      examples.markerA) +
    resultCard('Δείκτης B',
      't = ' + compactNumber(result.pointB.x) + ' s',
      'y = ' + compactNumber(result.pointB.y) + ' ' + series.unit,
      examples.markerB) +
    resultCard('Δt (A–B)', compactNumber(result.duration) + ' s',
      result.points.length + ' δείγματα', examples.duration) +
    resultCard('Δy (A–B)', compactNumber(result.delta) + ' ' + series.unit,
      'yB − yA', examples.delta) +
    resultCard('Κλίση A–B', compactNumber(result.abSlope) + ' ' + slopeUnit(series),
      'Δy / Δt', examples.slope) +
    resultCard('Μέση τιμή', compactNumber(result.mean) + ' ' + series.unit,
      '', examples.mean) +
    resultCard('Τυπική απόκλιση', compactNumber(result.stdDev) + ' ' + series.unit,
      'Δειγματική σ', examples.stdDev) +
    resultCard('Ελάχιστο / Μέγιστο',
      compactNumber(result.min) + ' / ' + compactNumber(result.max) + ' ' + series.unit,
      '', examples.minMax) +
    resultCard(areaMeaning(series), compactNumber(result.signedArea) + ' ' + areaUnit(series),
      'Προσημασμένη ολοκλήρωση', examples.area) +
    resultCard('Απόλυτο εμβαδό', compactNumber(result.absoluteArea) + ' ' + areaUnit(series),
      'Συνολικό εμβαδό |y|', examples.absoluteArea) +
    resultCard(slopeMeaning(series),
      result.linear ? compactNumber(result.linear.coefficients[1]) + ' ' + slopeUnit(series) : '—',
      'Από γραμμική παλινδρόμηση', examples.slope) +
    resultCard(fitName, fitEquation(result.fit),
      result.fit
        ? 'R² = ' + compactNumber(result.fit.r2) +
          ' · RMSE = ' + compactNumber(result.fit.rmse) + ' ' + series.unit
        : 'Η καμπύλη fit είναι απενεργοποιημένη',
      examples.fit, true);
}

function niceNumber(value, round) {
  if (!isFinite(value) || value <= 0) return 1;
  const exponent = Math.floor(Math.log10(value));
  const fraction = value / Math.pow(10, exponent);
  let niceFraction;
  if (round) {
    if (fraction < 1.5) niceFraction = 1;
    else if (fraction < 3) niceFraction = 2;
    else if (fraction < 7) niceFraction = 5;
    else niceFraction = 10;
  } else {
    if (fraction <= 1) niceFraction = 1;
    else if (fraction <= 2) niceFraction = 2;
    else if (fraction <= 5) niceFraction = 5;
    else niceFraction = 10;
  }
  return niceFraction * Math.pow(10, exponent);
}

function yScale(dataRows, selectedSeries) {
  const meta = currentMode();
  const seriesIndexes = selectedSeries.map(series => meta.series.indexOf(series));
  const values = [];
  dataRows.forEach(row => {
    seriesIndexes.forEach(index => {
      const value = row.values[index];
      if (isFinite(value)) values.push(value);
    });
  });

  let min = values.length ? Math.min(...values) : -meta.minimumSpan / 2;
  let max = values.length ? Math.max(...values) : meta.minimumSpan / 2;

  if (!autoScale) {
    if (meta.symmetric) {
      const half = meta.minimumSpan / 2;
      min = -half;
      max = half;
    } else {
      min = 0;
      max = meta.minimumSpan;
    }
  } else if (meta.symmetric) {
    const bound = Math.max(meta.minimumSpan / 2, Math.abs(min), Math.abs(max)) * 1.12;
    min = -bound;
    max = bound;
  } else {
    min = Math.min(0, min);
    const span = Math.max(meta.minimumSpan, max - min);
    min -= span * (min < 0 ? 0.08 : 0);
    max += span * 0.12;
    if (max - min < meta.minimumSpan) max = min + meta.minimumSpan;
  }

  const step = niceNumber((max - min) / 6, true);
  const niceMin = Math.floor(min / step) * step;
  const niceMax = Math.ceil(max / step) * step;
  return { min: niceMin, max: niceMax === niceMin ? niceMin + step : niceMax, step };
}

function axisLabel(selectedSeries) {
  const meta = currentMode();
  const units = [...new Set(selectedSeries.map(series => series.unit))];
  if (units.length === 1) {
    if (selectedSeries.length === 1) return selectedSeries[0].label + ' (' + units[0] + ')';
    return meta.yLabel;
  }
  return 'Τιμή';
}

function resizeCanvas() {
  const rect = canvas.getBoundingClientRect();
  const ratio = window.devicePixelRatio || 1;
  canvas.width = Math.max(320, Math.round(rect.width * ratio));
  canvas.height = Math.max(300, Math.round(rect.height * ratio));
  ctx.setTransform(ratio, 0, 0, ratio, 0, 0);
  draw();
}

function draw() {
  const rect = canvas.getBoundingClientRect();
  const width = rect.width;
  const height = rect.height;
  if (!width || !height) return;

  ctx.clearRect(0, 0, width, height);
  ctx.fillStyle = '#ffffff';
  ctx.fillRect(0, 0, width, height);

  const pad = { left: width < 520 ? 64 : 82, right: 24, top: 20, bottom: 58 };
  const plotW = Math.max(1, width - pad.left - pad.right);
  const plotH = Math.max(1, height - pad.top - pad.bottom);
  const view = getViewBounds();
  const dataRows = visibleRows();
  const selectedSeries = enabledSeries();
  const scale = yScale(dataRows, selectedSeries);
  const xMin = view.start;
  const xMax = view.end;
  const viewSpan = Math.max(0.5, xMax - xMin);

  chartGeometry = { pad, plotW, plotH, xMin, xMax, scale, dataRows, selectedSeries };

  ctx.save();
  ctx.font = '12px system-ui';
  ctx.lineWidth = 1;
  ctx.textBaseline = 'middle';

  const minorStep = scale.step / 2;
  for (let value = Math.ceil(scale.min / minorStep) * minorStep; value <= scale.max + minorStep / 2; value += minorStep) {
    const y = pad.top + (scale.max - value) * plotH / (scale.max - scale.min);
    const major = Math.abs(value / scale.step - Math.round(value / scale.step)) < 0.001;
    ctx.strokeStyle = Math.abs(value) < minorStep / 10 ? '#8798a7' : (major ? '#d5dee5' : '#eef2f5');
    ctx.lineWidth = Math.abs(value) < minorStep / 10 ? 1.5 : 1;
    ctx.beginPath();
    ctx.moveTo(pad.left, y);
    ctx.lineTo(width - pad.right, y);
    ctx.stroke();
    if (major) {
      ctx.fillStyle = '#526474';
      ctx.textAlign = 'right';
      ctx.fillText(formatValue(value, scale.max - scale.min), pad.left - 10, y);
    }
  }

  const xStep = niceNumber(viewSpan / 7, true);
  for (let value = Math.ceil(xMin / xStep) * xStep; value <= xMax; value += xStep) {
    const x = pad.left + (value - xMin) * plotW / (xMax - xMin);
    ctx.strokeStyle = '#dce4eb';
    ctx.lineWidth = 1;
    ctx.beginPath();
    ctx.moveTo(x, pad.top);
    ctx.lineTo(x, pad.top + plotH);
    ctx.stroke();
    ctx.fillStyle = '#526474';
    ctx.textAlign = 'center';
    ctx.textBaseline = 'top';
    const xDigits = xStep < 1 ? 2 : (xStep < 10 ? 1 : 0);
    ctx.fillText(value.toFixed(xDigits), x, pad.top + plotH + 9);
  }

  ctx.strokeStyle = '#657685';
  ctx.lineWidth = 1.3;
  ctx.strokeRect(pad.left, pad.top, plotW, plotH);

  ctx.fillStyle = '#344553';
  ctx.font = '600 13px system-ui';
  ctx.textAlign = 'center';
  ctx.textBaseline = 'bottom';
  ctx.fillText('Χρόνος (s)', pad.left + plotW / 2, height - 7);

  ctx.save();
  ctx.translate(16, pad.top + plotH / 2);
  ctx.rotate(-Math.PI / 2);
  ctx.textAlign = 'center';
  ctx.textBaseline = 'top';
  ctx.fillText(axisLabel(selectedSeries), 0, 0);
  ctx.restore();

  if (!dataRows.length) {
    ctx.fillStyle = '#718190';
    ctx.font = '600 16px system-ui';
    ctx.textAlign = 'center';
    ctx.textBaseline = 'middle';
    ctx.fillText('Αναμονή για μετρήσεις...', pad.left + plotW / 2, pad.top + plotH / 2);
  }

  ctx.save();
  ctx.beginPath();
  ctx.rect(pad.left, pad.top, plotW, plotH);
  ctx.clip();

  if (selection) {
    const start = Math.max(xMin, Math.min(selection.start, selection.end));
    const end = Math.min(xMax, Math.max(selection.start, selection.end));
    if (end >= start) {
      const startX = pad.left + (start - xMin) * plotW / (xMax - xMin);
      const endX = pad.left + (end - xMin) * plotW / (xMax - xMin);
      ctx.fillStyle = 'rgba(23, 105, 170, 0.09)';
      ctx.fillRect(startX, pad.top, Math.max(2, endX - startX), plotH);
      ctx.strokeStyle = 'rgba(23, 105, 170, 0.65)';
      ctx.lineWidth = 1;
      ctx.setLineDash([5, 4]);
      ctx.beginPath();
      ctx.moveTo(startX, pad.top);
      ctx.lineTo(startX, pad.top + plotH);
      ctx.moveTo(endX, pad.top);
      ctx.lineTo(endX, pad.top + plotH);
      ctx.stroke();
      ctx.setLineDash([]);
    }
  }

  if (analysisCache && analysisCache.points.length >= 2) {
    const series = analysisCache.series;
    const isVisible = selectedSeries.includes(series);
    const points = analysisCache.points.filter(point => point.x >= xMin && point.x <= xMax);
    if (isVisible && points.length >= 2) {
      const zeroY = pad.top + (scale.max - 0) * plotH / (scale.max - scale.min);
      const firstX = pad.left + (points[0].x - xMin) * plotW / (xMax - xMin);
      const lastX = pad.left + (points[points.length - 1].x - xMin) * plotW / (xMax - xMin);
      ctx.fillStyle = series.color + '24';
      ctx.beginPath();
      ctx.moveTo(firstX, zeroY);
      points.forEach(point => {
        const x = pad.left + (point.x - xMin) * plotW / (xMax - xMin);
        const y = pad.top + (scale.max - point.y) * plotH / (scale.max - scale.min);
        ctx.lineTo(x, y);
      });
      ctx.lineTo(lastX, zeroY);
      ctx.closePath();
      ctx.fill();
    }
  }

  selectedSeries.forEach(series => {
    const seriesIndex = currentMode().series.indexOf(series);
    ctx.strokeStyle = series.color;
    ctx.lineWidth = 2.4;
    ctx.lineJoin = 'round';
    ctx.lineCap = 'round';
    ctx.beginPath();
    let started = false;
    dataRows.forEach(row => {
      const value = row.values[seriesIndex];
      if (!isFinite(value)) return;
      const x = pad.left + (row.time - xMin) * plotW / (xMax - xMin);
      const y = pad.top + (scale.max - value) * plotH / (scale.max - scale.min);
      if (!started) {
        ctx.moveTo(x, y);
        started = true;
      } else {
        ctx.lineTo(x, y);
      }
    });
    ctx.stroke();
  });

  if (analysisCache && analysisCache.fit && analysisCache.points.length >= 2 &&
      selectedSeries.includes(analysisCache.series)) {
    const fit = analysisCache.fit;
    const start = Math.max(xMin, analysisCache.points[0].x);
    const end = Math.min(xMax, analysisCache.points[analysisCache.points.length - 1].x);
    if (end > start) {
      ctx.strokeStyle = '#111820';
      ctx.lineWidth = 2.2;
      ctx.setLineDash([8, 5]);
      ctx.beginPath();
      const segments = 100;
      for (let index = 0; index <= segments; index++) {
        const time = start + (end - start) * index / segments;
        const value = fit.predict(time);
        const x = pad.left + (time - xMin) * plotW / (xMax - xMin);
        const y = pad.top + (scale.max - value) * plotH / (scale.max - scale.min);
        if (index === 0) ctx.moveTo(x, y);
        else ctx.lineTo(x, y);
      }
      ctx.stroke();
      ctx.setLineDash([]);
    }
  }

  if (selection && analysisCache && analysisCache.pointA && analysisCache.pointB &&
      selectedSeries.includes(analysisCache.series)) {
    const markers = [
      { label: 'A', point: analysisCache.pointA },
      { label: 'B', point: analysisCache.pointB }
    ];
    markers.forEach(marker => {
      const x = pad.left + (marker.point.x - xMin) * plotW / (xMax - xMin);
      const y = pad.top + (scale.max - marker.point.y) * plotH / (scale.max - scale.min);
      if (x < pad.left || x > pad.left + plotW) return;
      ctx.fillStyle = '#ffffff';
      ctx.strokeStyle = '#172033';
      ctx.lineWidth = 2.5;
      ctx.beginPath();
      ctx.arc(x, y, 5, 0, Math.PI * 2);
      ctx.fill();
      ctx.stroke();

      const labelY = Math.max(pad.top + 17, y - 22);
      ctx.fillStyle = '#172033';
      ctx.beginPath();
      ctx.roundRect(x - 11, labelY - 11, 22, 20, 5);
      ctx.fill();
      ctx.fillStyle = '#ffffff';
      ctx.font = '800 12px system-ui';
      ctx.textAlign = 'center';
      ctx.textBaseline = 'middle';
      ctx.fillText(marker.label, x, labelY - 1);
    });
  }

  if (hoverPoint && dataRows.length) {
    const hoverTime = xMin + (hoverPoint.x - pad.left) * (xMax - xMin) / plotW;
    const nearest = dataRows.reduce((best, row) => {
      return Math.abs(row.time - hoverTime) < Math.abs(best.time - hoverTime) ? row : best;
    }, dataRows[0]);
    const x = pad.left + (nearest.time - xMin) * plotW / (xMax - xMin);
    ctx.strokeStyle = '#33495b';
    ctx.lineWidth = 1;
    ctx.setLineDash([4, 3]);
    ctx.beginPath();
    ctx.moveTo(x, pad.top);
    ctx.lineTo(x, pad.top + plotH);
    ctx.stroke();
    ctx.setLineDash([]);

    selectedSeries.forEach(series => {
      const seriesIndex = currentMode().series.indexOf(series);
      const value = nearest.values[seriesIndex];
      const y = pad.top + (scale.max - value) * plotH / (scale.max - scale.min);
      ctx.fillStyle = '#fff';
      ctx.strokeStyle = series.color;
      ctx.lineWidth = 2;
      ctx.beginPath();
      ctx.arc(x, y, 4, 0, Math.PI * 2);
      ctx.fill();
      ctx.stroke();
    });
    showTooltip(nearest, x, hoverPoint.y, selectedSeries);
  }
  ctx.restore();
  ctx.restore();
}

function showTooltip(row, x, y, selectedSeries) {
  const meta = currentMode();
  tooltip.innerHTML = '<strong>t = ' + row.time.toFixed(2) + ' s</strong>' +
    selectedSeries.map(series => {
      const index = meta.series.indexOf(series);
      return '<span style="color:' + series.color + '">●</span> ' +
        series.short + ' = ' + row.values[index].toFixed(3) + ' ' + series.unit;
    }).join('<br>');
  tooltip.hidden = false;
  const wrap = document.getElementById('plotWrap').getBoundingClientRect();
  const tooltipWidth = 170;
  tooltip.style.left = Math.min(wrap.width - tooltipWidth - 8, x + 12) + 'px';
  tooltip.style.top = Math.max(8, Math.min(wrap.height - 100, y - 30)) + 'px';
}

function pointerPosition(event) {
  const rect = canvas.getBoundingClientRect();
  return { x: event.clientX - rect.left, y: event.clientY - rect.top };
}

function inspectChart(event) {
  if (!chartGeometry) return;
  const point = pointerPosition(event);
  const pad = chartGeometry.pad;
  const inside = point.x >= pad.left && point.x <= pad.left + chartGeometry.plotW &&
    point.y >= pad.top && point.y <= pad.top + chartGeometry.plotH;
  hoverPoint = inside ? point : null;
  if (!inside) tooltip.hidden = true;
  draw();
}

function pointInsidePlot(point) {
  if (!chartGeometry) return false;
  const pad = chartGeometry.pad;
  return point.x >= pad.left && point.x <= pad.left + chartGeometry.plotW &&
    point.y >= pad.top && point.y <= pad.top + chartGeometry.plotH;
}

function timeAtX(x) {
  const geometry = chartGeometry;
  const boundedX = Math.max(geometry.pad.left, Math.min(geometry.pad.left + geometry.plotW, x));
  return geometry.xMin + (boundedX - geometry.pad.left) *
    (geometry.xMax - geometry.xMin) / geometry.plotW;
}

function setRunning(value) {
  running = value;
  const button = document.getElementById('toggle');
  const status = document.getElementById('sessionStatus');
  button.textContent = running ? 'Παύση' : 'Συνέχεια';
  button.classList.toggle('recording', running);
  status.textContent = running ? '● LIVE DATA' : '● ΑΝΑΛΥΣΗ';
  status.classList.toggle('paused', !running);
}

function setChartTool(tool) {
  chartTool = tool;
  selecting = false;
  panning = false;
  hoverPoint = null;
  tooltip.hidden = true;
  document.querySelectorAll('[data-chart-tool]').forEach(button => {
    button.classList.toggle('active', button.dataset.chartTool === tool);
  });
  const hints = {
    inspect: 'Μετακίνησε για ακριβείς τιμές ή σύρε για επιλογή διαστήματος.',
    select: 'Σύρε πάνω στη γραφική για να επιλέξεις το διάστημα ανάλυσης.',
    pan: 'Σύρε οριζόντια για μετακίνηση. Ροδέλα ή pinch για zoom.'
  };
  document.getElementById('chartHint').textContent = hints[tool];
  canvas.style.cursor = tool === 'select' ? 'crosshair' : (tool === 'pan' ? 'grab' : 'default');
  draw();
}

function beginPinch() {
  if (activePointers.size < 2 || !chartGeometry) return false;
  const points = [...activePointers.values()].slice(0, 2);
  const distance = Math.abs(points[1].x - points[0].x);
  if (distance < 8) return false;
  const midpoint = (points[0].x + points[1].x) / 2;
  pinchState = {
    distance,
    midpointTime: timeAtX(midpoint),
    view: getViewBounds()
  };
  selecting = false;
  panning = false;
  setRunning(false);
  return true;
}

function updatePinch() {
  if (!pinchState || activePointers.size < 2) return;
  const points = [...activePointers.values()].slice(0, 2);
  const distance = Math.max(8, Math.abs(points[1].x - points[0].x));
  const initialSpan = pinchState.view.end - pinchState.view.start;
  const nextSpan = initialSpan * pinchState.distance / distance;
  const initialRatio = (pinchState.midpointTime - pinchState.view.start) / initialSpan;
  const midpointX = (points[0].x + points[1].x) / 2;
  const currentMidpoint = timeAtX(midpointX);
  const start = currentMidpoint - nextSpan * initialRatio;
  setCustomView(start, start + nextSpan);
}

function startChartInteraction(event) {
  if (!chartGeometry || !rows.length) return;
  const point = pointerPosition(event);
  if (!pointInsidePlot(point)) return;
  if (event.pointerType === 'touch') {
    activePointers.set(event.pointerId, point);
    if (activePointers.size >= 2 && beginPinch()) {
      event.preventDefault();
      return;
    }
  }

  event.preventDefault();
  setRunning(false);
  hoverPoint = null;
  tooltip.hidden = true;

  if (chartTool === 'select' || chartTool === 'inspect') {
    selecting = true;
    selectionAnchor = timeAtX(point.x);
    selection = { start: selectionAnchor, end: selectionAnchor };
  } else if (chartTool === 'pan') {
    panning = true;
    panStartX = point.x;
    panStartView = getViewBounds();
    canvas.style.cursor = 'grabbing';
  }

  if (canvas.setPointerCapture && event.pointerId !== undefined) {
    canvas.setPointerCapture(event.pointerId);
  }
  updateAnalysis();
  draw();
}

function moveChartInteraction(event) {
  const point = pointerPosition(event);
  if (event.pointerType === 'touch' && activePointers.has(event.pointerId)) {
    activePointers.set(event.pointerId, point);
    if (pinchState || activePointers.size >= 2) {
      if (!pinchState) beginPinch();
      updatePinch();
      event.preventDefault();
      return;
    }
  }

  if (!selecting && !panning) {
    inspectChart(event);
    return;
  }
  event.preventDefault();
  if (selecting) {
    selection = { start: selectionAnchor, end: timeAtX(point.x) };
    updateAnalysis();
    draw();
  } else if (panning && panStartView) {
    const span = panStartView.end - panStartView.start;
    const shift = (panStartX - point.x) * span / chartGeometry.plotW;
    setCustomView(panStartView.start + shift, panStartView.end + shift);
  }
}

function finishChartInteraction(event) {
  if (event.pointerType === 'touch') {
    activePointers.delete(event.pointerId);
    if (activePointers.size < 2) pinchState = null;
  }
  const hadInteraction = selecting || panning;
  selecting = false;
  panning = false;
  panStartView = null;
  if (chartTool === 'pan') canvas.style.cursor = 'grab';
  if (canvas.releasePointerCapture && canvas.hasPointerCapture && event.pointerId !== undefined &&
      canvas.hasPointerCapture(event.pointerId)) {
    canvas.releasePointerCapture(event.pointerId);
  }
  if (hadInteraction && selection && Math.abs(selection.end - selection.start) < 0.05) {
    selection = null;
  }
  updateAnalysis();
  draw();
}

function handleWheel(event) {
  if (!chartGeometry || !rows.length) return;
  const point = pointerPosition(event);
  if (!pointInsidePlot(point)) return;
  event.preventDefault();
  const factor = event.deltaY > 0 ? 1.22 : 0.82;
  zoomAt(timeAtX(point.x), factor);
}

modeButtons.forEach(button => {
  button.addEventListener('click', () => setMode(Number(button.dataset.mode)));
});

document.querySelectorAll('[data-chart-tool]').forEach(button => {
  button.addEventListener('click', () => setChartTool(button.dataset.chartTool));
});

document.getElementById('toggle').addEventListener('click', () => {
  setRunning(!running);
});

document.getElementById('clear').addEventListener('click', resetMeasurement);

document.getElementById('sampleRate').addEventListener('change', async event => {
  const requestedRate = Number(event.target.value);
  try {
    const response = await fetch('/settings?rate=' + requestedRate, { cache: 'no-store' });
    if (!response.ok) throw new Error('HTTP ' + response.status);
    resetMeasurement();
    showToast('Ρυθμός δειγματοληψίας: ' + requestedRate + ' Hz');
  } catch (error) {
    showToast('Δεν εφαρμόστηκε ο νέος ρυθμός δειγματοληψίας.', true);
  }
});

document.getElementById('calibrate').addEventListener('click', async event => {
  const calibration = calibrationForMode();
  if (!calibration) return;
  const button = event.currentTarget;
  button.classList.add('busy');
  button.textContent = 'Βαθμονόμηση...';
  try {
    const response = await fetch('/calibrate?target=' + calibration.target, { cache: 'no-store' });
    if (!response.ok) throw new Error('HTTP ' + response.status);
    resetMeasurement();
    showToast(calibration.success);
  } catch (error) {
    showToast('Η βαθμονόμηση απέτυχε. Έλεγξε τη σύνδεση.', true);
  } finally {
    button.classList.remove('busy');
    updateCalibrationUi();
  }
});

document.getElementById('timeWindow').addEventListener('change', event => {
  timeWindow = Number(event.target.value);
  resetZoom(false);
});

document.getElementById('autoScale').addEventListener('change', event => {
  autoScale = event.target.checked;
  draw();
});

document.getElementById('analysisSeries').addEventListener('change', event => {
  analysisSeriesIndex = Number(event.target.value);
  const meta = currentMode();
  if (activeMode === 4) {
    meta.series.forEach((series, index) => {
      series.enabled = index === analysisSeriesIndex;
    });
  } else {
    meta.series[analysisSeriesIndex].enabled = true;
  }
  renderLegend();
  updateAnalysis();
  draw();
});

document.getElementById('fitType').addEventListener('change', event => {
  fitType = event.target.value;
  updateAnalysis();
  draw();
});

document.getElementById('clearSelection').addEventListener('click', () => {
  selection = null;
  updateAnalysis();
  draw();
});

document.getElementById('zoomIn').addEventListener('click', () => {
  const view = getViewBounds();
  zoomAt((view.start + view.end) / 2, 0.65);
});

document.getElementById('zoomOut').addEventListener('click', () => {
  const view = getViewBounds();
  zoomAt((view.start + view.end) / 2, 1.45);
});

document.getElementById('zoomReset').addEventListener('click', () => resetZoom(true));

document.getElementById('export').addEventListener('click', () => {
  const meta = currentMode();
  const headers = ['time_s', ...meta.series.map(series => series.key + '_' + series.unit.replaceAll('/', '_per_'))];
  const csvRows = [
    headers,
    ...rows.map(row => [row.time.toFixed(3), ...row.values.map(value => value.toFixed(5))])
  ];
  const csv = csvRows.map(row => row.join(',')).join('\n');
  const blob = new Blob([csv], { type: 'text/csv;charset=utf-8' });
  const url = URL.createObjectURL(blob);
  const link = document.createElement('a');
  link.href = url;
  link.download = 'palladio-' + meta.title.toLowerCase().replaceAll(' ', '-') + '.csv';
  link.click();
  URL.revokeObjectURL(url);
});

canvas.addEventListener('pointerdown', startChartInteraction);
canvas.addEventListener('pointermove', moveChartInteraction);
canvas.addEventListener('pointerup', finishChartInteraction);
canvas.addEventListener('pointercancel', finishChartInteraction);
canvas.addEventListener('wheel', handleWheel, { passive: false });
canvas.addEventListener('mouseleave', () => {
  if (selecting || panning) return;
  hoverPoint = null;
  tooltip.hidden = true;
  draw();
});

async function poll() {
  try {
    const response = await fetch('/data', { cache: 'no-store' });
    if (!response.ok) throw new Error('HTTP ' + response.status);
    const data = await response.json();
    document.getElementById('network').textContent = data.ap + ' · ' + data.ip;
    statusDot.classList.remove('offline');
    addRow(data);
  } catch (error) {
    document.getElementById('network').textContent = 'Αναμονή για το M5Stick';
    statusDot.classList.add('offline');
  } finally {
    setTimeout(poll, 30);
  }
}

window.addEventListener('resize', resizeCanvas);
updateModeUi();
updateReadouts({});
setChartTool('select');
updateZoomStatus();
resizeCanvas();
poll();
</script>
</body>
</html>
)rawliteral";
