
#include <Arduino.h>

#include <WiFi.h>

#include <Firebase_ESP_Client.h>
#include <string>
using namespace std;
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
bool taskCompleted = false;


unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;
const char* deviceID = "ESP32_DEVICE_1"; // Change this to your device ID

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  //Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    //Serial.print(".");
    delay(300);
  }
  // Serial.println();
  // Serial.print("Connected with IP: ");
  // Serial.println(WiFi.localIP());
  // Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")) {
    //Serial.println("ok");
    signupOK = true;
  } else {
    //Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

double get_distance(void){
  return random(0,100) +0.17;
}

String generatePathToTag(int num, int count){
  return "/tag"+String(num)+"/full history/test"+String(count);
}

String generateCurrentPath(int num){
  return "/tag"+String(num)+"/current";
}

int count1 = 0;
int count2 = 0;
int count3 = 0;

void loop()
{
  // Firebase.ready() should be called repeatedly to handle authentication tasks.

  if (Firebase.ready())
  {
    taskCompleted = true;
    int random_test = random(1,4);
    if (random_test == 1){
      
      String path = generatePathToTag(random_test,count1);
      String currentPath = generateCurrentPath(1);

      Firebase.RTDB.setTimestamp(&fbdo, currentPath+"/timestamp");
      Firebase.RTDB.setTimestamp(&fbdo, path+"/timestamp");
      float distance1 = random(0,100)+0.17;
      float distance2 = random(0,100)+0.17;
      float distance3 = random(0,100)+0.17;
      Firebase.RTDB.setFloat(&fbdo, currentPath+"/DistanceToBeacon1", distance1);
      Firebase.RTDB.setFloat(&fbdo, currentPath+"/DistanceToBeacon2", distance2);
      Firebase.RTDB.setFloat(&fbdo, currentPath+"/DistanceToBeacon3", distance3);

 
      Firebase.RTDB.setFloat(&fbdo, path+"/DistanceToBeacon1", distance1);
      Firebase.RTDB.setFloat(&fbdo, path+"/DistanceToBeacon2", distance2);
      Firebase.RTDB.setFloat(&fbdo, path+"/DistanceToBeacon3", distance3);
      count1 ++;

    }
    if (random_test == 2){
      String path = generatePathToTag(random_test,count2);
      String currentPath = generateCurrentPath(2);

      Firebase.RTDB.setTimestamp(&fbdo, currentPath+"/timestamp");
      Firebase.RTDB.setTimestamp(&fbdo, path+"/timestamp");
      float distance1 = random(0,100)+0.17;
      float distance2 = random(0,100)+0.17;
      float distance3 = random(0,100)+0.17;
      Firebase.RTDB.setFloat(&fbdo, currentPath+"/DistanceToBeacon1", distance1);
      Firebase.RTDB.setFloat(&fbdo, currentPath+"/DistanceToBeacon2", distance2);
      Firebase.RTDB.setFloat(&fbdo, currentPath+"/DistanceToBeacon3", distance3);


      Firebase.RTDB.setFloat(&fbdo, path+"/DistanceToBeacon1", distance1);
      Firebase.RTDB.setFloat(&fbdo, path+"/DistanceToBeacon2", distance2);
      Firebase.RTDB.setFloat(&fbdo, path+"/DistanceToBeacon3", distance3);

      count2 ++;
    }

    if (random_test == 3){
      String path = generatePathToTag(random_test,count3);
      String currentPath = generateCurrentPath(2);
      Firebase.RTDB.setTimestamp(&fbdo, currentPath+"/timestamp");
      Firebase.RTDB.setTimestamp(&fbdo, path+"/timestamp");
      float distance1 = random(0,100)+0.17;
      float distance2 = random(0,100)+0.17;
      float distance3 = random(0,100)+0.17;
      Firebase.RTDB.setFloat(&fbdo, currentPath+"/DistanceToBeacon1", distance1);
      Firebase.RTDB.setFloat(&fbdo, currentPath+"/DistanceToBeacon2", distance2);
      Firebase.RTDB.setFloat(&fbdo, currentPath+"/DistanceToBeacon3", distance3);


      Firebase.RTDB.setFloat(&fbdo, path+"/DistanceToBeacon1", distance1);
      Firebase.RTDB.setFloat(&fbdo, path+"/DistanceToBeacon2", distance2);
      Firebase.RTDB.setFloat(&fbdo, path+"/DistanceToBeacon3", distance3);
      count3 ++;
    }

    sleep(20);
/***

    Serial.printf("Set timestamp... %s\n", Firebase.RTDB.setTimestamp(&fbdo, "/test/timestamp") ? "ok" : fbdo.errorReason().c_str());

    if (fbdo.httpCode() == FIREBASE_ERROR_HTTP_CODE_OK)
    {

      // In setTimestampAsync, the following timestamp will be 0 because the response payload was ignored for all async functions.

      // Timestamp saved in millisecond, get its seconds from int value
      Serial.print("TIMESTAMP (Seconds): ");
      Serial.println(fbdo.to<int>());

      // Or print the total milliseconds from double value
      // Due to bugs in Serial.print in Arduino library, use printf to print double instead.
      printf("TIMESTAMP (milliSeconds): %lld\n", fbdo.to<uint64_t>());
    }

    Serial.printf("Get timestamp... %s\n", Firebase.RTDB.getDouble(&fbdo, "/test/timestamp") ? "ok" : fbdo.errorReason().c_str());
    if (fbdo.httpCode() == FIREBASE_ERROR_HTTP_CODE_OK)
      printf("TIMESTAMP: %lld\n", fbdo.to<uint64_t>());

    // To set and push data with timestamp, requires the JSON data with .sv placeholder
    FirebaseJson json;

    json.set("Data", "Hello");
    // now we will set the timestamp value at Ts
    json.set("Ts/.sv", "timestamp"); // .sv is the required place holder for sever value which currently supports only string "timestamp" as a value

    // Set data with timestamp
    Serial.printf("Set data with timestamp... %s\n", Firebase.RTDB.setJSON(&fbdo, "/test/set/data", &json) ? fbdo.to<FirebaseJson>().raw() : fbdo.errorReason().c_str());

    // Push data with timestamp
    Serial.printf("Push data with timestamp... %s\n", Firebase.RTDB.pushJSON(&fbdo, "/test/push/data", &json) ? "ok" : fbdo.errorReason().c_str());

    // Get previous pushed data
    Serial.printf("Get previous pushed data... %s\n", Firebase.RTDB.getJSON(&fbdo, "/test/push/data/" + fbdo.pushName()) ? fbdo.to<FirebaseJson>().raw() : fbdo.errorReason().c_str());
    sleep(10);
    ***/ 
  }
}