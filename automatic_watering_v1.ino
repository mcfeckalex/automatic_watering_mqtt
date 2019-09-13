#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define wifi_ssid "HumleBo"
#define wifi_password "HumleBo2014!"

#define mqtt_server "192.168.1.111"
#define mqtt_user "albe"
#define mqtt_password "albe"

#define control_topic "watering/control"
#define period_topic "watering/period"
#define duration_topic "watering/duration"
WiFiClient espClient;
PubSubClient client(espClient);

enum topic {
  control,
  period,
  duration,
  nothing
};


int LED_BOARD = 0;
int LED_CONN= 2;

int RELAY = 12;
int relay_state = 0;
topic received_topic = nothing;
char message[32];


void init_relay() {
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, LOW);
  relay_state = 0;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(LED_BOARD, OUTPUT);
  digitalWrite(LED_BOARD, LOW);

  pinMode(LED_CONN, OUTPUT);
  digitalWrite(LED_CONN, LOW);


  init_relay();
  setup_wifi();
  
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  Serial.print("\nMQTT Configured...\n");
}

void setup_wifi() {
  delay(10);
  
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    // Attempt to connect
    if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
      led_conn_control(1);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      led_conn_control(0);
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
  // Subscribe to topics
  client.subscribe(control_topic);
  client.subscribe(period_topic);
  client.subscribe(duration_topic);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Check if a new message has arrived
  switch (received_topic) {
    case control:
      if (strcmp(message, "ON") == 0) {
        relay_control(1);
      } else {
        relay_control(0);
      }

    default:
    // Do nothing if topic is empty
    break;
  }

  // Reset topic and message  
  received_topic = nothing;
  memset(message, 0, sizeof(message));
}

void callback(char* topic, byte* payload, unsigned int length) {
 
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
 
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.print("\n");

  // Parse topic type
  if (strcmp(topic, control_topic) == 0) {
    received_topic = control;
    memset(message, 0, sizeof(message));
    memcpy(message, payload, length); 
  }
}

int blink(int period) {
  digitalWrite(LED_BOARD, LOW);
  delay(period / 2);
  digitalWrite(LED_BOARD, HIGH);
  delay(period / 2);
  Serial.print("Blinked...\n");
}

int led_control(int signal) {
  if (signal == 1) {
    digitalWrite(LED_BOARD, LOW);
    Serial.print("LED on...\n");
  } else if (signal == 0) {
    digitalWrite(LED_BOARD, HIGH);
    Serial.print("LED off...\n");
  }
}

int led_conn_control(int signal) {
  if (signal == 1) {
    digitalWrite(LED_CONN, HIGH);
    Serial.print("LED Conn on...\n");
  } else if (signal == 0) {
    digitalWrite(LED_CONN, LOW);
    Serial.print("LED Conn off...\n");
  }
}

int relay_control(int signal) {
  if (signal == 1) {
    digitalWrite(RELAY, LOW);
    led_control(1);
    Serial.print("Relay on...\n");
  } else if (signal == 0) {
    digitalWrite(RELAY, HIGH);
    led_control(0);
    Serial.print("Relay off...\n");
  }
}

void relay_toggle() {
  if (relay_state == 1) {
    relay_control(0);
    relay_state = 0;
  } else if (relay_state == 0) {
    relay_control(1);
    relay_state = 1;
  }
}
