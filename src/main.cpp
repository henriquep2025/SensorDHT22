//Inicialização das blibliotecas
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>//Bliblioteca de comunicação do MQPT
#include <ArduinoJson.h>
#include <DHT.h>

//Inicialização das variáveis
const int LED_PIN = D2;

// ---------------- Configurações da Rede WiFi ----------------
const char* ssid = "profbru";
const char* password = "12345678";

// ==========================================
// CONFIGURAÇÕES DO BROKER MQTT
// ==========================================
const char* mqtt_server = "d895a66657a84666a1d0f741b27e2be8.s1.eu.hivemq.cloud"; 
const int mqtt_port = 8883; // Secure MQTT port
const char* mqtt_user = "mario";
const char* mqtt_password = "senai123";

//Tópico para receber comandos
const char* topic_led_control = "lab01/pedroEhenrique";

WiFiClientSecure espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando na rede: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  //Espere até conectar
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

void lerJson(const String& message) {
 
    JsonDocument doc;

    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, message);

    if (error) {
      Serial.print(F("Falha ao analisar JSON (deserializeJson() failed): "));
      Serial.println(error.f_str());
      return;
    }

    // Extrai o valor do comando do JSON
    // Verifica se a chave "comando" existe no JSON
    if (doc["comando"].is<String>()) {
      String comando = doc["comando"].as<String>();
     
      Serial.print("Comando extraído do JSON: ");
      Serial.println(comando);

      // Verifica o comando recebido
      if (comando == "LIGAR" || comando == "ligar") {
        digitalWrite(LED_PIN, HIGH);
        Serial.println("Ação executada: LED LIGADO");
      }
      else if (comando == "DESLIGAR" || comando == "desligar") {
        digitalWrite(LED_PIN, LOW);
        Serial.println("Ação executada: LED DESLIGADO");
      } else {
        Serial.println("Comando desconhecido no JSON.");
      }
    } else {
      Serial.println("A chave 'comando' não foi encontrada no JSON.");
    }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida no tópico [");
  Serial.print(topic);
  Serial.print("]: ");

  // Converte payload para uma String para comparação e análise
  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  // Verifica se a mensagem é para o nosso tópico
  if (String(topic) == topic_led_control) {
    // Processa o JSON recebido
    lerJson(message);
  }
}

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(115200);
 
  // Configure the LED pin as an output and turn it off initially
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
 
  // Connect to WiFi
  setup_wifi();

  // IMPORTANTE: No ESP8266, ao conectar com WiFiClientSecure a um broker público como o HiveMQ,
  // geralmente precisamos ativar o modo inseguro se o certificado da CA não estiver carregado;
  // caso contrário, a conexão falhará.
  espClient.setInsecure();

  // Configure MQTT server and callback function
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Tentando conexão MQTT...");
   
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
   
    // Attempt to connect using credentials
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("conectado!");
     
      // Once connected, subscribe to the command topic
      client.subscribe(topic_led_control);
      Serial.print("Inscrito no tópico: ");
      Serial.println(topic_led_control);
    } else {
      Serial.print("falhou, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5 segundos");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
  // Check if WiFi is still connected, if not, wait for it to reconnect
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Conexão WiFi perdida. Aguardando reconexão...");
    while(WiFi.status() != WL_CONNECTED){
      delay(500);
    }
    Serial.println("WiFi reconectado.");
  }

  // Check if MQTT client is connected
  if (!client.connected()) {
    reconnect();
  }
 
  // Maintain the MQTT connection and process incoming messages
  client.loop();

}