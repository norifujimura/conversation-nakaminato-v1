
#include <Arduino.h>
#include <M5Core2.h>
//#include <M5Unified.h>

//for firebase
#include <WiFi.h>
#include <FirebaseClient.h>
#include <ArduinoJson.h>

//for hue
#include <HTTPClient.h> // WiFiClientSecure から HTTPClient 変更
//#include <ArduinoJson.h>


#define WIFI_SSID "nakaminato-mmm"
#define WIFI_PASSWORD "bradbury"

/*
#define WIFI_SSID "akiko_network"
#define WIFI_PASSWORD "akobagus"
*/

/*
#define WIFI_SSID "Opportunity"
#define WIFI_PASSWORD "akobagus"
*/

// User Email and password that already registerd or added in your project.
#define DATABASE_SECRET "--"

#define DATABASE_URL "--"

//hue
#define HUE_APP_KEY "--"

//hue2
//#define HUE_APP_KEY "cO3YXNREKmZdskBmlL2DdbXVhpT98fA-jzEr7aaZ"

int layoutSystem = 0;
int layoutTime = 40;
int layoutDB = 120;
int layoutLights= 205;

String hubIpAddress = "";

//Timer settings
//rtc and timer
int month = 0;
int hour = 0;
int minute = 0;
int second = 0;

int startHour = 6;
int startMin = 0;
int endHour = 23;
int endMin = 0;

bool isOnAcrossMidnight = false;
//

void asyncCB(AsyncResult &aResult);

void printResult(AsyncResult &aResult);

DefaultNetwork network; // initilize with boolean parameter to enable/disable network reconnection

LegacyToken legacy_token(DATABASE_SECRET);

FirebaseApp app;

#include <WiFiClientSecure.h>
WiFiClientSecure ssl_client1, ssl_client2;

using AsyncClient = AsyncClientClass;

AsyncClient aClient(ssl_client1, getNetwork(network)), aClient2(ssl_client2, getNetwork(network));

RealtimeDatabase Database;

JsonDocument json;

unsigned long ms = 0;
unsigned long request_interval = 200; //request shall be less than 10 requests per second.


//hue

HTTPClient httpClient;

//hue
String url = "https://192.168.179.2/clip/v2/resource/light/";

//hue2
//String url = "https://192.168.86.25/clip/v2/resource/light/";
//

JsonDocument doc;

int breathPerMin = 5;
float oneBreathMSec = 1000 * 60 / float(breathPerMin);
float breathDegreesPerMSec = 360.0 / oneBreathMSec;
int breathDegrees = 0;

float oldTime = 0;

int breathCount = 0;

int lightStartNum = 20;
int lightNum = 4;

/*
struct light{
  int r;
  int g;
  int b;
};
*/

 struct colorXY{
  float x;
  float y;
  float z;
  float brightness;
 };

 struct colorRGB{
  int r;
  int g;
  int b;
 };

 struct colors{
  colorXY xy;
  colorRGB rgb;
 };

struct colorRGB lights[2];
int colorLength = 0;
int breath=0;

 String rids[24] = {
    "c8225ef3-0c42-45e4-b1ef-ad94e93a792d",
    "2aa8355c-38a2-4473-9ce4-38a2a9b8d372",
    "6104ddbe-b9a8-40b2-8cf1-9977248cc03a",
    "109ea496-2984-4c51-aeb5-9488d396db38",
    "c9dda515-c447-4e61-ba49-fdf714f15888",
    "eb7c23ad-6733-46dc-8352-626ebb4b1d2d",
    "fdb2e894-163d-4ec1-88f0-b421209ff57a",
    "60d84962-d2ae-42db-91fb-f7c85c849f08",
    "f39609c9-f230-4e41-9136-ff8b7f884a6f",
    "83b55a69-e774-4339-a43c-00a2d65c64b2",
    "3cfb470a-f81a-400a-a619-944840f26f15",
    "6b485b01-902f-4fe3-8e75-ad2871607644",
    "8a38d4e7-ff55-4ca2-ac0d-bc0dc1ac3d50",
    "d4ea859b-9aa5-4b80-a825-2d67e5e59b3f",
    "0e2a281c-fc7b-4326-9b35-fe87d6e6f937",
    "c73b67cb-05ed-4330-b883-5e0a9e60d5e2",
    "164f30c6-0e73-43d8-be40-917de4df3824", 
    "7609efd1-601b-46c1-8e5d-87256828dcf5", 
    "4b0217e3-acde-42dd-a7b3-60595b99cddc",
    "57d5c1f8-b086-4cdf-8bfd-a9f1acc2a271",
    "13a41395-2ea7-46e3-8b86-f6eddbcd9952",
    "5068599d-c347-416d-ab90-48d352706c9d",
    "4ddf0b51-41be-445f-8bb1-38b0b67d3f83",
    "20f1c49d-2a54-42e9-b39c-829f6ec3f8b3"
   };



  String state = "setup";//init,on,off
  String timerState = "init";//init,on,off

