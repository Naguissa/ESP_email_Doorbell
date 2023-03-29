#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

// ==================== TUNEABLE PARAMETERS ====================

// Uncomment these lines in order to activate Serial debug
//#define DEBUG
//#define DEBUGLIB_BAUDS 115200


// ====================== GLOBAL VARIABLES ======================
char* ApiKey = "YOUR_APIKEY"; // "ApiKey from https://www.foroelectro.net/arduino/";
char* ssid = "YOUR_WIFI_SSID";
char *password = "YOUR_WIFI_PASSWORD";


// =======================================================
// ==================== FUNCTIONALITY ====================
// =======================================================


// ==================== Function definitions ====================
void setup_wifi();
void setup();
void loop();

void emailMe();


// ==================== Debug functionality; simplified, taken from: https://github.com/Naguissa/uDebugLib ====================
#ifdef DEBUG
	#define uDebugLibInit() Serial.begin(DEBUGLIB_BAUDS)
	#define DEBUG_PRINT Serial.print
	#define DEBUG_PRINTLN Serial.println
#else
	#define uDebugLibInit()
	#define DEBUG_PRINT
	#define DEBUG_PRINTLN
#endif


// ==================== Setup functions ====================


void setup_wifi() {
	// We start by connecting to a WiFi network
	DEBUG_PRINT("Connecting to: ");
	DEBUG_PRINTLN(ssid);
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);
	delay(50);
	for (byte i = 0; i < 120 && WiFi.status() != WL_CONNECTED; i++) {
		DEBUG_PRINT(".");
		delay(500);
	}
	if (WiFi.status() == WL_CONNECTED) {
		DEBUG_PRINTLN("WiFi connected");
		DEBUG_PRINTLN("IP address: ");
		DEBUG_PRINTLN(WiFi.localIP());
	} else {
		DEBUG_PRINTLN("ERROR CONNECTING TO WIFI");
	}
	DEBUG_PRINTLN("");
}


// This section of code runs only once at start-up.
void setup() {
	uDebugLibInit();
	DEBUG_PRINTLN("Serial OK (if debug enabled)");

	setup_wifi();

	emailMe();

    // Infinite sleep
    ESP.deepSleep(0);
}



//
// ==================== LOOP ====================
//

void loop() { }




//
// ==================== AUX FUNCTIONS ====================
//

void emailMe() {
	DEBUG_PRINTLN("Sending email... ");
  String out;


	// HTTPS disabled for speed-up. No so private data....
	// WiFiClientSecure client;
	// char host[30] = "www.foroelectro.net";
	// const char* fingerprint = "23 66 E2 D6 94 B5 DD 65 92 0A 4A 9B 34 70 7B B4 35 9B B6 C9";
	// if (!client.connect(host, 443)) {

	WiFiClient client;
	IPAddress host(163, 172, 27, 140);
	if (!client.connect(host, 80)) {
		DEBUG_PRINTLN("Reporting connection failed.");
		DEBUG_PRINT(host);
		DEBUG_PRINTLN(":80");
		return;
	} else {
		DEBUG_PRINTLN("Connected!");
	}
	yield();

	// This will send the request to the server
	client.print("GET /arduino/api/");
	client.print(ApiKey);
	client.print("/mail/mailme?subject=Doorbell%20is%20ringing!&body=Doorbell%20is%20ringing%20just%20now");
	client.println(" HTTP/1.1");
	client.print("Host: ");
	client.println(host);
	client.println("Cache-Control: no-cache");
	client.println("Content-Type: application/x-www-form-urlencoded");
	client.println("Content-Length: 0");
	client.println("Connection: close\r\n");
	delay(100);

	// Read all the lines of the reply from server
	while (client.available()) {
		out = client.readStringUntil('\r');
    DEBUG_PRINTLN(out);
		yield();
	}
	DEBUG_PRINTLN("------------------");
	DEBUG_PRINTLN("API email done");
}

