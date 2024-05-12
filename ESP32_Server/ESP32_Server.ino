#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FirebaseClient.h>
#include <WiFiClientSecure.h>


const unsigned long EVENT_INTERVAL_MS = 5000; //페이지 업데이트 주기

const char *ssid = "Suzie"; // 와이파이 SSID & Password
const char *password = "79994448";


const char * API_KEY = "AIzaSyA3N0pctFrutd3FsAOuqosPrSoMkVCQlhs";
const char * USER_EMAIL = "k.suzie.97@gmail.com";
const char * USER_PASSWORD = "111111";
#define DATABASE_URL "https://stockcontrol-1599f-default-rtdb.asia-southeast1.firebasedatabase.app/"


//UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD);
UserAuth* user_auth = nullptr;
//
void checkSensorString(String sensorString);
void processResult(AsyncResult &aResult);
void InitFirebase();

DefaultNetwork network; 

FirebaseApp app;

WiFiClientSecure ssl_client;

AsyncResult aResult_no_callback;

AsyncClientClass aClient(ssl_client, getNetwork(network));

RealtimeDatabase Database;

AsyncWebServer server(80);
AsyncEventSource events("/events");

string dir; 
const char *sendData;

String sensorData;
String expData;

uint32_t prev_ms;

string table_html1 PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Current Stock</title>
<style>
    table {
        width: 50%;
        border-collapse: collapse;
        margin: 20px auto;
        font-family: 'Arial', sans-serif;
    }
    th, td {
        padding: 40px;
        text-align: center;
        border: 4px solid #ccc;
    }
    .center {
    text-align: center;
    margin-top: 20px;
    }
    .center p {
    display: inline;
    margin: 0 10px;
    }
</style>
</head>
<body>

<table id="binaryTable">
  <tr>
    <td></td><td></td><td></td><td></td>
  </tr>
  <tr>
    <td></td><td></td><td></td><td></td>
  </tr>
  <tr>
    <td></td><td></td><td></td><td></td>
  </tr>
  <tr>
    <td></td><td></td><td></td><td></td>
  </tr>
</table>
<div id="columnCounts" class="center"></div>
<h3 id="exp"></h3>
<form id="dateForm">
    <input type="date" id="dateInput" name="date">
    <input type="submit" value="유통기한 설정">
</form>

<script>
function updateTable(binaryString) {
    const cells = document.querySelectorAll('#binaryTable td');
    if (binaryString.length !== 16) {
      return;
    }

    cells.forEach((cell, index) => {
      cell.style.backgroundColor = binaryString[index] === '1' ? 'red' : 'white';
    });
    const columnCounts = [0, 0, 0, 0];

    for (let i = 0; i < binaryString.length; i++) {
      const columnIndex = i % 4;
      if (binaryString[i] === '1') {
        columnCounts[columnIndex]++;
      }
    }
    const columnCountsElement = document.getElementById('columnCounts');
    columnCountsElement.innerHTML = '';
    columnCounts.forEach((count, index) => {
      const paragraph = document.createElement('p');
      paragraph.textContent = `C${index + 1}: ${count}`;
      columnCountsElement.appendChild(paragraph);
    });
}

if (!!window.EventSource) {
        var source = new EventSource('/events');
        source.addEventListener('open', function(e) {
         console.log("Events Connected");
        }, false);

        source.addEventListener('update', function(e) {
            updateTable(e.data);
          }, false);
          
        source.addEventListener('exp', function(e) {
            document.getElementById("exp").innerHTML = '유통기한 : ' + e.data;
           }, false);
}
document.getElementById("dateForm").addEventListener("submit", function(event) {
        event.preventDefault(); // Prevent the form from submitting normally

        var date = document.getElementById("dateInput").value;

        var formData = new FormData();
        formData.append("date", date);
        
        var url = window.location.protocol + "//" + window.location.host + "/date";
        fetch(url, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/x-www-form-urlencoded',
            },
            body: new URLSearchParams(formData).toString()
        })
        .then(response => response.text())
        .then(data => {
            // Handle the response from the server
            console.log(data);
            // Reload the page with the received HTML content
            document.open();
            document.write(data);
            document.close();
        })
        .catch(error => {
            console.error('Error:', error);
        });
        
    });
    </script>

</body>
</html>
)rawliteral";


const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Login Form</title>
<style>
    body {
        font-family: Arial, sans-serif;
    }
    .container {
        max-width: 400px;
        margin: 0 auto;
        padding: 20px;
        border: 1px solid #ccc;
        border-radius: 5px;
        background-color: #f9f9f9;
    }
    input[type="text"],
    input[type="password"],
    input[type="submit"] {
        width: 100%;
        padding: 10px;
        margin: 5px 0;
        display: inline-block;
        border: 1px solid #ccc;
        border-radius: 4px;
        box-sizing: border-box;
    }
    input[type="submit"] {
        background-color: #4CAF50;
        color: white;
        cursor: pointer;
    }
    input[type="submit"]:hover {
        background-color: #45a049;
    }
