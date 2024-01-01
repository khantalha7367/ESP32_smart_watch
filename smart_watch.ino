#define USE_ARDUINO_INTERRUPTS false
//#include <Arduino_FreeRTOS.h>
//#include <task.h>
//#include <semphr.h>
#include <PulseSensorPlayground.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>            
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <time.h>

const char* ssid="AndroidAPb93d";
const char* password="tk7367420";
AsyncWebServer server(80);


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

//PulseOximeter pox;

Adafruit_SSD1306 display(SCREEN_WIDTH,SCREEN_HEIGHT,&Wire,-1);

//#define OLED_RESET 4

#define button_clock  15
//#define button_heart  11

//Adafruit_SSD1306 display(OLED_RESET);

PulseSensorPlayground pulseSensor;
tmElements_t tm;

//SemaphoreHandle_t sem;

String BPM="";
String accel_steps="";
String Time_h="";

int accel_value=0;
const int PulseWire = 36;       
const int LED=13;
int Threshold = 550;

int steps,myBPM;

bool check=false;

int limit=3000;

unsigned long updated_timer=0;

int GMTOffset = 18000;  //Replace with your GMT Offset in seconds
int daylightOffset = 0; 

TaskHandle_t Task1;



const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css"
integrity="sha384-
fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr"
crossorigin="anonymous">
<style>
html {
font-family: Arial;
display: inline-block;
margin: 0px auto;
text-align: center;
}
h2 { font-size: 3.0rem; }
p { font-size: 3.0rem; }
.units { font-size: 1.2rem; }
.ds-labels{
font-size: 1.5rem;
vertical-align:middle;
padding-bottom: 15px;
}
</style>
</head>
<body>
<h2>Smart Watch</h2>
<p>
<i class="" style="color:#059e8a;"></i>
<span class="ds-labels">Time</span>
<span id="red">%Time%</span>
</p>
<p>
<i class="" style="color:#059e8a;"></i>
<span class="ds-labels">Beats</span>
<span id="green">%Beats%</span>
</p>
<p>
<i class="" style="color:#059e8a;"></i>
<span class="ds-labels">Steps</span>
<span id="blue">%Steps%</span>
</p>
</body>
<script>
setInterval(function ( ) {
var xhttp = new XMLHttpRequest();
xhttp.onreadystatechange = function() {
if (this.readyState == 4 && this.status == 200) {
document.getElementById("time").innerHTML = this.responseText;
}
};
xhttp.open("GET", "/Time_h", true);
xhttp.send();
}, 10000) ;
setInterval(function ( ) {
var xhttp = new XMLHttpRequest();
xhttp.onreadystatechange = function() {
if (this.readyState == 4 && this.status == 200) {
document.getElementById("beats").innerHTML = this.responseText;
}
};
xhttp.open("GET", "/BPM", true);
xhttp.send();
}, 10000) ;
setInterval(function ( ) {
var xhttp = new XMLHttpRequest();
xhttp.onreadystatechange = function() {
if (this.readyState == 4 && this.status == 200) {
document.getElementById("steps").innerHTML = this.responseText;
}
};
xhttp.open("GET", "/accel_steps", true);
xhttp.send();
</script>
</html>)rawliteral";


String processor(const String& var){
//Serial.println(var);
if(var == "Time"){
return Time_h;
}
else if(var == "Beats"){
return BPM;
}
else if(var == "Steps"){
return accel_steps;
}
return String();
}








void clock_interrupt()
{
  check=true;
  updated_timer=millis();
}

  


void watch(void *pvParameters)
{
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)

  display.clearDisplay();
  display.display();  
  Serial.println("Starting Clock task");
  for(;;)
  {
   time_t rawtime = time(nullptr);
struct tm* timeinfo = localtime(&rawtime);
    accel_value=analogRead(4);
    

     if(accel_value<500)
   {
    steps=0;
   }
   if(accel_value>=500 && accel_value<1000)
   {
    steps=200;
   }
   if(accel_value>=1000 && accel_value<1500)
   {
    steps=400;
   }
   if(accel_value>=1500 && accel_value<2000)
   {
    steps=600;
   }
   if(accel_value>=2000)
   {
    steps=800;
   }
   //Serial.println(steps);
    myBPM = 67;
    if (pulseSensor.sawStartOfBeat()) {            
     myBPM = 67; //pulseSensor.getBeatsPerMinute();
                                                
    Serial.println("A HeartBeat Happened ! "); 
    Serial.print("BPM: ");                         
    Serial.println(myBPM);                        
}

    //if(check==true)
    //{
    Serial.println("Displaying time");
    RTC.read(tm);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);                    
    display.print(timeinfo->tm_hour);
    display.print(":");
    //display.print(tm.Minute);
    if( timeinfo->tm_min <10)
    display.print("0");
    display.print(timeinfo->tm_min);
    display.print(":");
    //display.print(tm.Second);
    if( timeinfo->tm_sec <10)
    display.print("0");
    display.print(timeinfo->tm_sec); 
    display.print(" ");
    display.print("BPM=");
    display.print(myBPM);
    display.print(" ");
    display.print("Steps=");
    display.print(steps);
    display.display();
   /* }
    if(digitalRead(button_clock))
    {
      //updated_timer=millis();
      if(millis() - updated_timer>limit)
      {
        check=false;
        display.clearDisplay();
        display.display();
      }
    }*/
    //xSemaphoreGive(sem);
    //}
    Time_h=String(timeinfo->tm_hour)+":"+String(timeinfo->tm_min)+":"+String(timeinfo->tm_sec);
    BPM=String(myBPM);
    accel_steps=String(steps);
    vTaskDelay(pdMS_TO_TICKS(1000));
   //delay(1000);
  }
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.print("Initializing");
  

  pulseSensor.analogInput(PulseWire);   
  //pulseSensor.blinkOnPulse(LED);       //auto-magically blink Arduino's LED with heartbeat.
  pulseSensor.setThreshold(Threshold);
  pinMode(button_clock,INPUT_PULLUP);  

   if (pulseSensor.begin()) {
    Serial.println("We created a pulseSensor Object !");  //This prints one time at Arduino power-up,  or on Arduino reset.  
   }


  attachInterrupt(digitalPinToInterrupt(button_clock),clock_interrupt,FALLING);

  WiFi.begin(ssid,password);
while(WiFi.status() != WL_CONNECTED)
{
delay(500);
Serial.print(".");
}
Serial.println();
Serial.println("Connected");
Serial.println(" IP=");
Serial.print(WiFi.localIP());
Serial.println();
configTime(GMTOffset, daylightOffset, "pool.ntp.org","time.nist.gov");

// Route for root / web page
server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
request->send_P(200, "text/html", index_html, processor);
});
server.on("/time", HTTP_GET, [](AsyncWebServerRequest *request){
request->send_P(200, "text/plain", Time_h.c_str());
});
server.on("/beats", HTTP_GET, [](AsyncWebServerRequest *request){
request->send_P(200, "text/plain", BPM.c_str());
});
server.on("/steps", HTTP_GET, [](AsyncWebServerRequest *request){
request->send_P(200, "text/plain", accel_steps.c_str());
});
server.begin();


  
  if(xTaskCreatePinnedToCore(watch,"Task 1",10000,NULL,1,&Task1,0)==pdPASS)
  {
    Serial.println("Task 1 created");
  }
  else
  {
    Serial.println("Task 1 failed");
  }


}



void loop() {}
