/*
   ____  __  __  ____  _  _  _____       ___  _____  ____  _  _
  (  _ \(  )(  )(_  _)( \( )(  _  )___  / __)(  _  )(_  _)( \( )
   )(_) ))(__)(  _)(_  )  (  )(_)((___)( (__  )(_)(  _)(_  )  (
  (____/(______)(____)(_)\_)(_____)     \___)(_____)(____)(_)\_)
  Official code for ESP8266 boards                   version 3.3
  With Additional OLED SSD1306 Display Monitor - by Widyo Rio

  Duino-Coin Team & Community 2019-2022 © MIT Licensed
  https://duinocoin.com
  https://github.com/revoxhere/duino-coin

  Additional Display modul by: Widyo Rio
  https://www.youtube.com/SkwaDroneSalatiga
  https://3lektronika.blogspot.com/

  If you don't know where to start, visit official website and navigate to the Getting Started page. Have fun mining!
*/

/* If optimizations cause problems, change them to -O0 (the default)
  NOTE: For even better optimizations also edit your Crypto.h file.
  On linux that file can be found in the following location:
  ~/.arduino15//packages/esp8266/hardware/esp8266/3.0.2/cores/esp8266/ */
#pragma GCC optimize ("-Ofast")

/* If during compilation the line below causes a "fatal error: arduinoJson.h: No such file or directory" message to occur; it means

  that you do NOT have the ArduinoJSON library installed. To install it,
  go to the below link and follow the instructions:
  https://github.com/revoxhere/duino-coin/issues/832 */
#include <ArduinoJson.h>

/* If during compilation the line below causes a "fatal error: Crypto.h: No such file or directory" message to occur; it means that

  you do NOT have the latest version of the ESP8266/Arduino Core library.
  To install/upgrade it, go to the below link and follow the instructions of the readme file:
  https://github.com/esp8266/Arduino */
#include <bearssl/bearssl.h>
//#include <TypeConversion.h>

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Ticker.h>
#include <ESP8266WebServer.h>

#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
/*
   CHANGE THIS SCREEN RESOLUTION FIT TO YOUR OLED DISPLAY
*/
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

/*
    OLED I2C ADDRESS

    USE 0x3D IF SCREEN SIZE 128x64 PIXELS
    USE 0x3C IF SCREEN SIZE 128x32 PIXELS

*/
#define SCREEN_ADDRESS 0x3C
#define OLED_RESET     D4
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
String statusLCD = "on";


