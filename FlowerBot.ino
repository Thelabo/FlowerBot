/* Post a simple message to Twitter  */
#if defined(ARDUINO) && ARDUINO > 18
#include <SPI.h>
#endif
//#include "StringListsProgmem.h";
#include <avr/pgmspace.h>
#include <Ethernet.h>
//#include <EthernetDNS.h> // only for IDE 0022 or earlier
#include <Twitter.h>

#include <avr/eeprom.h>
#define eeprom_read_to(dst_p, eeprom_field, dst_size) eeprom_read_block(dst_p, (void *)offsetof(__eeprom_data, eeprom_field), MIN(dst_size, sizeof((__eeprom_data*)0)->eeprom_field))
#define eeprom_read(dst, eeprom_field) eeprom_read_to(&dst, eeprom_field, sizeof(dst))
#define eeprom_write_from(src_p, eeprom_field, src_size) eeprom_write_block(src_p, (void *)offsetof(__eeprom_data, eeprom_field), MIN(src_size, sizeof((__eeprom_data*)0)->eeprom_field))
#define eeprom_write(src, eeprom_field) { typeof(src) x = src; eeprom_write_from(&x, eeprom_field, sizeof(x)); }
#define MIN(x,y) ( x > y ? y : x )

extern int16_t __bss_end; 
extern void *__brkval;
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = { 192, 168, 0, 101 };
byte gateway[] = { 192, 168, 0, 1 };
byte dnsS[] = {4,2,2,2};
byte subnet[] = { 255, 255, 255, 0 };
//char buffer[10];
unsigned long debug=0;
Twitter twitter("964902433-M3yml4e9d6qAnF3FDyOuXgi0kOMkD429fKTu3ywg");
EthernetServer server(23);
bool connectionStatus = false;
//char *msg[] = {"Im Alive!","Hello"};
EthernetClient client;

int16_t lightPin = 0;
int16_t ledPin = 13;
int16_t tempPin = 1;
int16_t soilPin = 2;
int16_t soilOnPin = 12;
int16_t driverPinTop = 11;
int16_t driverPinDown = 7;

const long magicNum = 0xff4;
const float TEMP_CONSTANT = .004882814;
const bool DEBUG = true;

prog_char soilLowWater_0[] PROGMEM = "soilLowWater";
prog_char soilLowWater_1[] PROGMEM = "soilLowWater 1";
prog_char soilLowWater_2[] PROGMEM = "soilLowWater 2";
prog_char soilLowWater_3[] PROGMEM = "soilLowWater 3";
prog_char soilLowWater_4[] PROGMEM = "soilLowWater 4";
prog_char soilLowWater_5[] PROGMEM = "soilLowWater 5";
prog_char soilLowWater_6[] PROGMEM = "soilLowWater 6";
prog_char soilLowWater_7[] PROGMEM = "soilLowWater Rec1";
prog_char soilLowWater_8[] PROGMEM = "soilLowWater Rec2";
prog_char soilLowWater_9[] PROGMEM = "soilLowWater Rec3";
prog_char soilLowWater_10[] PROGMEM = "soilLowWater Rec4";
PROGMEM const char *soilSensorLowWater[] = 
{
	soilLowWater_0,
	soilLowWater_1,
	soilLowWater_2,
	soilLowWater_3,
	soilLowWater_4,
	soilLowWater_5,
	soilLowWater_6,
	soilLowWater_7,
	soilLowWater_8,
	soilLowWater_9,
	soilLowWater_10,
};

//prog_char soilLongTimeSinceWatered_0[] PROGMEM = "soilLongTimeSinceWatered 0";
//prog_char soilLongTimeSinceWatered_1[] PROGMEM = "soilLongTimeSinceWatered 1";
//prog_char soilLongTimeSinceWatered_2[] PROGMEM = "soilLongTimeSinceWatered 2";
//prog_char soilLongTimeSinceWatered_3[] PROGMEM = "soilLongTimeSinceWatered 3";
//prog_char soilLongTimeSinceWatered_4[] PROGMEM = "soilLongTimeSinceWatered 4";
//prog_char soilLongTimeSinceWatered_5[] PROGMEM = "soilLongTimeSinceWatered 5";
//PROGMEM const char *soilSensorHighWater[] = 
//{
//	soilLongTimeSinceWatered_0,
//	soilLongTimeSinceWatered_1,
//	soilLongTimeSinceWatered_2,
//	soilLongTimeSinceWatered_3,
//	soilLongTimeSinceWatered_4,
//	soilLongTimeSinceWatered_5,
//};

