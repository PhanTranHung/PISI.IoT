int btnPress = 6;

void setup() {
  // put your setup code here, to run once:
  pinMode(btnPress, INPUT);
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (digitalRead(btnPress)){
    Serial.println("Clicked");
  } else Serial.println("Nothing");
  delay(500);
}
