#include <WiFi.h>
#include <WebServer.h>

// SSID & Password
const char *ssid = "KT_GiGA_B6A1";
const char *password = "a1fj29ji78";

WebServer server(80);  // Object of WebServer(HTTP port, 80 is defult)
void handle_root();

// HTML 페이지
#if 1
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>4x4 테이블</title>
<style>
    table {
        border-collapse: collapse;
        width: 50%;
    }
    td {
        border: 1px solid black;
        padding: 10px;
        text-align: center;
    }
</style>
</head>
<body>

<table>
    <tr>
        <td id="cell11"></td>
        <td id="cell12"></td>
        <td id="cell13"></td>
        <td id="cell14"></td>
    </tr>
    <tr>
        <td id="cell21"></td>
        <td id="cell22"></td>
        <td id="cell23"></td>
        <td id="cell24"></td>
    </tr>
    <tr>
        <td id="cell31"></td>
        <td id="cell32"></td>
        <td id="cell33"></td>
        <td id="cell34"></td>
    </tr>
    <tr>
        <td id="cell41"></td>
        <td id="cell42"></td>
        <td id="cell43"></td>
        <td id="cell44"></td>
    </tr>
</table>

<script>
    // 각 셀에 대해 랜덤한 색상을 지정합니다.
    function randomColor() {
        return '#' + Math.floor(Math.random()*16777215).toString(16);
    }

    // 각 셀에 랜덤한 색상을 적용합니다.
    document.getElementById('cell11').style.backgroundColor = randomColor();
    document.getElementById('cell12').style.backgroundColor = randomColor();
    document.getElementById('cell13').style.backgroundColor = randomColor();
    document.getElementById('cell14').style.backgroundColor = randomColor();
    document.getElementById('cell21').style.backgroundColor = randomColor();
    document.getElementById('cell22').style.backgroundColor = randomColor();
    document.getElementById('cell23').style.backgroundColor = randomColor();
    document.getElementById('cell24').style.backgroundColor = randomColor();
    document.getElementById('cell31').style.backgroundColor = randomColor();
    document.getElementById('cell32').style.backgroundColor = randomColor();
    document.getElementById('cell33').style.backgroundColor = randomColor();
    document.getElementById('cell34').style.backgroundColor = randomColor();
    document.getElementById('cell41').style.backgroundColor = randomColor();
    document.getElementById('cell42').style.backgroundColor = randomColor();
    document.getElementById('cell43').style.backgroundColor = randomColor();
    document.getElementById('cell44').style.backgroundColor = randomColor();
</script>

</body>
</html>
)rawliteral";
#endif


//페이지 요청이 들어 오면 처리 하는 함수
void handle_root()
{
  server.send(200, "text/html", index_html);
}


void InitWebServer() 
{
	//페이지 요청 처리 함수 등록
	server.on("/", handle_root);

    server.begin();
}

void setup() {
	Serial.begin(115200);
	Serial.println("ESP32 Simple web Start");
	Serial.println(ssid);

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

void loop() {
  server.handleClient();
}
