const int e5 = 659, d5s = 622, b4 = 493;
const int d5 = 587, c5 = 523, a4 = 440;
const int c4 = 261, e4 = 329, g4s = 415;
bool s = false;
void setup() {
  // put your setup code here, to run once:
  pinMode(7, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  for (int i = 0; i < 2; i++) {
    x(e5);
    x(d5s);
  }
  x(e5);
  x(b4);
  x(d5);
  x(c5);
  x(a4);
  x(c4);
  x(e4);
  x(a4);
  x(b4);
  if (!s) {
    s = !s;
    x(e4);
    x(g4s);
    x(b4);
    x(c5);
  } else {
    s = !s;
    x(e4);
    x(c5);
    x(b4);
    x(a4);
  }
  x(e4);
}

void x(int note) {
  tone(7, note);
  delay(300);
}
