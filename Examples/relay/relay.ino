#define relay 5

void setup() {
  // put your setup code here, to run once:
  pinMode(relay, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(relay, LOW);
  delay(1000);
  digitalWrite(relay, HIGH);
  delay(1000);
}