prog_char soilIsWatered_0[] PROGMEM = "soilIsWatered 0";
prog_char soilIsWatered_1[] PROGMEM = "soilIsWatered 1";
prog_char soilIsWatered_2[] PROGMEM = "soilIsWatered 2";
prog_char soilIsWatered_3[] PROGMEM = "soilIsWatered 3";
prog_char soilIsWatered_4[] PROGMEM = "soilIsWatered 4";
prog_char soilIsWatered_5[] PROGMEM = "soilIsWatered 5";
PROGMEM const char *soilSensorIsWatered[] = 
{
	soilIsWatered_0,
	soilIsWatered_1,
	soilIsWatered_2,
	soilIsWatered_3,
	soilIsWatered_4,
	soilIsWatered_5,
};

prog_char tempLow_0[] PROGMEM = "tempLow 0";
prog_char tempLow_1[] PROGMEM = "tempLow 1";
prog_char tempLow_2[] PROGMEM = "tempLow 2";
prog_char tempLow_3[] PROGMEM = "tempLow 3";
prog_char tempLow_4[] PROGMEM = "tempLow 4";
prog_char tempLow_5[] PROGMEM = "tempLow 5";
prog_char tempLow_6[] PROGMEM = "tempLow 6";
prog_char tempLow_7[] PROGMEM = "tempLowRecovery 7";
prog_char tempLow_8[] PROGMEM = "tempLowRecovery 8";
prog_char tempLow_9[] PROGMEM = "tempLowRecovery 9";
prog_char tempLow_10[] PROGMEM = "tempLowRecovery 10";
PROGMEM const char *tempSensorLow[] = 
{
	tempLow_0,
	tempLow_1,
	tempLow_2,
	tempLow_3,
	tempLow_4,
	tempLow_5,
	tempLow_6,
	tempLow_7,
	tempLow_8,
	tempLow_9,
	tempLow_10
};

prog_char tempHigh_0[] PROGMEM = "tempHigh 0";
prog_char tempHigh_1[] PROGMEM = "tempHigh 1";
prog_char tempHigh_2[] PROGMEM = "tempHigh 2";
prog_char tempHigh_3[] PROGMEM = "tempHigh 3";
prog_char tempHigh_4[] PROGMEM = "tempHigh 4";
prog_char tempHigh_5[] PROGMEM = "tempHigh 5";
prog_char tempHigh_6[] PROGMEM = "tempHigh 6";
prog_char tempHigh_7[] PROGMEM = "tempHighRec 7";
prog_char tempHigh_8[] PROGMEM = "tempHighRec 8";
prog_char tempHigh_9[] PROGMEM = "tempHighRec 9";
prog_char tempHigh_10[] PROGMEM = "tempHighRec 10";

PROGMEM const char *tempSensorHigh[] = 
{
	tempHigh_0,
	tempHigh_1,
	tempHigh_2,
	tempHigh_3,
	tempHigh_4,
	tempHigh_5,
	tempHigh_6,
	tempHigh_7,
	tempHigh_8,
	tempHigh_9,
	tempHigh_10,
};


enum CMD1
{
	CMD1_READ = 'r', // Read
	CMD1_HELP = 'h', // Help
	CMD1_CONFIGURE = 'c', // Configure 
	CMD1_RESET = 'R',
	CMD1_STATUS = 's',
	CMD1_SAVE = 'W',
};
enum CMD2
{
	CMD2_SOIL_SENSOR = 's',
	CMD2_TEMP_SENSOR = 't',
	CMD2_LIGHT_SENSOR = 'l',
	CMD2_ALL_SENSORS  = 'a',
	CMD2_MEMORY_DATA  = 'm',

};
enum ACTION
{
	ACTION_THRESHHOLD_HIGH =		'h',
	ACTION_THRESHHOLD_LOW =			'l',
	ACTION_SET_SENSOR_DELAY =		'd',
	ACTION_DARKNESS_THRESHHOLD =	't',
	ACTION_TEMPERATURE_LOW =		'l',
	ACTION_TEMPERATURE_HIGH =		'h'

};
enum CMD_LINE_LOC
{
	CMD1 = 0,
	CMD2,
	ACTION = 3,
	VALUE = 5,
};
enum TELNET_STATE
{
	IDLE = 1,
	CONNECTED,
	PARSE_CMD,
	CLEANUP,
}; 
enum SENSOR_READER_STATE
{
	WAIT = 1,
	READ_SOIL,
	READ_LIGHT,
	READ_TEMP,

};
enum TWEET_HANDLER_STATE
{
	CHECK_SENSORS = 1,
	SOIL_SENSOR,
	LIGHT_SENSOR,
	TEMP_SENSOR,
	TWEET,
};
enum SENSOR_ORDER
{
	SOIL = 1,
	LIGHT,
	TEMP
};
//typedef unsigned char state;
// Globals

