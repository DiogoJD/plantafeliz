/*

#####################################################################################
#  	Arquivo:               plantafeiz.ino                                             
#   Processador:           Arduino UNO, MEGA ou Teensy++ 2.0      
#  	Verção                 1.0      
#						
#	Objetivo:              Medição de clima e controle de irrigação
#										  	
#   Autor:                 Diogo j Dias(Severance)
#   Date:                  10/06/15	
#   
#	Recife/PE Brazil	
#					
#	COMERCIALIZAÇÃO PROIBIDA, CODIGO LIVRE!!!
#####################################################################################

*/

// Libraries
#include <Adafruit_CC3000.h>
#include <SPI.h>
#include <avr/wdt.h>

// Define CC3000 chip pins
#define ADAFRUIT_CC3000_IRQ   3
#define ADAFRUIT_CC3000_VBAT  5
#define ADAFRUIT_CC3000_CS    10

#include <Wire.h>    //estudar
#include <RTClib.h>  //biblioteca do relogio
#include <DHT.h>     //nome ja dis
#include <LiquidCrystal_I2C.h>

//Sensor de humidade e temperatura
#define DHTPIN 7       // Pino conectado ao pino de dados do sensor
#define DHTTYPE DHT22  // Sensor DHT 22  (AM2302)

DHT dht(DHTPIN, DHTTYPE);  //ou DHT dht(7, DHT22); 

//---------------------------------------------------------------------

//RTC - Real Time Clock
RTC_DS1307 RTC;
//------------------------

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display


// Create CC3000 instances
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT,
                                         SPI_CLOCK_DIV2); // you can change this clock speed
                                         
// WLAN parameters
#define WLAN_SSID       "xxx"
#define WLAN_PASS       "xxx"
#define WLAN_SECURITY   WLAN_SEC_WPA2


// ThingSpeak Settings
char thingSpeakAddress[] = "api.thingspeak.com";
String writeAPIKey = "xxxxxxxxxx";

int buffer_size = 20;

uint32_t ip;


//Variaveis
int sensorSolo = 1;    //variavel que armazena o valor do sensor
int sensorSolo2 = 1;
int sensorSolo3 = 1;
int sensorSolo4 = 1;

void setup(void){

  // Initialize
  	Serial.begin(9600);
  	Wire.begin();       //Inicializacao do protocolo wire
 	dht.begin(); //inicia sensor dht22
  	RTC.begin(); //inicia RTC
 	
	//RTC.adjust(DateTime(__DATE__, __TIME__));//acerta hora de acordo com a do sistema

	//Verifica se o modulo esta funcionando ou nao
	if (! RTC.isrunning()) {
	  	Serial.println("RTC não funcionando!");
	   	//Ajusta a data/hora do Clock com a data/hora em que o codigo foi compilado, basta descomentar a linha
	   	RTC.adjust(DateTime(__DATE__, __TIME__));
	}
	//Fim da verificação

	lcd.init();                      // initialize the lcd
	lcd.backlight();
	 
	 //inicia o pino como tipo saida
	pinMode(8, INPUT);  //sensor 1
	pinMode(9, INPUT);  //sensor 2
	pinMode(4, INPUT); //sensor 3
	pinMode(11, INPUT); //sensor 4

	//pinMode(1, OUTPUT); // porta digital do rele 1
	//pinMode(13, OUTPUT); // porta digital do rele 2
	pinMode(6, OUTPUT); // porta digital do rele 3
	pinMode(12, OUTPUT); // porta digital do rele 4


	Serial.println(F("Initializing WiFi chip..."));
	  if (!cc3000.begin())
	  {
	    Serial.println(F("Couldn't begin()! Check your wiring?"));
	    while(1);
	  }
	  
	  // Connect to WiFi network
	  Serial.print(F("Connecting to WiFi network ..."));
	  cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY);
	  Serial.println(F("done!"));
	  
	  // Wait for DHCP to complete
	  Serial.println(F("Request DHCP"));
	  while (!cc3000.checkDHCP())
	  {
	    delay(100);
	  }


	 	delay(3000);
}

