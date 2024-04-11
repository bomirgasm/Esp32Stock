/**
 * SYNTAX:
 *
 * Firestore::Databases::create(<AsyncClient>, <Firestore::Parent>, <Database>);
 * Firestore::Databases::create(<AsyncClient>, <Firestore::Parent>, <Database>, <AsyncResult>);
 * Firestore::Databases::create(<AsyncClient>, <Firestore::Parent>, <Database>, <AsyncResultCallback>, <uid>);
 * 
 * <AsyncClient> - The async client.
 * <Firestore::Parent> - The Firestore::Parent object included project Id and database Id in its constructor.
 * <Database> - The Firestore::Database object that hold the database information to create.
 * <AsyncResult> - The async result (AsyncResult).
 * <AsyncResultCallback> - The async result callback (AsyncResultCallback).
 * <uid> - The user specified UID of async result (optional).
 * 
 * The Firebase project Id should be only the name without the firebaseio.com.
 * The Firestore database id is the id of database to create.
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
/**
 * This information can be taken from the service account JSON file.
 *
 * To download service account file, from the Firebase console, goto project settings,
 * select "Service accounts" tab and click at "Generate new private key" button
 */
#define FIREBASE_PROJECT_ID "stockcontrol-1599f"
#define FIREBASE_CLIENT_EMAIL "firebase-adminsdk-b4wq1@stockcontrol-1599f.iam.gserviceaccount.com"
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----\nMIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQCf1b8P3MQiKM0r\n3LrVoIWB9B0U9tr0hTH7ybtUFx4o5wWDZLyhmWP12IF72L1n12ByrbgVGpfgvVZD\nx6KncEi9NLQARZWBnKFY64oRL2iyM8ephLZMabpOGWuOPwlX2zoKkwfI1YyUuVuo\n7FFHNhnGkflYd+4LgMH9ZzR6lEMK7/svL4KDaT1uWzIguYbYH+rSV0Wn/1uSgtAI\nSfCiCcI6W44Kk02mFtTYFcQQwCltvXYKHyMBqEsDtykZv7+urF4Q5owJtuvkhRHs\nbtGMHcv41KgVGvj32JKythY+Pri7QM2KDdCId6ugHMHd/vxMxjJ/hgK57uxB8DCR\nBB7opU7jAgMBAAECggEAGFl3wfLf7lRI7WWzENhjfmiyBX+V26pu8uzInQbokVKg\nhGPUWMoFcG6LS1K5iBhHmTDd2lkyygoOGkVPgjKY4jolMeVn4Vb7PZCkd/nA+WTq\nBekx3HI+76bvu7vesIO1U+cSSIY/5GhYjmNx/mc/hgC2+8NJgHcdFn2igQoVLJUl\nC1APOyEIN9+g6+KSJNnd0vSr7JuuDg5AezfqAMVe2b9eJANlnUOwDsox0lO7rWE0\nyX/Xr5lo+xLa4rNrzxzE+rH5VTz5UHubYhig7xq15WJn1S4RL9iHp5h/Ubb2LMVb\n8xQcHE0CVA/svuaUJnsXeJ2vpcxoSnT2h3trg1jwAQKBgQDNWO0L0neBRIPMAw7d\nerK3DucaZyaVq5hZVpusJvGidYGuDvhp9kVhJtAU5ADG0D7udsAUnW/sYoodMpVn\nFW7HyPiIYPudhiM8zI0EBPzflnAd5DWUcuH93kt/bsYiNCxH3Mo9L2vdO1/vRbDk\nJZUhZSiVXp1cybL8FLE0JWoPKwKBgQDHQtjB5Dg2mgzqvmXJ/sJ3iYcsCn95WD3I\ngKUbpIvTe98BiYOrDfBKMWtphTh2R2uaLEc7H63kqyD/CuaxJiYY9hxFHX43DJ1s\n+nIiqbOqvarD17gZ5/8BIlx2jHAuOhEEOs0IQqiPL8G5mXNZdZHwmQB/UZLXFqsg\n4m2BxMIjKQKBgQChalGLhXmjB3SCnbTFrHlnJDPNsV/HLwem0km0XTh799pkNChZ\nQtjusU/H3CIwJeoSEQ6UJd2g9ySSJv0B9vsJ2JRkv7eJ77MrS69dhX4xU1P2azki\n49t5b4/xL+ZXZKOaKZhf2DL1yIJ011dhP5mmkpoAMEuKcJJdTPauguqH2QKBgQCm\nfcUtJKqPfOJEFfho8dO9G411EAR4+HEPIeZTFYa7mym3WBZLC6d15psQHP8TXROc\nS1jtHsgmY5J7Z7lf1TdJ7gGrAjyedIzpLM3HBrKXfEi9wHYUN1NLuFE3wJpPFbd4\nKEV+nGvI0Cd3E69c1S6usLjIqPkffNJZ9Ld5EVUBGQKBgDa3OGFqi8kMqhtuubOX\nqoRIcwupVdQVuFwVRH6DUEM7V0Z5no5cW/z+T9CKVJ5Y1uGCik8yInv8Oi1n6jqu\nbYN/c2J3eFigUE/Q6fcRO2V16eM8Lt9xWH6gGShTPObHums4uZHEMP0Aqi7mP9Ky\nTytpJ5Zhw3Z9tSW0z4kZoBAx\n-----END PRIVATE KEY-----\n";


