(function (global) {
  'use strict';

  function escapeHtml(value) {
    return String(value ?? '')
      .replace(/&/g, '&amp;')
      .replace(/</g, '&lt;')
      .replace(/>/g, '&gt;')
      .replace(/"/g, '&quot;')
      .replace(/'/g, '&#39;');
  }

  function listItems(items) {
    return items.map(item => '<li>' + escapeHtml(item) + '</li>').join('');
  }

  function buildSonarWorksheetRows(rows) {
    return rows.map((row, index) =>
      '<tr class="' + (row.active ? 'active' : '') + '">' +
      '<td>' + escapeHtml(row.label) + '</td>' +
      '<td>' + escapeHtml(row.dtMs || '—') + '</td>' +
      '<td><input data-sonar-seconds-answer="' + index + '" inputmode="decimal" value="' +
      escapeHtml(row.dtS) + '" aria-label="Χρόνος σε δευτερόλεπτα για ' + escapeHtml(row.label) + '"></td>' +
      '<td><input data-sonar-distance-answer="' + index + '" inputmode="decimal" value="' +
      escapeHtml(row.distance) + '" aria-label="Απόσταση μαθητή για ' + escapeHtml(row.label) + '"></td>' +
      '</tr>'
    ).join('');
  }

  function buildSonarWorksheetData(positions, activeIndex, measuredEchoMs, answers, formatNumber) {
    const seconds = answers && answers.seconds ? answers.seconds : [];
    const distances = answers && answers.distance ? answers.distance : [];
    return positions.map((position, index) => ({
      label: position.label,
      active: index === activeIndex,
      dtMs: Number.isFinite(measuredEchoMs[index]) ? formatNumber(measuredEchoMs[index]) : '',
      dtS: seconds[index] || '',
      distance: distances[index] || ''
    }));
  }

  function buildSonarWorksheetReport(rows, conclusion, timestamp) {
    const rowsHtml = rows.map(row =>
      '<tr><td>' + escapeHtml(row.label) + '</td><td>' + escapeHtml(row.dtMs) + '</td><td>' +
      escapeHtml(row.dtS) + '</td><td>' + escapeHtml(row.distance) + '</td></tr>'
    ).join('');
    const conclusionText = escapeHtml(conclusion || '');
    return '<!doctype html><html lang="el"><head><meta charset="utf-8"><title>Αναφορά Sonar</title>' +
      '<style>body{font-family:system-ui,-apple-system,BlinkMacSystemFont,\"Segoe UI\",sans-serif;margin:32px;color:#172033;}' +
      'h1{margin:0 0 6px;font-size:26px;}h2{margin:22px 0 8px;font-size:17px;color:#1f3650;}p,li{color:#4c5f75;line-height:1.5;}table{width:100%;border-collapse:collapse;margin:18px 0;}' +
      'th,td{border:1px solid #cfdbe8;padding:10px;text-align:left;}th{background:#edf4fb;color:#344861;font-size:12px;text-transform:uppercase;}' +
      '.box{border:1px solid #cfdbe8;border-radius:12px;padding:14px;margin-top:16px;}.meta{margin-top:4px;color:#60758c;font-size:13px;}' +
      '.grid{display:grid;grid-template-columns:1fr 1fr;gap:12px}.answer-line{min-height:34px;border-bottom:1px solid #cfdbe8;margin-top:8px;}' +
      '.formula{display:inline-block;padding:8px 12px;border-radius:10px;background:#edf4fb;color:#173b73;font-weight:800;}' +
      '@media(max-width:760px){.grid{grid-template-columns:1fr}}' +
      '@media print{body{margin:18mm}.no-print{display:none}}</style></head><body>' +
      '<h1>Αναφορά πειράματος Sonar</h1><div class="meta">Palladio Physics Lab · ' + escapeHtml(timestamp) + '</div>' +
      '<div class="box"><strong>Κεντρική ερώτηση</strong><p>Πώς μπορούμε να υπολογίσουμε μια απόσταση όταν μετράμε τον συνολικό χρόνο που χρειάζεται ένας ηχητικός παλμός για να πάει σε ένα εμπόδιο και να επιστρέψει;</p></div>' +
      '<div class="grid">' +
      '<div class="box"><strong>Στόχος</strong><p>Να χρησιμοποιήσω πραγματικό χρόνο ηχούς από DFRobot URM10 και να υπολογίσω απόσταση με τη σύμβαση ότι ο ήχος ταξιδεύει στο νερό.</p></div>' +
      '<div class="box"><strong>Υλικά / εργαλεία</strong><p>Palladio Physics Lab, M5Stick, αισθητήρας DFRobot URM10, επίπεδο εμπόδιο, τρεις θέσεις σκάφους, αριθμομηχανή.</p></div>' +
      '</div>' +
      '<h2>Θεωρητική ιδέα</h2>' +
      '<p>Ο DFRobot URM10 μετρά πραγματικό χρόνο ηχούς στον αέρα. Στο πείραμα κάνουμε τη σύμβαση ότι αυτός ο χρόνος είναι χρόνος sonar μέσα στο νερό.</p>' +
      '<p>Ο ήχος διανύει τη διαδρομή αισθητήρας → στόχος → αισθητήρας. Άρα ο συνολικός χρόνος Δt αντιστοιχεί σε διπλάσια απόσταση από αυτή που ζητάμε.</p>' +
      '<p><span class="formula">d = υ · Δt / 2</span> με ταχύτητα ήχου στο νερό περίπου υ = 1480 m/s.</p>' +
      '<h2>Διαδικασία</h2>' +
      '<ol><li>Διάλεξα μία θέση του σκάφους.</li><li>Μέτρησα με τον DFRobot URM10 τον συνολικό χρόνο ηχούς Δt σε ms.</li><li>Μετέτρεψα τα ms σε s.</li><li>Υπολόγισα την απόσταση d με τη σύμβαση του νερού.</li><li>Σύγκρινα τις τρεις θέσεις.</li></ol>' +
      '<h2>Μετρήσεις και υπολογισμοί</h2>' +
      '<table><thead><tr><th>Θέση</th><th>Δt URM10 (ms)</th><th>Δt (s)</th><th>d μαθητή (m)</th></tr></thead><tbody>' + rowsHtml + '</tbody></table>' +
      '<h2>Ερωτήσεις</h2>' +
      '<div class="box"><strong>1. Γιατί διαιρούμε τον συνολικό χρόνο με το 2;</strong><div class="answer-line"></div></div>' +
      '<div class="box"><strong>2. Ποια θέση αντιστοιχεί στο μεγαλύτερο βάθος;</strong><div class="answer-line"></div></div>' +
      '<div class="box"><strong>3. Ο αισθητήρας μετρά άμεσα απόσταση ή χρόνο;</strong><div class="answer-line"></div></div>' +
      '<div class="box"><strong>Συμπέρασμα μαθητή</strong><p>' + (conclusionText || '&nbsp;') + '</p></div>' +
      '<p class="no-print">Στο παράθυρο εκτύπωσης διάλεξε “Save as PDF” / “Αποθήκευση ως PDF”.</p>' +
      '<script>window.addEventListener(\"load\",()=>setTimeout(()=>window.print(),150));<\/script>' +
      '</body></html>';
  }

  global.PalladioWorksheetUtils = {
    listItems,
    buildSonarWorksheetRows,
    buildSonarWorksheetData,
    buildSonarWorksheetReport
  };
})(typeof window !== 'undefined' ? window : globalThis);
