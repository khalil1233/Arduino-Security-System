#include<Wire.h>
#include<LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  // put your setup code here, to run once:
  lcd.begin();

  lcd.print("It's Workin");
  lcd.setCursor(3, 1);
  lcd.print("Khalil Warwar");
}

void loop() {
  // put your main code here, to run repeatedly:

}
