#include "src/behavior.h"
#include "src/physical/MicroPhone.h"
#include "src/physical/button.h"
#include "src/physical/bluetooth.h"
#include "src/physical/fileSystem.h"
#include "src/physical/battery.h"
#include "src/utils/utils.h"
#include "src/alerts.h"

#include <bluefruit.h>

//#define USE_BLUETOOTH

void setup()
{
  Serial.begin(115200);
  analogReference(AR_DEFAULT);
  
  // Necessary for sleep mode for some reason ??
  Bluefruit.begin(1);
  
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.

  strip.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.clear();
  strip.show();   // Turn OFF all pixels ASAP
  strip.setBrightness(50);

  // attach the button interrupt
  pinMode(BUTTON_PIN, INPUT_PULLUP_SENSE);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), button_state_interrupt, CHANGE);
  
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_POWER_PIN, OUTPUT);
  pinMode(BATTERY_CHARGE_PIN, INPUT);

  pinMode(BUTTON_RED, OUTPUT);
  pinMode(BUTTON_GREEN, OUTPUT);
  pinMode(BUTTON_BLUE, OUTPUT);
  digitalWrite(BUTTON_RED, LOW);
  digitalWrite(BUTTON_GREEN, LOW);
  digitalWrite(BUTTON_BLUE, LOW);

  // power up the system
  digitalWrite(LED_POWER_PIN, HIGH);

  // activate microphone readings
  sound::enable_microphone();
#ifdef USE_BLUETOOTH
  bluetooth::enable_bluetooth();
  bluetooth::startup_sequence();
#endif

  // start the file system
  fileSystem::setup();

  // initialize the battery level
  get_battery_level(true);
  for(uint i = 0; i < 100; ++i)
  {
    get_battery_level(false);
  }

  // read from the stored config
  read_parameters();
}

void check_loop_runtime(const uint32_t runTime)
{
  static constexpr uint8_t maxAlerts = 5;
  // check the loop duration
  static uint8_t isOnSlowLoopCount = 0;
  if(runTime > LOOP_UPDATE_PERIOD + 1)
  {
    isOnSlowLoopCount = min(isOnSlowLoopCount+1, maxAlerts);
  }
  else if(isOnSlowLoopCount > 0)
  {
    isOnSlowLoopCount--;
  }

  if(isOnSlowLoopCount >= maxAlerts)
  {
    AlertManager.raise_alert(Alerts::LONG_LOOP_UPDATE);
  }
  // lower the alert
  else if(isOnSlowLoopCount <= 1)
  {
    AlertManager.clear_alert(Alerts::LONG_LOOP_UPDATE);
  };
}

void loop() {
  uint32_t start = millis();

  // loop is not ran in shutdown mode
  handle_button_events(button_clicked_callback, button_hold_callback);

#ifdef USE_BLUETOOTH
  bluetooth::parse_messages();
#endif
  color_mode_update();

  strip.show(); // show at the end of the loop (only does it if needed)

  // wait for a delay if we are faster than the set refresh rate
  uint32_t stop = millis();
  const uint32_t loopDuration = (stop - start);
  if(loopDuration < LOOP_UPDATE_PERIOD)
  {
    delay(LOOP_UPDATE_PERIOD - loopDuration);
  }

  stop = millis();
  check_loop_runtime(stop - start);

  // display alerts if needed (for some reason, using the global variable creates a bug)
  handle_alerts(AlertManager);
}