//for sound
extern const unsigned char previewR[120264];

void setup()
{
    M5.begin(); 
    M5.Rtc.begin();
    M5.Lcd.fillScreen(BLACK); 
    M5.Lcd.setTextColor(WHITE); 
    M5.Lcd.setTextSize(2);  
    M5.Lcd.setCursor(10,10);  
    M5.Lcd.print("Start");

    Serial.begin(115200);

    //timer check isOnAcrossMidnight
    if(startHour < endHour){
      isOnAcrossMidnight = false;
    }else if(endHour < startHour){
      isOnAcrossMidnight = true;
    }else if(startHour == endHour){
      if(startMin <= endMin){
        isOnAcrossMidnight = false; 
      }else{
        isOnAcrossMidnight = true;
      }
    }

    wifiSetup();
    firebaseSetup();

    state="init";
    //M5.Lcd.fillScreen(BLACK); 
    M5.Lcd.fillRect(0,0, 320, 240, BLACK); 
    M5.Lcd.setTextColor(WHITE); 
    M5.Lcd.setTextSize(2);  

    //system 
    M5.Lcd.setTextSize(2); 
    M5.Lcd.setCursor(10,layoutSystem + 0);
    M5.Lcd.printf("Network: %s",WIFI_SSID);  
    M5.Lcd.setCursor(10,layoutSystem + 20);
    M5.Lcd.print("HubIP:"+hubIpAddress); 

  /*
  //for hue2
  rids[20]="353a91a5-5bba-43de-b6dd-2faa6ab14e75";
  rids[21]="b78e7f69-b576-4d98-b8be-37dc35c6b346";
  rids[22]="192d860c-e906-4280-8358-fdde62e0dd7b";
  rids[23]="0eaff6c1-ea2c-414c-b534-efea6e145670";
  */
}

void wifiSetup(){
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.print("Connecting to Wi-Fi");
    M5.Lcd.setCursor(10,30); 
    M5.Lcd.print("Connecting to Wi-Fi: ");
    M5.Lcd.setCursor(10,50); 
    M5.Lcd.print(WIFI_SSID);

    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }
    sound();
    Serial.println();
    Serial.print("Connected with IP: ");
    M5.Lcd.setCursor(10,70); 
    M5.Lcd.print("Connected with IP: ");
    M5.Lcd.setCursor(10,90); 
    M5.Lcd.print(WiFi.localIP());

    Serial.println(WiFi.localIP());
    Serial.println();
}

/*
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

	switch(type) {
		case WStype_DISCONNECTED:
			Serial.printf("[WSc] Disconnected!\n");
      wsState = "WStype_DISCONNECTED";
			break;
		case WStype_CONNECTED:
			Serial.printf("[WSc] Connected to url: %s\n", payload);
			//webSocket.sendTXT("Connected");
      wsState = "WStype_CONNECTED";
			break;
		case WStype_TEXT:
        wsState = "WStype_TEXT";
			break;
		case WStype_BIN:
      wsState = "WStype_BIN";
		case WStype_ERROR:	
      wsState = "WStype_ERROR";		
		case WStype_FRAGMENT_TEXT_START:
      wsState = "WStype_FRAGMENT_TEXT_START";	
		case WStype_FRAGMENT_BIN_START:
      wsState = "WStype_FRAGMENT_BIN_START";	
		case WStype_FRAGMENT:
      wsState = "WStype_FRAGMENT";
		case WStype_FRAGMENT_FIN:
      wsState = "WStype_FRAGMENT_FIN";
			break;
	}
}
*/

