#include <Logger.h>
#include <DHT.h>
#include <ConnectionManager.h>

#define DHTPIN D4               // what pin we're connected to
#define DHTTYPE DHT22           // DHT 22  (AM2302)
#define DEEP_SLEEP_MINUTES 10

DHT dht(DHTPIN, DHTTYPE);
boolean isFahrenheit=false;
const char *temperatureUnit;

Logger logger("TempSensor", log_debug);
ConnectionManager connectionManager;
MQTTManager mqtt("TempSensor");
const char *room;

char temperatureTopic[64];
char humidityTopic[64];
char humidexTopic[64];

void setup() {
  Serial.begin(9600);
  delay(2000);
  logger.debug("Entering setup()");

  if(isFahrenheit) {
    temperatureUnit = "*F";
  } else {
    temperatureUnit = "*C";
  }

  connectionManager.init();
  mqtt.init(connectionManager);
  room = connectionManager.getMQTTPath();
  sprintf(temperatureTopic, "/%s/temperature", room);
  sprintf(humidityTopic, "/%s/humidity", room);
  sprintf(humidexTopic, "/%s/humidex", room);

  if (!mqtt.connect()) {
    logger.error("MQTT Connection failed.");
  }

  dht.begin();
  logger.info("DHT initialized.");
  logger.debug("setup() done.");
}

void loop() {
  logger.debug("Entering loop()");
  delay(200);

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature(isFahrenheit);
  float humidex = dht.computeHeatIndex(temperature, humidity, isFahrenheit);

  char tempChar[8]; //+100,00
  dtostrf(temperature, 3, 2, tempChar);
  char humidityChar[7]; //100,00
  dtostrf(humidity, 3, 2, humidityChar);
  char humidexChar[8]; //+100,00
  dtostrf(humidex, 3, 2, humidexChar);

  printResult(tempChar, humidityChar, humidexChar);

  if (!mqtt.isConnected()) {
    mqtt.connect();
  }

  if (mqtt.isConnected()) {
    logger.debug("Connected to MQTT-Server. Start publishing...");

    if (!isnan(temperature)) {
      if (!mqtt.publish(temperatureTopic, tempChar)) {
        logger.error("Publishing temperature failed.");
      }
    } else {
      logger.error("Temperature is not a number.");
    }

    if (!isnan(humidity)) {
      if (!mqtt.publish(humidityTopic, humidityChar)) {
        logger.error("Publishing humidity failed.");
      }
    } else {
      logger.error("Humidity is not a number.");
    }

    if (!isnan(humidex)) {
      if (!mqtt.publish(humidexTopic, humidexChar)) {
        logger.error("Publishing humidex failed.");
      }
    } else {
      logger.error("Humidex is not a number.");
    }

    logger.debug("Publishing done, disconnect.");
    mqtt.disconnect();
  } else {
    logger.error("No connection to MQTT-Server.");
  }

  logger.debug("loop() done. Deep sleep ...");
  ESP.deepSleep(DEEP_SLEEP_MINUTES * 60 * 1000000);
  delay(100);
}

void printResult(const char *temperature,
                  const char *humidity,
                  const char *humidex) {
  char tempStr[66];

  sprintf(tempStr, "Temperature: %s %s, Humidity: %s%, Humidex: %s %s",
                temperature, temperatureUnit,
                humidity,
                humidex, temperatureUnit
               );
  logger.info(tempStr);
}
