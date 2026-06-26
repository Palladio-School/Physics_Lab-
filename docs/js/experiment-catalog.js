(function (global) {
const modes = {
  0: {
    symmetric: true,
    minimumSpan: 24,
    views: {
      accel: {
        title: 'Επιτάχυνση',
        subtitle: 'Συνιστώσες επιτάχυνσης ως προς τον χρόνο',
        yLabel: 'Επιτάχυνση (m/s²)',
        minimumSpan: 24,
        series: [
          { key: 'ax', label: 'Άξονας X', short: 'X', unit: 'm/s²', color: '#d63c3c', enabled: true },
          { key: 'ay', label: 'Άξονας Y', short: 'Y', unit: 'm/s²', color: '#2d9d57', enabled: true },
          { key: 'az', label: 'Άξονας Z', short: 'Z', unit: 'm/s²', color: '#1769aa', enabled: true }
        ]
      },
      force: {
        title: 'Δύναμη από επιτάχυνση',
        subtitle: 'Υπολογισμός F = m·a από τις μετρήσεις επιτάχυνσης',
        yLabel: 'Δύναμη (N)',
        minimumSpan: 2,
        series: [
          { key: 'fx', label: 'Δύναμη X', short: 'Fx', unit: 'N', color: '#d63c3c', enabled: true },
          { key: 'fy', label: 'Δύναμη Y', short: 'Fy', unit: 'N', color: '#2d9d57', enabled: true },
          { key: 'fz', label: 'Δύναμη Z', short: 'Fz', unit: 'N', color: '#1769aa', enabled: true },
          { key: 'fnet', label: 'Συνισταμένη |F|', short: '|F|', unit: 'N', color: '#7b4db7', enabled: false }
        ]
      }
    }
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
    title: 'Κίνηση με αισθητήρα υπερήχων',
    subtitle: 'Επίλεξε ένα φυσικό μέγεθος για σωστή κλίμακα και μονάδα',
    yLabel: '',
    symmetric: false,
    minimumSpan: 1,
    series: [
      { key: 'distance', label: 'Απόσταση', short: 'd', unit: 'm', color: '#7b4db7', enabled: true },
      { key: 'velocity', label: 'Ταχύτητα', short: 'υ', unit: 'm/s', color: '#1769aa', enabled: false },
      { key: 'acceleration', label: 'Επιτάχυνση', short: 'α', unit: 'm/s²', color: '#2d9d57', enabled: false }
    ]
  },
  5: {
    title: 'Θερμότητα',
    subtitle: 'Δύο αισθητήρες θερμοκρασίας DS18B20 ως προς τον χρόνο',
    yLabel: 'Θερμοκρασία (°C)',
    symmetric: false,
    minimumSpan: 5,
    series: [
      { key: 'temp1', label: 'Θερμόμετρο 1', short: 'T1', unit: '°C', color: '#d63c3c', enabled: true },
      { key: 'temp2', label: 'Θερμόμετρο 2', short: 'T2', unit: '°C', color: '#1769aa', enabled: true },
      { key: 'tempDelta', label: 'Διαφορά ΔT', short: 'ΔT', unit: '°C', color: '#2d9d57', enabled: false }
    ]
  },
  6: {
    title: 'Απλό εκκρεμές',
    subtitle: 'Ψηφιακή προσομοίωση απομάκρυνσης και πλάτους ως προς τον χρόνο',
    yLabel: 'Απομάκρυνση (cm)',
    symmetric: true,
    minimumSpan: 30,
    digital: true,
    series: [
      { key: 'pendulumDisplacementCm', label: 'Απομάκρυνση x', short: 'x', unit: 'cm', color: '#7b4db7', enabled: true },
      { key: 'pendulumAmplitudeCm', label: 'Πλάτος A', short: 'A', unit: 'cm', color: '#e58a16', enabled: true },
      { key: 'theta', label: 'Γωνία θ', short: 'θ', unit: '°', color: '#2d9d57', enabled: false },
      { key: 'omegaPendulum', label: 'Γωνιακή ταχύτητα', short: 'ω', unit: '°/s', color: '#1769aa', enabled: false }
    ]
  }
};

const experimentCatalog = {
  'a-motion': {
    mode: 4,
    title: 'Απόσταση και χρόνος',
    ready: 'Βάλε ένα αντικείμενο μπροστά στον αισθητήρα υπερήχων. Πάτα Έναρξη και μετακίνησέ το αργά.',
    running: 'Παρατήρησε αν η απόσταση μεγαλώνει ή μικραίνει όσο περνά ο χρόνος.',
    selected: 'Στο κομμάτι που διάλεξες, δες πόσο άλλαξε η απόσταση και πόσο χρόνο χρειάστηκε.'
  },
  'a-sonar': {
    mode: 4,
    title: 'SONAR: Μέτρηση απόστασης με ήχο',
    ready: 'Βάλε ένα επίπεδο αντικείμενο απέναντι από τον DFRobot URM10, διάλεξε θέση σκάφους και πάτα Έναρξη για να κρατήσεις τον χρόνο ηχούς.',
    running: 'Ο DFRobot URM10 μετρά πραγματικό χρόνο ηχούς στον αέρα. Στο σενάριο τον ερμηνεύουμε σαν μέτρηση sonar στο νερό.',
    selected: 'Χρησιμοποίησε το Δt που κρατήθηκε και υπολόγισε την απόσταση με d = 1480 · Δt / 2.'
  },
  'a-speed': {
    mode: 4,
    title: 'Μέση ταχύτητα',
    ready: 'Κίνησε το αντικείμενο όσο πιο ομαλά μπορείς μπροστά από τον αισθητήρα.',
    running: 'Αν η γραμμή θέσης είναι σχεδόν ευθεία, η ταχύτητα είναι πιο σταθερή.',
    selected: 'Η κλίση στο επιλεγμένο κομμάτι δείχνει τη μέση ταχύτητα.'
  },
  'a-weight': {
    mode: 2,
    forceExperiment: 'weight',
    title: 'Μάζα/Βάρος - επιμήκυνση ελατηρίου',
    ready: 'Κάνε μηδενισμό δύναμης, κρέμασε μάζα στο ελατήριο και μέτρα βάρος και επιμήκυνση.',
    running: 'Ψάξε ένα ήρεμο οριζόντιο τμήμα, όπου το βάρος μένει σχεδόν σταθερό.',
    selected: 'Γράψε τη μάζα και την επιμήκυνση Δx, και σύγκρινε το πειραματικό βάρος με w = m·g.'
  },
  'a-shake': {
    mode: 0,
    title: 'Απότομη κίνηση και επιτάχυνση',
    ready: 'Κράτησε τη συσκευή σταθερά, πάτα Έναρξη και κάνε μια μικρή απότομη κίνηση.',
    running: 'Παρατήρησε τις κορυφές: εκεί η επιτάχυνση αλλάζει πιο έντονα.',
    selected: 'Σύγκρινε την αρχή και το τέλος της απότομης κίνησης.'
  },
  'a-heat': {
    mode: 5,
    title: 'Θερμότητα',
    ready: 'Βάλε το ένα θερμόμετρο στο ποτήρι και το άλλο στη λεκάνη νερού. Η διάρκεια θα μπει στα 10 λεπτά.',
    running: 'Παρατήρησε αν οι δύο θερμοκρασίες πλησιάζουν ή απομακρύνονται με τον χρόνο.',
    selected: 'Στο επιλεγμένο κομμάτι σύγκρινε T1, T2 και τη διαφορά ΔT.'
  },
  'a-pendulum': {
    mode: 6,
    title: 'Απλό εκκρεμές',
    ready: 'Διάλεξε μήκος και μικρή αρχική γωνία και πάτησε Προσομοίωση.',
    running: 'Παρατήρησε την απομάκρυνση x-t και τη σταθερή γραμμή του πλάτους A.',
    selected: 'Στο επιλεγμένο κομμάτι σύγκρινε τη διάρκεια με T/4, T/2 ή μία πλήρη περίοδο.'
  },
  'b-uniform-motion': {
    mode: 4,
    title: 'Ευθύγραμμη ομαλή κίνηση',
    ready: 'Κίνησε ένα αμαξίδιο ή αντικείμενο σε ευθεία γραμμή με όσο γίνεται σταθερή ταχύτητα.',
    running: 'Στόχος είναι να δεις μια γραμμή θέσης-χρόνου που μοιάζει με ευθεία.',
    selected: 'Η κλίση της ευθείας δίνει την ταχύτητα της κίνησης.'
  },
  'b-accelerated-motion': {
    mode: 4,
    title: 'Επιταχυνόμενη κίνηση',
    ready: 'Άφησε το αντικείμενο να κινηθεί έτσι ώστε να αλλάζει η ταχύτητά του, π.χ. σε μικρή κλίση.',
    running: 'Αν η γραμμή θέσης καμπυλώνει, η ταχύτητα δεν μένει σταθερή.',
    selected: 'Δες αν η κλίση μεγαλώνει ή μικραίνει στο επιλεγμένο κομμάτι.'
  },
  'b-hooke': {
    mode: 2,
    forceExperiment: 'hooke',
    title: 'Νόμος του Hooke',
    ready: 'Κρέμασε μάζες στο ελατήριο, διάλεξε σταθερό τμήμα δύναμης και γράψε την επιμήκυνση.',
    running: 'Περίμενε η δύναμη να σταθεροποιηθεί πριν αποθηκεύσεις δοκιμή.',
    selected: 'Αποθήκευσε ζεύγη δύναμης και επιμήκυνσης για να δεις αν βγαίνει ευθεία σχέση.'
  },
  'b-buoyancy': {
    mode: 2,
    forceExperiment: 'buoyancy',
    title: 'Άνωση',
    ready: 'Μέτρησε πρώτα το βάρος στον αέρα και μετά το φαινόμενο βάρος μέσα στο υγρό.',
    running: 'Περίμενε κάθε μέτρηση να γίνει ήρεμη πριν την κρατήσεις.',
    selected: 'Η διαφορά των δύο δυνάμεων δείχνει την άνωση.'
  },
  'b-rotation': {
    mode: 1,
    title: 'Περιστροφή',
    ready: 'Μηδένισε το γυροσκόπιο με τη συσκευή ακίνητη και μετά κάνε μια αργή περιστροφή.',
    running: 'Παρατήρησε πότε η γωνιακή ταχύτητα γίνεται μεγαλύτερη.',
    selected: 'Το επιλεγμένο κομμάτι δείχνει πόσο γρήγορα έγινε η περιστροφή.'
  }
};

global.PalladioExperimentCatalog = {
  modes,
  experimentCatalog
};
})(window);
