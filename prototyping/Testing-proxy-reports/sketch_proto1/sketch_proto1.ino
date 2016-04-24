/*
  Serial Event example

 When new serial data arrives, this sketch adds it to a String.
 When a newline is received, the loop prints the string and
 clears it.

 A good test for this is to try it with a GPS receiver
 that sends out NMEA 0183 sentences.

 Created 9 May 2011
 by Tom Igoe

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/SerialEvent

 */

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
int counter1 = 0;
int counter2 = 0;

void setup() {
  long timeout_millis = 500;
  // initialize serial:
  Serial.begin(9600);
  Serial.setTimeout(timeout_millis);
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
}

void loop() {
  // print the string when a newline arrives:
}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */


void serialEvent(){

  if(Serial.available()){

    inputString = Serial.readStringUntil('\n');

    String toSend = "USF 0 USR " + String(counter1) + " IRFR " + String(inputString.length()) + " IRRR 0 IRRC 0,";
    counter1++;

    Serial.println(encodeNetstring(toSend));
    
    int result = decodeNetstring(inputString);

    while(abs(result)){
      int removeChars = abs(result);
      
      if(removeChars >= inputString.length()){
        inputString = "";
        result = 0;
      } else {
        inputString = inputString.substring(removeChars,inputString.length());
      }

      if(inputString.length()){
        result = decodeNetstring(inputString);
      } else {
        result = 0;
      }
    }
  }
}

String encodeNetstring(String toEncode){
  if (toEncode.length() < 1){
    return "";
  }
  return String(toEncode.length()) + ":" + toEncode + ",";
}

int decodeNetstring(String rawData){
    
    // DO NOT MODIFY NEXT LINE:
    // CREDIT TO PETER SCOTT @ https://github.com/PeterScott/netstring-c/

    int NETSTRING_ERROR_TOO_LONG    = -9;      //More than 999999999 bytes in a field            // Discard 9 chars
    int NETSTRING_ERROR_LEADING_ZERO= -1;      //Leading zeros are not allowed                   // DISCARD 1 char
    int NETSTRING_ERROR_NO_LENGTH   = -1;      //Length not given at start of netstring          // DISCARD 1 char

    //if the Netstring is less than 3 characters, it's either an invalid one or contains an empty string
    if (rawData.length() < 2) return ((rawData.length() - 1) * -1);
    
    // NO LEADING 0
    if (rawData[0] == '0' && isdigit(rawData[1])) return NETSTRING_ERROR_LEADING_ZERO;

    /* The netstring must start with a number */
    if (!isdigit(rawData[0])) return NETSTRING_ERROR_NO_LENGTH;

    // How many chars represent digits, compute expected payload length
    unsigned int expectedLength = 0;
    unsigned int i = 0;
    
    for (; i < rawData.length() && isdigit(rawData[i]); i++) {
        /* Error if more than 9 digits */
        if (i >= 9) return NETSTRING_ERROR_TOO_LONG;
        /* Accumulate each digit, assuming ASCII. */
        expectedLength = expectedLength*10 + (rawData[i] - '0');
    }

    // If we only got digits and EOF char
    if(i + 1 == rawData.length()) return 0;
    
    /* Read the colon */
    if (rawData[i] != ':') return (i * -1);                 

    /* Check buffer length once and for all. To check if the buffer is longer than the digit chars and the colon and the comma. */
    if ((unsigned int)(i++ + expectedLength + 1) >= rawData.length()) return 0; // WAIT FOR MORE

    /* Test for the trailing comma, and set the return values */
    if (rawData[i + expectedLength] != ',') return -(i+expectedLength); // DISCARD ALL TILL EXPECTED COMA POS INCLUSIVE
//    Serial.println(rawData);
//    Serial.println(i);
//    Serial.println(expectedLength);
//    Serial.println(rawData.substring(i,expectedLength));
    // TODO invoke stringlistener with payload
    invokeStringListener(rawData.substring(i,expectedLength+i));
    
    return (expectedLength + i + 1);
}

void invokeStringListener(String s){
  int number = s.length();

//  String netstring = String(number);
//  netstring += ":";
//  netstring += s;
//  netstring += ",";

  String toSend = "USF 2 USR " + String(counter2) + " IRFR 0 IRRR 0 IRRC 0,";
  counter2++;
  
  Serial.println(encodeNetstring(toSend));
  Serial.println(encodeNetstring(toSend));
  Serial.println(encodeNetstring(toSend));
  Serial.println(encodeNetstring(toSend));
  Serial.println(encodeNetstring(toSend));
  Serial.println(encodeNetstring(toSend));
  Serial.println(encodeNetstring(toSend));
  Serial.println(encodeNetstring(toSend));
  Serial.println(encodeNetstring(toSend));
  Serial.println(encodeNetstring(toSend));
  Serial.println(encodeNetstring(toSend));
  Serial.println(encodeNetstring(toSend));
  Serial.println(encodeNetstring(toSend));
  Serial.println(encodeNetstring(toSend));
  
//  Serial.println(netstring);
}

