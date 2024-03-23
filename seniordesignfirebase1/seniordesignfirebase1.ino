
// #include <Arduino.h>
// #if defined(ESP32)
//   #include <WiFi.h>
// #elif defined(ESP8266)
//   #include <ESP8266WiFi.h>
// #endif
// #include <Firebase_ESP_Client.h>

// //Provide the token generation process info.
// #include "addons/TokenHelper.h"
// //Provide the RTDB payload printing info and other helper functions.
// #include "addons/RTDBHelper.h"

// // Insert your network credentials
// #define WIFI_SSID "cse-tech"
// #define WIFI_PASSWORD "ZestarYum21"

// // Insert Firebase project API Key
// #define API_KEY "AIzaSyBC_nyH5JY1v7wx3XguMSXkZ8TJ2n1OVs4"

// // Insert RTDB URLefine the RTDB URL */
// #define DATABASE_URL "https://esp32-firebase-demo-664b6-default-rtdb.firebaseio.com/" 

// //Define Firebase Data object
// FirebaseData fbdo;

// FirebaseAuth auth;
// FirebaseConfig config;

// unsigned long sendDataPrevMillis = 0;
// int count = 0;
// bool signupOK = false;

// void setup(){
//   Serial.begin(115200);
//   WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
//   Serial.print("Connecting to Wi-Fi");
//   while (WiFi.status() != WL_CONNECTED){
//     Serial.print(".");
//     delay(300);
//   }
//   Serial.println();
//   Serial.print("Connected with IP: ");
//   Serial.println(WiFi.localIP());
//   Serial.println();

//   /* Assign the api key (required) */
//   config.api_key = API_KEY;

//   /* Assign the RTDB URL (required) */
//   config.database_url = DATABASE_URL;

//   /* Sign up */
//   if (Firebase.signUp(&config, &auth, "", "")){
//     Serial.println("ok");
//     signupOK = true;
//   }
//   else{
//     Serial.printf("%s\n", config.signer.signupError.message.c_str());
//   }

//   /* Assign the callback function for the long running token generation task */
//   config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
//   Firebase.begin(&config, &auth);
//   Firebase.reconnectWiFi(true);
// }

// void loop(){
//   if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)){
//     sendDataPrevMillis = millis();
//     // Write an Int number on the database path test/int
//     if (Firebase.RTDB.setInt(&fbdo, "test/word", count)){
//       Serial.println("PASSED");
//       Serial.println("PATH: " + fbdo.dataPath());
//       Serial.println("TYPE: " + fbdo.dataType());
//     }
//     else {
//       Serial.println("FAILED");
//       Serial.println("REASON: " + fbdo.errorReason());
//     }
//     count++;
    
//     // Write an Float number on the database path test/float
//     if (Firebase.RTDB.setFloat(&fbdo, "fart/mitra", 0.01 + random(0,100))){
//       Serial.println("PASSED");
//       Serial.println("PATH: " + fbdo.dataPath());
//       Serial.println("TYPE: " + fbdo.dataType());
//     }
//     else {
//       Serial.println("FAILED");
//       Serial.println("REASON: " + fbdo.errorReason());
//     }
//   }


// }



#include <Arduino.h>

#include <WiFi.h>
#include <Firebase.h>

#include <Firebase_ESP_Client.h>

// Provide the token generation process info.
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "cse-tech"
#define WIFI_PASSWORD "ZestarYum21"

// Insert Firebase project API Key
#define API_KEY "AIzaSyBC_nyH5JY1v7wx3XguMSXkZ8TJ2n1OVs4"

// Insert RTDB URL
#define DATABASE_URL "https://esp32-firebase-demo-664b6-default-rtdb.firebaseio.com/"

// Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

FirebaseJson json;



unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;
const char* deviceID = "ESP32_DEVICE_1"; // Change this to your device ID

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  } else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}




void loop() {










  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 10000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();
    
    // Generate a random distance location that ends with 0.17
    float distance_location = random(0, 100) + 0.17;

    // Get current timestamp

   // time_t time = getCurrentTime()
    //documentRef.set({ updateTime:Firestore.Timestamp.now() });
    
    


    // Construct the path with device ID and timestamp
    String path1 = String(deviceID) + "/" + "distance_locations";
    
    String data = "{\"deviceId\":\"" + String(deviceID) + "\",\"reading\":" + String(distance_location) + ",\"timestamp\":{\".sv\":\"timestamp\"}}";



    // Write the distance location to the database
    if (Firebase.RTDB.setFloat(&fbdo, path1.c_str(), distance_location )) {
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    } else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

 if (json.setJsonData(data)) {
    Serial.println("Data sent to Firebase successfully");
  } else {
    Serial.println("Failed to send data to Firebase");

  }


    if (Firebase.RTDB.pushJSON(&fbdo, path1.c_str(),  )) {
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    } else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }


  }
}