void firebaseSetup(){
    Firebase.printf("Firebase Client v%s\n", FIREBASE_CLIENT_VERSION);

    Serial.println("Initializing app...");
    M5.Lcd.setCursor(10,110); 
    M5.Lcd.println("Initializing app...");

    ssl_client1.setInsecure();
    ssl_client2.setInsecure();

    //initializeApp(aClient2, app, getAuth(user_auth), asyncCB, "authTask");
    initializeApp(aClient2, app, getAuth(legacy_token));

    // Binding the FirebaseApp for authentication handler.
    // To unbind, use Database.resetApp();
    app.getApp<RealtimeDatabase>(Database);

    Database.url(DATABASE_URL);

    // Since v1.2.1, in SSE mode (HTTP Streaming) task, you can filter the Stream events by using RealtimeDatabase::setSSEFilters(<keywords>),
    // which the <keywords> is the comma separated events.
    // The event keywords supported are:
    // get - To allow the http get response (first put event since stream connected).
    // put - To allow the put event.
    // patch - To allow the patch event.
    // keep-alive - To allow the keep-alive event.
    // cancel - To allow the cancel event.
    // auth_revoked - To allow the auth_revoked event.
    // To clear all prevousely set filter to allow all Stream events, use RealtimeDatabase::setSSEFilters().
    //Database.setSSEFilters("get,put,patch,keep-alive,cancel,auth_revoked");
    Database.setSSEFilters("get,put,patch");

    // The "unauthenticate" error can be occurred in this case because we don't wait
    // the app to be authenticated before connecting the stream.
    // This is ok as stream task will be reconnected automatically when the app is authenticated.
    Database.get(aClient, "/colors", asyncCB, true /* SSE mode (HTTP Streaming) */, "streamTask");

    M5.Lcd.setCursor(10,130); 
    M5.Lcd.println("App has initialized.");
    Serial.println("firebase setup done");
}