namespace {
// Change the part in brackets to your WiFi name
const char *SSID = "DaggerHasimoto";
// Change the part in brackets to your WiFi password
const char *PASSWORD = "00000000";
// Change the part in brackets to your Duino-Coin username
const char *USERNAME = "fatur";
// Change the part in brackets if you want to set a custom miner name (use Auto to autogenerate)
const char *RIG_IDENTIFIER = "RiG 2";
// Change the part in brackets to your mining key (if you have enabled it in the wallet)
const char *MINER_KEY = "";

// Screen refresh every microsecond
const int screenRefresh = 10000;

const String ducoReportJsonUrl = "https://server.duinocoin.com/v2/users/" + String(USERNAME) + "?limit=1";
float lastAverageHash = 0.0;
float lastTotalHash = 0.0;

// Change false to true if using 160 MHz clock mode to not get the first share rejected
const bool USE_HIGHER_DIFF = true;

// Change true to false if you want to disable led blinking(But the LED will work in the beginning until esp connects to the pool)
const bool LED_BLINKING = true;
// Set to true if you want to host the dashboard page (available on ESPs IP address)
const bool WEB_DASHBOARD = true;
// Set to true if you want to update hashrate in browser without reloading the page


/* Do not change the lines below. These lines are static and dynamic variables
   that will be used by the program for counters and measurements. */
const char * DEVICE = "ESP8266";
const char * POOLPICKER_URL[] = {"https://server.duinocoin.com/getPool"};
const char * MINER_BANNER = "Official ESP8266 Miner";
const char * MINER_VER = "3.3";
unsigned int share_count = 0;
unsigned int port = 0;
unsigned int difficulty = 0;
float hashrate = 0;
String AutoRigName = "";
String host = "";
String node_id = "";

const char WEBSITE[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
   <title>@@ID@@ @@DEVICE@@ ducOLED Dashboard</title>    
</head>

<body bgcolor="#F3E9F3">
<br />
<h2 align="center" >
 <table border="0" width="100%" bgcolor="#FAF254">
    <tr>
      <td> 
  		<font face="Verdana">ducOLED Miner</font>
      </td>
    <tr>
      <td><small>(@@WALLET@@)</small></td>
   </tr>
 </table>
</h2>

<p align="center">
    <script>
            setInterval(function(){
                getData();
            }, 3000);
            
            function getData() {
                var xhttp = new XMLHttpRequest();
                xhttp.onreadystatechange = function() {
                    if (this.readyState == 4 && this.status == 200) {
          			  const myArray = this.responseText.split("_");
                      document.getElementById("hashratex").innerHTML = myArray[0];
          			  document.getElementById("sharesx").innerHTML = myArray[1];
					  document.getElementById("memoryx").innerHTML = myArray[2];
                    }
                };
                xhttp.open("GET", "dataread", true);
                xhttp.send();
            }	  
    </script>
	
</p>
<div align="center"><center>

<table border="0" width="80%" bgcolor="#EDFFC1">
    <tr>
        <td><table border="0" width="100%">
            <tr>
                <td><h3><font face="Verdana">HASHRATE:</font></h3>
				<span id="hashratex">@@HASHRATE@@</span>&nbsp;kH/s
                </td>
                <td><h3><font face="Verdana">DIFFICULTY:</font></h3> @@DIFF@@
                </td>
            </tr>
            <tr>
                <td><h3><font face="Verdana">SHARES:</font></h3>
				<span id="sharesx">@@SHARES@@</span>
                </td>
                <td><h3><font face="Verdana">NODE:</font></h3> @@NODE@@
                </td>
            </tr>
        </table>
        </td>
        <td><table border="0" width="100%">
            <tr>
                <td><h3><font face="Verdana">DEVICE TYPE:</font></h3> @@DEVICE@@
                </td>
                <td><h3><font face="Verdana">DEVICE ID:</font></h3> @@ID@@
                </td>
            </tr>
            <tr>
                <td><h3><font face="Verdana">FREE MEMORY:</font></h3>
				<span id="memoryx">@@MEMORY@@</span>
                </td>
                <td><h3><font face="Verdana">MINER VERSION:</font></h3> @@VERSION@@
                </td>
            </tr>
        </table>
        </td>
    </tr>
</table>
<br />
                <div>
                    <form action="http://@@LCDONOFF@@ /></form><br />
                    <a href="https://duinocoin.com/">
                        Duco Official Page
                    </a>
                    &nbsp;
                    <a href="https://www.youtube.com/SkwaDroneSalatiga">
                        ducOLED Miner YT Channel
                    </a>                
                </div>

</center></div>
</body>

</html>
)=====";

ESP8266WebServer server(80);

void dataupdater(){ //update hashrate every 3 sec in browser without reloading page
  server.send(200, "text/plain", String(hashrate / 1000)+"_"+String(share_count)+"_"+String(ESP.getFreeHeap()));
  Serial.println("Update portion data on page");
};

void UpdateHostPort(String input) {
  // Thanks @ricaun for the code
  DynamicJsonDocument doc(256);
  deserializeJson(doc, input);
  const char* name = doc["name"];
  
  host = String((const char*)doc["ip"]);
  port = int(doc["port"]);
  node_id = String(name);

  Serial.println("Poolpicker selected the best mining node: " + node_id);
}

String httpGetString(String URL) {
  String payload = "";
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;
  
  if (http.begin(client, URL)) {
    int httpCode = http.GET();
    
    if (httpCode == HTTP_CODE_OK) payload = http.getString();
    else Serial.printf("Error fetching node from poolpicker: %s\n", http.errorToString(httpCode).c_str());

    http.end();
  }
  return payload;
}

void UpdatePool() {
  String input = "";
  int waitTime = 1;
  int poolIndex = 0;
  int poolSize = sizeof(POOLPICKER_URL) / sizeof(char*);

  while (input == "") {
    Serial.println("Fetching mining node from the poolpicker in " + String(waitTime) + "s");
    input = httpGetString(POOLPICKER_URL[poolIndex]);
    poolIndex += 1;

    // Check if pool index needs to roll over
    if( poolIndex >= poolSize ){
      poolIndex %= poolSize;
      delay(waitTime * 1000);

      // Increase wait time till a maximum of 32 seconds (addresses: Limit connection requests on failure in ESP boards #1041)
      waitTime *= 2;
      if( waitTime > 32 )
        waitTime = 32;
    }
  }

  // Setup pool with new input
  UpdateHostPort(input);
}

WiFiClient client;
String client_buffer = "";
String chipID = "";
String START_DIFF = "";

// Loop WDT... please don't feed me...
// See lwdtcb() and lwdtFeed() below
Ticker lwdTimer;
#define LWD_TIMEOUT   20000

unsigned long lwdCurrentMillis = 0;
unsigned long lwdTimeOutMillis = LWD_TIMEOUT;

#define END_TOKEN  '\n'
#define SEP_TOKEN  ','

#define LED_BUILTIN 2

#define BLINK_SHARE_FOUND    1
#define BLINK_SETUP_COMPLETE 2
#define BLINK_CLIENT_CONNECT 3
#define BLINK_RESET_DEVICE   5

void SetupWifi() {
  Serial.println("Connecting to: " + String(SSID));
  
		/* Modul Display */
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
		display.println("Koneksi ke: " + String(SSID));
		display.display();
		
		
  WiFi.mode(WIFI_STA); // Setup ESP in client mode
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  WiFi.begin(SSID, PASSWORD);

  int wait_passes = 0;
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
	
	/* Modul Display */
	display.print(".");
	display.display();
	
    if (++wait_passes >= 10) {
      WiFi.begin(SSID, PASSWORD);
      wait_passes = 0;
    }
  }

