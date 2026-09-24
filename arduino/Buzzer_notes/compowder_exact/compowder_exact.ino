// ============================================================================
// compowder_exact.ino — Jingle "Totally Spies Compowder Sound"
// NUCLEO-F767ZI / STM32duino
// Câblage : PA8 --[100 Ω]-- (+)PS1720P02(−) -- GND
//
// ✅ TRANSCRIPTION EXACTE depuis analyse audio (librosa pyin)
//    Fichier source : ytmp3free_cc_totally-spies-compowder-sound.mp3
//    Détection de pitch frame par frame, sr=44100 Hz, hop=512
//
// Résultat de l'analyse :
//   3 notes uniquement : B6, G♯6, F♯6
//   Le jingle se répète 2 fois avec ~1,36 s de silence entre les deux phrases
//
//   Phrase : B6(116ms) → G♯6(500ms) → F♯6(104ms) → G♯6(128ms) → B6(197ms)
//   Silence  1359ms
//   Reprise : B6(128ms) → G♯6(476ms) → F♯6(116ms) → G♯6(116ms) → B6(174ms)
// ============================================================================

#define BUZZER_PIN  PA8

// ── Fréquences exactes (standard Arduino pitches.h, octave 6) ────────────────
#define NOTE_FS6  1480   // Fa♯6  — détecté à 1480–1507 Hz
#define NOTE_GS6  1661   // Sol♯6 — détecté à 1659–1678 Hz
#define NOTE_B6   1976   // Si6   — détecté à 1983–1991 Hz
#define REST         0

// ── Structure note + durée ms ────────────────────────────────────────────────
struct Note {
    int freq;       // Hz (0 = silence)
    int dur_ms;     // durée en ms
};

// ── Phrase 1 (occurrence originale) ──────────────────────────────────────────
const Note phrase1[] = {
    { NOTE_B6,  116 },   // Si6  court
    { NOTE_GS6, 499 },   // Sol♯6 long (116 ms + 23 ms transition + 383 ms)
    { NOTE_FS6, 104 },   // Fa♯6 court
    { NOTE_GS6, 128 },   // Sol♯6 court
    { NOTE_B6,  197 },   // Si6  fin de phrase (noire pointée ~)
    { REST,    1359 },   // silence entre les deux phrases
};

// ── Phrase 2 (répétition) ─────────────────────────────────────────────────────
const Note phrase2[] = {
    { NOTE_B6,  128 },   // Si6  court
    { NOTE_GS6, 476 },   // Sol♯6 long (93 ms + 23 ms + 383 ms)
    { NOTE_FS6, 116 },   // Fa♯6 court
    { NOTE_GS6, 116 },   // Sol♯6 court
    { NOTE_B6,  174 },   // Si6  fin de phrase
};

const int NB_P1 = sizeof(phrase1) / sizeof(phrase1[0]);
const int NB_P2 = sizeof(phrase2) / sizeof(phrase2[0]);

// ── Lecture d'une phrase (bloquant) ──────────────────────────────────────────
void jouerPhrase(const Note* phrase, int nb) {
    for (int i = 0; i < nb; i++) {
        if (phrase[i].freq == REST) {
            noTone(BUZZER_PIN);
            delay(phrase[i].dur_ms);
        } else {
            // 92 % de la durée = articulation légère inter-notes
            tone(BUZZER_PIN, phrase[i].freq, phrase[i].dur_ms * 92 / 100);
            delay(phrase[i].dur_ms);
            noTone(BUZZER_PIN);
        }
    }
}

// ── Jingle complet ────────────────────────────────────────────────────────────
void jouerCompowder() {
    jouerPhrase(phrase1, NB_P1);
    jouerPhrase(phrase2, NB_P2);
}

// ── setup() ───────────────────────────────────────────────────────────────────
void setup() {
    Serial.begin(115200);
    while (!Serial && millis() < 3000);

    Serial.println("=== Compowder Sound — Totally Spies ===");
    Serial.println("Notes detectees : B6 (1976 Hz), G#6 (1661 Hz), F#6 (1480 Hz)");
    Serial.println("Structure : phrase x2, silence 1,36 s entre les deux");
    Serial.println();

    pinMode(BUZZER_PIN, OUTPUT);
    delay(1000);
}

// ── loop() ────────────────────────────────────────────────────────────────────
void loop() {
    Serial.println("WOOHP !");
    jouerCompowder();
    delay(4000);
}

// ============================================================================
//  INTÉGRATION DANS MMM2026 (exemple — appel unique à l'entrée d'un état)
// ============================================================================
//
//  #include "buzzer.h"    // ton module existant pour les bips système
//
//  // Le jingle Compowder est SÉPARÉ du module buzzer (durées ms fixes)
//  // Appel typique au démarrage ou à un événement ponctuel :
//
//  case ATTENTE:
//      if (premier_demarrage) {
//          jouerCompowder();       // bloquant ~2,5 s — acceptable au boot
//          premier_demarrage = false;
//      }
//      break;
//
//  // Si tu veux que ce soit non-bloquant, dis-le moi :
//  // je te fais un jouerCompowder_tick() sur le même principe que buzzer_tick()
//
// ============================================================================
