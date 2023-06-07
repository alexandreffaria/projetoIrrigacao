#include <RtcDS1302.h>

ThreeWire myWire(4,5,2); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);


const int relayPin = 6; // Pino do rele
const int moistureSensorPin = A0; // Pino do sensor de humidade


void setup() {
  Serial.begin(9600); // Set the baud rate to 9600
  
  // Configurando relé e começando ele desligado
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);


  //RTC
  Serial.print("Preparando RTC: ");
  Serial.print(__DATE__);
  Serial.println(__TIME__);

  Rtc.Begin();

  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  printDateTime(compiled);
  Serial.println();


}

void loop() {

  Serial.println("1 - Ligar Rele");
  Serial.println("2 - Desligar Rele");
  Serial.println("3 - Ler Humidade do solo");
  Serial.println("4 - Ler dia e hora");
  Serial.println("5 - Adicionar 4 dias");

  if (Serial.available() > 0) {
    char command = Serial.read(); // Read the incoming command
    
    // Perform actions based on the received command
    switch (command) {
      case '1':
        Serial.println("Ligando Rele...");
        ligarRele();
        break;
        
      case '2':
        Serial.println("Desligando Rele...");
        desligarRele();
        break;

      case '3':
        Serial.println("Lendo a Humidade do solo...");
        Serial.println(String(lerHumidade()) + "%" );
        break;

      case '4':
        Serial.println("Lendo data e hora...");
        RtcDateTime dataAgora = lerDataHora();
        printDateTime(dataAgora);
        break;
      // case '9':
      //   Serial.println("Adicionando 4 dias...");
      //   break;
      // // case '5':
      // //   Serial.println("Adicionando 4 dias...");
      // //   // proximaIrrigacao(lerDataHora());
      // //   break;
      default:
        // Handle invalid commands
        Serial.println("Comando Invalido");
        break;
    }
  }


  delay(3000);
}


void ligarRele(){
  digitalWrite(relayPin, LOW);
}

void desligarRele(){
  digitalWrite(relayPin, HIGH);
}

int lerHumidade(){
  const int calibragemSeco = 500; // Analog reading when the sensor is dry
  const int calibragemMolhado = 200; // Analog reading when the sensor is wet
  int moisture = analogRead(moistureSensorPin);
  int moisturePercentage = map(moisture, calibragemSeco, calibragemMolhado, 0, 100);
  return moisturePercentage;
}

#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime& dt)
{
    char datestring[20];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            dt.Day(),
            dt.Month(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
    Serial.print(datestring);
}

RtcDateTime lerDataHora(){
  RtcDateTime dataHoraAgora = RtcDateTime(__DATE__, __TIME__);
  return dataHoraAgora;
}

unsigned long diasSegundos(int dias) {
  unsigned long segundos = (unsigned long)dias * 24 * 60 * 60;
  return segundos;
}


// += segundos para adicionar dias a data lida
void proximaIrrigacao(RtcDateTime data){
  Serial.println("proximaIrrigacao()");
  uint32_t diasEmSegundos = diasSegundos(4);
  data += diasEmSegundos;
  printDateTime(data);
}