#include <SoftwareSerial.h>
#include <TinyGPS++.h>

SoftwareSerial gpsSerial(7, 8);  // RX, TX for Neo-6M GPS
SoftwareSerial gsmSerial(9, 10); // RX, TX for SIM800L GSM

const int buttonPin = 2; // Pin for the push button
bool buttonState = false;
unsigned long buttonPressStartTime = 0;
const unsigned long requiredButtonPressDuration = 3000; // Press duration in milliseconds (3 seconds)

TinyGPSPlus gps;

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);
  gsmSerial.begin(9600);

  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  // Check if the button is pressed
  buttonState = digitalRead(buttonPin);

  if (buttonState == LOW) {
    // Button is pressed, start timing
    buttonPressStartTime = millis();

    // Wait for the button to be released
    while (digitalRead(buttonPin) == LOW) {
      // You can add additional actions here if needed
    }

    // Check the duration
    unsigned long buttonPressDurationActual = millis() - buttonPressStartTime;

    // If the button was pressed for at least the required duration
    if (buttonPressDurationActual >= requiredButtonPressDuration) {
      // Trigger your action (send SMS and make a call)
      String gpsData = getGPSData();
      sendEmergencySMS(gpsData);
      delay(5000); // Delay to avoid sending multiple SMS quickly
      makeEmergencyCall();
    }
  }
}

String getGPSData() {
  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      // Extract latitude and longitude from GPS data
      String latitude = String(gps.location.lat(), 6);
      String longitude = String(gps.location.lng(), 6);
      return "Latitude: " + latitude + ", Longitude: " + longitude;
    }
  }

  // If GPS data is not available, use the provided coordinates
  return "Latitude: 16.814066 , Longitude: 74.296970";
}

void sendEmergencySMS(String message) {
  // Compose SMS message
  String smsMessage = "Emergency! Location: " + message;
 
  // Send SMS to the emergency number (replace "123456789" with the actual emergency number)
  gsmSerial.println("AT+CMGF=1"); // Set SMS to text mode
  delay(1000);
  gsmSerial.println("AT+CMGS=\"+917248969182\""); // replace "123456789" with the actual emergency number
  delay(1000);
  gsmSerial.print(smsMessage);
  delay(1000);
  gsmSerial.write(26); // Send Ctrl+Z to end SMS
  delay(1000);
}

void makeEmergencyCall() {
  // Make an emergency call (replace "123456789" with the actual emergency number)
  gsmSerial.println("ATD+917248969182;"); // Replace "123456789" with the actual emergency number
  delay(90000); // Wait for 90 seconds for the call
  gsmSerial.println("ATH"); // Hang up the call
}