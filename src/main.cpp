//Inicialização das bibliotecas
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h> //Biblioteca de comunicação MQTT
#include <ArduinoJson.h>
#include <DHT.h>

//Inicialização das variáveis de pinos
const int LED_PIN_VERDE = D2;
const int LED_PIN_AZUL = D8;

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
      Serial.print(F("Falha ao analisar JSON: "));
      Serial.println(error.f_str());
      return;
    }

    // 1. FORMATO NOVO COM CONTROLE INDEPENDENTE: {"led": "verde", "estado": "LIGAR"}
    if (doc["led"].is<String>() && doc["estado"].is<String>()) {
      String ledTarget = doc["led"].as<String>();
      String estado = doc["estado"].as<String>();
      
      ledTarget.toLowerCase();
      estado.toUpperCase();

      int pinoSelecionado = -1;

      if (ledTarget == "verde") {
        pinoSelecionado = LED_PIN_VERDE;
      } else if (ledTarget == "azul") {
        pinoSelecionado = LED_PIN_AZUL;
      }

      if (pinoSelecionado != -1) {
        if (estado == "LIGAR") {
          digitalWrite(pinoSelecionado, HIGH);
          Serial.print("Ação executada: LED ");
          Serial.print(ledTarget);
          Serial.println(" LIGADO");
        } else if (estado == "DESLIGAR") {
          digitalWrite(pinoSelecionado, LOW);
          Serial.print("Ação executada: LED ");
          Serial.print(ledTarget);
          Serial.println(" DESLIGADO");
        } else {
          Serial.println("Estado desconhecido. Use LIGAR ou DESLIGAR.");
        }
      } else {
        Serial.println("LED especificado não encontrado. Use 'verde' ou 'azul'.");
      }
    }
    // 2. FALLBACK PARA COMANDO ANTIGO: {"comando": "LIGAR"} -> Controla o LED Verde por padrão
    else if (doc["comando"].is<String>()) {
      String comando = doc["comando"].as<String>();
      comando.toUpperCase();

      if (comando == "LIGAR") {
        digitalWrite(LED_PIN_VERDE, HIGH);
        Serial.println("Ação executada (modo legado): LED VERDE LIGADO");
      } else if (comando == "DESLIGAR") {
        digitalWrite(LED_PIN_VERDE, LOW);
        Serial.println("Ação executada (modo legado): LED VERDE DESLIGADO");
      }
    } else {
      Serial.println("Estrutura do JSON não reconhecida.");
    }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida no tópico [");
  Serial.print(topic);
  Serial.print("]: ");

  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  if (String(topic) == topic_led_control) {
    lerJson(message);
  }
}

void setup() {
  Serial.begin(115200);
 
  // Configuração dos pinos dos LEDs como saída e desligados no início
  pinMode(LED_PIN_VERDE, OUTPUT);
  digitalWrite(LED_PIN_VERDE, LOW);

  pinMode(LED_PIN_AZUL, OUTPUT);
  digitalWrite(LED_PIN_AZUL, LOW);
 
  setup_wifi();

  espClient.setInsecure();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conexão MQTT...");
   
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
   
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("conectado!");
      client.subscribe(topic_led_control);
      Serial.print("Inscrito no tópico: ");
      Serial.println(topic_led_control);
    } else {
      Serial.print("falhou, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Conexão WiFi perdida. Aguardando reconexão...");
    while(WiFi.status() != WL_CONNECTED){
      delay(500);
    }
    Serial.println("WiFi reconectado.");
  }

  if (!client.connected()) {
    reconnect();
  }
 
  client.loop();
}