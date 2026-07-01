(function (global) {
  'use strict';

  function pendulumPeriod(lengthM) {
    return 2 * Math.PI * Math.sqrt(Math.max(0.01, lengthM) / 9.81);
  }

  function pendulumSample(time, lengthM, angleDeg) {
    const period = pendulumPeriod(lengthM);
    const theta = angleDeg * Math.cos(2 * Math.PI * time / period);
    const omega = -angleDeg * (2 * Math.PI / period) * Math.sin(2 * Math.PI * time / period);
    const amplitudeCm = lengthM * (angleDeg * Math.PI / 180) * 100;
    const displacementCm = lengthM * (theta * Math.PI / 180) * 100;
    return {
      pendulumDisplacementCm: displacementCm,
      pendulumAmplitudeCm: amplitudeCm,
      theta,
      omegaPendulum: omega
    };
  }

  function pendulumIntervalInfo(delta, lengthM, formatNumber) {
    const period = pendulumPeriod(lengthM);
    const format = formatNumber || (value => String(value));
    if (!isFinite(delta) || delta <= 0 || !isFinite(period) || period <= 0) return null;
    const ratio = delta / period;
    const whole = Math.round(ratio);
    if (whole >= 1 && Math.abs(ratio - whole) <= 0.08) {
      return {
        label: whole === 1 ? 'περίπου 1 πλήρης ταλάντωση' : 'περίπου ' + whole + ' πλήρεις ταλαντώσεις',
        detail: 'Μπορείς να βάλεις N = ' + whole + ' και να υπολογίσεις T = Δt / N.',
        oscillations: whole
      };
    }
    const targets = [
      { value: 0.25, label: 'περίπου T/4', detail: 'Χρόνος από τη θέση ισορροπίας σε μία ακραία θέση, ή αντίστροφα.' },
      { value: 0.5, label: 'περίπου T/2', detail: 'Χρόνος από την ακραία θέση Α στην ακραία θέση Β.' },
      { value: 0.75, label: 'περίπου 3T/4', detail: 'Τρία τέταρτα μιας πλήρους ταλάντωσης.' }
    ];
    const nearest = targets.reduce((best, item) => {
      return Math.abs(ratio - item.value) < Math.abs(ratio - best.value) ? item : best;
    }, targets[0]);
    if (Math.abs(ratio - nearest.value) <= 0.08) return nearest;
    return {
      label: format(ratio) + ' T',
      detail: 'Δεν είναι καθαρός χαρακτηριστικός χρόνος. Διάλεξε από κορυφή σε κορυφή για πλήρη περίοδο.',
      oscillations: null
    };
  }

  global.PalladioPendulumUtils = {
    pendulumPeriod,
    pendulumSample,
    pendulumIntervalInfo
  };
})(window);
