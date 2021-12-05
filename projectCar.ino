 //            UFG - ENGENHARIA DE COMPUTAÇÃO
//                ALL RIGHTS RESERVED



// velocidades padrão do motor
#define pwmLento 75  // constante PWM(0-255) para velocidade lenta
#define pwmRapido 200 // constante PWM(0-255) para velocidade rápida

#define anda 50 // constante de tempo para manobras delicadas
#define le 50 // constante de tempo para leituras de distância delicadas

// conexões do motor
#define mTrasPWM 6 // Motor traseiro
#define mTrasSentido 7 // sentido de rotação Motor traseiro (p/ Frente/Trás)

#define mFrenteEsq 30 // Motor da frente (direção: esquerdo)
#define mFrenteDir 31 // Motor da frente (direção: direito)


// comandos TRAÇÃO TRASEIRA
#define corre 'R' // R= Rápido para frente
#define devagar 'f' // f= devagar para frente
#define rezona 't' // t= Rápido para tras
#define rezinha 'L' // L= lento para tras
#define desliga 'p' // p= motor traseiro para (desliga motor)
#define freio 'P' // P= Freio (trava motor traseiro)

#define botaoFreio 0 // indica que o botão freio foi pressionado para distinguir do freio automático

// comandos DIREÇÃO DIANTEIRA
#define reto 'r' // r= direção reta (desliga motor da frente)
#define direita 'd' // d= vira para DIREITA
#define esquerda 'e' // e= vira para ESQUERDA

// comandos de estacionar
#define mede 'm' //m= medir vagar
#define aut 'a' //a= auto-estacionar

// distâncias LIMITES
#define dMinCorre 35  // distância mínima para poder correr para FRENTE
#define dMinDevagar 22  // distância mínima para poder ir devagar para FRENTE
#define dMinRezinha 5  // distância mínima para poder ir devagar para TRAS
#define dMinRezona 20  // distância mínima para poder correr para TRAS


//nome dos SENSORES ULTRASONICOS
#define deTras 0
#define doCantoTras 1
#define doLado 2
#define doCantoFrente 3
#define daFrente 4

//Carrega a biblioteca do sensor ultrassonico
#include <Ultrasonic.h>

// PINOS dos sensores ultrasonicos
#define sensor0rx 44
#define sensor0tx 45
#define sensor1rx 46
#define sensor1tx 47
#define sensor2rx 48
#define sensor2tx 49
#define sensor3rx 50
#define sensor3tx 51
#define sensor4rx 52
#define sensor4tx 53

//COMUNICAÇÃO COM O OUTRO ARDUINO QUE MONITORA A RODA
#define pinSentido 22
#define pinInicio 23
#define pinOk 24
#define pinParado 25
#define pinPosicao 26

//Inicializa os sensores nos pinos definidos acima
Ultrasonic ultrasonic0(sensor0tx, sensor0rx);
Ultrasonic ultrasonic1(sensor1tx, sensor1rx);
Ultrasonic ultrasonic2(sensor2tx, sensor2rx);
Ultrasonic ultrasonic3(sensor3tx, sensor3rx);
Ultrasonic ultrasonic4(sensor4tx, sensor4rx);
Ultrasonic ultrasonic[5]={ultrasonic0, ultrasonic1, ultrasonic2, ultrasonic3, ultrasonic4};



  int distancia[5]={0,0,0,0,0}; //distancia dos 5 sensores em centímetros truncados
  int distanciaAnt[5]={0,0,0,0,0}; //distancia ANTERIOR dos 5 sensores em centímetros truncados 
  int vagaOk=0;
    int objetoLado=100; //100=indica objeto do lado ainda não detectado  
  int discrepancia;  //distâncias discrepantes a serem ignoradas
  char sentido; // sentido que o carro está ou estava
  char blue; //Armazena o caracter recebido bluetooth
  
