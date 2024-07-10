#include <SoftwareSerial.h>
#include <Adafruit_Soundboard.h>

// SoftwareSerial RX & TX
#define SFX_TX 8
#define SFX_RX 9

// RST pin on the Sound Board
#define SFX_RST 7

// ACT pin on the Sound Board
// If you call stop while nothing's playing, it will break the board
#define SFX_ACT 4
#define ACT_PLAYING 0
#define ACT_NOT_PLAYING 1

// Hook Switch, phone is off hook when this is 0
#define HOOK_SW 5
#define OFF_HOOK 0
#define ON_HOOK 1

SoftwareSerial ss = SoftwareSerial(SFX_TX, SFX_RX);
Adafruit_Soundboard sfx = Adafruit_Soundboard(&ss, NULL, SFX_RST);

// Stores the number of files, and incidentally, the length of *playlist
uint8_t num_files;

// Stores the order to play the song's in, by index
uint8_t *playlist;

// Stores the track to be played
uint8_t track_i;

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);
  Serial.println("Rustys Calling!");
  
  // softwareserial at 9600 baud
  ss.begin(9600);

  if (!sfx.reset()) {
    Serial.println("ERROR: SFX Board not found");
    while (1);
  }

  num_files = sfx.listFiles();
  playlist = (uint8_t *) malloc(sizeof(uint8_t) * num_files);
  track_i = 0;

  // Initialize playlist
  for (int i = 0; i < num_files; i++) playlist[i] = i;
  shuffle_playlist();
  
  pinMode(HOOK_SW, INPUT_PULLUP);
  pinMode(SFX_ACT, INPUT_PULLUP);
}

void shuffle_playlist() {
  // Fisher-yates and Durstenfeld with the good shuffle
  // for i from n−1 down to 1 do
  for (int i = num_files - 1; i > 0; i--) {
    // j gets random integer such that 0 <= j <= i
    int j = random(i+1); 
    // exchange [j] and [i]
    uint8_t tmp = playlist[i];
    playlist[i] = playlist[j];
    playlist[j] = tmp;
  }
}

void loop() {
  // Don't need a fast loop
  delay(100);

  // Always be churning the RNG
  random(num_files);

  switch (digitalRead(HOOK_SW)) {
    default:
    case ON_HOOK:
      if (digitalRead(SFX_ACT) == ACT_PLAYING) sfx.stop();
      break;

    case OFF_HOOK:
      if (digitalRead(SFX_ACT) == ACT_NOT_PLAYING) sfx.playTrack(track_i++);
      break;
  }

  // Reshuffle the playlist once we get to the end
  if (track_i >= num_files) {
    track_i = 0;
    shuffle_playlist();
  }
}