  Serial.println("\n\nSuccessfully connected to WiFi");
  Serial.println("Local IP address: " + WiFi.localIP().toString());
  Serial.println("Rig name: " + String(RIG_IDENTIFIER));
  Serial.println();
  
  /* Modul Display */
    display.clearDisplay();
	display.println("KONEKSI WIFI SUKSES!");
	display.println("\nRIG id:" + String(RIG_IDENTIFIER));
	display.println("IP:" + WiFi.localIP().toString());
	display.display();
		
  UpdatePool();
}

void SetupOTA() {
  // Prepare OTA handler
  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });

  ArduinoOTA.setHostname(RIG_IDENTIFIER); // Give port a name not just address
  ArduinoOTA.begin();
}

void blink(uint8_t count, uint8_t pin = LED_BUILTIN) {
  if (LED_BLINKING){
    uint8_t state = HIGH;

    for (int x = 0; x < (count << 1); ++x) {
      digitalWrite(pin, state ^= HIGH);
      delay(50);
    }
  }
}

void RestartESP(String msg) {
  Serial.println(msg);
  Serial.println("Restarting ESP...");
  blink(BLINK_RESET_DEVICE);
  ESP.reset();
}

// Our new WDT to help prevent freezes
// code concept taken from https://sigmdel.ca/michel/program/esp8266/arduino/watchdogs2_en.html
void ICACHE_RAM_ATTR lwdtcb(void) {
  if ((millis() - lwdCurrentMillis > LWD_TIMEOUT) || (lwdTimeOutMillis - lwdCurrentMillis != LWD_TIMEOUT))
    RestartESP("Loop WDT Failed!");
}

void lwdtFeed(void) {
  lwdCurrentMillis = millis();
  lwdTimeOutMillis = lwdCurrentMillis + LWD_TIMEOUT;
}

void VerifyWifi() {
  while (WiFi.status() != WL_CONNECTED || WiFi.localIP() == IPAddress(0, 0, 0, 0))
    WiFi.reconnect();
}

void handleSystemEvents(void) {
  VerifyWifi();
  ArduinoOTA.handle();
  yield();
}

// https://stackoverflow.com/questions/9072320/split-string-into-string-array
String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int max_index = data.length() - 1;

  for (int i = 0; i <= max_index && found <= index; i++) {
    if (data.charAt(i) == separator || i == max_index) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == max_index) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void waitForClientData(void) {
  client_buffer = "";

  while (client.connected()) {
    if (client.available()) {
      client_buffer = client.readStringUntil(END_TOKEN);
      if (client_buffer.length() == 1 && client_buffer[0] == END_TOKEN)
        client_buffer = "???\n"; // NOTE: Should never happen

      break;
    }
    handleSystemEvents();
  }
}

void ConnectToServer() {
  if (client.connected())
    return;

  Serial.println("\n\nConnecting to the Duino-Coin server...");
  while (!client.connect(host.c_str(), port));

  waitForClientData();
  Serial.println("Connected to the server. Server version: " + client_buffer );
  blink(BLINK_CLIENT_CONNECT); // Sucessfull connection with the server
}

bool max_micros_elapsed(unsigned long current, unsigned long max_elapsed) {
  static unsigned long _start = 0;

  if ((current - _start) > max_elapsed) {
    _start = current;
    return true;
  }
  return false;
}

