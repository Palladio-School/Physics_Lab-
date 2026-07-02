(function (global) {
  'use strict';

  function buildMotionSummaryCards(options, formatNumber) {
    const metrics = options.metrics;
    if (!metrics || !metrics.ready) return [];
    const cards = [
      { label: 'Διάρκεια', value: formatNumber(metrics.duration), detail: 's στο επιλεγμένο τμήμα' }
    ];
    if (options.velocityArea) {
      cards.push({
        label: 'Εμβαδό υ–t',
        value: formatNumber(options.velocityArea.signedArea),
        detail: 'm μετατόπιση στο A–B'
      });
    }
    if (options.seriesKey === 'distance') {
      cards.push(
        { label: 'Δx', value: formatNumber(metrics.delta), detail: 'm μετατόπιση' },
        { label: 'Μέση ταχύτητα', value: formatNumber(metrics.slope), detail: 'm/s από την κλίση' },
        { label: 'R² fit', value: metrics.fitR2 !== null ? formatNumber(metrics.fitR2) : '—', detail: 'όσο πιο κοντά στο 1 τόσο πιο γραμμικό' }
      );
    } else if (options.seriesKey === 'velocity') {
      cards.push(
        { label: 'Μέση ταχύτητα', value: formatNumber(metrics.mean), detail: 'm/s στο τμήμα' },
        { label: 'Τυπική απόκλιση', value: formatNumber(metrics.stdDev), detail: 'm/s σταθερότητα' },
        { label: 'Κλίση υ-t', value: formatNumber(metrics.slope), detail: 'm/s² ιδανικά κοντά στο 0' }
      );
    } else {
      cards.push(
        { label: 'Μέση επιτάχυνση', value: formatNumber(metrics.mean), detail: 'm/s²' },
        { label: 'Τυπική απόκλιση', value: formatNumber(metrics.stdDev), detail: 'm/s²' },
        { label: 'Γραμμικότητα', value: metrics.linearR2 !== null ? formatNumber(metrics.linearR2) : '—', detail: 'βοηθητικός έλεγχος' }
      );
    }
    return cards;
  }

  function buildPendulumSummaryCards(options, formatNumber) {
    const cards = [
      { label: 'Μήκος L', value: formatNumber(options.lengthM), detail: 'm' },
      { label: 'Μάζα m', value: formatNumber(options.massG), detail: 'g · δεν αλλάζει το T' },
      {
        label: 'Πλάτος A',
        value: formatNumber(options.lengthM * (options.angleDeg * Math.PI / 180) * 100),
        detail: 'cm από τη θέση ισορροπίας'
      }
    ];
    if (options.inspectSample) {
      cards.push({
        label: 'Θέση σώματος',
        value: formatNumber(options.inspectSample.pendulumDisplacementCm),
        detail: 'cm στη στιγμή t = ' + formatNumber(options.inspectTime) + ' s'
      });
    }
    cards.push(
      { label: 'T από μέτρηση', value: formatNumber(options.measuredPeriod), detail: 's = Δt / N' },
      { label: 'T θεωρητική', value: formatNumber(options.period), detail: 's = 2π√(L/g), ανεξάρτητη από m' },
      { label: 'T/2', value: formatNumber(options.period / 2), detail: 'ακραία θέση σε αντίθετη ακραία' },
      { label: 'T/4', value: formatNumber(options.period / 4), detail: 'ισορροπία προς ακραία θέση' }
    );
    if (options.selectedDelta !== null) {
      cards.push({ label: 'Επιλεγμένο Δt A-B', value: formatNumber(options.selectedDelta), detail: 's από τη γραφική' });
      if (options.selectedInfo) {
        cards.push({ label: 'Το A-B μοιάζει με', value: options.selectedInfo.label, detail: options.selectedInfo.detail });
      }
    }
    return cards;
  }

  function buildHeatSummaryCards(options, formatNumber) {
    return [
      { label: 'Αισθητήρας T1', value: formatNumber(options.t1), detail: '°C κρύο νερό' },
      { label: 'Αισθητήρας T2', value: formatNumber(options.t2), detail: '°C ζεστό νερό' },
      { label: 'Διαφορά ΔT', value: formatNumber(options.delta), detail: '°C = T1 - T2' },
      { label: '|ΔT|', value: formatNumber(options.absDelta), detail: 'όσο μικραίνει, πλησιάζουν ισορροπία' }
    ];
  }

  global.PalladioPanelDataUtils = {
    buildMotionSummaryCards,
    buildPendulumSummaryCards,
    buildHeatSummaryCards
  };
})(typeof window !== 'undefined' ? window : globalThis);