</style>
</head>
<body>
<div class="container">
    <h2>Login</h2>
    <form id="loginForm">
        <label for="email">Email:</label>
        <input type="text" id="email" name="email" required>
        <label for="password">Password:</label>
        <input type="password" id="password" name="password" required>
        <input type="submit" value="Login">
    </form>
</div>

<script>
    document.getElementById("loginForm").addEventListener("submit", function(event) {
        event.preventDefault(); // Prevent the form from submitting normally

        // Get the values from the form
        var email = document.getElementById("email").value;
        var password = document.getElementById("password").value;

        var formData = new FormData();
        formData.append("email", email);
        formData.append("password", password);

        var url = window.location.protocol + "//" + window.location.host + "/login"

        fetch(url, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/x-www-form-urlencoded',
            },
            body: new URLSearchParams(formData).toString()
        })
        .then(response => response.text())
        .then(data => {
            // Handle the response from the server
            console.log(data);
            // Reload the page with the received HTML content
            document.open();
            document.write(data);
            document.close();
        })
        .catch(error => {
            console.error('Error:', error);
        });
        
    });
</script>

</body>
</html>
)rawliteral";

void handle_root(AsyncWebServerRequest *request) {
  request->send_P(200, "text/html", index_html);
}

void handle_login(AsyncWebServerRequest *request)
{
  string html;
  Serial.println("Login request arrived");
  if(request->hasParam("email", true)) {
    AsyncWebParameter* p = request->getParam("email", true);
    USER_EMAIL = p->value().c_str();
    p = request->getParam("password", true);
    USER_PASSWORD = p->value().c_str();

    InitFirebase();
  }
  request->send_P(200, "text/html", (table_html1).c_str() );
}

void handle_date(AsyncWebServerRequest *request)
{
  if(request->hasParam("date", true)) {
    AsyncWebParameter* p = request->getParam("date", true);
    Serial.println(p->value().c_str());
    
    Database.set<String>(aClient, (dir+"/2").c_str(),p->value().c_str(),aResult_no_callback);
  }
}

void InitWebServer() 
{
  server.addHandler(&events);
  server.on("/", handle_root);
  server.on("/login", handle_login);
  server.on("/date", handle_date);
  server.begin();
}



void InitFirebase() {
  Serial.println(USER_EMAIL);
  user_auth = new UserAuth(API_KEY, USER_EMAIL, USER_PASSWORD);
  ssl_client.setInsecure();
  initializeApp(aClient, app, getAuth(*user_auth),  aResult_no_callback);

  app.getApp<RealtimeDatabase>(Database);
  Database.url(DATABASE_URL);
}

void setup() {
	Serial.begin(115200);
	Serial.println("ESP32 Simple web Start");
	Serial.println(ssid);
  
  prev_ms = millis();
  /*
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  }*/
	//WiFi 접속
	WiFi.begin(ssid, password);

	//접속 완료 하면
	while (WiFi.status() != WL_CONNECTED) {
	delay(1000);
	Serial.print(".");
	}
    
	Serial.print("Wifi IP: ");
	Serial.println(WiFi.localIP());  //Show ESP32 IP on serial

	InitWebServer(); 
	Serial.println("HTTP server started");
	delay(100); 
}

void ProcessUpdate()
{
  if (millis() > prev_ms + EVENT_INTERVAL_MS&&app.ready())
  {
    
    dir = ("/"+app.getUid()).c_str();
    Database.get(aClient, (dir+"/1").c_str(), aResult_no_callback);
    Database.get(aClient, (dir+"/2").c_str(), aResult_no_callback);            
    prev_ms = millis();
  }
  
}

void loop() {
  ProcessUpdate();
  app.loop();
  Database.loop();
  processResult(aResult_no_callback);
}


void processResult(AsyncResult &aResult)
{
    if (aResult.isEvent())
    {
        Firebase.printf("Event task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.appEvent().message().c_str(), aResult.appEvent().code());
    }

    if (aResult.isDebug())
    {
        Firebase.printf("Debug task: %s, msg: %s\n", aResult.uid().c_str(), aResult.debug().c_str());
    }

    if (aResult.isError())
    {
        Firebase.printf("Error task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.error().message().c_str(), aResult.error().code());
    }

    if (aResult.available())
    {
      String data = String(aResult.c_str());
      data = data.substring(1,data.length()-1);
      if(data.length()>15) {
        sensorData = data;
        events.send(sensorData.c_str(),"update", millis());
      }
      else {
        expData = data;
        events.send(expData.c_str(),"exp", millis());
      }
      Serial.println(data);
      //Firebase.printf("payload: %s\n", aResult.c_str());
    }
}