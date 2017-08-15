#include <SmingCore/SmingCore.h>
#include <Libraries/OSC/OSCMessage.h>
#include <Libraries/OSC/OSCBundle.h>

#ifndef WIFI_SSID
  #define WIFI_SSID "PleaseEnterSSID"
  #define WIFI_PWD "PleaseEnterPass"
#endif

#ifndef COM_SPEED_SERIAL
  #define COM_SPEED_SERIAL 115200
#endif

#define LED_PIN 2 // GPIO2, build in led


// forward declarations
void showSystemInfo();

void led(OSCMessage &msg);

void
onReceive(UdpConnection &connection, char *data, int size, IPAddress remoteIP, uint16_t remotePort);

void onStationGotIP(IPAddress ip, IPAddress gateway, IPAddress netmask);

OSCErrorCode error;
uint8_t ledState = HIGH;


// UDP server
const uint16_t localPort = 8888;
UdpConnection udp(onReceive);


void init() {
  System.setCpuFrequency(eCF_80MHz); //CpuFrequency: eCF_80MHz (default) or eCF_160MHz
  system_set_os_print(0); // Zero to disable, 1 to enable. Disable SDK output to serial
  Serial.begin(COM_SPEED_SERIAL);
  Serial.systemDebugOutput(true); // Enables/disables framework debugging.

  WifiStation.config(WIFI_SSID, WIFI_PWD);

  WifiAccessPoint.enable(false); // Enables/disables WiFi Access point mode
  WifiStation.enable(true); // Enables/disables WiFi client mode

  showSystemInfo();

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, static_cast<uint8_t>(ledState));

  WifiEvents.onStationGotIP(onStationGotIP);

}

void showSystemInfo() {
  Serial.println("\r\n\n=============================");
  Serial.printf("sming version: %s\r\n", SMING_VERSION);
  Serial.printf("sdk: v%s\r\n", system_get_sdk_version());
  Serial.printf("free heap: %d\r\n", system_get_free_heap_size());
  Serial.printf("cpu frequency: %d MHz\r\n", system_get_cpu_freq());
  Serial.printf("system chip id: 0x%x\r\n", system_get_chip_id());
  Serial.printf("spi flash id: 0x%x\r\n", spi_flash_get_id());
  Serial.printf("boot mode: %d\r\n", system_get_boot_mode());
  Serial.printf("adc: %d\r\n", system_adc_read());
  Serial.printf("wifi ssid: %s\r\n", WifiStation.getSSID().c_str());
  Serial.println("=============================\r\n\n");
}

void onStationGotIP(IPAddress ip, IPAddress gateway, IPAddress netmask) {
  udp.listen(localPort);

  Serial.println("\r\n=== UDP SERVER STARTED ===");
  Serial.print(WifiStation.getIP().toString());
  Serial.print(":");
  Serial.println(localPort);
  Serial.println("=============================\r\n");
}

void onReceive(UdpConnection &connection, char *data, int size, IPAddress remoteIP,
               uint16_t remotePort) {
  OSCBundle bundle;
  char address[256];
  char message[1024];


  Serial.printf("UDP Sever callback from %s:%d, %d bytes\r\n", remoteIP.toString().c_str(),
                remotePort, size);

  bundle.fill(reinterpret_cast<uint8_t *>(data), size);

  Serial.printf("Got %d messages:\r\n", bundle.size());

  if (bundle.hasError()) {
    Serial.printf("bundle with error %d\r\n", bundle.getError());
  }


  for (int i = 0; i < bundle.size(); i++) {
    bundle.getOSCMessage(i)->getAddress(address, 0);
    Serial.print(address);
    Serial.printf(" [%c]:", bundle.getOSCMessage(i)->getType(0));

    switch (bundle.getOSCMessage(i)->getType(0)) {
      case 'i':
        Serial.printf(" %d\r\n", bundle.getOSCMessage(i)->getInt(0));
        break;
      case 'f':
        Serial.printf(" %f\r\n", bundle.getOSCMessage(i)->getFloat(0));
        break;
      case 's':
        bundle.getOSCMessage(i)->getString(0, message, 1024);
        Serial.printf(" %s\r\n", message);
        break;
      default:
        Serial.print(" unknown\r\n");
    }

  }
  Serial.println("\n");


  if (!bundle.hasError()) {
    bundle.dispatch("/led", led);
  } else {
    error = bundle.getError();
    Serial.print("error: ");
    Serial.println(error);
  }
}

void led(OSCMessage &msg) {
  if (msg.isInt(0)) {
    Serial.printf("got /led: %d", msg.getInt(0));
    ledState = static_cast<uint8_t>(msg.getInt(0));
  }

  if (msg.isFloat(0)) {
    Serial.printf("got /led: %f", msg.getFloat(0));
    ledState = static_cast<uint8_t>(msg.getFloat(0));
  }

  digitalWrite(LED_PIN, ledState);
}