void setup() {
  
  //inicializa pinos dos motores
  pinMode( mTrasPWM, OUTPUT );
  pinMode( mTrasSentido, OUTPUT );
  pinMode( mFrenteEsq, OUTPUT );
  pinMode( mFrenteDir, OUTPUT );
  pinMode(pinSentido, INPUT);
  pinMode(pinParado, INPUT);
  pinMode(pinOk, INPUT);
  pinMode(pinInicio, OUTPUT);
  pinMode(pinPosicao, INPUT);
  digitalWrite( mTrasPWM, LOW );
  digitalWrite( mTrasSentido, LOW );
  digitalWrite( mFrenteEsq, LOW );
  digitalWrite( mFrenteDir, LOW );
  digitalWrite( pinInicio, LOW );
  Serial.begin(9600);


}

void loop() {

           lerDistancias(0);
           antiColisao();


           controleBlue(); //COMUNICAÇÃO BLUETOOTH

}

void rapidoFrente(char comando){
           sentido=comando; // sentido do carro para outras funções saberem
           Serial.println( "Rápido para frente." );
               // velocidade e direção
           digitalWrite( mTrasSentido, HIGH ); // direção = frente
           analogWrite( mTrasPWM, 255-pwmRapido ); // velocidade PWM = Rápido
  }

void rapidoTras(char comando){
            sentido=comando; // sentido do carro para outras funções saberem
            Serial.println( "Rápido para tras." );
               // velocidade e direção
            digitalWrite( mTrasSentido, LOW ); // direção = para trás
            analogWrite(mTrasPWM, pwmRapido);  // velocidade PWM = Rápido
}

void lentoFrente(char comando){
            Serial.println( "lento para frente." );
               // velocidade e direção
            digitalWrite( mTrasSentido, HIGH ); // direção = para trás
            digitalWrite(mTrasPWM, LOW);   //PARTIDA NO MOTOR, QUANDO EM BAIXA VELOCIDADE
            delay(10);
            analogWrite(mTrasPWM, 255-pwmLento);  // velocidade PWM = lento
            sentido=comando; // sentido do carro para outras funções saberem
}

void lentoTras(char comando){
            Serial.println( "lento para tras." );
               // velocidade e direção
            digitalWrite( mTrasSentido, LOW ); // direção = para trás
            digitalWrite(mTrasPWM, HIGH);   //PARTIDA NO MOTOR, QUANDO EM BAIXA VELOCIDADE
            delay(10);
            analogWrite(mTrasPWM, pwmLento);  // velocidade PWM = lento
            sentido=comando; // sentido do carro para outras funções saberem
}

void para(){
//            Serial.println( "motor traseiro para" );
            digitalWrite( mTrasSentido, LOW );
            digitalWrite( mTrasPWM, LOW );
}

void freia(int margem){
            Serial.println( "FREIO" );
            int s=digitalRead(pinSentido);
            int p=digitalRead(pinParado);

            
            /*if (margem!=botaoFreio) { //se o freio não for pelo botão freio, ou seja, for freio automático e precisar de margem de tolerância
                int dFrenagem= (distancia[daFrente]- margem);
                int dAnterior=distancia[daFrente];
                
                if (distancia[daFrente]<dFrenagem) 
                  for( int ciclo=1; (distancia[daFrente] < dFrenagem)&&(ciclo<=5); ciclo++){//ciclos para impedir que ele ande na direção oposta
                    lerDistancias();
                    delay(20);
                    // se a distancia for maior
                  } else for( int ciclo=1; ((distancia[daFrente]-1) < dAnterior)&&(ciclo<=5); ciclo++){//ciclos para impedir que ele ande na direção oposta
                          lerDistancias();
                          delay(30);
                          
                  }
            } else delay(100); //se o botão de freio estiver apertado */
            if (p==LOW) {//SE O CARRO NÃO ESTIVER PARADO
                int cont=0;
                inverte(sentido);
                while ((s==digitalRead(pinSentido))&&(cont<10)){ //enquanto o carro não mudar o sentido e for decorrido 300ms, tracionará ao contrário
                    cont++;
                    delay(30);
                }
            }
            para(); 
            
}
                
void inverte(char comando){ //usada no FREIO
              switch (comando)
            {
                case corre: // caso (sentido) então movimento oposto
                    rapidoTras(desliga);
                    break;
                case rezona: // caso (sentido) então movimento oposto
                    rapidoFrente(desliga);
                    break;
                case rezinha: // caso (sentido) então movimento oposto
                    lentoFrente(desliga);
                    break;
                case devagar: // caso (sentido) então movimento oposto
                    lentoTras(desliga);
                    break;    

                default:

                    break;
                                
            }
}

