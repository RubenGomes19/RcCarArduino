#include <HCSR04.h>
#include <SoftwareSerial.h>
#include <Servo.h>

#define som 13
#define pir 10

Servo myservo;

//os servos estao a ser usados nas portas analogicas mas como digitais por falta de espaço
//   A0 A1 A2 A3 A4 A5
//13 14 15 16 17 18 19
HCSR04 hc(14, 15); //inicialização HCSR04 (trig pin , echo pin)
HCSR04 hc_direita(16, 17);
HCSR04 hc_esquerda(11, 12);

SoftwareSerial bluetooth(2,3); //(RX,TX)



int servo_pos = 9;
int Mfrente = 5;
int Mtras = 6;
int LED = 8;
int estado = 1;
char comando;
int clicou = 0;
boolean verifica = false;

int obstaculo;

int velocidade = 200;

int dis_sonar_frente;
int dis_sonar_direita;
int dis_sonar_esquerda;

void setup()
{
  pinMode(LED, OUTPUT);
  pinMode(Mfrente, OUTPUT);
  pinMode(Mtras, OUTPUT);
  pinMode(som, OUTPUT);
  pinMode(pir, INPUT);
  Serial.begin(9600);

  delay(100);
  myservo.attach(servo_pos);
  myservo.write(20);

  delay(200);
  bluetooth.begin(9600);

  inicio();
    
}

void loop()
{

  if (bluetooth.available() > 0){
    
    while(bluetooth.available())  {      
      comando = bluetooth.read();  
       
    }
    
    Serial.println(comando);

    //serial.write("teste");

    switch (comando) {
      case 'f':
        if(hc.dist() < 50){
          seguranca();
          
        }else if((hc_direita.dist() <= 10) || (hc_esquerda.dist() <= 10)){
          bluetooth.println("1");
        }else{
          bluetooth.println("0");
          //digitalWrite(Mtras, LOW);
          digitalWrite(Mfrente, !digitalRead(Mfrente));
          digitalWrite(LED, LOW);
          //digitalWrite(LED, !digitalRead(LED));
          Serial.println("Frente");
         
        }
        
        //deteta();
        break;

      case 't':
        if(hc.dist() <= 50){
            digitalWrite(LED, HIGH);
            //bluetooth.println("obstaculo");
            digitalWrite(Mfrente, LOW);
            digitalWrite(Mtras, !digitalRead(Mtras));
          
        }else{
          //bluetooth.println("Sobstaculo");
          digitalWrite(Mfrente, LOW);
          digitalWrite(Mtras, !digitalRead(Mtras));
          digitalWrite(LED, LOW);
          Serial.println("Tras");
        }

        deteta();
        break;


      case 'p':

        digitalWrite(Mfrente, LOW);
        digitalWrite(Mtras, LOW);
        digitalWrite(LED, LOW);
        Serial.println("PARAR MOTORES");
        break;


      case 's':

        digitalWrite(Mfrente, LOW);
        digitalWrite(Mtras, LOW);
        myservo.write(20);
        digitalWrite(LED, LOW);
        Serial.println("PARAGEM EMERGENCIA");
        break;

      case 'e':

        estado = estado + 1;
        if (estado % 2 == 0) {
          myservo.write(42);
          Serial.println("ESQUERDA");
          //bluetooth.println("esquerda");
          
        } else {
          myservo.write(20);
          Serial.println("Indireita rodas");
          //bluetooth.println("normal");
        }

        deteta();
        break;



      case 'd':

        estado = estado + 1;
        if (estado % 2 == 0) {
          myservo.write(0);
          Serial.println("DIREITA");
        } else {
          myservo.write(20);
          Serial.println("Indireita rodas");
        }

        deteta();
        break;
        

      case 'a':
         Serial.println("Modo autonomo ativado");
         modo_autonomo();
         delay(1000);
  
         
         break;
       

        case 'b':
          
          digitalWrite(Mtras, LOW);
          myservo.write(20);
          digitalWrite(Mfrente, LOW);
          digitalWrite(LED, LOW);
          Serial.println("Fim modo autonomo");
          break;

        
        default:
        Serial.println("Desconectou, movimentos parados");
        para_mov();
        break;



    }
    
      
  }
  
  
}



void inicio() {

  
  
  digitalWrite(Mfrente, LOW);
  digitalWrite(Mtras, LOW);
  digitalWrite(LED, LOW);
  myservo.write(20);
  for(int i=0;i<10;i++){
    digitalWrite(LED, HIGH);
    delay(100);
    digitalWrite(LED, LOW);
    delay(100);
    
  }
  
  Serial.println("Ligado");
}


