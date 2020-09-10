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
  Serial.begin(9600);
  bluetooth.begin(9600);
  myservo.attach(servo_pos);
  myservo.write(20);

  pinMode(som, OUTPUT);
  pinMode(pir, INPUT);

  inicio();
}
void loop()
{

     
    //bluetooth.println("ligado");
    //bluetooth.println(hc.dist());
   //Serial.println("Teste");

//bool valor_pir = digitalRead(pir);



  if (bluetooth.available() > 0){
    
    while(bluetooth.available())  {      
      comando = bluetooth.read();  
       

    }    
    
    Serial.println(comando);

    //serial.write("teste");

    switch (comando) {
      case 'f':
        if((hc.dist() <= 50) || (hc_direita.dist() <= 50) || (hc_esquerda.dist() <= 50)){
          seguranca();
          
        }else{
          bluetooth.println("Sobstaculo");
          digitalWrite(Mfrente, !digitalRead(Mfrente));
          digitalWrite(LED, LOW);
          //digitalWrite(LED, !digitalRead(LED));
          Serial.println("Frente");
         
        }
        
        deteta();
        break;

      case 't':
        if(hc.dist() <= 50){
            digitalWrite(LED, HIGH);
            bluetooth.println("obstaculo");
            digitalWrite(Mfrente, LOW);
            digitalWrite(Mtras, !digitalRead(Mtras));
          
        }else{
          bluetooth.println("Sobstaculo");
          digitalWrite(Mfrente, LOW);
          digitalWrite(Mtras, !digitalRead(Mtras));
          digitalWrite(LED, LOW);
          //digitalWrite(LED, !digitalRead(LED));
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
          bluetooth.println("esquerda");
          
        } else {
          myservo.write(20);
          Serial.println("Indireita rodas");
          bluetooth.println("normal");
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
  
         /*if(digitalRead(pir)== HIGH){
          //inicio();
          digitalWrite(LED, HIGH);
          Serial.println("detetado");
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
          
         }*/
               
         
         delay(100);
         
         break;
       

        case 'b':
          
          digitalWrite(Mtras, LOW);
          myservo.write(20);
          digitalWrite(Mfrente, LOW);
          digitalWrite(LED, LOW);
          Serial.println("Fim modo autonomo");
          break;

        


      /*default:
        inicio();
        Serial.println("Algo falhou");
        bluetooth.println("desligou");
        break;*/


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

    //sirene
    /*tone(som, 1450);
    delay(200);
    tone(som, 1890);*/

    
  
  
    
  }
  
  
  Serial.println("Ligado");
}


void modo_autonomo(){
  
  dis_sonar_frente = hc.dist();
  dis_sonar_direita = hc_direita.dist();
  dis_sonar_esquerda = hc_esquerda.dist();

  deteta();

  if( (dis_sonar_frente >= 50)){
      bluetooth.println("Sobstaculo"); 
      delay(100);
      myservo.write(20);
      
      
      //Serial.println("Sobstaculo");
      //vel = 64;
      //analogWrite(velocidadeMotor,vel);
      digitalWrite(Mtras, LOW);
      digitalWrite(LED, LOW);
      
      
      analogWrite(Mfrente, velocidade);
      //Serial.println("Sem obstaculo, a andar");
      
      

  }else{
      delay(100);
      bluetooth.println("obstaculo"); 
      delay(100);
      
      //Recuar durante 900 ms
      //velocidade = 120;
      //analogWrite(velocidadeMotor,vel);
      digitalWrite(LED, HIGH);
      digitalWrite(Mfrente, LOW);

      
      

      //Virar para a direita durante 700ms
      myservo.write(0);
      //Serial.println("Esquerda");
      
     
      analogWrite(Mtras, velocidade);
      //Serial.println("Obstaculo detetado, a recuar");
      delay(200);
       

    }

    if(dis_sonar_direita >= 20){
      //bluetooth.println("Sobstaculo"); 

    }else{
      
     
      
    
      //bluetooth.println("obstaculo"); 
      //delay(100);
       digitalWrite(LED, HIGH);
       //digitalWrite(Mfrente, LOW);
      

      //Virar para a direita durante 700ms
      myservo.write(42);
      Serial.println("Obstaculo a direita");
      
     
      //analogWrite(Mtras, velocidade);
      //digitalWrite(Mtras, HIGH);
      //delay(200);
    }

    if(dis_sonar_esquerda >= 20){
      //bluetooth.println("Sobstaculo"); 

    }else{
      
     
      
    
      //bluetooth.println("obstaculo"); 
      //delay(100);
       digitalWrite(LED, HIGH);
       //digitalWrite(Mfrente, LOW);
      

      //Virar para a direita durante 700ms
      myservo.write(0);
      Serial.println("Obstaculo a direita");
      
     
      //analogWrite(Mtras, velocidade);
      //digitalWrite(Mtras, HIGH);
      //delay(200);
    }

   /* if(dis_sonar_esquerda <= 30){
        //bluetooth.println("Sobstaculo"); 
      
   
        //bluetooth.println("obstaculo"); 
      delay(100);
       digitalWrite(LED, HIGH);
       //digitalWrite(Mfrente, LOW);
      

      //Virar para a direita durante 700ms
      myservo.write(0);
      Serial.println("Obstaculo a Esquerda");
      
     
      //analogWrite(Mtras, velocidade);
      //digitalWrite(Mtras, HIGH);
      delay(200);
      
    }*/
    
    
}

void seguranca(){
    bluetooth.println("obstaculo");
    digitalWrite(Mfrente, LOW);
    digitalWrite(Mtras, LOW);
    digitalWrite(LED, HIGH);
    myservo.write(20);
    Serial.println("Obstaculo a < 50 cm");
    


}


void deteta(){
  if(digitalRead(pir)== HIGH){
          //inicio();
          digitalWrite(LED, HIGH);
          Serial.println("detetado");
          bluetooth.println("detetado");
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
          bluetooth.println("Sobstaculo");
          
         }
}
