void setup() {
  // put your setup code here, to run once:
  pinMode(7, OUTPUT);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  int i = 200; // The starting pitch
  while(i < 800) {
    i++;
    tone(7, i); // Emit the noise
    delay(5);
  }
  delay(100); // A short break in between each whoop
}
