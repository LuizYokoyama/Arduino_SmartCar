//            UFG - ENGENHARIA DE COMPUTAÇÃO
//                ALL RIGHTS RESERVED

/* ATENÇÃO
 *  DEVIDO AS LIMITAÇÕES FÍSICAS DOS SENSORES DA RODA,
 *  ESTAS FUNÇÕES SÓ FUNCIONAM QUANDO RODA ESTÁ EM BAIXA VELOCIDADE
 */

#define sensorFrente 8
#define sensorTras 7

#define pinSentido 2
#define pinInicio 3
#define pinOk 4
#define pinParado 5
#define pinPosicao 6

int magFrente=0;
int antFrente=2;
int magTras=0;
int antTras=2;
int sentido=1; // 1 indica sentido p/ frente, 0 (ré) p/ tras
int iniciado=0; // indica qua a medição não iniciou
int vagaOk=0;
int compOk=0;
int cont=0;

unsigned long tempoAnt=0, tVolta=0, tempo=0;
double distancia=0;


void setup() {
 
pinMode(sensorFrente,INPUT); //sensor magnético que indicara o sentido para frente
pinMode(sensorTras,INPUT);   //sensor magnético que indicara o sentido para tras
pinMode(pinSentido, OUTPUT); // responde o sentido para o outro arduino
pinMode(pinInicio, INPUT); // recebe o comando do outro arduino para inicio de medição do comprimento da vaga
pinMode(pinOk, OUTPUT);     //responde para o outro arduino que comprimento foi atingido
pinMode(pinParado, OUTPUT); // responde para o outro arduino se o carro está parado ou movendo-se
pinMode(pinPosicao, OUTPUT); // responde para o outro arduino se o carro está na posição certa para entrar na vaga

Serial.begin(9600);
digitalWrite(pinSentido, LOW);
digitalWrite(pinOk, LOW);
digitalWrite(pinParado, LOW);
digitalWrite(pinPosicao, LOW);

}

void loop() {

  compVaga();
  sensorRoda();
  digitalWrite(pinSentido, sentido);

 
}

void compVaga(){
  int in;
  in=digitalRead(pinInicio);

  
  if ((in==HIGH)&&(iniciado==0)) {
    Serial.println("iniciando");
    distancia=0; // zera o tacógrafo/acumulador de distância
    iniciado=1; // indica que a medição foi iniciada  
    digitalWrite(pinOk, LOW);
    digitalWrite(pinPosicao, LOW);
    compOk=0;
    cont=0;
  } else if (in==LOW) { // outro arduino mandou aborta/finaliza medição
        if (cont==0) {
          
          cont++;
          Serial.println("aborta");
        }
        iniciado=0;
        compOk=0;
        digitalWrite(pinOk, LOW);
        digitalWrite(pinPosicao, LOW);
      }
        else if ((iniciado==1)&&(distancia>80)){
            compOk=1;
            digitalWrite(pinOk, HIGH);
            Serial.println("compOk");
            Serial.println(distancia);
            if ((compOk==1)&&(distancia>=92)){
                digitalWrite(pinPosicao, HIGH);
                compOk=0;
                Serial.println("pOk");
                Serial.println(distancia);
             } 
          }
  
  
}

void sensorRoda(){

  magFrente=digitalRead(sensorFrente);
  magTras=digitalRead(sensorTras);

  if ((magFrente==HIGH)&&(antFrente==LOW)){ //detecta mudança de estado
    tempo=millis();
    tVolta=tempo-tempoAnt;
    tempoAnt=tempo;
     if (tVolta>200){  
    digitalWrite(pinParado, HIGH); //carro parado ==1
  } else {
    digitalWrite(pinParado, LOW); //carro andando==0
  }

    if (magTras==0) {
      sentido=1;
      distancia=distancia+2.4;
      Serial.println(distancia);
      }
      else sentido=0;  

   }

  if ((magFrente==LOW)&&(antFrente==HIGH)){//detecta mudança de estado
       tempo=millis();
      tVolta=tempo-tempoAnt;
      tempoAnt=tempo;
       if (tVolta>200){  
    digitalWrite(pinParado, HIGH); //carro parado ==1
  } else {
    digitalWrite(pinParado, LOW); //carro andando==0
  }

    if (magTras==HIGH)  {
        sentido=1; 
    }
      else {
        sentido=0;

      }
  }
  antFrente=magFrente;


  if ((magTras==HIGH)&&(antTras==LOW)) //detecta mudança de estado
  {
  tempo=millis();
  tVolta=tempo-tempoAnt;
  tempoAnt=tempo;
   if (tVolta>200){  
    digitalWrite(pinParado, HIGH); //carro parado ==1
  } else {
    digitalWrite(pinParado, LOW); //carro andando==0
  }

    if (magFrente==0) sentido=0;
      else{
         sentido=1;
         distancia=distancia+0.8;
         Serial.println(distancia);
      }
  
  }
  if ((magTras==LOW)&&(antTras==HIGH)){//detecta mudança de estado
     tempo=millis();
    tVolta=tempo-tempoAnt;
    tempoAnt=tempo;
     if (tVolta>200){  
    digitalWrite(pinParado, HIGH); //carro parado ==1
  } else {
    digitalWrite(pinParado, LOW); //carro andando==0
  }

    if (magFrente==HIGH)  sentido=0;
      else {
        sentido=1;
        distancia=distancia+1;
        Serial.println(distancia);
        }

   }
  antTras=magTras;

 
}