struct __eeprom_data 
{
	long ifSet;
	int soilThreshholdLow;
	int soilThreshholdHigh;
	int soilIsWatered;
	int soilDelay;
	int tempThreshholdLow;
	int tempThreshholdHigh;
	int tempDelay;
	int lightDelay;
  
};

typedef struct
{
	char characters[11];
	int16_t  count;
}CharBuffer;
typedef struct 
{
	unsigned long timeSinceLastReading;
	unsigned long timeSinceWatered;
	
	int16_t threshholdLow;
	int16_t threshholdHigh;
	bool alarmTriggeredLowWater;
	bool alarmTriggeredHighWater;
	bool alarmTriggeredIsWatered;
	int16_t isWateredThreshhold;
	float rate;
	int16_t value;
	int16_t delay;
	
} SoilSensor;
typedef struct
{
	unsigned long timeSinceLastReading;
	
	unsigned long timeInDarkness;
	unsigned long timeInDaylight;
	int16_t timeInDarknessThreshhold;
	int16_t value;
	int16_t delay;
} LightSensor;
typedef struct
{
	unsigned long timeSinceLastReading;
	bool alarmTriggeredLowTemp;
	bool alarmTriggeredHighTemp;
	int16_t threshholdLow;
	int16_t threshholdHigh;
	int16_t delay;
	int16_t value;
} TemperatureSensor;
SoilSensor soilSensor;
LightSensor lightSensor;
TemperatureSensor temperatureSensor;
CharBuffer buffer;
char msg[] = "här är jag";
void setup()
{
	long magic;
	eeprom_read(magic, ifSet);
	if(magic != magicNum)
		initEEPROM();
	Ethernet.begin(mac,ip,dnsS,gateway,subnet);
	Serial.begin(9600);
	server.begin();

	loadConfig();

	

	soilSensor.value=70;
	//soilSensor.threshholdLow=50;
	soilSensor.rate=0;
	soilSensor.timeSinceLastReading=0;
	soilSensor.timeSinceWatered=0;
	soilSensor.alarmTriggeredLowWater= false;
	soilSensor.alarmTriggeredHighWater = false;
	soilSensor.alarmTriggeredIsWatered = false;
	lightSensor.timeSinceLastReading = 0;
	lightSensor.value= 0;
	buffer.count=0;
	temperatureSensor.timeSinceLastReading = 0;
	temperatureSensor.value = 0;
	
	temperatureSensor.alarmTriggeredLowTemp= false;
	temperatureSensor.alarmTriggeredHighTemp = false;

	pinMode(ledPin,OUTPUT);
	pinMode(soilOnPin,OUTPUT);
	pinMode(driverPinDown,OUTPUT);
	pinMode(driverPinTop,OUTPUT);

	//delay(1000);
	/*Serial.println(msg);
	Serial.println("connecting ...");
	if (twitter.post(msg)) {
    int status = twitter.wait(&Serial);
    if (status == 200) {
      Serial.println("OK.");
    } else {
      Serial.print("failed : code ");
      Serial.println(status);
    }
  } else {
    Serial.println("connection failed.");
	}*/
  
}

