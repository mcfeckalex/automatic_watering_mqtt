int LED_BOARD = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BOARD, OUTPUT);

}

void loop() {
  blink(1000);
}

int blink(int period) {
  digitalWrite(LED_BOARD, HIGH);
  delay(period/2);
  digitalWrite(LED_BOARD, LOW);
  delay(period/2);
}
