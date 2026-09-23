const broker = "d895a66657a84666a1d0f741b27e2be8.s1.eu.hivemq.cloud";
const port = 8884;
const clientId = "web_client_" + Math.random().toString(16).substr(2, 8);
const topico = "lab01/pedroEhenrique";

const client = new Paho.MQTT.Client(broker, Number(port), clientId);

client.connect({
  useSSL: true,
  userName: "mario",
  password: "senai123",
  onSuccess: () => {
    console.log("Conectado ao HiveMQ via Paho MQTT!");
  },
  onFailure: (err) => {
    console.error("Falha ao conectar ao MQTT:", err);
  }
});

const led = document.getElementById('status-led');
const texto = document.getElementById('status-texto');
const btnLigar = document.getElementById('btn-ligar');
const btnDesligar = document.getElementById('btn-desligar');

btnLigar.addEventListener('click', () => {
  led.classList.remove('desligado');
  led.classList.add('ligado');
  texto.textContent = 'LIGADO';

  enviarComando('LIGAR');
});

btnDesligar.addEventListener('click', () => {
  led.classList.remove('ligado');
  led.classList.add('desligado');
  texto.textContent = 'DESLIGADO';

  enviarComando('DESLIGAR');
});

function enviarComando(valor) {
  if (client.isConnected()) {
    const payload = JSON.stringify({ comando: valor });
    const message = new Paho.MQTT.Message(payload);
    message.destinationName = topico;
    client.send(message);
  } else {
    console.warn("MQTT ainda não está conectado!");
  }
}