


void setup() {
  Serial.begin(9600);    // serial / USB port
}

byte in_byte = 0;        // stores received byte

void loop() {
  // check if byte available from USB port
  if (Serial.available()) {
    in_byte = Serial.read();
    Serial.println(in_byte);
    //Serial.write(rx_byte);
  }
}