void modo_autonomo(){
  
  dis_sonar_frente = hc.dist();
  dis_sonar_direita = hc_direita.dist();
  dis_sonar_esquerda = hc_esquerda.dist();

  int somaFrente = 0;
  int mediaFrente = 0;

  int somaDireita = 0;
  int mediaDireita = 0;

  int somaEsquerda = 0;
  int mediaEsquerda = 0;

  //delay(2000);
  
  //ciclo que so envia dados e evita obstaculos apos uma media de 40 dados 
  for(int i=0;i<=40;i++){
        somaFrente = somaFrente + dis_sonar_frente;
        somaDireita = somaDireita + dis_sonar_direita;
        somaEsquerda = somaEsquerda + dis_sonar_esquerda;
        
        //delay(100);
  }
     
     
  mediaFrente = (somaFrente/40);
  mediaDireita = (somaDireita/40);
  mediaEsquerda = (somaEsquerda/40);
     

  //Sensor da frente
  if(mediaFrente > 50){
      
    myservo.write(20);
    digitalWrite(Mtras, LOW);
    digitalWrite(LED, LOW);
     
    analogWrite(Mfrente, velocidade);
    Serial.println("Sem obstaculo, a andar");
      
    bluetooth.println("0");
      
    Serial.println("media: ");
    Serial.println(mediaFrente);
    


     //obstaculo detetado pelos sensores se o sensor da frente nao detetar nada <50cm
     if((mediaDireita < 20) && (mediaEsquerda < 20)){
        
      //analogWrite(Mfrente, velocidade);
      //myservo.write(20);

      Serial.println("Obstaculo a direita e a esquerda, continua em frente");

     //obstaculo a direita
     }else if(mediaDireita < 20){
        //Virar para a esquerda durante 500ms
        myservo.write(42);
        Serial.println("Obstaculo a direita");
        delay(100);

     //obstaculo a esquerda 
     }else if(mediaEsquerda < 20){
        //Virar para a direita durante 500ms
        myservo.write(0);
        Serial.println("Obstaculo a esquerda");
        delay(100);
     } 

     //((mediaFrente < 50) ||  ((mediaFrente < 50) && (mediaDireita <20)) || ((mediaFrente < 50 && (mediaEsquerda <20) || (mediaFrente < 50) && (mediaDireita < 20) && (mediaEsquerda < 20)))) 
     }else{

      myservo.write(0);
      digitalWrite(LED, HIGH);
      digitalWrite(Mfrente, LOW);
      analogWrite(Mtras, velocidade);
      Serial.println("Obstaculo a frente, a recuar");
      //Serial.println("Todos sensores detetaram, a recuar ate poder efetuar manobra");
  
      Serial.println("media: ");
      Serial.println(mediaFrente);
      //bluetooth.print("obstaculo");
      //delay(100);
      bluetooth.println("1");
     }


     if((mediaFrente <50 ) &&(mediaDireita < 20) && (mediaEsquerda < 20) ){
      myservo.write(0);
      digitalWrite(LED, HIGH);
      digitalWrite(Mfrente, LOW);
      analogWrite(Mtras, velocidade);
    
      Serial.println("Todos sensores detetaram, a recuar ate poder efetuar manobra");
 
     }

     deteta();

   
}

void seguranca(){
    bluetooth.println("1");
    digitalWrite(Mfrente, LOW);
    digitalWrite(Mtras, LOW);
    digitalWrite(LED, HIGH);
    myservo.write(20);
    Serial.println("Obstaculo a < 50 cm");
    


}

void para_mov(){
    digitalWrite(Mfrente, LOW);
    digitalWrite(Mtras, LOW);
    myservo.write(20);
    for(int i=0;i<10;i++){
      digitalWrite(LED, HIGH);
      delay(100);
      digitalWrite(LED, LOW);
      delay(100);
    }
}


void deteta(){
  if(digitalRead(pir)== HIGH){
          //inicio();
          digitalWrite(Mfrente, LOW);
          digitalWrite(Mtras, LOW);
          digitalWrite(LED, HIGH);
          Serial.println("detetado");
          bluetooth.println("2");
          //tone(som, 1450);
          //delay(100);
          //noTone(som);
          for(int tom = 600; tom <1200; tom++){
                delay(5);
                tone(som,tom);
            }
            for(int tom = 1200; tom > 600; tom--){
                delay(5);
                tone(som,tom);
            }
            noTone(som);
            
         }else{
          Serial.println("nao detetado");
          digitalWrite(LED, LOW);
          //bluetooth.println("Sobstaculo");
          
         }
}