void loop(void){

	//Leitura digital dos sensores de solo
	sensorSolo  = digitalRead(8);//faz uma leitura digital do pino 8
	sensorSolo2 = digitalRead(9);
	sensorSolo3 = digitalRead(4);
	sensorSolo4 = digitalRead(11);
	//----------------------------------------

	delay(3000);

	if(sensorSolo3 == 0){
		Serial.print("Sensor 3: ");
		Serial.println(sensorSolo3);
		digitalWrite(12, LOW);
			
	}else{
		Serial.print("Sensor 3: ");
		Serial.println(sensorSolo3);
		digitalWrite(12, HIGH);
			
	}

		
	if(sensorSolo4 == 0){
		Serial.print("Sensor 4: ");
		Serial.println(sensorSolo4);
		digitalWrite(6, LOW);
			
	}else{
		Serial.print("Sensor 4: ");
		Serial.println(sensorSolo4);
		digitalWrite(6, HIGH);
			
	}



	// Start watchdog 
	wdt_enable(WDTO_8S); 
	  
	// Get IP
	uint32_t ip = 0;
	Serial.print(F("api.thingspea.com -> "));
	while  (ip  ==  0)  {
		if  (!  cc3000.getHostByName(thingSpeakAddress, &ip))  {
	      Serial.println(F("Couldn't resolve!"));
	      while(1){}
	    }
		delay(500);
	}  

	cc3000.printIPdotsRev(ip);
	Serial.println(F(""));
	  
	// Get data & transform to integers
	float h = dht.readHumidity();
	float t = dht.readTemperature();
	  
	int temperatura = (int) t;
	int humidade = (int) h;

	lcd.setCursor(0,0);
	lcd.print("VG ");
	lcd.setCursor(3,0);
	lcd.print("T: ");
	lcd.setCursor(5,0);
	lcd.print(temperatura,1);

	lcd.setCursor(10,0);
	lcd.print("H:");
	lcd.setCursor(12,0);
	lcd.print(humidade,1);

	//Leitura de tempo
	DateTime tempo = RTC.now();//instancia do rtc, recupera a data e hora atual

	int hora    = tempo.hour();
	int minuto  = tempo.minute();
	//-------------------------------------------------------------------------------




	// get length
	int length = 0;

	  
	String data = "";
	data = "field1=" + String(temperatura) + "&field2=" + String(humidade);
	  
	Serial.println(data);

	// Get length
	length = data.length();
	 
	// Reset watchdog
	wdt_reset();
	  
	// Check connection to WiFi
	Serial.print(F("Checking WiFi connection ..."));
	if(!cc3000.checkConnected()){
	   while(1){}
	}
	
	Serial.println(F("done."));
	wdt_reset();

	  
	// Send request
	Adafruit_CC3000_Client client = cc3000.connectTCP(ip, 80);

	if (client.connected()) {

		Serial.println(F("Connected to Thingspeak server."));
	    Serial.print(F("Sending headers "));
	    client.print("POST /update HTTP/1.1\n");
	    client.print("Host: api.thingspeak.com\n");
	    client.print("Connection: close\n");
	    client.print("X-THINGSPEAKAPIKEY: "+writeAPIKey+"\n");
	    client.print("Content-Type: application/x-www-form-urlencoded\n");
	    client.print("Content-Length: ");
	    client.print(data.length());
	    client.print("\n\n");
	    client.print(data);    
	    Serial.println(F(" done."));
	    
	    // Reset watchdog
	    wdt_reset();
	    
	  } else {
	    Serial.println(F("Connection failed"));    
	    return;
	  }
	  
	// Reset watchdog
	wdt_reset();
	  
	Serial.println(F("Reading answer ..."));
	while (client.connected()) {
	  	while (client.available()) {
	      char c = client.read();
	      Serial.print(c);
	    }
	}
	  
	// Reset watchdog
	wdt_reset();
	   
	// Close connection and disconnect
	client.close();
	Serial.println(F("Closing connection"));
	  
	// Reset watchdog & disable
	wdt_reset();
	wdt_disable();
	  
	// Wait 20 seconds until next update
	Serial.println(F("Waiting 20 seconds"));
	delay(20000);
  
}