void arreta(){
            Serial.println( "direção reta" );
            digitalWrite( mFrenteEsq, LOW );
            digitalWrite( mFrenteDir, LOW );
}

void viraDireita(){
            Serial.println( "vira DIREITA." );
               // velocidade e direção
            digitalWrite( mFrenteDir, HIGH ); // direção = DIREITA
            digitalWrite( mFrenteEsq, LOW );// velocidade = Rápido
}

void viraEsquerda(){
            Serial.println( "vira ESQUERDA." );
               // velocidade e direção
            digitalWrite( mFrenteDir, LOW ); // direção = ESQUERDA
            digitalWrite( mFrenteEsq, HIGH );// velocidade = Rápido
}

void lerDistancias(int print){
  if (print==1){
   //RECEBE E ENVIA P/ SERIAL AS DISTÂNCIAS DOS 5 ULTRASONICOS
    for (int i=0; i<=4; i++)
    {
      discrepancia = ultrasonic[i].convert(ultrasonic[i].timing(), Ultrasonic::CM); //recebe distância para verificar se é discrepante
      if ((discrepancia<=300)&&(discrepancia!=0)) distancia[i] = discrepancia; //se a distância não for discrepante em 300cm ou se for o 0 da inicialização, então recebe a distância atual
          else if(discrepancia>300) distancia[i]=300;
      Serial.print(" D"); 
      Serial.print(i); // número do sensor
      Serial.print(":");
      Serial.print(distancia[i]);
    }
    Serial.println(); //quebra de linha nas distâncias informadas na tela
  } else {
    for (int i=0; i<=4; i++)
    {
      discrepancia = ultrasonic[i].convert(ultrasonic[i].timing(), Ultrasonic::CM); //recebe distância para verificar se é discrepante
      if ((discrepancia<300)&&(discrepancia!=0)) distancia[i] = discrepancia; //se a distância não for discrepante em 300cm ou se for o 0 da inicialização, então recebe a distância atual
      
    }
  }
  
}

int controleBlue(){
     //COMUNICAÇÃO BLUETOOTH
  while(Serial.available() > 0)  { //enquanto houver caracteres a serem lidos na serial/bluetooth    

      blue = Serial.read(); //recebe o comando do bluetooth
      switch( blue )   // movimenta os motores conforme o caracter recebido
      {
        case corre: //  Rápido para frente
          if (distancia[daFrente]>dMinCorre) rapidoFrente(blue);
              else {
                Serial.println("Pouco espaço para CORRER!");
              }
            break;      
        case rezona: //  Rápido para tras
            rapidoTras(blue);
            break;         
        case devagar: //  devagar para frente
            lentoFrente(blue);
            break;            
        case rezinha: //  lento para tras
            lentoTras(blue);            
            break;   
        case desliga: //  motor traseiro para (desliga motor)
            para();
            return(1);
            break;             
        case freio: //  Freio (trava motor traseiro)
            freia(botaoFreio);
            break;   
        case reto: //  direção reta (desliga motor Frente)
            arreta();
            break;      
        case direita: //  vira para DIREITA
            viraDireita();
            break;      
        case esquerda: // vira para ESQUERDA
            viraEsquerda();
            break;      
        case mede: // mede vaga
            medeVaga();
            break;    
        case aut: // auto-estaciona
            estaciona();   
            break;        
      default:     
          return(0); 
          break;
    }
  }
}

void antiColisao(){   //SISTEMA ANTI-COLISÃO
  if (digitalRead(pinParado)==LOW) { //se carro parado ==não
    switch (sentido)
    {
      case corre:
          if ((blue==corre) && (distancia[daFrente]<(dMinCorre+60))) freia(0); 
          else if (distancia[daFrente]<(dMinCorre+10)) freia(0);
          break;
      case rezona:
          if ((blue==rezona) && (distancia[deTras]<(dMinRezona+60))) freia(0);
          else  if (distancia[deTras]<(dMinRezona+10)) freia(0);
          break;
      case devagar:
          if ((blue==devagar)&&(distancia[daFrente]<(dMinDevagar+30))) freia(0); 
          else if (distancia[daFrente]<(dMinDevagar)) para();
          break;
      case rezinha:
          if ((blue==rezinha)&&(distancia[deTras]<(dMinRezinha+20))) freia(0);
          else if (distancia[deTras]<(dMinRezinha)) para();
          break;
     default:

        break;
    }
  }
}

