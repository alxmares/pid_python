/* ___________________________________________________________ *
 *                                                             *
 * =========================================================== *
 * = = = P I D  C O N T R O L  -  A R D U I N O  N A N O = = = *
 * =========================================================== *
 * ___________________________________________________________ *
 * 
 * Author: Francisco Alex Mares Solano
 * 
 */

// - - -  A R D U I N O   N A N O  - - -

// Librerías
#include <LiquidCrystal_I2C.h>
#include <Thread.h>
#include <ThreadController.h>

// Declaración de constantes
#define ventilador  3
#define lampara     6
#define lm35        A0 
#define target_pot  A1
#define k_pot       A2
#define boton       A3

// Declaración de variables
int celsius = 0;
// Valores 'k' encontrados  // 159, 161
float valores_k[] = {42,3.46,2.78}; // Valores guardados desde los Pots.  
int indice = 3;     // indice del botón
float target = 0;   // objetivo
float p,i,d;        // Valores proporcinal, integral, derivativo
float setP,e1,e2;   // Setpoint y errores
float kp,ki,kd;     // Valores de kp,ki,kd (mismos que 'valores_k')
int ut;             // Salida de control
bool comenzar = false;

// Pantalla LCD
LiquidCrystal_I2C lcd(0x27,20,4); // Declarar objeto

// Declaración de hilos
ThreadController control_hilos = ThreadController(); //Controlador de todos los hilos
Thread hilo_pantalla = Thread();
Thread hilo_sensor = Thread();
Thread hilo_control = Thread();
Thread hilo_lecturas = Thread();

/* =========================== *
 * = = =  S E T  -  U P  = = = *
 * =========================== */
void setup() {
  // Pines
  pinMode(boton,INPUT_PULLUP);
  pinMode(ventilador,OUTPUT);
  pinMode(lampara,OUTPUT);
  // Configuración inicial de pines
  analogWrite(ventilador,0); // Ventilador en 0
  analogWrite(lampara,255);  // Lampara en 0 (pwm inverso)
  // Configuración de puerto Serial
  Serial.begin(115200);
  // Configuración de LCD
  lcd.init();
  lcd.backlight();

  // Configuración de hilos
  hilo_control.onRun(controlar);  // Hilo de control con 10ms de retardo
  hilo_control.setInterval(150);

  hilo_sensor.onRun(lectura_lm35);// Hilo de lectura de sensor LM35
  hilo_sensor.setInterval(100);

  hilo_lecturas.onRun(lecturas);  // Hilo de lecturas de pots. y botón
  hilo_lecturas.setInterval(10);
  
  hilo_pantalla.onRun(mostrar_lcd);//Hilo de pantalla LCD
  hilo_pantalla.setInterval(300); 
  
  // Controlador de hilos
  // Se agregan todos los hilos a un controlador de hilos
  control_hilos.add(&hilo_pantalla);
  control_hilos.add(&hilo_control);
  control_hilos.add(&hilo_sensor);
  control_hilos.add(&hilo_lecturas);


  // Inicialización de sistema
  bienvenida(500); // Mensaje de bienvenida
  sinc();         // Sincronización con Python1
  lcd.setCursor(0,0);
  lcd.print("Celsius: ");
  lcd.setCursor(0,1);
  lcd.print("Target: ");
}

/* ===================================== *
 * = = =  C I C L O   E T E R N O  = = = *
 * ===================================== */
void loop() {
  control_hilos.run();
}

/* ================================= *
 * = = = Función 'controlar()' = = = *
 * ================================= */
void controlar(){
  comenzar = true;
  analogWrite(ventilador,60);

  // Control PID
  e2 = e1;
  e1 = target - celsius;
  

  /*if (e1>=2){
    //Serial.println("Máximo");
    analogWrite(lampara,0);
    comenzar = false;
  }
  else if (e1<=-2){
    //Serial.println("Mínimo");
    analogWrite(lampara,126);
    comenzar = false;
  }
  else comenzar = true;*/

  if (comenzar == true){
    p = (kp*e1);
    i = i+ki*(0.1*e1);
    d = kd*(e2/0.1);
  
    // Anti wind-up
    float imax = 300;
    float imin = -70;
    if (i>imax) i=imax;
    if (i<imin) i=imin;
  
    ut = 255 - p - i - d; // Valor de PID
    if(ut>220)ut=220; // Valores máx. y mín.
    if(ut<20)ut=20;
    //ut = map(ut,0,255,0,255); //Mapear de 0 a 126 (100% a 50%)
  
    //Serial.print("p = ");Serial.println(p);
    //Serial.print("i = ");Serial.println(i);
    //Serial.print("d = ");Serial.println(d);

    //Serial.print("kp = ");Serial.println(kp);
    //Serial.print("ki = ");Serial.println(ki);
    //Serial.print("kd = ");Serial.println(kd);
    
    analogWrite(lampara,ut);
    //Serial.print("ut: ");Serial.println(ut);
  }
  
}