void loop()
{

  if(state == "setup"){
    //do nthing
    return;
  }

  //Serial.print("loop ");
  M5.update();
  // The async task handler should run inside the main loop
  // without blocking delay or bypassing with millis code blocks.
  updateRTC();
  timeCheck();

  if(WiFi.status() != WL_CONNECTED){
    M5.Lcd.fillRect(0,0, 320, 240, BLACK); 

    M5.Lcd.setCursor(10,10);  
    M5.Lcd.print("Re-start");
    state = "setup";
    wifiSetup();
    firebaseSetup();
    state="init";
    //M5.Lcd.fillScreen(BLACK); 
  }

  app.loop();
  Database.loop();

  //update timer part
  M5.Lcd.fillRect(0,layoutTime, 320, 80, DARKGREY); 
  M5.Lcd.setTextColor(WHITE); 
  M5.Lcd.setTextSize(2);  

  M5.Lcd.setCursor(10,layoutTime+10); 
  M5.Lcd.printf("Now: %d : %d : %d", hour,minute,second); 
  M5.Lcd.setCursor(10,layoutTime+30);
  M5.Lcd.printf("Start:%d:%d", startHour,startMin); 
  M5.Lcd.printf(" End:%d:%d", endHour,endMin);   
  M5.Lcd.setCursor(10,layoutTime+50);
  M5.Lcd.print("Timer: "+timerState);  

  //delay(200);//must take 200milisec to make realtime db work smoothly
  delay(request_interval*2);

  if(timerState == "on"){
    state = "on";
  }else if(timerState == "off"){
    state = "off";
  }

  
  if(state == "on"){
    //Serial.println("state:on");
    M5.Lcd.setTextSize(2); 

    /*
    int x = 280;
    if(colorLength==1){
        M5.Lcd.fillCircle(x, 120+60, 30, M5.Lcd.color565(lights[0].r, lights[0].g, lights[0].b));
    }

    if(colorLength==2){
        M5.Lcd.fillCircle(x, 120+40, 20, M5.Lcd.color565(lights[0].r, lights[0].g, lights[0].b));
        M5.Lcd.fillCircle(x, 120+80, 20, M5.Lcd.color565(lights[1].r, lights[1].g, lights[1].b));
    }
    */

    //hue
    float deltaTime = getDeltaTime();

    //Serial.print("delta time:");
    //Serial.println(deltaTime);

    float deltaDegrees = breathDegreesPerMSec * deltaTime;

    breathCount += round(deltaDegrees);
    if(breathCount>360){
      breathCount = breathCount - 360;
    }

    //Serial.print("color length:");
    //Serial.println(colorLength);

    int d =7;
    int ms = round(deltaTime);

    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(10,layoutLights+0);
    M5.Lcd.printf("Delay: %d",request_interval);  

    if(colorLength == 1){
      //Serial.println("colLength ONE");
      struct colors colorsOne = bleathSin4(lights[0],breathCount);
      //for(int i=lightStartNum;i<(lightStartNum+lightNum);i++){
      for(int i=0;i<20;i++){
        int x = 13*i+10;
        M5.Lcd.fillCircle(x, layoutLights+20, d, M5.Lcd.color565(colorsOne.rgb.r, colorsOne.rgb.g, colorsOne.rgb.b));
        send(colorsOne.xy,rids[i],true,ms);
        delay(request_interval);
      }
    }

    if(colorLength == 2 || colorLength == 3){
      //Serial.println("colLength TWO");
      struct colors colorsOne = bleathSin4(lights[0],breathCount);
      struct colors colorsTwo = bleathSin4(lights[1],breathCount);
      
      //for(int i=lightStartNum;i<(lightStartNum+lightNum/2);i++){
      for(int j=0;j<9;j++){
        int x = 13*j+10;
        M5.Lcd.fillCircle(x, layoutLights+20, d, M5.Lcd.color565(colorsOne.rgb.r, colorsOne.rgb.g, colorsOne.rgb.b));
        send(colorsOne.xy,rids[j],true,ms);
        delay(request_interval);
      }

      //for(int i=(lightStartNum+lightNum/2);i<(lightStartNum+lightNum);i++){
      for(int k=9;k<18;k++){
        int x = 13*k+10;
        M5.Lcd.fillCircle(x, layoutLights+20, d, M5.Lcd.color565(colorsTwo.rgb.r, colorsTwo.rgb.g, colorsTwo.rgb.b));
        send(colorsTwo.xy,rids[k],true,ms);
        delay(request_interval);
      }

      int x = 0;

      //18
      x = 13* 18 + 10;
      M5.Lcd.fillCircle(x, layoutLights+20, d, M5.Lcd.color565(colorsOne.rgb.r, colorsOne.rgb.g, colorsOne.rgb.b));
      send(colorsOne.xy,rids[18],true,ms);
      delay(request_interval);

      //19
      x = 13* 19 + 10;
      M5.Lcd.fillCircle(x, layoutLights+20, d, M5.Lcd.color565(colorsTwo.rgb.r, colorsTwo.rgb.g, colorsTwo.rgb.b));
      send(colorsTwo.xy,rids[19],true,ms);
      delay(request_interval);
      
    }
    
    return;
  }

  if(state == "off"){
    //timerState = off
    //Serial.println("state:off");
    struct colors colorsOne = bleathSin4(lights[0],breathCount);

    for(int i=lightStartNum;i<(lightStartNum+lightNum);i++){
        int x = 13*i+10;
        M5.Lcd.fillCircle(x, layoutLights+20, 10, M5.Lcd.color565(0,0,0));
        M5.Lcd.drawCircle(x, layoutLights+20, 10, M5.Lcd.color565(255,255,255));
        send(colorsOne.xy,rids[i],false,100);
        delay(request_interval*5);
    }

    return;
  }
}

void asyncCB(AsyncResult &aResult)
{
    // WARNING!
    // Do not put your codes inside the callback and printResult.

    printResult(aResult);
    
    //sound();
}