void loop()
{
	telnetServer();
	readSensors();
	tweetHandler();

}
void readSensors()
{
	static enum SENSOR_READER_STATE nextState=WAIT;
	//static char *stateName[]= {"","WAIT","READ_SOIL","READ_TEMP","READ_LIGHT"};

	switch (nextState)
	{
	
	case WAIT:

		if ((soilSensor.timeSinceLastReading + minToMillis(soilSensor.delay)) < millis())
		{
			nextState =READ_SOIL;
			//debugStateMatchine(nextState,stateName);
		}
		else if ((lightSensor.timeSinceLastReading + minToMillis(temperatureSensor.delay)) < millis())
		{
			nextState = READ_LIGHT;
			//debugStateMatchine(nextState,stateName);
		}
		else if ((temperatureSensor.timeSinceLastReading + minToMillis(lightSensor.delay)) < millis())
		{
			nextState = READ_TEMP;
			//debugStateMatchine(nextState,stateName);
		}

		break;

	case READ_SOIL:

		static unsigned long timeSinceStartup=0;
		static bool soilSensorActive= false;
		if (!soilSensorActive)
		{
			digitalWrite(driverPinDown,HIGH);
			timeSinceStartup = millis();
			soilSensorActive= true;

		}

		else if (hasTimeElapsed(timeSinceStartup,2000) && soilSensorActive)
		{
			int16_t reading = analogRead(soilPin);
			soilSensor.rate =soilSensor.value - reading;
			soilSensor.value= reading;
			if (soilSensor.rate > 100)
			{
				soilSensor.timeSinceWatered = millis();
			}
			digitalWrite(driverPinDown,LOW);
			soilSensorActive = false;
			soilSensor.timeSinceLastReading = millis();
			nextState =WAIT;
			//debugStateMatchine(nextState,stateName);
		}
		
		break;

	case READ_LIGHT:

		lightSensor.value = analogRead(lightPin);
		lightSensor.timeSinceLastReading = millis();

		nextState =WAIT;
		//debugStateMatchine(nextState,stateName);

		break;

	case READ_TEMP:
		static float sum = analogRead(tempPin) * TEMP_CONSTANT;
		
		temperatureSensor.value =  ((sum  - 0.5) * 100);
		Serial.println(temperatureSensor.value);
		temperatureSensor.timeSinceLastReading = millis();

		nextState =WAIT;
		//debugStateMatchine(nextState,stateName);

		break;

	default:

		nextState = WAIT;
		break;
	}
}
void tweetHandler()
{
	static enum TWEET_HANDLER_STATE
	{
		CHECK_SENSORS = 1,
		SOIL_SENSOR,
		LIGHT_SENSOR,
		TEMP_SENSOR,
		TWEET,
	}nextState= CHECK_SENSORS ;
	//static enum TWEET_HANDLER_STATE nextState= CHECK_SENSORS;
	static enum SENSOR_ORDER sensorOrder = SOIL;
	//static char *stateName[]= {"","CHECK_SENSORS","SOIL_SENSOR","","TEMP_SENSOR","TWEET"};
	static char buffer[128];
	static unsigned long timeSinceLastTweet= 0;
	switch (nextState)
	{
	case CHECK_SENSORS:
		
		switch (sensorOrder)
		{
		case SOIL:
			if ((timeSinceLastTweet + minToMillis(2)) < millis())
			{
				nextState = SOIL_SENSOR;
				//debugStateMatchine(nextState,stateName);
			}
			sensorOrder = TEMP;
			
			break;
		case TEMP:
			if ((timeSinceLastTweet + minToMillis(2)) < millis())
			{
				nextState = TEMP_SENSOR;
				//debugStateMatchine(nextState,stateName);
			}
			sensorOrder = SOIL;
			
			break;
		default:
			sensorOrder = SOIL;
			break;
		}
		break;

	case SOIL_SENSOR:

		if (soilSensor.value < soilSensor.threshholdLow && !soilSensor.alarmTriggeredLowWater)
		{
			strcpy_P(buffer, (char*)pgm_read_word(&(soilSensorLowWater[random(0,5)])));
			nextState = TWEET;
			soilSensor.alarmTriggeredLowWater = true;
			//debugStateMatchine(nextState,stateName);
		}
		else if (soilSensor.value > soilSensor.threshholdLow && soilSensor.alarmTriggeredLowWater)
		{
			strcpy_P(buffer, (char*)pgm_read_word(&(soilSensorLowWater[random(6,10)])));
			nextState = TWEET;
			soilSensor.alarmTriggeredLowWater = false;
			//debugStateMatchine(nextState,stateName);
		}
		else if (soilSensor.rate > 5)
		{
			soilSensor.timeSinceWatered = millis();
			strcpy_P(buffer, (char*)pgm_read_word(&(soilSensorIsWatered[random(0,5)])));
			nextState = TWEET;
			//debugStateMatchine(nextState,stateName);
		}
		else
		{
			nextState = CHECK_SENSORS;
			//debugStateMatchine(nextState,stateName);
		}
		
		break;
	case TEMP_SENSOR:

		if (temperatureSensor.value < temperatureSensor.threshholdLow && !temperatureSensor.alarmTriggeredLowTemp)
		{
			strcpy_P(buffer, (char*)pgm_read_word(&(tempSensorLow[random(0,5)])));
			nextState = TWEET;
			temperatureSensor.alarmTriggeredLowTemp = true;
			//debugStateMatchine(nextState,stateName);
		}
		else if (temperatureSensor.value > temperatureSensor.threshholdLow && temperatureSensor.alarmTriggeredLowTemp)
		{
			strcpy_P(buffer, (char*)pgm_read_word(&(tempSensorLow[random(6,10)])));
			nextState = TWEET;
			temperatureSensor.alarmTriggeredLowTemp = false;
			//debugStateMatchine(nextState,stateName);
		}
		else if (temperatureSensor.value > temperatureSensor.threshholdHigh && !temperatureSensor.alarmTriggeredHighTemp)
		{
			strcpy_P(buffer, (char*)pgm_read_word(&(tempSensorHigh[random(0,5)])));
			nextState = TWEET;
			temperatureSensor.alarmTriggeredHighTemp = true;
			//debugStateMatchine(nextState,stateName);
		}
		else if (temperatureSensor.value < temperatureSensor.threshholdHigh && temperatureSensor.alarmTriggeredHighTemp)
		{
			strcpy_P(buffer, (char*)pgm_read_word(&(tempSensorHigh[random(6,10)])));
			nextState = TWEET;
			temperatureSensor.alarmTriggeredHighTemp = false;
			//debugStateMatchine(nextState,stateName);
		}
		else 
		{
			nextState = CHECK_SENSORS;
			//debugStateMatchine(nextState,stateName);
		}

		break;

	case TWEET:
		timeSinceLastTweet = millis();
		//Serial.println(timeSinceLastTweet);
		Serial.println(buffer);
		//twitter.post(buffer);
		nextState = CHECK_SENSORS;
		//debugStateMatchine(nextState,stateName);

		break;
	}
}

