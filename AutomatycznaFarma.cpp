#include <WiFi.h>
#include <DHT.h>

const char* ssid = "Cenzurka:DD";
const char* password = "ITutajTeż";

int PinCzujnikWilgotnościGleby1 = 26;
int PinCzujnikWilgotnościGleby2 = 27;
int PinCzujnikŚwiatła = 28;
int PinCzujnikDeszczu = 15;
int PinPrzekaźnik = 16;
#define DHTPIN1 2
#define DHTPIN2 3
#define DHTTYPE DHT11

int progSuszy = 3000;      // Próg górny dla przekaźnika pompy
int progCiemnosci = 1000;  // Próg dolny dnia

DHT dht1(DHTPIN1, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);
WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  pinMode(PinPrzekaźnik, OUTPUT);
  pinMode(PinCzujnikDeszczu, INPUT);
  digitalWrite(PinPrzekaźnik, LOW); //Test pompy
  dht1.begin();
  dht2.begin();

  Serial.print("Lacze z: ");   // POkazanie danych siecie, usnąć po testach

  Serial.println(ssid);
  WiFi.begin(ssid, password);
  Serial.println(WiFi.localIP()); //Wpisać adres z IDE
  server.begin();
}

void loop() {
  int wilg1 = analogRead(PinCzujnikWilgotnościGleby1);
  int wilg2 = analogRead(PinCzujnikWilgotnościGleby2);
  int swiatlo = analogRead(PinCzujnikŚwiatła);
  int czyPada = digitalRead(PinCzujnikDeszczu);
  float temp1 = dht1.readTemperature();
  float temp2 = dht2.readTemperature();
  bool CzyPodlewać = false;
  if (wilg1 > progSuszy || wilg2 > progSuszy) {
    CzyPodlewać = true;
  }
 
 //Sekcja poniżej po dodana po testach. Podlawa 3 sekundy i czeka 5 sekund aż wsiąknie w glebe
  if (CzyPodlewać == true && czyPada == HIGH) {
     digitalWrite(PinPrzekaźnik, HIGH);
     delay(3000);
     digitalWrite(PinPrzekaźnik, LOW);
     delay(5000); 
  } else {
     digitalWrite(PinPrzekaźnik, LOW);
  }

  WiFiClient client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println("Connection: close");  
        client.println();
        
        client.println("<!DOCTYPE HTML><html>");
        client.println("<h1>Farma Raspberry Pico</h1>");
        
        client.print("<p>Gleba 1: <b>"); client.print(wilg1); client.println("</b> (limit: 3000)</p>");
        client.print("<p>Gleba 2: <b>"); client.print(wilg2); client.println("</b></p>");
        
        client.print("<p>Swiatlo: "); client.print(swiatlo); client.println("</p>");
        
        if (czyPada == LOW) client.println("<h2 style='color:red;'>PADA DESZCZ!</h2>");
        else client.println("<p>Nie pada.</p>");
        
        client.print("<p>Temp 1: "); client.print(temp1); client.println(" C</p>");
        
        client.println("</html>");
        break;
      }
    }
    delay(10);
    client.stop();
  }
}
