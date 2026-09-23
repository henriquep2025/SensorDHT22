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

// ---------------- ELEMENTOS DO LED VERDE ----------------
const ledVerde = document.getElementById('status-led-verde');
const textoVerde = document.getElementById('status-texto-verde');
const btnLigarVerde = document.getElementById('btn-ligar-verde');
const btnDesligarVerde = document.getElementById('btn-desligar-verde');

// ---------------- ELEMENTOS DO LED AZUL ----------------
const ledAzul = document.getElementById('status-led-azul');
const textoAzul = document.getElementById('status-texto-azul');
const btnLigarAzul = document.getElementById('btn-ligar-azul');
const btnDesligarAzul = document.getElementById('btn-desligar-azul');

// EVENTOS - LED VERDE
btnLigarVerde.addEventListener('click', () => {
  ledVerde.classList.remove('desligado');
  ledVerde.classList.add('ligado');
  textoVerde.textContent = 'LIGADO';

  enviarComando('verde', 'LIGAR');
});

btnDesligarVerde.addEventListener('click', () => {
  ledVerde.classList.remove('ligado');
  ledVerde.classList.add('desligado');
  textoVerde.textContent = 'DESLIGADO';

  enviarComando('verde', 'DESLIGAR');
});

// EVENTOS - LED AZUL
btnLigarAzul.addEventListener('click', () => {
  ledAzul.classList.remove('desligado');
  ledAzul.classList.add('ligado');
  textoAzul.textContent = 'LIGADO';

  enviarComando('azul', 'LIGAR');
});

btnDesligarAzul.addEventListener('click', () => {
  ledAzul.classList.remove('ligado');
  ledAzul.classList.add('desligado');
  textoAzul.textContent = 'DESLIGADO';

  enviarComando('azul', 'DESLIGAR');
});

// FUNÇÃO DE ENVIO COM A NOVA ESTRUTURA JSON
function enviarComando(corLed, estado) {
  if (client.isConnected()) {
    // Monta o JSON com a cor e o estado desejado
    const payload = JSON.stringify({ 
      led: corLed, 
      estado: estado 
    });
    
    const message = new Paho.MQTT.Message(payload);
    message.destinationName = topico;
    client.send(message);

    console.log(`Comando enviado -> LED: ${corLed}, Estado: ${estado}`);
  } else {
    console.warn("MQTT ainda não está conectado!");
  }
}