int16_t get_free_memory()
{
  int16_t free_memory;

  if((int16_t)__brkval == 0)
    free_memory = ((int16_t)&free_memory) - ((int16_t)&__bss_end);
  else
    free_memory = ((int16_t)&free_memory) - ((int16_t)__brkval);

  return free_memory;
}
void telnetServer()
{
	//static char *stateName[]= {"","IDLE","CONNECTED","PARSE_CMD","CLEANUP"};
	static enum TELNET_STATE nextState=IDLE;
	

	switch (nextState)
	{
	case IDLE:
		
		 client = server.available();
		if (client.connected())
		{
			
			client.println("FlowerBot configuration interface v0.1");
			

			
			nextState = CONNECTED;
			//debugStateMatchine(CONNECTED,stateName);
			client.flush();

			client.setTimeout(minToMillis(5));
		}
		
		break;

	case CONNECTED:

		if (!client.connected())
		{
			nextState = CLEANUP;
			//debugStateMatchine(CLEANUP,stateName);
			break;
		}

		//static int count = 0;
		while (client.available() > 0)
		{
			char thisChar = client.read();
			//if (thisChar == 0x0d)
			//	break;
			if (buffer.count <= 11)
			{
				buffer.characters[buffer.count] = thisChar;
				
			}

			buffer.count++;

			
		}

		if (buffer.count > 0 && buffer.count <= 11)
		{
			
			nextState = PARSE_CMD;
			//debugStateMatchine(PARSE_CMD,stateName);
		}
		else if (buffer.count > 0 && buffer.count > 11)
		{
			nextState = CLEANUP;
			//debugStateMatchine(CLEANUP,stateName);
		}

		break;

	case PARSE_CMD:

		switch (buffer.characters[CMD1])
		{
		case CMD1_READ:

			parseReadCommands();
			break;

		case CMD1_STATUS:

			parseStatusCommands();
			break;

		case CMD1_HELP:

			client.println("Help!");
			client.println("Read commands: ");
			client.print("rs,rt,rl,ra,W");
			client.println("Config commands: ");
			client.print("cs d,l,h,ct");

			break;

		case CMD1_CONFIGURE:

			parseConfigureCommands();
			break;

		case CMD1_SAVE:

			saveConfig();
			break;

		default:
			client.println("Wrong command");
			break;
		}
		nextState = CLEANUP;
		//debugStateMatchine(CLEANUP,stateName);
		break;

	case CLEANUP:

		for (int16_t x = 0;x < 10;x++)
		{
			buffer.characters[x] = NULL;
		}

		buffer.count = 0;
		client.flush();

		if (client.connected())
		{
			nextState = CONNECTED;
			//debugStateMatchine(CONNECTED,stateName);
		}
		else
		{
			nextState = IDLE;
			//debugStateMatchine(IDLE,stateName);
		}
		break;
	
	}
}

