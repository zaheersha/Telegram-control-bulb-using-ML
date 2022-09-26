#include <ESP8266WiFi.h>
//Secure Connection with Telegram Server
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
// Wifi network station credentials
#define WIFI_SSID "xxxxxx"              // your network SSID (wifi hotspot name)
                                    
#define WIFI_PASSWORD "xxxxxx"            //wifi hotspot password

//Token is available in BotFather in Telegram.
#define BOT_TOKEN "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"      //telegram bot id number

// mean time between scan messages(Delay b/w the scans)
const unsigned long BOT_MTBS = 1000; 

X509List cert(TELEGRAM_CERTIFICATE_ROOT); 

// Header Files are classes in arduino. Object Created. 
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long bot_lasttime; // last time messages' scan has been done

const int bulb = D0;                         //asssign bulb pin to D0 in esp8266 board
int bulbStatus = 0;

void handleNewMessages(int numNewMessages)
{
  Serial.print("handleNewMessages ");
  Serial.println(numNewMessages);

  for (int i = 0; i < numNewMessages; i++)
  {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "")
      from_name = "Guest";

    if (text == "/bulbon")
    {
      digitalWrite(bulb, HIGH); // turn the bulb on (HIGH is the voltage level)
      bulbStatus = 1;
      bot.sendMessage(chat_id, "bulb is on", "");
    }

    if (text == "/bulboff")
    {
      bulbStatus = 0;
      digitalWrite(bulbPin, LOW); // turn the bulb off (LOW is the voltage level)
      bot.sendMessage(chat_id, "bulb is off", "");
    }

    if (text == "/status")
    {
      if (bulbStatus = 1)
      {
        bot.sendMessage(chat_id, "bulb is ON", "");
      }
      else
      {
        bot.sendMessage(chat_id, "bulb is off", "");
      }
    }

    if (text == "/start")
    {
      String welcome = "Telegram Based Home Automation, " + from_name + ".\n";
      welcome += "This is Telegram Bot example.\n\n";
      welcome += "/bulboff : to switch the bulb off\n";
      welcome += "/bulbon : to switch the bulb on\n";
      welcome += "/status : Returns current status of bulb\n";
      bot.sendMessage(chat_id, welcome, "Markdown");
    }
  }
}


void setup()
{
  Serial.begin(115200);
  Serial.println();

  pinMode(bulb, OUTPUT); // initialize digital bulb as an output.
  delay(10);
  digitalWrite(bulb, HIGH); // initialize pin as off (active LOW)

  // attempt to connect to Wifi network:
  secured_client.setTrustAnchors(&cert);//Telegram secure COnnection. 
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Retrieving time: ");
  configTime(0, 0, "pool.ntp.org"); // get UTC time via NTP
  time_t now = time(nullptr);
  while (now < 24 * 3600)
  {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  Serial.println(now);
}

void loop()
{
  if (millis() - bot_lasttime > BOT_MTBS)
  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages)
    {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    bot_lasttime = millis();
  }
}