/**
 * # VIM CLUTCH!
 * Author: https://github.com/wooo-
 * Board: SparkFun Pro Micro https://www.sparkfun.com/products/12640
 * Board Manager: https://raw.githubusercontent.com/sparkfun/Arduino_Boards/main/IDE_Board_Manager/package_sparkfun_index.json
 */
#include <Keyboard.h>

#define BASE_KEY_CODE KEY_ESC
#define DUAL_KEY_CODE 'i'
#define DUAL_MODE_PIN 3
#define DUAL_MODE_ON LOW
#define DUAL_MODE_DELAY_MS 20
#define FOOT_SWITCH_PIN 2
#define FOOT_SWITCH_DOWN LOW
#define DEBOUNCE_DELAY_MS 40
#define POLLING_DELAY_MS 5

int lastFootSwitchState = HIGH;
unsigned long lastFootSwitchTime = 0;

void setup() {
  // Enable internal pull-up resistors on mode and foot switches
  pinMode(DUAL_MODE_PIN, INPUT_PULLUP);
  pinMode(FOOT_SWITCH_PIN, INPUT_PULLUP);

  // Start virtual keyboard
  Keyboard.begin();
}

/**
 * Periodically checks the foot switch and sends the corresponding keys to the
 * host if required. We use polling instead of interrupts to avoid conflicts
 * with the Keyboard library that requires precise timing to work properly.
 */
void loop() {
  unsigned long currentTime = millis();
  int footSwitchState = digitalRead(FOOT_SWITCH_PIN);

  if (
    footSwitchState != lastFootSwitchState &&
    currentTime >= lastFootSwitchTime + DEBOUNCE_DELAY_MS
  ) {
    lastFootSwitchState = footSwitchState;
    lastFootSwitchTime = currentTime;

    if (footSwitchState == FOOT_SWITCH_DOWN) {
      onFootDown();
    } else {
      onFootUp();
    }
  }

  // We don't need to loop faster, most keyboards have a polling rate of 125hz.
  delay(POLLING_DELAY_MS);
}

/**
 * In dual mode, two key codes are sent to the pc. When not in dual mode, only
 * one (base) key code is sent:
 *   DUAL MODE  |   ACTION   |   SENT TO HOST
 * -------------|------------|-----------------------------
 *      yes     |  foot down | press(DUAL_KEY_CODE)
 *              |            | releaseAll()
 * -------------|------------|-----------------------------
 *      yes     |  foot up   | press(BASE_KEY_CODE)
 *              |            | releaseAll()
 * -------------|------------|-----------------------------
 *      no      |  foot down | press(BASE_KEY_CODE)
 * -------------|------------|-----------------------------
 *      no      |  foot up   | releaseAll()
 */
bool isInDualMode() {
  return digitalRead(DUAL_MODE_PIN) == DUAL_MODE_ON; 
}

void onFootDown() {
  if (isInDualMode()) {
    Keyboard.press(DUAL_KEY_CODE);
    delay(DUAL_MODE_DELAY_MS);
    Keyboard.releaseAll();
  } else {
    Keyboard.press(BASE_KEY_CODE);
  }
}

void onFootUp() {
  if (isInDualMode()) {
    Keyboard.press(BASE_KEY_CODE);
    delay(DUAL_MODE_DELAY_MS);
  }
  Keyboard.releaseAll();
}