void printResult(AsyncResult &aResult)
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
        RealtimeDatabaseResult &RTDB = aResult.to<RealtimeDatabaseResult>();
        if (RTDB.isStream())
        {
          
            sound();
            Serial.println("----------------------------");
            String task = RTDB.event().c_str();

            Firebase.printf("task: %s\n", aResult.uid().c_str());
            //Firebase.printf("event: %s\n", RTDB.event().c_str());
            Firebase.printf("event: %s\n", task);
            Firebase.printf("path: %s\n", RTDB.dataPath().c_str());
            Firebase.printf("data: %s\n", RTDB.to<const char *>());
            Firebase.printf("type: %d\n", RTDB.type());

            if(task == "keep-alive"){
              Serial.println(" was keep alive");
              return;
            }

            if(task != "get" && task != "put" && task != "patch"){
              Serial.println(" was not get nor put nor patch");
              return;
            }

            const char* rawData = RTDB.to<const char *>();
            Serial.print("rawData");
            Serial.println(rawData);

            // Deserialize the JSON document
            DeserializationError error = deserializeJson(json, rawData);

            // Test if parsing succeeds
            if (error) {
              Serial.print(F("deserializeJson() failed: "));
              Serial.println(error.f_str());
              return;
            }

            //int length = sizeof(json["colors"]);

            double unixtime = json["unixtime"];
            const char* answer = json["answer"];
            int breathTemp =  json["breath"];
            int colorLengthTemp =  json["colorlength"];
            //int colorLengthTemp = sizeof(json["colors"])/4;

            if(breathTemp == 0){
              breath=0.5;
            }else{
              breath=breathTemp;
            }

            if(colorLengthTemp >0){
              colorLength = colorLengthTemp;
            }

            if(colorLength ==1){
              lights[0].r = json["colors"][0]["r"];
              lights[0].g = json["colors"][0]["g"];
              lights[0].b = json["colors"][0]["b"];
            }

            if(colorLength ==2 || colorLength ==3){
              lights[0].r = json["colors"][0]["r"];
              lights[0].g = json["colors"][0]["g"];
              lights[0].b = json["colors"][0]["b"];
              lights[1].r = json["colors"][1]["r"];
              lights[1].g = json["colors"][1]["g"];
              lights[1].b = json["colors"][1]["b"];
            }
   
            //JsonDocument colors = json["colors"];
            Serial.print("answer:");
            Serial.println(answer);
            Serial.print("breath:");
            Serial.println(breath);
            Serial.print("colors:");
            Serial.println(colorLength);

            M5.Lcd.setTextSize(2);

            M5.Lcd.fillRect(0,layoutDB, 320, 80, DARKGREY);
            //M5.Lcd.fillScreen(BLACK);
            M5.Lcd.setCursor(10,layoutDB+10); 
            M5.Lcd.printf("Breath:%d",breath);
            //M5.Lcd.setCursor(10,110); 
            M5.Lcd.printf("  Colors:%d",colorLength);
            M5.Lcd.setCursor(10,layoutDB+30); 
            M5.Lcd.printf("0: R:%d G:%d B:%d", lights[0].r,lights[0].g,lights[0].b);

            if(colorLength == 2 || colorLength == 3){
              M5.Lcd.setCursor(10,layoutDB+50); 
              M5.Lcd.printf("1: R:%d G:%d B:%d", lights[1].r,lights[1].g,lights[1].b);
            }

            int x = 280;
            if(colorLength==1){
                M5.Lcd.fillCircle(x, layoutDB+40, 30, M5.Lcd.color565(lights[0].r, lights[0].g, lights[0].b));
            }

            if(colorLength == 2 || colorLength == 3){
                M5.Lcd.fillCircle(x, layoutDB+20, 16, M5.Lcd.color565(lights[0].r, lights[0].g, lights[0].b));
                M5.Lcd.fillCircle(x, layoutDB+60, 16, M5.Lcd.color565(lights[1].r, lights[1].g, lights[1].b));
            }

            breathPerMin = breath;
            oneBreathMSec = 1000 * 60 / float(breathPerMin);
            breathDegreesPerMSec = 360.0 / oneBreathMSec;
            breathCount = 90;
            //sound();
        }
        else
        {
            Serial.println("----------------------------");
            Firebase.printf("task: %s, payload: %s\n", aResult.uid().c_str(), aResult.c_str());
        }

        Firebase.printf("Free Heap: %d\n", ESP.getFreeHeap());
    }
}

void send(struct colorXY xy,String zoneId,bool on,int ms) {

   // URL 設定
  httpClient.begin(url+zoneId);
  // Content-Type
  httpClient.addHeader("Content-Type", "application/json");
  // Authorization
  httpClient.addHeader("hue-application-key", HUE_APP_KEY);

  String query = "";

  doc["on"]["on"] = on;
  doc["dimming"]["brightness"] = xy.brightness;
  doc["color"]["xy"]["x"] = xy.x;
  doc["color"]["xy"]["y"] = xy.y;
  doc["dynamics"]["duration"] = ms;//in ms

  serializeJson(doc,query);

  /*
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(10, 10);
  M5.Lcd.println("-> Put"+ query);
  Serial.println("-> Put"+ query);
  */

  // ポストする
  int status_code = 0;

  status_code = httpClient.PUT(query);

  //M5.Lcd.println(status_code );
  //Serial.print("send() status code:");
  //Serial.println(status_code );

  if( status_code == 200 || status_code == 400 ){
    String response = httpClient.getString();
 
    //M5.Lcd.println("sent");
    //Serial.println("sent");
     
    //M5.Lcd.println(response);
    //Serial.println(response);
  }
}