void dashboard() {
  Serial.println("Handling HTTP client");

  String s = WEBSITE;
  s.replace("@@IP_ADDR@@", WiFi.localIP().toString());
  s.replace("@@WALLET@@", String(USERNAME));
  
  
  s.replace("@@HASHRATE@@", String(hashrate / 1000));
  s.replace("@@DIFF@@", String(difficulty / 100));
  s.replace("@@SHARES@@", String(share_count));
  s.replace("@@NODE@@", String(node_id));

  s.replace("@@DEVICE@@", String(DEVICE));
  s.replace("@@ID@@", String(RIG_IDENTIFIER));
  s.replace("@@MEMORY@@", String(ESP.getFreeHeap()));
  s.replace("@@VERSION@@", String(MINER_VER));
  
  if (statusLCD=="on") s.replace("@@LCDONOFF@@",  WiFi.localIP().toString()+"/off\"><input type=\"submit\" value=\"Turn OLED off\"");
  else s.replace("@@LCDONOFF@@",  WiFi.localIP().toString()+"/on\"><input type=\"submit\" value=\"Turn OLED on\"");

  server.send(200, "text/html", s);
}

void HidupkanLCD(){
  statusLCD = "on";
  dashboard();  
}
void MatikanLCD(){
  statusLCD = "off";
  dashboard();
}

void initDisplayOled() {

    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;); // Don't proceed, loop forever
    }

    // Show initial display buffer contents on the screen --
    // the library initializes this with an Adafruit splash screen.
    display.display();
    delay(2000); // Pause for 2 seconds

    // Clear the buffer
    display.clearDisplay();

}

boolean runEvery(unsigned long interval) {
    static unsigned long previousMillis = 0;
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        return true;
    }
    return false;
}

} // namespace

// https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TypeConversion.cpp
const char base36Chars[36] PROGMEM = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
const uint8_t base36CharValues[75] PROGMEM {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0, 0, // 0 to 9
          10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 0, 0, 0, 0, 0, 0, // Upper case
          10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35  // Lower case letters
};
 uint8_t *hexStringToUint8Array(const String &hexString, uint8_t *uint8Array, const uint32_t arrayLength) {
    assert(hexString.length() >= arrayLength * 2); 
    for (uint32_t i = 0; i < arrayLength; ++i) {
        uint8Array[i] = (pgm_read_byte(base36CharValues + hexString.charAt(i * 2) - '0') << 4) + pgm_read_byte(base36CharValues + 

hexString.charAt(i * 2 + 1) - '0');
    }
    return uint8Array;
}

void setup() {
  Serial.begin(115200);
  Serial.println("\nDuino-Coin " + String(MINER_VER));
  pinMode(LED_BUILTIN, OUTPUT);

  // Autogenerate ID if required
  chipID = String(ESP.getChipId(), HEX);
  if( strcmp(RIG_IDENTIFIER, "Auto") == 0 ){
    AutoRigName = "ESP8266-" + chipID;
    AutoRigName.toUpperCase();
    RIG_IDENTIFIER = AutoRigName.c_str();
  }
  initDisplayOled();
  SetupWifi();
  SetupOTA();

  lwdtFeed();
  lwdTimer.attach_ms(LWD_TIMEOUT, lwdtcb);
  if (USE_HIGHER_DIFF) START_DIFF = "ESP8266NH";
  else START_DIFF = "ESP8266N";

  if(WEB_DASHBOARD) {
    if (!MDNS.begin(RIG_IDENTIFIER)) {
      Serial.println("mDNS unavailable");
    }
    MDNS.addService("http", "tcp", 80);
    Serial.print("Configured mDNS for dashboard on http://" 
                  + String(RIG_IDENTIFIER)
                  + ".local (or http://"
                  + WiFi.localIP().toString()
                  + ")");
    server.on("/", dashboard);
    server.on("/on", HidupkanLCD);
    server.on("/off", MatikanLCD);  	
	
    server.on("/dataread", dataupdater);
    server.begin();
  }

  blink(BLINK_SETUP_COMPLETE);
}