void timeStatusCB(uint32_t &ts);

void asyncCB(AsyncResult &aResult);

DefaultNetwork network; // initilize with boolean parameter to enable/disable network reconnection

// ServiceAuth is required for Databases functions.
ServiceAuth sa_auth(timeStatusCB, FIREBASE_CLIENT_EMAIL, FIREBASE_PROJECT_ID, PRIVATE_KEY, 3000 /* expire period in seconds (<= 3600) */);

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

Firestore::Databases Databases;

AsyncResult aResult_no_callback;

bool taskCompleted = false;

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

    initializeApp(aClient, app, getAuth(sa_auth));

    // Waits for app to be authenticated.
    // For asynchronous operation, this blocking wait can be ignored by calling app.loop() in loop().
    ms = millis();
    while (app.isInitialized() && !app.ready() && millis() - ms < 120 * 1000)
    {
        // This JWT token process required for ServiceAuth and CustomAuth authentications
        JWT.loop(app.getAuth());
    }

    app.getApp<Firestore::Databases>(Databases);
}

void loop()
{
    // This JWT token process required for ServiceAuth and CustomAuth authentications
    JWT.loop(app.getAuth());

    // This function is required for handling and maintaining the authentication tasks.
    app.loop();

    // To get the authentication time to live in seconds before expired.
    // app.ttl();

    // This required when different AsyncClients than used in FirebaseApp assigned to the Firestore functions.
    Databases.loop();

    // To get anyc result without callback
    // printResult(aResult_no_callback);

    if (app.ready() && !taskCompleted)
    {
        taskCompleted = true;

        Serial.println("Create a database... ");

        Firestore::Database db;
        db.concurrencyMode(Firestore::ConcurrencyMode::PESSIMISTIC);
        db.databaseType(Firestore::DatabaseType::FIRESTORE_NATIVE);
        db.name("myDb");
        // See https://cloud.google.com/firestore/docs/locations
        db.locationId("us-east1");

        // You can set the content of db object directly with db.setContent("your content")

        Databases.create(aClient, Firestore::Parent(FIREBASE_PROJECT_ID, "myDb" /* database Id */), db, asyncCB);
        // To assign UID for async result
        // Databases.create(aClient, Firestore::Parent(FIREBASE_PROJECT_ID, "myDb"), db, asyncCB, "myUID");

        // To get anyc result without callback
        // Databases.create(aClient, Firestore::Parent(FIREBASE_PROJECT_ID, "myDb"), db, aResult_no_callback);
    }
}

void timeStatusCB(uint32_t &ts)
{
#if defined(ESP8266) || defined(ESP32) || defined(CORE_ARDUINO_PICO)
    if (time(nullptr) < FIREBASE_DEFAULT_TS)
    {

        configTime(3 * 3600, 0, "pool.ntp.org");
        while (time(nullptr) < FIREBASE_DEFAULT_TS)
        {
            delay(100);
        }
    }
    ts = time(nullptr);
#elif __has_include(<WiFiNINA.h>) || __has_include(<WiFi101.h>)
    ts = WiFi.getTime();
#endif
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

    if (aResult.available())
    {
        // To get the UID (string) from async result
        // aResult.uid();
        Firebase.printf("payload: %s\n", aResult.c_str());
    }
}
