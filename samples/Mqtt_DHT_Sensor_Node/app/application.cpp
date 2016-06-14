#include <user_config.h>
#include <SmingCore/SmingCore.h>
#include <Libraries/DHT/DHT.h>

#define WORK_PIN 5 // D1 on nodeMCU


//MQTT BROKER
#ifndef MQTT_BROKER
	#define MQTT_BROKER "192.168.1.40"
	#define MQTT_BORKER_PORT 1883
#endif

// If you want, you can define WiFi settings globally in environment variables
#ifndef WIFI_SSID
  #define WIFI_SSID "PleaseEnterSSID" // Put you SSID and Password here
  #define WIFI_PWD "PleaseEnterPass"
#endif


// Forward declarations
void startMqttClient( );
void onMessageReceived( String topic, String message );


Timer procTimer;
DHT dht( WORK_PIN );
TempAndHumidity tempAndHumidity;


// MQTT client
MqttClient mqtt( MQTT_BROKER, MQTT_BORKER_PORT, onMessageReceived );

// Publish our message
void publishMessage( ) {
  if( mqtt.getConnectionState( ) != eTCS_Connected ){
    startMqttClient( ); // Auto reconnect
  }
	
	if( dht.readTempAndHumidity( tempAndHumidity ) ){
		Serial.println( "Let's publish message now!" );
		mqtt.publishWithQoS( "sensor/temperature", String( tempAndHumidity.temp ), 3, true  );
		mqtt.publishWithQoS( "sensor/humidity", String( tempAndHumidity.humid ), 3, true  );		
		
	} else {
		mqtt.publish( "sensor/lastError", String( "Failed to read from DHT: " + dht.getLastError() ) );	
		Serial.print( "Failed to read from DHT: " );
		Serial.println( dht.getLastError() );
	}		
}

// Callback for messages, arrived from MQTT server
void onMessageReceived( String topic, String message ) {
  Serial.print( topic );
  Serial.print( ":\r\n\t" ); // Prettify alignment for printing
  Serial.println( message );

}

// Run MQTT client
void startMqttClient( ) {
  if( !mqtt.setWill( "last/will", "The connection from this device is lost:(", 1, true ) ){
    debugf( "Unable to set the last will and testament. Most probably there is not enough memory on the device." );
  }
  mqtt.connect( "esp8266" );
  mqtt.subscribe( "main/status/#" );
}

// Will be called when WiFi station was connected to AP
void connectOk( ) {
  Serial.println( "I'm CONNECTED" );


  // Run MQTT client
  startMqttClient( );

  // Start publishing loop
  procTimer.initializeMs( 20 * 1000, publishMessage ).start( ); // every 20 seconds
}

// Will be called when WiFi station timeout was reached

void connectFail( ) {
  Serial.println( "I'm NOT CONNECTED. Need help :(" );

  // .. some you code for device configuration ..
}

void init( ) {
  Serial.begin( SERIAL_BAUD_RATE ); // 115200 by default
  Serial.systemDebugOutput( true ); // Debug output to serial

	//disable watchdog
	WDT.enable( false );
	//wait for sensor startup
	delay( 1000 );
	dht.begin();
	WDT.enable( true );	

	WifiStation.config( WIFI_SSID, WIFI_PWD );
  WifiStation.enable( true );
  WifiAccessPoint.enable( false );

	
	
  // Run our method when station was connected to AP (or not connected)
  WifiStation.waitConnection( connectOk, 20, connectFail ); // We recommend 20+ seconds for connection timeout at start
}
