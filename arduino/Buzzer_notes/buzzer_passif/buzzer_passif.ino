// ============================================================================
// compowder_uno_r4.ino — Jingle "Totally Spies Compowder Sound"
// Arduino UNO R4 WiFi — NON BLOQUANT (millis)
// Câblage : D8 --[100 Ω]-- (+)PS1720P02(−) -- GND
//
// Transcription issue de l'analyse audio : 3 notes (B6, G#6, F#6),
// phrase jouée 2 fois avec ~1,36 s de silence entre les deux.
//
// API :
//   compowder_start()      lance le jingle (redémarre s'il est déjà en cours)
//   compowder_tick()       à appeler à chaque tour de loop()
//   compowder_stop()       coupe immédiatement
//   compowder_isPlaying()  true tant que le jingle joue
// ============================================================================

#define BUZZER_PIN  6

// ── Fréquences (pitches.h, octave 6) ─────────────────────────────────────────
#define NOTE_FS6  1480
#define NOTE_GS6  1661
#define NOTE_B6   1976
#define REST         0

struct Note {
    uint16_t freq;     // Hz (0 = silence)
    uint16_t dur_ms;   // durée en ms
};

// Les deux phrases sont mises bout à bout dans une seule séquence
const Note compowder[] = {
    // Phrase 1
    { NOTE_B6,  116 },
    { NOTE_GS6, 499 },
    { NOTE_FS6, 104 },
    { NOTE_GS6, 128 },
    { NOTE_B6,  197 },
    { REST,    1359 },
    // Phrase 2 (reprise)
    { NOTE_B6,  128 },
    { NOTE_GS6, 476 },
    { NOTE_FS6, 116 },
    { NOTE_GS6, 116 },
    { NOTE_B6,  174 },
};
const uint8_t NB_NOTES = sizeof(compowder) / sizeof(compowder[0]);

// ── État interne ─────────────────────────────────────────────────────────────
static bool     cp_playing = false;
static uint8_t  cp_index   = 0;
static uint32_t cp_tNote   = 0;   // instant de début de la note courante

static void cp_startNote(uint8_t i) {
    const Note &n = compowder[i];
    if (n.freq == REST) {
        noTone(BUZZER_PIN);
    } else {
        // 92 % de la durée = articulation légère entre les notes
        tone(BUZZER_PIN, n.freq, (uint32_t)n.dur_ms * 92 / 100);
    }
}

void compowder_start() {
    cp_index   = 0;
    cp_playing = true;
    cp_tNote   = millis();
    cp_startNote(0);
}

void compowder_stop() {
    noTone(BUZZER_PIN);
    cp_playing = false;
}

bool compowder_isPlaying() {
    return cp_playing;
}

void compowder_tick() {
    if (!cp_playing) return;

    if (millis() - cp_tNote >= compowder[cp_index].dur_ms) {
        cp_tNote += compowder[cp_index].dur_ms;   // évite la dérive cumulative
        cp_index++;

        if (cp_index >= NB_NOTES) {
            compowder_stop();
            return;
        }
        cp_startNote(cp_index);
    }
}

// ============================================================================
//  DÉMO — le jingle joue pendant que la LED clignote (preuve du non-bloquant)
// ============================================================================
static uint32_t tDernierJingle = 0;
static uint32_t tLed           = 0;

void setup() {
    Serial.begin(115200);
    while (!Serial && millis() < 3000);

    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.println("WOOHP !");
    compowder_start();
    tDernierJingle = millis();
}

void loop() {
    compowder_tick();                         // à appeler à chaque tour

    // Autre tâche : clignotement LED toutes les 250 ms
    if (millis() - tLed >= 250) {
        tLed = millis();
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    }

    // Relance du jingle toutes les 8 s
    if (millis() - tDernierJingle >= 8000) {
        tDernierJingle = millis();
        Serial.println("WOOHP !");
        compowder_start();
    }
}