void parseConfigureCommands()
{
	switch (buffer.characters[CMD2])
		{
		case CMD2_SOIL_SENSOR:

			parseSoilSensorConfigCommands();
			break;
		case CMD2_TEMP_SENSOR:

			parseTempSensorConfigCommands();
			break;
		case CMD2_LIGHT_SENSOR:

			parseLightSensorConfigCommands();
			break;
	}
}
void parseSoilSensorConfigCommands()
{
	switch (buffer.characters[ACTION])
	{
	case ACTION_SET_SENSOR_DELAY:

		soilSensor.delay = constrain(getArgumentFromBuffer(),1,1000);
		break;

	case ACTION_THRESHHOLD_LOW:
		soilSensor.threshholdLow = constrain(getArgumentFromBuffer(),1,1000);
		break;



	
	}
}
int16_t parseInt(char theChar)
{
	char num = 0;
	num = theChar - '0';
	
	if (!(num >= 0 && num <= 9))
		num = -1;
	return (int16_t) num;
}
int16_t getArgumentFromBuffer()
{
	int16_t value= 0;
	if (buffer.count < 6)
		return -1;

	int16_t argPosition = VALUE; 
	while (buffer.characters[argPosition] != 0x0d)
	{
		int16_t digit = parseInt(buffer.characters[argPosition]);
		if (digit == -1)
		{
			value = -1;
			break;
		}
		value = value * 10 + digit; 
		argPosition++;
	}
	return value;
}
void parseTempSensorConfigCommands()
{
	switch (buffer.characters[ACTION])
	{
	case ACTION_SET_SENSOR_DELAY:

		temperatureSensor.delay = constrain(getArgumentFromBuffer(),1,1000);
		break;

	case ACTION_THRESHHOLD_LOW:

		temperatureSensor.threshholdLow = constrain(getArgumentFromBuffer(),1,1000);
		break;

	case ACTION_THRESHHOLD_HIGH:

		temperatureSensor.threshholdHigh = constrain(getArgumentFromBuffer(),1,1000);
		break;
	}

}
void parseLightSensorConfigCommands()
{

}
void parseReadCommands()
{
		switch (buffer.characters[CMD2])
		{
		case CMD2_SOIL_SENSOR:
			client.println(soilSensor.value);
			break;
		case CMD2_TEMP_SENSOR:
			client.println(temperatureSensor.value);
			break;
		case CMD2_LIGHT_SENSOR:
			client.println(lightSensor.value);
			break;
		case CMD2_ALL_SENSORS:
			client.print(temperatureSensor.value);
			client.print(":");
			client.print(soilSensor.value);
			client.print(":");
			client.print(lightSensor.value);
			break;
		case CMD2_MEMORY_DATA:
			client.print("Memory: ");
			client.println(get_free_memory());
			break;
		default:
			client.println("Wrong command2");
			break;
		}
}
void parseStatusCommands()
{
	
	switch (buffer.characters[CMD2])
	{
	case CMD2_SOIL_SENSOR:
		client.println("Current status for soil sensor:");
		client.print("Time since last sensor reading: ");
		client.print((millis() - soilSensor.timeSinceLastReading) / minToMillis(1));
		client.println("min");
		client.print("Current Value: ");
		client.println(soilSensor.value);
		client.print("Current rate: ");
		client.println(soilSensor.rate);
		client.print("Delay: ");
		client.print(soilSensor.delay);
		client.println("min");
		client.print("High threshhold: ");
		client.println(soilSensor.threshholdHigh);
		client.print("Low threshhold: ");
		client.println(soilSensor.threshholdLow);
		client.print("Watered threshhold: ");
		client.println(soilSensor.isWateredThreshhold);
		

		break;

	case CMD2_LIGHT_SENSOR:
		client.println("Current status for light sensor:");
		client.print("Time since last sensor reading: ");
		client.print((millis() - lightSensor.timeSinceLastReading) / minToMillis(1));
		client.println("min");
		client.print("Current Value: ");
		client.println(lightSensor.value);
		
		break;

	case CMD2_TEMP_SENSOR:
		client.println("Current status for temperature sensor:");
		client.print("Time since last sensor reading: ");
		client.print((millis() - temperatureSensor.timeSinceLastReading) / minToMillis(1));
		client.println("min");
		client.print("Current Value: ");
		client.println(temperatureSensor.value);
		client.print("Low threshhold: ");
		client.println(temperatureSensor.threshholdLow);
		client.print("High threshhold: ");
		client.println(temperatureSensor.threshholdHigh);
		

	default:
		break;
	}
}
void debugStateMatchine(unsigned char nextState,char *stateName[])
{
	

	static unsigned char inState;
	if (inState != nextState)
	{
		Serial.println(stateName[nextState]);
		inState = nextState;

		
	}
}

