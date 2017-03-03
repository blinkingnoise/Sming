#include <SmingCore/SmingCore.h>
#include <Libraries/AppleMidi/AppleMidi.h>

/**
 * ported arduino library from https://github.com/lathoub/Arduino-AppleMidi-Library
 */


// If you want, you can define WiFi settings globally in environment variables
#ifndef WIFI_SSID
  #define WIFI_SSID "PleaseEnterSSID" // Put you SSID and Password here
  #define WIFI_PWD "PleaseEnterPass"
#endif


bool isConnected = false;

// forward declarations
void showInfo();
void onConnected();
void onConnectionFail();

UdpConnection udp

APPLEMIDI_CREATE_INSTANCE(UdpConnection, AppleMIDI);

void init() {
  showInfo();

  Serial.begin( SERIAL_BAUD_RATE ); // 115200 by default
  Serial.systemDebugOutput( true ); // Debug output to serial

  WifiStation.config( WIFI_SSID, WIFI_PWD );
  WifiStation.enable( true );
  WifiAccessPoint.enable( false );


  // Run our method when station was connected to AP (or not connected)
  WifiStation.waitConnection( onConnected, 20, onConnectionFail ); // We recommend 20+ seconds for connection timeout at start

  /*
  Serial.print("Getting IP address...");


  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");


  Serial.println();
  Serial.print("IP address is ");
  Serial.println(WiFi.localIP());

  Serial.println("OK, now make sure you an rtpMIDI session that is Enabled");
  Serial.print("Add device named Arduino with Host/Port ");
  Serial.print(WiFi.localIP());
  Serial.println(":5004");
  Serial.println("Then press the Connect button");
  Serial.println("Then open a MIDI listener (eg MIDI-OX) and monitor incoming notes");

  // Create a session and wait for a remote host to connect to us
  AppleMIDI.begin("test");

  AppleMIDI.OnConnected(OnAppleMidiConnected);
  AppleMIDI.OnDisconnected(OnAppleMidiDisconnected);

  AppleMIDI.OnReceiveNoteOn(OnAppleMidiNoteOn);
  AppleMIDI.OnReceiveNoteOff(OnAppleMidiNoteOff);

  Serial.println("Sending NoteOn/Off of note 45, every second");
*/
}


void onConnected(){
  Serial.println("connected ip: " + WifiStation.getIP().toString() );

  // Create a session and wait for a remote host to connect to us
 // AppleMIDI.begin("test");
}


void onConnectionFail(){
  Serial.println("Connection failed!");
}

// ====================================================================================
// Event handlers for incoming MIDI messages
// ====================================================================================

// -----------------------------------------------------------------------------
// rtpMIDI session. Device connected
// -----------------------------------------------------------------------------
void OnAppleMidiConnected(uint32_t ssrc, char* name) {
  isConnected  = true;
  Serial.print("Connected to session ");
  Serial.println(name);
}

// -----------------------------------------------------------------------------
// rtpMIDI session. Device disconnected
// -----------------------------------------------------------------------------
void OnAppleMidiDisconnected(uint32_t ssrc) {
  isConnected  = false;
  Serial.println("Disconnected");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OnAppleMidiNoteOn(byte channel, byte note, byte velocity) {
  Serial.print("Incoming NoteOn from channel:");
  Serial.print(channel);
  Serial.print(" note:");
  Serial.print(note);
  Serial.print(" velocity:");
  Serial.print(velocity);
  Serial.println();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OnAppleMidiNoteOff(byte channel, byte note, byte velocity) {
  Serial.print("Incoming NoteOff from channel:");
  Serial.print(channel);
  Serial.print(" note:");
  Serial.print(note);
  Serial.print(" velocity:");
  Serial.print(velocity);
  Serial.println();
}

/**
 * some basic information of the esp
 */
void showInfo( ){
  Serial.printf( "\r\nSDK: v%s\r\n", system_get_sdk_version( ) );
  Serial.printf( "Free Heap: %d\r\n", system_get_free_heap_size( ) );
  Serial.printf( "CPU Frequency: %d MHz\r\n", system_get_cpu_freq( ) );
  Serial.printf( "System Chip ID: 0x%x\r\n", system_get_chip_id( ) );
  Serial.printf( "SPI Flash ID: 0x%x\r\n", spi_flash_get_id( ) );
  Serial.printf( "ADC: %f", system_get_vdd33( ) );
}


