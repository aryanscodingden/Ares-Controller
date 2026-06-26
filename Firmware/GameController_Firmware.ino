#include <BleGamepad.h>
// Initialize BLE Gamepad as Xbox Controller
BleGamepad bleGamepad("Xbox Wireless Controller", "Microsoft", 100);
// ESP32-C6 Analog Pin Mapping (Joysticks Only)
#define LEFT_STICK_X 39
#define LEFT_STICK_Y 18
#define RIGHT_STICK_X 26
#define RIGHT_STICK_Y 25
// Digital Buttons Configuration (Pin, BLE ID)
struct ButtonMapping {
uint8_t pin;
uint16_t gamepadButton;
};
// Your precise working layout
ButtonMapping actionButtons[] = {
{34, BUTTON_1},  // A
{32, BUTTON_2},  // B
{33, BUTTON_3},  // X
{1, BUTTON_4},  // Y
{21, BUTTON_5},  // LB
{27, BUTTON_6},  // RB
{15,  BUTTON_9},  // Home Button
{12,  BUTTON_10}, // Start Button
};
const int numButtons = sizeof(actionButtons) / sizeof(ButtonMapping);
// Trigger Buttons
#define LEFT_TRIGGER_BTN 2
#define RIGHT_TRIGGER_BTN 14
// D-Pad Pin Assignments
#define DPAD_U 17
#define DPAD_D 4
#define DPAD_L 5
#define DPAD_R 19
void setup() {
  Serial.begin(115200);

pinMode(LEFT_STICK_X, INPUT);
pinMode(LEFT_STICK_Y, INPUT);
pinMode(RIGHT_STICK_X, INPUT);
pinMode(RIGHT_STICK_Y, INPUT);

  for (int i = 0; i < numButtons; i++) {
    pinMode(actionButtons[i].pin, INPUT_PULLUP);
  }

pinMode(LEFT_TRIGGER_BTN, INPUT_PULLUP);
pinMode(RIGHT_TRIGGER_BTN, INPUT_PULLUP);

pinMode(DPAD_U, INPUT_PULLUP);
pinMode(DPAD_D, INPUT_PULLUP);
pinMode(DPAD_L, INPUT_PULLUP);
pinMode(DPAD_R, INPUT_PULLUP);

// Configure BLE Gamepad reports
BleGamepadConfiguration bleGamepadConfig;
bleGamepadConfig.setAutoReport(false);
bleGamepadConfig.setWhichAxes(true, true, true, true, true, true, false, false);
bleGamepad.begin(&bleGamepadConfig);
Serial.println("ESP32-C6 Joystick Precision Patch Loaded!");
}
void loop() {
if (bleGamepad.isConnected()) {
// 1. Process Action & Xbox Guide buttons
for (int i = 0; i < numButtons; i++) {
if (digitalRead(actionButtons[i].pin) == LOW) {
bleGamepad.press(actionButtons[i].gamepadButton);
} else {
bleGamepad.release(actionButtons[i].gamepadButton);
}
}
// 2. Process Digital Triggers
if (digitalRead(LEFT_TRIGGER_BTN) == LOW) {
bleGamepad.setLeftTrigger(65535);
} else {
bleGamepad.setLeftTrigger(0);
}
if (digitalRead(RIGHT_TRIGGER_BTN) == LOW) {
bleGamepad.setRightTrigger(65535);
} else {
bleGamepad.setRightTrigger(0);
}
// 3. Process D-Pad Vectors
bool up = (digitalRead(DPAD_U) == LOW);
bool down = (digitalRead(DPAD_D) == LOW);
bool left = (digitalRead(DPAD_L) == LOW);
bool right = (digitalRead(DPAD_R) == LOW);
if (up && right) bleGamepad.setHat1(DPAD_UP_RIGHT);
else if (down && right) bleGamepad.setHat1(DPAD_DOWN_RIGHT);
else if (down && left) bleGamepad.setHat1(DPAD_DOWN_LEFT);
else if (up && left) bleGamepad.setHat1(DPAD_UP_LEFT);
else if (up) bleGamepad.setHat1(DPAD_UP);
else if (down) bleGamepad.setHat1(DPAD_DOWN);
else if (left) bleGamepad.setHat1(DPAD_LEFT);
else if (right) bleGamepad.setHat1(DPAD_RIGHT);
else bleGamepad.setHat1(HAT_CENTERED);
// 4. Process Joysticks
// OPTIMIZED: Re-scaled to standard signed integer limits to eliminate the floating center issue.
// Note: If an axis moves opposite to your hand, swap the last two numbers (e.g., -32767, 32767 to 32767, -32767)
int lsX = map(analogRead(LEFT_STICK_Y), 0, 4095, -32767, 32767);
int lsY = map(analogRead(LEFT_STICK_X), 0, 4095, -32767, 32767);
int rsX = map(analogRead(RIGHT_STICK_X), 0, 4095, -32767, 32767);
int rsY = map(analogRead(RIGHT_STICK_Y), 0, 4095, -32767, 32767);
bleGamepad.setLeftThumb(lsX, lsY);
bleGamepad.setRightThumb(rsX, rsY);
// Send unified controller state data
bleGamepad.sendReport();
delay(8);
}
}