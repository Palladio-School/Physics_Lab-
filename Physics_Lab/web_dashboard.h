const char DASHBOARD_HTML[] PROGMEM = R"rawliteral(
<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Palladio Physics Lab</title>
<style>
:root {
  color-scheme: dark;
  --bg: #111316;
  --panel: #1b1f24;
  --panel-2: #242a31;
  --text: #f4f7fb;
  --muted: #9aa6b2;
  --line: #35404b;
  --blue: #3f8cff;
  --green: #35c46a;
  --red: #ff4d4d;
  --orange: #ffb02e;
}
* { box-sizing: border-box; }
body {
  margin: 0;
  background: var(--bg);
  color: var(--text);
  font-family: system-ui, -apple-system, BlinkMacSystemFont, "Segoe UI", sans-serif;
}
button {
  border: 0;
  border-radius: 8px;
  background: var(--panel-2);
  color: var(--text);
  min-height: 42px;
  padding: 0 14px;
  font: inherit;
  font-weight: 700;
}
button.active { background: var(--blue); }
button.primary { background: var(--blue); }
button.danger { background: #3a2228; color: #ffb5c0; }
.app {
  min-height: 100vh;
  display: grid;
  grid-template-rows: auto 1fr;
}
.topbar {
  min-height: 64px;
  padding: 12px clamp(12px, 3vw, 28px);
  border-bottom: 1px solid var(--line);
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 16px;
}
.brand h1 {
  margin: 0;
  font-size: clamp(20px, 2vw, 30px);
  letter-spacing: 0;
}
.brand span {
  color: var(--muted);
  font-size: 13px;
}
.status {
  display: flex;
  align-items: center;
  gap: 10px;
  color: var(--muted);
  font-weight: 700;
}
.dot {
  width: 10px;
  height: 10px;
  border-radius: 999px;
  background: var(--green);
}
.layout {
  display: grid;
  grid-template-columns: 280px 1fr;
  gap: 16px;
  padding: 16px;
  min-height: 0;
}
.sidebar, .chart-panel, .table-panel {
  background: var(--panel);
  border: 1px solid var(--line);
  border-radius: 8px;
}
.sidebar {
  padding: 14px;
  display: flex;
  flex-direction: column;
  gap: 14px;
}
.modes, .actions {
  display: grid;
  gap: 8px;
}
.readouts {
  display: grid;
  gap: 8px;
}
.readout {
  background: var(--panel-2);
  border-radius: 8px;
  padding: 12px;
}
.readout span {
  display: block;
  color: var(--muted);
  font-size: 12px;
  font-weight: 800;
  text-transform: uppercase;
}
.readout strong {
  display: block;
  margin-top: 4px;
  font-size: 24px;
  line-height: 1.1;
}
.main {
  min-width: 0;
  display: grid;
  grid-template-rows: minmax(340px, 1fr) auto;
  gap: 16px;
}
.chart-panel {
  min-width: 0;
  min-height: 340px;
  padding: 14px;
  display: grid;
  grid-template-rows: auto 1fr;
  gap: 10px;
}
.chart-head {
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 12px;
}
.chart-head h2 {
  margin: 0;
  font-size: clamp(18px, 2vw, 28px);
}
.chart-head span {
  color: var(--muted);
  font-weight: 700;
}
.canvas-wrap {
  position: relative;
  min-height: 280px;
}
canvas {
  width: 100%;
  height: 100%;
  display: block;
}
.table-panel {
  overflow: hidden;
}
table {
  width: 100%;
  border-collapse: collapse;
  font-size: 14px;
}
th, td {
  padding: 10px 12px;
  border-bottom: 1px solid var(--line);
  text-align: right;
  white-space: nowrap;
}
th:first-child, td:first-child { text-align: left; }
th {
  color: var(--muted);
  background: #171a1f;
}
@media (max-width: 820px) {
  .topbar {
    align-items: flex-start;
    flex-direction: column;
  }
  .layout {
    grid-template-columns: 1fr;
  }
  .sidebar {
    order: 2;
  }
  .main {
    order: 1;
    grid-template-rows: minmax(320px, 58vh) auto;
  }
  .modes, .actions {
    grid-template-columns: repeat(2, minmax(0, 1fr));
  }
  .readouts {
    grid-template-columns: repeat(2, minmax(0, 1fr));
  }
}
@media (max-width: 520px) {
  .layout { padding: 10px; }
  .modes, .actions, .readouts {
    grid-template-columns: 1fr;
  }
  th, td { padding: 8px; font-size: 12px; }
}
</style>
</head>
<body>
<div class="app">
  <header class="topbar">
    <div class="brand">
      <h1>Palladio Physics Lab</h1>
      <span>M5StickC Plus2 live dashboard</span>
    </div>
    <div class="status"><span class="dot"></span><span id="network">PhysicsLab-M5</span></div>
  </header>
  <main class="layout">
    <aside class="sidebar">
      <div class="modes">
        <button data-mode="0">Acceleration</button>
        <button data-mode="1">Gyroscope</button>
        <button data-mode="2">Load Cell</button>
        <button data-mode="4">Ultrasonic</button>
      </div>
      <div class="readouts" id="readouts"></div>
      <div class="actions">
        <button id="toggle" class="primary">Pause</button>
        <button id="clear">Clear</button>
        <button id="export">Export CSV</button>
      </div>
    </aside>
    <section class="main">
      <div class="chart-panel">
        <div class="chart-head">
          <h2 id="chartTitle">Acceleration</h2>
          <span id="sampleCount">0 samples</span>
        </div>
        <div class="canvas-wrap">
          <canvas id="chart"></canvas>
        </div>
      </div>
      <div class="table-panel">
        <table>
          <thead id="tableHead"></thead>
          <tbody id="tableBody"></tbody>
        </table>
      </div>
    </section>
  </main>
</div>
<script>
const maxSamples = 180;
let running = true;
let rows = [];
let activeMode = 0;
const canvas = document.getElementById('chart');
const ctx = canvas.getContext('2d');
const modeButtons = [...document.querySelectorAll('[data-mode]')];

const modes = {
  0: { title: 'Acceleration', units: 'm/s2', keys: ['ax', 'ay', 'az'], labels: ['X', 'Y', 'Z'], colors: ['#ff4d4d', '#35c46a', '#3f8cff'] },
  1: { title: 'Gyroscope', units: 'deg/s', keys: ['gx', 'gy', 'gz'], labels: ['Gx', 'Gy', 'Gz'], colors: ['#ff4d4d', '#35c46a', '#3f8cff'] },
  2: { title: 'Load Cell Force', units: 'N', keys: ['force'], labels: ['Force'], colors: ['#ffb02e'] },
  4: { title: 'Ultrasonic Motion', units: '', keys: ['distance', 'velocity', 'acceleration'], labels: ['Distance m', 'Velocity m/s', 'Acceleration m/s2'], colors: ['#ffb02e', '#3f8cff', '#35c46a'] }
};

function resizeCanvas() {
  const rect = canvas.getBoundingClientRect();
  const ratio = window.devicePixelRatio || 1;
  canvas.width = Math.max(320, Math.floor(rect.width * ratio));
  canvas.height = Math.max(220, Math.floor(rect.height * ratio));
  ctx.setTransform(ratio, 0, 0, ratio, 0, 0);
  draw();
}

function setMode(mode) {
  fetch('/set-mode?mode=' + mode).catch(() => {});
  activeMode = mode;
  rows = [];
  updateModeUi();
}

function updateModeUi() {
  const meta = modes[activeMode] || modes[0];
  document.getElementById('chartTitle').textContent = meta.title;
  modeButtons.forEach(button => button.classList.toggle('active', Number(button.dataset.mode) === activeMode));
  document.getElementById('tableHead').innerHTML = '<tr><th>Time</th>' + meta.labels.map(label => '<th>' + label + '</th>').join('') + '</tr>';
  updateTable();
  draw();
}

function updateReadouts(data) {
  const meta = modes[activeMode] || modes[0];
  const cards = meta.keys.map((key, index) => {
    const value = Number(data[key] || 0).toFixed(2);
    return '<div class="readout"><span>' + meta.labels[index] + '</span><strong>' + value + '</strong></div>';
  });
  cards.push('<div class="readout"><span>Battery</span><strong>' + Number(data.battery || 0).toFixed(2) + 'V</strong></div>');
  document.getElementById('readouts').innerHTML = cards.join('');
}

function addRow(data) {
  const mode = Number(data.mode);
  if (mode !== activeMode && modes[mode]) {
    activeMode = mode;
    rows = [];
    updateModeUi();
  }
  if (!running || !modes[activeMode]) return;
  const meta = modes[activeMode];
  const row = { time: new Date().toLocaleTimeString(), values: meta.keys.map(key => Number(data[key] || 0)) };
  rows.push(row);
  if (rows.length > maxSamples) rows.shift();
  updateReadouts(data);
  updateTable();
  draw();
}

function updateTable() {
  const latest = rows.slice(-8).reverse();
  document.getElementById('sampleCount').textContent = rows.length + ' samples';
  document.getElementById('tableBody').innerHTML = latest.map(row => {
    return '<tr><td>' + row.time + '</td>' + row.values.map(value => '<td>' + value.toFixed(3) + '</td>').join('') + '</tr>';
  }).join('');
}

function draw() {
  const rect = canvas.getBoundingClientRect();
  const width = rect.width;
  const height = rect.height;
  ctx.clearRect(0, 0, width, height);
  ctx.fillStyle = '#12161a';
  ctx.fillRect(0, 0, width, height);

  const pad = { left: 54, right: 16, top: 18, bottom: 34 };
  const plotW = Math.max(1, width - pad.left - pad.right);
  const plotH = Math.max(1, height - pad.top - pad.bottom);
  const meta = modes[activeMode] || modes[0];
  const values = rows.flatMap(row => row.values);
  let min = values.length ? Math.min(...values) : -1;
  let max = values.length ? Math.max(...values) : 1;
  if (Math.abs(max - min) < 0.001) { max += 1; min -= 1; }
  const span = max - min;
  min -= span * 0.12;
  max += span * 0.12;

  ctx.strokeStyle = '#35404b';
  ctx.lineWidth = 1;
  ctx.font = '12px system-ui';
  ctx.fillStyle = '#9aa6b2';
  for (let i = 0; i <= 4; i++) {
    const y = pad.top + (plotH * i / 4);
    const value = max - ((max - min) * i / 4);
    ctx.beginPath();
    ctx.moveTo(pad.left, y);
    ctx.lineTo(width - pad.right, y);
    ctx.stroke();
    ctx.fillText(value.toFixed(1), 8, y + 4);
  }

  meta.keys.forEach((key, seriesIndex) => {
    ctx.strokeStyle = meta.colors[seriesIndex];
    ctx.lineWidth = 2.5;
    ctx.beginPath();
    rows.forEach((row, index) => {
      const x = pad.left + (rows.length <= 1 ? 0 : index * plotW / (rows.length - 1));
      const value = row.values[seriesIndex];
      const y = pad.top + (max - value) * plotH / (max - min);
      if (index === 0) ctx.moveTo(x, y);
      else ctx.lineTo(x, y);
    });
    ctx.stroke();
  });

  let legendX = pad.left;
  meta.labels.forEach((label, index) => {
    ctx.fillStyle = meta.colors[index];
    ctx.fillRect(legendX, height - 22, 18, 3);
    ctx.fillStyle = '#f4f7fb';
    ctx.fillText(label, legendX + 24, height - 17);
    legendX += 110;
  });
}

modeButtons.forEach(button => button.addEventListener('click', () => setMode(Number(button.dataset.mode))));
document.getElementById('toggle').addEventListener('click', event => {
  running = !running;
  event.target.textContent = running ? 'Pause' : 'Resume';
});
document.getElementById('clear').addEventListener('click', () => {
  rows = [];
  updateTable();
  draw();
});
document.getElementById('export').addEventListener('click', () => {
  const meta = modes[activeMode] || modes[0];
  const csv = [['time', ...meta.labels], ...rows.map(row => [row.time, ...row.values])].map(row => row.join(',')).join('\n');
  const blob = new Blob([csv], { type: 'text/csv' });
  const url = URL.createObjectURL(blob);
  const link = document.createElement('a');
  link.href = url;
  link.download = meta.title.toLowerCase().replaceAll(' ', '-') + '.csv';
  link.click();
  URL.revokeObjectURL(url);
});

async function poll() {
  try {
    const response = await fetch('/data', { cache: 'no-store' });
    const data = await response.json();
    document.getElementById('network').textContent = data.ap + ' / ' + data.ip;
    addRow(data);
  } catch (error) {
    document.getElementById('network').textContent = 'Waiting for M5';
  } finally {
    setTimeout(poll, 100);
  }
}

window.addEventListener('resize', resizeCanvas);
updateModeUi();
resizeCanvas();
poll();
</script>
</body>
</html>
)rawliteral";
