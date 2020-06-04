#include <Arduino.h>
#include <ArduinoJson.h>
#include "config/Config.h"
#include "communication/bluetooth/Bluetooth.h"
#include "communication/bluetooth/BluetoothHandler.h"
#include "communication/common/Internet.h"
#include "maintenance/Logger.h"

void registerSensor(const char *type)
{
  String apiUrlBase = Config::instance().getApiUrl();
  String registerUrl = String(apiUrlBase) + "/stations/" + Config::instance().getApiStationId() + "/sensors";
  String body = String("{ \"type\": \"") + type + "\", \"id\": \"" + type + "\" }";
  Internet::setHeader("Authorization", String("Bearer ") + Config::instance().getAccessToken());
  Http::Response response = Internet::httpPost(registerUrl, body);

  String debugText = String("Sensor add response: ") + response.code + " payload: " + response.payload;
  Logger::debug(debugText.c_str());
}

void updateAccessToken()
{
  String registrationToken = Config::instance().getRegistratonToken();
  String apiUrlBase = Config::instance().getApiUrl();
  String url = apiUrlBase + "/auth/refresh-token";
  String refreshToken = Config::instance().getRefreshToken();
  String body = "{ \"refreshToken\": \"" + refreshToken + "\" }";
  Http::Response response = Internet::httpPost(url, body);

  String debugText = String("Get token response: ") + response.code + " payload: " + response.payload;
  Logger::debug(debugText.c_str());
  if (response.code == 200)
  {
    DynamicJsonDocument doc(JSON_OBJECT_SIZE(1) + 2 * JSON_OBJECT_SIZE(2) + 280);
    deserializeJson(doc, response.payload);
    const char *accessToken = doc["data"]["accessToken"]["token"];
    Logger::debug(accessToken);
    Config::instance().setAccessToken(accessToken);
  }
}

void beginRegistration()
{
  String registrationToken = Config::instance().getRegistratonToken();
  String apiUrlBase = Config::instance().getApiUrl();
  String registerUrl = apiUrlBase + "/auth/register-station";
  String body = "{ \"stationRegistrationToken\": \"" + registrationToken + "\" }";
  Http::Response response = Internet::httpPost(registerUrl, body);

  String debugText = String("Registraton response: ") + response.code + " payload: " + response.payload;
  Logger::debug(debugText.c_str());

  if (response.code == 201)
  {
    DynamicJsonDocument doc(512);
    deserializeJson(doc, response.payload);
    int id = doc["data"]["id"];
    const char *refreshToken = doc["data"]["refreshToken"];
    Config::instance().setApiStationId(String(id));
    Config::instance().setRefreshToken(String(refreshToken));

    updateAccessToken();

    registerSensor("temperature");
    registerSensor("humidity");
    registerSensor("pm10");
    registerSensor("pm25");
  }
}

class MyBluetoothHandler : public BluetoothHandler
{
  void deviceRefreshRequest()
  {
    Serial.println("Refresh request!");
    Config::instance().save();
    Internet::stop();
    Internet::start();

    //there isn't refreshToken but there is registrationToken
    if (
        !Config::instance().getRegistratonToken().equals(""))
    {
      beginRegistration();
    }
  }
};

void setup()
{
  Serial.begin(115200);
  Config::instance().load();
  Bluetooth::start(new MyBluetoothHandler());
  Internet::setType(Internet::WIFI);
  Internet::start();
  // Internet::httpGet("...");
}

void loop()
{
  Logger::info("test");
  delay(2000);
}
