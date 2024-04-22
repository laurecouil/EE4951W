#include <WiFi.h>


#include "dw3000.h"

#include <Arduino.h>
//#include <Wifi.h>
#include <Firebase_ESP_Client.h>
#include <string>
using namespace std;
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "cse-tech"
#define WIFI_PASSWORD "ZestarYum21"

#define API_KEY "AIzaSyDRe1FVl_h_aRYJxB0ivJ9geN_JLyU52kw"
#define DATABASE_URL "https://object-tracking-firebase-default-rtdb.firebaseio.com/"

FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;
bool taskCompleted = false;

unsigned long sendDataPrevMillis = 0;
//int count = 0;
bool signupOK = false;
const char* deviceID = "ESP32_DEVICE_1";

#define PIN_RST 27
#define PIN_IRQ 34
#define PIN_SS 4

/*
 * TESTED ON WITH TAG 0
 * TAG       0      1      2        
 * TX:     16385  16430  16375  
 * RX:     16385  16430  16375  
 */

#define RNG_DELAY_MS 1000
#define TX_ANT_DLY 164385
#define RX_ANT_DLY 16385
#define ALL_MSG_COMMON_LEN 10
#define ALL_MSG_SN_IDX 2
#define RESP_MSG_POLL_RX_TS_IDX 10
#define RESP_MSG_RESP_TX_TS_IDX 14
#define RESP_MSG_TS_LEN 4
#define POLL_TX_TO_RESP_RX_DLY_UUS 240
#define RESP_RX_TIMEOUT_UUS  400

#define NUM_BEAC 4
#define TAG_ID 0

/* Default communication configuration. We use default non-STS DW mode. */
static dwt_config_t dwconfig = {
    5,                /* Channel number. */
    DWT_PLEN_128,     /* Preamble length. Used in TX only. */
    DWT_PAC8,         /* Preamble acquisition chunk size. Used in RX only. */
    9,                /* TX preamble code. Used in TX only. */
    9,                /* RX preamble code. Used in RX only. */
    1,                /* 0 to use standard 8 symbol SFD, 1 to use non-standard 8 symbol, 2 for non-standard 16 symbol SFD and 3 for 4z 8 symbol SDF type */
    DWT_BR_6M8,       /* Data rate. */
    DWT_PHRMODE_STD,  /* PHY header mode. */
    DWT_PHRRATE_STD,  /* PHY header rate. */
    (129 + 8 - 8),    /* SFD timeout (preamble length + 1 + SFD length - PAC size). Used in RX only. */
    DWT_STS_MODE_OFF, /* STS disabled */
    DWT_STS_LEN_64,   /* STS length see allowed values in Enum dwt_sts_lengths_e */
    DWT_PDOA_M0       /* PDOA mode off */
};

