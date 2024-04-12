/**
 * SYNTAX:
 *
 * T value  = RealtimeDatabase::get<T>(<AsyncClient>, <path>);
 * T value  = RealtimeDatabase::get<T>(<AsyncClient>, <path>, <DatabaseOptions>);
 * 
 * T - The type of value to return.
 * <AsyncClient> - The async client.
 * <path> - The node path to get the value.
 * <DatabaseOptions> - The database options (DatabaseOptions).
 *
 * In case of data filtering and additional request options are required,
 * the DatabaseOptions can be assigned to the get function.
 *
 * Please see examples/Database/extras/FilteringData/FilteringData.ino for how to filter the data.
 *
 * The value of type T returns from sync Get functions is primitype type value or Arduino String.
 *
 * In case of error, the operation error information can be obtain from AsyncClient via aClient.lastError().message() and
 * aClient.lastError().code().
 *
 * Note that the objects used as placeholder (input) that introduced to use in Push, Update and Set i.e.
 * bool_t, number_t, string_t and object_t cannot be used to keep the output value.
 * 
 * The complete usage guidelines, please visit https://github.com/mobizt/FirebaseClient
 */

#include <Arduino.h>
#if defined(ESP32) || defined(ARDUINO_RASPBERRY_PI_PICO_W)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#elif __has_include(<WiFiNINA.h>)
#include <WiFiNINA.h>
#elif __has_include(<WiFi101.h>)
#include <WiFi101.h>
#elif __has_include(<WiFiS3.h>)
#include <WiFiS3.h>
#endif

#include <FirebaseClient.h>
#define WIFI_SSID "TastyWifi"
#define WIFI_PASSWORD "mars1234"

// The API key can be obtained from Firebase console > Project Overview > Project settings.
#define API_KEY "AIzaSyA3N0pctFrutd3FsAOuqosPrSoMkVCQlhs"

// User Email and password that already registerd or added in your project.
#define USER_EMAIL "simon5678@naver.com"
#define USER_PASSWORD "mars-1234"
#define DATABASE_URL "https://stockcontrol-1599f-default-rtdb.asia-southeast1.firebasedatabase.app/"

void printError(int code, const String &msg);

void asyncCB(AsyncResult &aResult);

DefaultNetwork network; // initilize with boolean parameter to enable/disable network reconnection

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD);

FirebaseApp app;

#if defined(ESP32) || defined(ESP8266) || defined(PICO_RP2040)
#include <WiFiClientSecure.h>
WiFiClientSecure ssl_client;
#elif defined(ARDUINO_ARCH_SAMD)
#include <WiFiSSLClient.h>
WiFiSSLClient ssl_client;
#endif

// In case the keyword AsyncClient using in this example was ambigous and used by other library, you can change
// it with other name with keyword "using" or use the class name AsyncClientClass directly.

using AsyncClient = AsyncClientClass;

AsyncClient aClient(ssl_client, getNetwork(network));

RealtimeDatabase Database;

void setup()
{

    Serial.begin(115200);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.print("Connecting to Wi-Fi");
    unsigned long ms = millis();
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();

    Firebase.printf("Firebase Client v%s\n", FIREBASE_CLIENT_VERSION);

    Serial.println("Initializing app...");

#if defined(ESP32) || defined(ESP8266) || defined(PICO_RP2040)
    ssl_client.setInsecure();
#if defined(ESP8266)
    ssl_client.setBufferSizes(4096, 1024);
#endif
#endif

    app.setCallback(asyncCB);

    initializeApp(aClient, app, getAuth(user_auth));

    // Waits for app to be authenticated.
    // For asynchronous operation, this blocking wait can be ignored by calling app.loop() in loop().
    ms = millis();
    while (app.isInitialized() && !app.ready() && millis() - ms < 120 * 1000)
        ;

    app.getApp<RealtimeDatabase>(Database);

    Database.url(DATABASE_URL);

    bool status = Database.set<String>(aClient, "/test/string", "1000100011111011");
    if (status)
        Serial.println("Set string is ok");
    else
        printError(aClient.lastError().code(), aClient.lastError().message());

    Serial.println("Synchronous Get... ");
    delay(1000);
    // Library does not provide JSON parser library, to get JSON object, get the string and deseialize it.

    Serial.print("Get string... ");
    String v3 = Database.get<String>(aClient, "/test/string");
    if (aClient.lastError().code() == 0) {
      
        Serial.println(v3);
        checkSensorString(v3);
    }
    else
        printError(aClient.lastError().code(), aClient.lastError().message());
}

void loop()
{
    // This function is required for handling async operations and maintaining the authentication tasks.
    app.loop();

    // (For Async Only) This required when different AsyncClients than used in FirebaseApp assigned to the Realtime database functions.
    Database.loop();
}

void checkSensorString(String sensorString) {
  for (int i = 0; i < sensorString.length(); ++i) {
    char currentChar = sensorString.charAt(i);
    Serial.println(currentChar);
    // 여기서 각각의 문자열을 처리할 수 있습니다.
  }
}

void asyncCB(AsyncResult &aResult)
{
    if (aResult.appEvent().code() > 0)
    {
        Firebase.printf("Event msg: %s, code: %d\n", aResult.appEvent().message().c_str(), aResult.appEvent().code());
    }

    if (aResult.isDebug())
    {
        Firebase.printf("Debug msg: %s\n", aResult.debug().c_str());
    }

    if (aResult.isError())
    {
        Firebase.printf("Error msg: %s, code: %d\n", aResult.error().message().c_str(), aResult.error().code());
    }
}

void printError(int code, const String &msg)
{
    Firebase.printf("Error, msg: %s, code: %d\n", msg.c_str(), code);
}