/* ==================================== *
 * = = = Función 'lectura_lm35()' = = = *
 * ==================================== */
void lectura_lm35(){
  float lectura = 0;
  float lecturas = 0;
  for (int i=0;i<20;i++){
    lectura = analogRead(lm35);
    lecturas += (lectura*5.0*100.0/1023.0);
  }
  celsius=(lecturas/20) - 2; // Obtiene el promedio de 20 lecturas
  
}

/* ================================ *
 * = = = Función 'lecturas()' = = = *
 * ================================ */
void lecturas(){
  // Botón
  if (boton_presionado(50) == true){
    actualizar_valores();
    indice++;
    if(indice==4) indice=0;
  }

  // Target Pot.
  target = map(analogRead(target_pot),1,1023,44,37);

  // Valores Kp, Kd, Ki
  if(indice<3){
    if (pot_variado(k_pot,0,10)==true){
      switch(indice){
        case 0:
          valores_k[indice] = map(analogRead(k_pot),1,1023,80,20);
          break;
        case 1:
          valores_k[indice] = floatMap(analogRead(k_pot),1,1023,5,0);
          break;
        case 2:
          valores_k[indice] = floatMap(analogRead(k_pot),1,1023,4,0);
          break;
      }
      
    }
  }
}
float floatMap(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/* =================================== *
 * = = = Función 'mostrar_lcd()' = = = *
 * =================================== */
void mostrar_lcd(){
  mensaje(target,celsius,ut);
  lcd.setCursor(9,0);
  lcd.print(celsius); lcd.print(char(223));

  lcd.setCursor(9,1);
  lcd.print(int(target)); lcd.print(char(223));
  
  switch (indice){
    case 0:
      lcd.setCursor(10,3);
      lcd.print("Kp:");lcd.print(valores_k[0]);
      break;
    case 1:
      lcd.setCursor(10,3);
      lcd.print("Ki:");lcd.print(valores_k[1]);
      break;
    case 2:
      lcd.setCursor(10,3);
      lcd.print("Kd:");lcd.print(valores_k[2]);
      break;
    case 3:
      actualizar_valores();
      break;
  }
}

/* ========================================== *
 * = = = Función 'actualizar_valores()' = = = *
 * ========================================== */
// Actualiza los valores que se muestras en la LCD
void actualizar_valores(){
  kp = valores_k[0];
  ki = valores_k[1];
  kd = valores_k[2];
  lcd.setCursor(0,2); 
  lcd.print("Kp:    ");lcd.setCursor(4,2);lcd.print(int(kp));
  lcd.setCursor(10,2);
  lcd.print("Ki: ");lcd.print(ki);
  lcd.setCursor(0,3);
  lcd.print("Kd: ");lcd.print(kd);
  lcd.print("           ");
}

/* =============================== *
 * = = = Función 'mensaje()' = = = *
 * =============================== */
// x1 ---> Target
// x2 ---> Valor de respuesta (temperatura) 
void mensaje(float x1, float x2, float x3){
  Serial.print("C");Serial.print(int(x1));Serial.print("C");Serial.print(int(x2));Serial.print("C");Serial.println(int(x3));
}


/* =================================== *
 * = = = Función 'pot_variado()' = = = *
 * =================================== */
bool pot_variado(int pin_1, int x1, int x2){ // Devuelve 'true' si se varia el pot. 
  int pot;                                   // en cierto rango
  pot = map(analogRead(pin_1),0,1023,x1,x2);
  delay(50);
  if (pot != map(analogRead(pin_1),0,1023,x1,x2)) return true;
  return false;
}

/* ======================================== *
 * = = = Función 'boton_presionado()' = = = *
 * ======================================== */
bool boton_presionado(int retardo){ // Devuelve 'true' si se presiona el botón
  if (digitalRead(boton)==HIGH){
    delay(retardo);  // Delay para evitar doble toque
    if (digitalRead(boton)==HIGH) return true;
  }
  else return false;
}

/* ============================ *
 * = = = Función 'sinc()' = = = *
 * ============================ */
void sinc(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Esperando...");
  // Esperando comunicación con Python
  while (Serial.read() != 66){
    Serial.print("B");
    delay(100);
    if (boton_presionado(50) == true) {
      delay(200);
      break;
    }
  }
  Serial.flush();
  Serial.print("A");
  lcd.clear();
}

/* ================================== *
 * = = = Función 'bienvenida()' = = = *
 * ================================== */
void bienvenida(int retardo){
  lcd.setCursor(3,0);
  lcd.print("SISTEMA P I D");
  lcd.setCursor(4,2);
  lcd.print("CONTROL  DE");
  lcd.setCursor(4,3);
  lcd.print("TEMPERATURA");
  delay(retardo);
}