void medeVaga(){
  arreta();
  vagaOk=0;
  int abortar=0;
  int compOk=0;
  int pOk=0;
  lerDistancias(1);
  
  while((vagaOk==0)&&(abortar==0)){ 
    abortar=controleBlue(); //se apertar botão parar ou freio, aborta 
    lentoFrente(desliga);
    lerDistancias(1);
    antiColisao();
    delay(anda); //intervalo para movimentar
    para();
    delay(le); //intervalo para estabilizar
    lerDistancias(1);
    if ((distancia[doLado]>=2)&&(distancia[doLado]<=6)){
      objetoLado=distancia[doLado]; // detecta ojbeto do lado
  
    
    } else {
            if ((distancia[doLado]>22)&&(objetoLado<=6)) 
            digitalWrite(pinInicio, HIGH); //inicio de vaga detectado >>> outro arduíno começa a medir comprimento
            }
    compOk=digitalRead(pinOk);
    if (compOk==HIGH) { //se comprimento da vaga == ok
          Serial.println("COMPRIMENTO DE VAGA OK");
          delay(1000);
          pOk=digitalRead(pinPosicao);
          while ((pOk==LOW)&&(vagaOk==0)&&(abortar==0)) {//enquanto o carro não estiver na posição certa
                abortar=controleBlue(); //se apertar botão parar ou freio, aborta 
                lentoFrente(desliga);
                lerDistancias(1);
                antiColisao();
                delay(anda); //intervalo para movimentar
                para();
                delay(le); //intervalo para estabilizar
                lerDistancias(1);

                if (distancia[doCantoTras]<=10)
                   vagaOk=1;
                pOk=digitalRead(pinPosicao);
     }    
             // então se o carro estiver na posição certa
         if (pOk=HIGH){
            Serial.println("VAGA DISPONIVEL");
            delay(1000);
            freia(0);
            vagaOk=1;
            digitalWrite(pinInicio, LOW); //encerra medição
         }
      }
    }

  digitalWrite(pinInicio, LOW); //encerra medição
}

int estaciona(){
  int abortar=0;
  int etapa=0;
  if (vagaOk==1){
    vagaOk=0;
    Serial.println("ESTACIONANDO");
    delay(500); 
    etapa++;
    viraDireita();
    for (int i=0; i<10; i++){ //começa a entrar na vaga mesmo se houver algo na frente do sensor do canto traseiro (se tiver passado um pouco do ponto)
        lentoTras(desliga);
        delay(anda);
        para();
        delay(le);
        lerDistancias(1);
        antiColisao();
        if ((abortar=controleBlue())==1) vagaOk=0;
    }
    while ((distancia[doCantoTras]>(18))&&(etapa<=2)&&(abortar==0)) { // manobra para entrar na vaga
        if (etapa==1)  etapa++;
        lentoTras(desliga);
        delay(anda);
        para();
        delay(le);
        lerDistancias(1);
        if ((abortar=controleBlue())==1) vagaOk=0;
        
    }
    if (etapa==2){
        viraEsquerda();
    }
    lerDistancias(1);
    while ((distancia[deTras]>=10)&&(etapa>=2)&&(etapa<=3)&&(abortar==0)) { //não entra se tiver na etapa 1
         if (etapa==2)  etapa++;
         lentoTras(desliga);
         delay(anda-10);
         para();
         delay(le+10);
         lerDistancias(1);
         abortar=controleBlue();
    }
    if (etapa==3) {
        viraDireita();
    }
    lerDistancias(1);
    while ((distancia[deTras]<12)&&(etapa>=3)&&(etapa<=4)&&(abortar==0)){//não entra se tiver na etapa 2
         if (etapa==3)  etapa++;
         lentoFrente(desliga);
         delay(anda+20);
         para();
         delay(le);
         lerDistancias(1);
         abortar=controleBlue();  
    }
    arreta();
     
  } else {
            Serial.println("VAGA NÃO ENCONTRADA");
            delay(1000); 
  }
}