unsigned long minToMillis(int min)
{
	return 1000L*60L*(long) min;
}
unsigned long daysToMillis(int16_t days)
{
	return 1000L*60L*60L*24L*days;
}
unsigned long hourToMillis(int16_t hour)
{
	return 1000L*60L*60L*hour;
}
bool hasTimeElapsed(unsigned long &startTime,unsigned long timeToWait)
{
	bool status = false;
	if ((startTime + timeToWait) < millis())
	{
		status = true;
		startTime = millis();
	}
	return status;
}
bool saveConfig()
{
	eeprom_write(soilSensor.threshholdLow, soilThreshholdLow);
	eeprom_write(soilSensor.threshholdHigh, soilThreshholdHigh);
	eeprom_write(soilSensor.isWateredThreshhold, soilIsWatered);
	eeprom_write(soilSensor.delay, soilDelay);

	eeprom_write(temperatureSensor.threshholdLow, tempThreshholdLow);
	eeprom_write(temperatureSensor.threshholdHigh, tempThreshholdHigh);
	eeprom_write(temperatureSensor.delay, tempDelay);

	eeprom_write(lightSensor.delay, lightDelay);
  
	eeprom_write(magicNum, ifSet);
	client.println("Counfiguration saved");
}
bool loadConfig()
{
	eeprom_read(soilSensor.threshholdHigh,soilThreshholdHigh);
	eeprom_read(soilSensor.threshholdLow,soilThreshholdLow);
	eeprom_read(soilSensor.delay,soilDelay);
	eeprom_read(soilSensor.isWateredThreshhold,soilIsWatered);
	eeprom_read(temperatureSensor.threshholdLow,tempThreshholdLow);
	eeprom_read(temperatureSensor.threshholdHigh,tempThreshholdHigh);
	eeprom_read(temperatureSensor.delay,tempDelay);
	eeprom_read(lightSensor.delay,lightDelay);
	client.println("Configuration loaded");
}
/*
long ifSet;
	int soilThreshholdLow;
	int soilThreshholdHigh;
	int soilIsWatered;
	int soilDelay;
	int tempThreshholdLow;
	int tempThreshholdHigh;
	int tempDelay;
	int lightDelay;
*/
void initEEPROM() {
  eeprom_write(100, soilThreshholdLow);
  eeprom_write(900, soilThreshholdHigh);
  eeprom_write(100, soilIsWatered);
  eeprom_write(1, soilDelay);

  eeprom_write(100, tempThreshholdLow);
  eeprom_write(100, tempThreshholdHigh);
  eeprom_write(1, tempDelay);

  eeprom_write(1, lightDelay);
  
  eeprom_write(magicNum, ifSet);
}