void loop() {
  br_sha1_context sha1_ctx, sha1_ctx_base;
  uint8_t hashArray[20];
  String duco_numeric_result_str;
  
  // 1 minute watchdog
  lwdtFeed();

  // OTA handlers
  VerifyWifi();
  ArduinoOTA.handle();
  if(WEB_DASHBOARD) server.handleClient();

  ConnectToServer();
  Serial.println("Asking for a new job for user: " + String(USERNAME));
  // client.print("JOB," + String(USERNAME) + "," + String(START_DIFF));
    client.print("JOB," + 
                 String(USERNAME) + SEP_TOKEN +
                 String(START_DIFF) + SEP_TOKEN +
                 String(MINER_KEY) + END_TOKEN);

  waitForClientData();
  String last_block_hash = getValue(client_buffer, SEP_TOKEN, 0);
  String expected_hash_str = getValue(client_buffer, SEP_TOKEN, 1);
  difficulty = getValue(client_buffer, SEP_TOKEN, 2).toInt() * 100 + 1;

  if (USE_HIGHER_DIFF) system_update_cpu_freq(160);

  int job_len = last_block_hash.length() + expected_hash_str.length() + String(difficulty).length();

  Serial.println("Received job with size of " + String(job_len) + " bytes: " + last_block_hash + " " + expected_hash_str + " " + 

difficulty);

  uint8_t expected_hash[20];
  hexStringToUint8Array(expected_hash_str, expected_hash, 20);

  br_sha1_init(&sha1_ctx_base);
  br_sha1_update(&sha1_ctx_base, last_block_hash.c_str(), last_block_hash.length());

  float start_time = micros();
  max_micros_elapsed(start_time, 0);

  String result = "";
  if (LED_BLINKING) digitalWrite(LED_BUILTIN, LOW);
  for (unsigned int duco_numeric_result = 0; duco_numeric_result < difficulty; duco_numeric_result++) {
    // Difficulty loop
    sha1_ctx = sha1_ctx_base;
    duco_numeric_result_str = String(duco_numeric_result);

    br_sha1_update(&sha1_ctx, duco_numeric_result_str.c_str(), duco_numeric_result_str.length());
    br_sha1_out(&sha1_ctx, hashArray);

    if (memcmp(expected_hash, hashArray, 20) == 0) {
      // If result is found
      if (LED_BLINKING) digitalWrite(LED_BUILTIN, HIGH);
      unsigned long elapsed_time = micros() - start_time;
      float elapsed_time_s = elapsed_time * .000001f;
      hashrate = duco_numeric_result / elapsed_time_s;
      share_count++;
      client.print(String(duco_numeric_result)
                   + ","
                   + String(hashrate)
                   + ","
                   + String(MINER_BANNER)
                   + " "
                   + String(MINER_VER)
                   + ","
                   + String(RIG_IDENTIFIER)
                   + ",DUCOID"
                   + String(chipID)
                   + "\n");

      waitForClientData();
      Serial.println(client_buffer
                     + " share #"
                     + String(share_count)
                     + " (" + String(duco_numeric_result) + ")"
                     + " hashrate: "
                     + String(hashrate / 1000, 2)
                     + " kH/s ("
                     + String(elapsed_time_s)
                     + "s)");
      break;
    }
    if (max_micros_elapsed(micros(), 500000))
      handleSystemEvents();
  }

	/* Modul Display */
  if (statusLCD == "on")
  {
    if (runEvery(screenRefresh)) {
        float totalHashrate = 0.0;
        float avgHashrate = 0.0;
        int total_miner = 0;
        String DataInput = httpGetString(ducoReportJsonUrl);
        DynamicJsonDocument doc (8000);
        DeserializationError error = deserializeJson(doc, DataInput);
        if (error) {
            Serial.print("deserializeJson() failed: ");
            Serial.println(error.c_str());
            return;
        }
        JsonObject result = doc["result"];
        JsonObject result_balance = result["balance"];
        double result_balance_balance = result_balance["balance"];
        const char *result_balance_created = result_balance["created"];
        const char *result_balance_username = result_balance["username"];
        const char *result_balance_verified = result_balance["verified"];

        for (JsonObject result_miner : result["miners"].as<JsonArray>()) {
            float result_miner_hashrate = result_miner["hashrate"];
            totalHashrate = totalHashrate + result_miner_hashrate;
            total_miner++;
        }
        avgHashrate = totalHashrate / long(total_miner);
        long run_span = screenRefresh / 1000;

/*
        Serial.println("result_balance_username : " + String(result_balance_username));
        Serial.println("result_balance_balance : " + String(result_balance_balance));
        Serial.println("totalHashrate : " + String(totalHashrate));
        Serial.println("avgHashrate H/s : " + String(avgHashrate));
        Serial.println("total_miner : " + String(total_miner));
*/

        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.println("DUCOLED By X.RutaF\n");
        display.println("IP: " + WiFi.localIP().toString());
        display.println("User: " + String(result_balance_username));
        display.println("Total Miner: " + String(total_miner));
        display.println("Balance: " + String(result_balance_balance));
        display.println("KH/s (" + String(run_span) + "s): " + String(totalHashrate/1000));
        display.display();
    }
    display.print(".");
    display.display();   
  } else
  {
        display.clearDisplay();
        display.display();		
  } // end if statusLCD
} // end of main Loop