static uint8_t tx_poll_msg[3][4][12] = {
                                  {{0x41, 0x88, 0, 0xCA, 0xDE, 'I', 'D', '0', '0', 0xE0, 0, 0},
                                  {0x41, 0x88, 0, 0xCA, 0xDE, 'I', 'D', '0', '1', 0xE0, 0, 0},
                                  {0x41, 0x88, 0, 0xCA, 0xDE, 'I', 'D', '0', '2', 0xE0, 0, 0},
                                  {0x41, 0x88, 0, 0xCA, 0xDE, 'I', 'D', '0', '3', 0xE0, 0, 0}},

                                  {{0x41, 0x88, 0, 0xCA, 0xDE, 'I', 'D', '1', '0', 0xE0, 0, 0},
                                  {0x41, 0x88, 0, 0xCA, 0xDE, 'I', 'D', '1', '1', 0xE0, 0, 0},
                                  {0x41, 0x88, 0, 0xCA, 0xDE, 'I', 'D', '1', '2', 0xE0, 0, 0},
                                  {0x41, 0x88, 0, 0xCA, 0xDE, 'I', 'D', '1', '3', 0xE0, 0, 0}},

                                  {{0x41, 0x88, 0, 0xCA, 0xDE, 'I', 'D', '2', '0', 0xE0, 0, 0},
                                  {0x41, 0x88, 0, 0xCA, 0xDE, 'I', 'D', '2', '1', 0xE0, 0, 0},
                                  {0x41, 0x88, 0, 0xCA, 0xDE, 'I', 'D', '2', '2', 0xE0, 0, 0},
                                  {0x41, 0x88, 0, 0xCA, 0xDE, 'I', 'D', '2', '3', 0xE0, 0, 0}}
                                  
};
//static uint8_t tx_poll_msg[][] = {0x41, 0x88, 0, 0xCA, 0xDE, 'T', 'A', 'G', '2', 0xE0, 0, 0};
static uint8_t rx_resp_msg[3][4][20] = {
                                  {{0x41, 0x88, 0, 0xCA, 0xDE, 'I', 'D', '0', '0', 0xE1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                  {0x41, 0x88, 0, 0xCA, 0xDE, 'I', 'D', '0', '1', 0xE1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                  {0x41, 0x88, 0, 0xCA, 0xDE, 'I', 'D', '0', '2', 0xE1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                  {0x41, 0x88, 0, 0xCA, 0xDE, 'I', 'D', '0', '3', 0xE1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},

                                  {{0x41, 0x88, 0, 0xCA, 0xDE, 'I', 'D', '1', '0', 0xE1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                  {0x41, 0x88, 0, 0xCA, 0xDE, 'I', 'D', '1', '1', 0xE1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                  {0x41, 0x88, 0, 0xCA, 0xDE, 'I', 'D', '1', '2', 0xE1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                  {0x41, 0x88, 0, 0xCA, 0xDE, 'I', 'D', '1', '3', 0xE1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},

                                  {{0x41, 0x88, 0, 0xCA, 0xDE, 'I', 'D', '2', '0', 0xE1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                  {0x41, 0x88, 0, 0xCA, 0xDE, 'I', 'D', '2', '1', 0xE1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                  {0x41, 0x88, 0, 0xCA, 0xDE, 'I', 'D', '2', '2', 0xE1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                  {0x41, 0x88, 0, 0xCA, 0xDE, 'I', 'D', '2', '3', 0xE1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}
};

//static uint8_t rx_resp_msg1[] = {0x41, 0x88, 0, 0xCA, 0xDE, 'B', 'E', 'A', '2', 0xE1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static uint8_t frame_seq_nb = 0;
static uint8_t rx_buffer[20];
static uint32_t status_reg = 0;
static double tof;
static double curDistance;
extern dwt_txconfig_t txconfig_options;

float distances_now[4] = {0,0,0,0};

float beac0_buf[10] = {0};
float beac1_buf[10] = {0};
float beac2_buf[10] = {0};
float beac3_buf[10] = {0};
int index = 0;

int testCount = 0;
String generatePathToTag(int num, int count){
  return "/tag"+String(num)+"/full history/test"+String(count);
}

String generateCurrentPath(int num){
  return "/tag"+String(num)+"/current";
}

void setup()
{
  UART_init();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    //Serial.print(".");
    delay(300);
  }

   /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

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

  spiBegin(PIN_IRQ, PIN_RST);
  spiSelect(PIN_SS);

  delay(2); // Time needed for DW3000 to start up (transition from INIT_RC to IDLE_RC, or could wait for SPIRDY event)

  while (!dwt_checkidlerc()) // Need to make sure DW IC is in IDLE_RC before proceeding
  {
    UART_puts("IDLE FAILED\r\n");
    while (1)
      ;
  }

  if (dwt_initialise(DWT_DW_INIT) == DWT_ERROR)
  {
    UART_puts("INIT FAILED\r\n");
    while (1)
      ;
  }

  // Enabling LEDs here for debug so that for each TX the D1 LED will flash on DW3000 red eval-shield boards.
  dwt_setleds(DWT_LEDS_ENABLE | DWT_LEDS_INIT_BLINK);

  /* Configure DW IC. See NOTE 6 below. */
  if (dwt_configure(&dwconfig)) // if the dwt_configure returns DWT_ERROR either the PLL or RX calibration has failed the host should reset the device
  {
    UART_puts("CONFIG FAILED\r\n");
    while (1)
      ;
  }

  /* Configure the TX spectrum parameters (power, PG delay and PG count) */
  dwt_configuretxrf(&txconfig_options);

  /* Apply default antenna delay value. See NOTE 2 below. */
  dwt_setrxantennadelay(RX_ANT_DLY);
  dwt_settxantennadelay(TX_ANT_DLY);

  /* Set expected response's delay and timeout. See NOTE 1 and 5 below.
   * As this example only handles one incoming frame with always the same delay and timeout, those values can be set here once for all. */
  dwt_setrxaftertxdelay(POLL_TX_TO_RESP_RX_DLY_UUS);
  dwt_setrxtimeout(RESP_RX_TIMEOUT_UUS);

  /* Next can enable TX/RX states output on GPIOs 5 and 6 to help debug, and also TX/RX LEDs
   * Note, in real low power applications the LEDs should not be used. */
  dwt_setlnapamode(DWT_LNA_ENABLE | DWT_PA_ENABLE);

  Serial.println("Range RX");
  Serial.println("Setup over........");
  // sendDataPrevMillis = millis();
}

void loop()
{
  // if (millis() >= sendDataPrevMillis + 1000) {
  if(Firebase.ready()) {
    Serial.println("Entering WiFi Section");
    taskCompleted = true;
    String path = generatePathToTag(TAG_ID, testCount);
    String currentPath = generateCurrentPath(TAG_ID);

    Firebase.RTDB.setTimestamp(&fbdo, currentPath+"/timestamp");
    Firebase.RTDB.setTimestamp(&fbdo, path+"/timestamp");

    Firebase.RTDB.setFloat(&fbdo, currentPath+"/DistanceToBeacon0", distances_now[0]);
    Firebase.RTDB.setFloat(&fbdo, currentPath+"/DistanceToBeacon1", distances_now[1]);
    Firebase.RTDB.setFloat(&fbdo, currentPath+"/DistanceToBeacon2", distances_now[2]);
    Firebase.RTDB.setFloat(&fbdo, currentPath+"/DistanceToBeacon3", distances_now[3]);
    
    Firebase.RTDB.setFloat(&fbdo, path+"/DistanceToBeacon0", distances_now[0]);
    Firebase.RTDB.setFloat(&fbdo, path+"/DistanceToBeacon1", distances_now[1]);
    Firebase.RTDB.setFloat(&fbdo, path+"/DistanceToBeacon2", distances_now[2]);
    Firebase.RTDB.setFloat(&fbdo, path+"/DistanceToBeacon3", distances_now[3]);
    testCount++;
    //delay(750);
    //sleep(1);
    // sendDataPrevMillis = millis() + 1000;
    // Serial.println("Sent data to Firebase");
  }
  // }

  int i = 0;
  while(i < NUM_BEAC) {
    // 9 for tag 0
    // 10 for tag 1
    // 4 for tag 2
    delay(9); // EDIT FOR DIFFERENT TAGS TO REDUCE INTERFERANCE
  /* Write frame data to DW IC and prepare transmission. See NOTE 7 below. */
 // Serial.println(dwt_readtxtimestamplo32());
  tx_poll_msg[TAG_ID][i][ALL_MSG_SN_IDX] = frame_seq_nb;
  dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_TXFRS_BIT_MASK);
  dwt_writetxdata(sizeof(tx_poll_msg[TAG_ID][i]), tx_poll_msg[TAG_ID][i], 0); /* Zero offset in TX buffer. */
  dwt_writetxfctrl(sizeof(tx_poll_msg[TAG_ID][i]), 0, 1);          /* Zero offset in TX buffer, ranging. */

  /* Start transmission, indicating that a response is expected so that reception is enabled automatically after the frame is sent and the delay
   * set by dwt_setrxaftertxdelay() has elapsed. */
  dwt_starttx(DWT_START_TX_IMMEDIATE | DWT_RESPONSE_EXPECTED);
  Serial.println("Sent");
  /* We assume that the transmission is achieved correctly, poll for reception of a frame or error/timeout. See NOTE 8 below. */
  while (!((status_reg = dwt_read32bitreg(SYS_STATUS_ID)) & (SYS_STATUS_RXFCG_BIT_MASK | SYS_STATUS_ALL_RX_TO | SYS_STATUS_ALL_RX_ERR)))
  {
  };

  /* Increment frame sequence number after transmission of the poll message (modulo 256). */
  frame_seq_nb++;

  if (status_reg & SYS_STATUS_RXFCG_BIT_MASK)
  {
    uint32_t frame_len;

    /* Clear good RX frame event in the DW IC status register. */
    dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_RXFCG_BIT_MASK);

    /* A frame has been received, read it into the local buffer. */
    frame_len = dwt_read32bitreg(RX_FINFO_ID) & RXFLEN_MASK;
    if (frame_len <= sizeof(rx_buffer))
    {
      dwt_readrxdata(rx_buffer, frame_len, 0);

      /* Check that the frame is the expected response from the companion "SS TWR responder" example.
       * As the sequence number field of the frame is not relevant, it is cleared to simplify the validation of the frame. */
      rx_buffer[ALL_MSG_SN_IDX] = 0;
      if (memcmp(rx_buffer, rx_resp_msg[TAG_ID][i], ALL_MSG_COMMON_LEN) == 0)
      {
        // Serial.println(i);
        
        uint32_t poll_tx_ts, resp_rx_ts, poll_rx_ts, resp_tx_ts;
        int32_t rtd_init, rtd_resp;
        float clockOffsetRatio;

        /* Retrieve poll transmission and response reception timestamps. See NOTE 9 below. */
        poll_tx_ts = dwt_readtxtimestamplo32();
        //Serial.println(dwt_readtxtimestamplo32());
        resp_rx_ts = dwt_readrxtimestamplo32();

        /* Read carrier integrator value and calculate clock offset ratio. See NOTE 11 below. */
        clockOffsetRatio = ((float)dwt_readclockoffset()) / (uint32_t)(1 << 26);

        /* Get timestamps embedded in response message. */
        resp_msg_get_ts(&rx_buffer[RESP_MSG_POLL_RX_TS_IDX], &poll_rx_ts);
        resp_msg_get_ts(&rx_buffer[RESP_MSG_RESP_TX_TS_IDX], &resp_tx_ts);

        /* Compute time of flight and distance, using clock offset ratio to correct for differing local and remote clock rates */
        rtd_init = resp_rx_ts - poll_tx_ts;
        rtd_resp = resp_tx_ts - poll_rx_ts;

        tof = ((rtd_init - rtd_resp * (1 - clockOffsetRatio)) / 2.0) * DWT_TIME_UNITS;
        curDistance = tof * SPEED_OF_LIGHT;

        /* Display computed distance on LCD. */
        // Serial.println(i);
        // snprintf(dist_str, sizeof(dist_str), "DIST: %3.2f m", curDistance);
        // test_run_info((unsigned char *)dist_str);
        if (curDistance > 0) {
          distances_now[i] = curDistance;
          i++; 
          Serial.println("logged");
        } else {
          Serial.println(i);
          Serial.print(rx_buffer[7]);
          Serial.println(rx_buffer[8]);
        }
        
      }
    }
    
  }
  // else if (status_reg & SYS_STATUS_ALL_RX_TO) {
  //   Serial.println("Timeout");
  //   dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_RX_TO | SYS_STATUS_ALL_RX_ERR);
  // }
  else
  {
    /* Clear RX error/timeout events in the DW IC status register. */
    //Serial.println("Error/Timeout");
    dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_RX_TO | SYS_STATUS_ALL_RX_ERR);
  }

  /* Execute a delay between ranging exchanges. */
  //Sleep(RNG_DELAY_MS);
}
for(int it = 0; it < NUM_BEAC; it++) {
  Serial.print(it);
  Serial.print(", ");
  Serial.println(distances_now[it]);
}

//Transmit data to server
// sleep(1);

}