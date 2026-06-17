const char DASHBOARD_HTML[] PROGMEM = R"rawliteral(
<!doctype html>
<html lang="el">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Palladio Physics Lab API</title>
<style>
:root {
  color-scheme: light;
  --blue: #2563eb;
  --blue-dark: #173f8a;
  --ink: #142033;
  --muted: #64748b;
  --line: #dbe3ed;
  --page: #f3f6fa;
  --panel: #ffffff;
  --green: #23824a;
  --red: #c43d3d;
}
* { box-sizing: border-box; }
body {
  margin: 0;
  min-width: 320px;
  min-height: 100vh;
  background: var(--page);
  color: var(--ink);
  font-family: system-ui, -apple-system, BlinkMacSystemFont, "Segoe UI", sans-serif;
}
.shell {
  width: min(760px, calc(100% - 32px));
  margin: 0 auto;
  padding: 38px 0;
}
.panel {
  border: 1px solid var(--line);
  border-radius: 16px;
  background: var(--panel);
  box-shadow: 0 14px 34px rgba(31, 47, 74, 0.08);
  overflow: hidden;
}
.hero {
  padding: 26px;
  background: linear-gradient(120deg, #102a5a, #174b9a 62%, #2563eb);
  color: #fff;
}
.brand {
  display: flex;
  align-items: center;
  gap: 14px;
}
.brand img {
  width: 56px;
  height: 56px;
  border-radius: 50%;
  background: #fff;
}
h1 {
  margin: 0;
  font-size: clamp(28px, 5vw, 44px);
  line-height: 1.05;
}
.hero p {
  max-width: 580px;
  margin: 14px 0 0;
  color: #d9e8ff;
  font-size: 16px;
  line-height: 1.55;
}
.content {
  padding: 22px 26px 26px;
}
.status {
  display: grid;
  grid-template-columns: repeat(3, minmax(0, 1fr));
  gap: 10px;
  margin-bottom: 18px;
}
.card {
  min-height: 86px;
  padding: 13px 14px;
  border: 1px solid var(--line);
  border-radius: 12px;
  background: #fbfdff;
}
.card span {
  display: block;
  color: var(--muted);
  font-size: 12px;
  font-weight: 800;
  text-transform: uppercase;
  letter-spacing: 0.04em;
}
.card strong {
  display: block;
  margin-top: 5px;
  font-size: 22px;
}
.actions {
  display: flex;
  flex-wrap: wrap;
  gap: 10px;
  margin: 18px 0;
}
a.button, button {
  min-height: 42px;
  display: inline-flex;
  align-items: center;
  justify-content: center;
  border: 1px solid #cbd6e2;
  border-radius: 10px;
  background: #fff;
  color: var(--ink);
  padding: 0 16px;
  font: inherit;
  font-weight: 800;
  text-decoration: none;
}
a.primary {
  border-color: var(--blue);
  background: var(--blue);
  color: #fff;
}
.note {
  margin: 0;
  color: var(--muted);
  font-size: 14px;
  line-height: 1.55;
}
code {
  padding: 2px 5px;
  border-radius: 6px;
  background: #eef4ff;
  color: var(--blue-dark);
}
.dot {
  display: inline-block;
  width: 10px;
  height: 10px;
  margin-right: 7px;
  border-radius: 50%;
  background: var(--green);
}
.offline { background: var(--red); }
@media (max-width: 640px) {
  .shell { width: min(100% - 20px, 760px); padding: 20px 0; }
  .hero, .content { padding: 20px; }
  .status { grid-template-columns: 1fr; }
}
</style>
</head>
<body>
<main class="shell">
  <section class="panel">
    <header class="hero">
      <div class="brand">
        <img src="/logo.png" alt="Palladio Lab">
        <div>
          <h1>Palladio Lab</h1>
          <p>Το M5Stick λειτουργεί τώρα ως ελαφριά συσκευή μέτρησης/API. Το πλήρες dashboard τρέχει εξωτερικά από GitHub Pages ή τοπικό αρχείο.</p>
        </div>
      </div>
    </header>
    <div class="content">
      <div class="status">
        <div class="card"><span>Συσκευή</span><strong id="device">M5Stick</strong></div>
        <div class="card"><span>Λειτουργία</span><strong id="mode">-</strong></div>
        <div class="card"><span>Μπαταρία</span><strong id="battery">-</strong></div>
      </div>
      <p class="note"><span class="dot" id="dot"></span><span id="message">Έλεγχος σύνδεσης API...</span></p>
      <div class="actions">
        <a class="button primary" id="external" href="https://sotkap.github.io/Physics_Lab-/">Άνοιγμα external dashboard</a>
        <a class="button" href="/samples?since=0">Δείγματα JSON</a>
        <a class="button" href="/data">Τρέχουσα μέτρηση</a>
      </div>
      <p class="note">
        Αν το GitHub Pages dashboard δεν μπορεί να διαβάσει το <code>http://192.168.4.1</code> λόγω περιορισμών browser,
        άνοιξε το ίδιο <code>docs/index.html</code> τοπικά ή τρέξε έναν μικρό local server από τον υπολογιστή.
      </p>
    </div>
  </section>
</main>
<script>
const dashboardUrl = 'https://sotkap.github.io/Physics_Lab-/?device=http%3A%2F%2F192.168.4.1';
document.getElementById('external').href = dashboardUrl;

async function refresh() {
  const dot = document.getElementById('dot');
  const message = document.getElementById('message');
  try {
    const response = await fetch('/data', { cache: 'no-store' });
    if (!response.ok) throw new Error('HTTP ' + response.status);
    const data = await response.json();
    document.getElementById('device').textContent = data.ap || 'PhysicsLab-M5';
    document.getElementById('mode').textContent = data.modeName || '-';
    document.getElementById('battery').textContent = Number(data.battery || 0).toFixed(2) + ' V';
    message.textContent = 'Το API είναι ενεργό στο http://192.168.4.1';
    dot.classList.remove('offline');
  } catch (error) {
    message.textContent = 'Δεν διαβάζονται δεδομένα αυτή τη στιγμή.';
    dot.classList.add('offline');
  } finally {
    setTimeout(refresh, 1000);
  }
}
refresh();
</script>
</body>
</html>
)rawliteral";
