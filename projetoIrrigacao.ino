#include <RtcDS1302.h>
#include <EEPROM.h>


ThreeWire myWire(4,5,2); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);


const int relayPin = 6; // Pino do rele
const int moistureSensorPin = A0; // Pino do sensor de humidade
const int eepromAddress = 0;
RtcDateTime EEPROMData;

void setup() {
  Serial.begin(9600); // Set the baud rate to 9600
  
  // Configurando relé e começando ele desligado
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);


  //RTC
  Rtc.Begin();

  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  Serial.println();
  Serial.print("Compilado as: ");
  printDateTime(compiled);
  Serial.println();


  // Funcionalidades
  // Serial.println("1 - Ligar Rele");
  // Serial.println("2 - Desligar Rele");
  // Serial.println("3 - Ler Humidade do solo");
  // Serial.println("4 - Ler dia e hora");
  // Serial.println("5 - Adicionar 4 dias");

}

void loop() {
  
  // Le a data salva no EEPROM
  EEPROM.get(eepromAddress, EEPROMData);
  Serial.print("Data lida do EEPROM: ");
  printDateTime(EEPROMData);

  RtcDateTime agora = Rtc.GetDateTime();
  Serial.print("Data lida agora: ");
  printDateTime(agora);

  // Útil pra testar a funcionalidade a cada minuto
  // if(EEPROMData.Second() == agora.Second()){
  if(EEPROMData.Day() == agora.Day() && EEPROMData.Hour() == agora.Hour()){
    
    Serial.println("Dia e Hora de irrigar");

    unsigned long startTime = millis(); 

    while(true){
      int leituraHumidade = lerHumidade();

      if (leituraHumidade > 80){
        // Parar se humidade maior que 80%
        desligarRele();
        break;
      }

      // Inicia um relógio
      unsigned long currentTime = millis();

      // Desliga o relé se passar mais de 20 segundos
      if (currentTime - startTime >= 20000) {
        desligarRele();
        break;
      }

    }

    //Atualiza EEPROM pra próxima data
    RtcDateTime proxirrigacao = proximaIrrigacaoEEPROM(EEPROMData, 4);
    Serial.print("Proxima Irrigacao EEPROM: ");
    printDateTime(proxirrigacao);
    EEPROM.put(eepromAddress, proxirrigacao);
  }
  else{
    Serial.println("Dia de esperar");
  }
  

  if (Serial.available() > 0) {
    char comando = Serial.read(); // Comando pra executar um teste

    // Discard any remaining characters in the serial buffer
    while (Serial.available() > 0) {
      Serial.read(); // Discarta tudo que não for o primeiro char lido
    }
    
    // Teste das funcionalidades
    switch (comando) {
      case '1':
        Serial.println("Ligando Rele...");
        ligarRele();
        Serial.println("Rele ligado...");
        break;
        
      case '2':
        Serial.println("Desligando Rele...");
        desligarRele();
        Serial.println("Rele desligado...");
        break;

      case '3':
        Serial.println("Lendo a Humidade do solo...");
        Serial.println(String(lerHumidade()) + "%" );
        break;

      case '4':
        Serial.println("Olhando o calendário...");
        printDateTime(lerDataHora());
        break;

      case '5':
        Serial.println("Adicionando 4 dias...");
        RtcDateTime dia = lerDataHora();
        proximaIrrigacaoEEPROM(dia, 4);
        Serial.println();
        break;

      case '6':
        Serial.println("Lendo do EEPROM...");
        
        break;
        
      default:  
        Serial.println("Comando Invalido");
        break;
    }
  }

  // Olha a hora a cada 5 minutos
  Serial.print("Esperando mais ");
  Serial.print(5);
  Serial.print(" minutos pra olhar o relogio de novo...");
  
  delay(minutosToMillis(5));

}


void ligarRele(){
  digitalWrite(relayPin, LOW);
}

void desligarRele(){
  digitalWrite(relayPin, HIGH);
}

int lerHumidade(){
  const int calibragemSeco = 520; // Analog reading when the sensor is dry
  const int calibragemMolhado = 230; // Analog reading when the sensor is wet
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
    Serial.println(datestring);
}

RtcDateTime lerDataHora(){
  RtcDateTime dia = Rtc.GetDateTime();
  return dia;
}

unsigned long diasSegundos(int dias) {
  unsigned long segundos = (unsigned long)dias * 24 * 60 * 60;
  return segundos;
}

// += segundos para adicionar dias a data lida
RtcDateTime proximaIrrigacaoEEPROM(RtcDateTime& data, int dias){
  uint32_t diasEmSegundos = diasSegundos(dias);
  data += diasEmSegundos;
  return data;
}

unsigned long minutosToMillis(int minutos){
  return (unsigned long)minutos * 60 * 1000;
}