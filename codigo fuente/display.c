
unsigned short int numeroVueltasJugador;   //variable para las vueltas del jugador
unsigned short int numeroVueltasCoche;     //variable para las vueltas del coche
unsigned long int contador;                //variable para el tiempo por si el coche se saliera
int auxpwm;                                //variable para el pwm
int detInterrupcion;                       //variable para determinar la patilla en la interrupción

int nivelInterrupt;                         //variable para procesar el tiempo en función del nivel
double tiempovuelta;                        //variable para ajustar el tiempo de vuelta
unsigned int interrupcionesvuelta;                    //numero interrupciones producidos por una vuelta

void mostrarCadena(char * cadena)
{    int i =0;

     while (cadena[i] != '\0')
         Usart_Write(cadena[i++]);
     Usart_Write(' ');
     Usart_Write(13);

}



void acelerar(int pwmConcreto)
{
    //char * aux="lkjasflkjasdflkasjdf";
    //int cincoporciento = 20;
    
    int cincoporciento = (pwmConcreto - auxpwm) *5/100;
    if (cincoporciento ==0) cincoporciento = 5; //a veces la division de arriba puede dar cero y se cuelga
    
    while (auxpwm < (pwmConcreto- cincoporciento))
    { auxpwm+=cincoporciento;
      Pwm_Change_Duty(auxpwm);
      //InttoStr(auxpwm,aux);
     //mostrarcadena(aux);
    }
    auxpwm = pwmConcreto;
    Pwm_Change_Duty(auxpwm);
  //  InttoStr(auxpwm,aux);
  //   mostrarcadena(aux);

}


void decelerar(int pwmConcreto)
{
    //char * aux="lkjasflkjasdflkasjdf";
    //int cincoporciento = 20;


    int cincoporciento = ((auxpwm - pwmConcreto) *2/100);
    if (cincoporciento == 0) cincoporciento = 5; //a veces la division de arriba puede dar cero

    while(auxpwm > (pwmConcreto+cincoporciento))
    {
     auxpwm -= cincoporciento;
     Pwm_Change_Duty(auxpwm);
     //InttoStr(auxpwm,aux);
     //mostrarcadena(aux);

    }
    
    auxpwm = pwmConcreto;
   // InttoStr(auxpwm,aux);
   //  mostrarcadena(aux);
    Pwm_Change_Duty(auxpwm);


}

void decision(int pwmConcreto, int nivel)
{ pwmConcreto = pwmConcreto - (pwmConcreto * 2 * nivel /100);
  if (tiempoVuelta > 1) pwmConcreto = pwmConcreto * tiempoVuelta;
  //aqui se utilizará el factor de correccion que sera calculado en la interrupcion
 if (auxpwm > pwmConcreto)
     decelerar(pwmConcreto);
  else if (auxpwm < pwmConcreto)
         acelerar(pwmConcreto);

}


void interrupt(void)
 {


    //INTCON=0x00;
    INTCON.GIE=0;
     //interrupcion patilla por lo tanto miramos que entrada es del puerto A
     //distinguir tramo y restar vueltas al jugador y al coche
     //resetear contador, cada vez que se produzca una interrupción por patilla.

       if (PORTA.F0){
                        numeroVueltasJugador--;
                        detInterrupcion=9;
                        delay_ms(200);

                    }
       if (PORTA.F1) {               //medirá la vuelta completa y calculará el factor de ajuste
                       numeroVueltasCoche--;
                       contador=0; //reseteamos contador porque paso por un sensor
                        switch (nivelInterrupt)
                          {
                        case 0: //tarda 4 segundos 80 interrupciones
                                tiempovuelta = interrupcionesvuelta/80;
                                break;
                        case 1://tarda 6 segundos 120 interrupciones
                                tiempovuelta = interrupcionesvuelta/120;
                                break;

                        case 2: //tarda 8 segundos 160 interrupciones
                                tiempovuelta = interrupcionesvuelta/160;
                                break;
                            }
                        interrupcionesvuelta=0;
                       detInterrupcion=10;
                       delay_ms(200);

                      }
       if (PORTA.F2) {
                       detInterrupcion=1;
                       contador =0;
                      }
       if (PORTA.F3) {detInterrupcion=2;
                       contador=0;
                      }
       if (PORTA.F4) {
                      detInterrupcion=3;
                       contador=0;
                      }
       if (PORTA.F5) {
                      detInterrupcion=4;
                       contador=0;
                      }
       if (PORTB.F6) {
                      detInterrupcion=5;
                       contador=0;
                      }
       if (PORTB.F1) {
                      detInterrupcion=6;
                       contador=0;
                      }
       if (PORTB.F2) {
                      detInterrupcion=7;
                       contador=0;
                      }
       if (PORTB.F7) {
                      detInterrupcion=8;
                      contador=0;
                      }
       if (INTCON.TMR0IF)
          {
       interrupcionesvuelta++;
       contador++;  // incremento valor de contador en cada IRQ
       TMR0L  = 61;
       }
       INTCON = 0xB0;

  }



/*Configuracion del pic*/
void configuracion(void)
{ TRISA=0x3F;  //puerto a entrada
  TRISA=0x2F;
  ADCON1=0x87; //puerto a digital
  TRISD = 0x00;// Puerto D salida
  TRISB= 0xDF; // Puerto B entrada
  TRISC =0x80; //Puerto C
  
  //configuración del LCD
  Lcd_Config(&PORTD, 0, 1, 2, 7, 6, 5, 4);
  Lcd_Cmd(Lcd_CLEAR);       // Clear display
  Lcd_Cmd(Lcd_CURSOR_OFF);  // Turn cursor off

  //configuracion del TMR0
   T0CON = 0xC7;      // TMR0 modo 8bit, asigno preescaler a TMR0, div 256
   TMR0L = 61;        // inicializar Timer0
   INTCON =0x00;        //Desactivamos las interrupciones inicialmente

  //Configuracion del control de ancho de pulso
 Pwm_Init(15000);
 Pwm_Start();
 Pwm_Change_Duty(0);
 
 //tiempoVuelta para calibrar el sistema
 //tiempovuelta =0;
 
 //Usart_Init(9600);
}



/* Devuelve el nivel con el que competira el jugador, desde 0 hasta 2*/
unsigned short int seleccionarNivel(){
  short int nivel = 0;
  Lcd_Cmd(Lcd_Clear);
  Lcd_Out(1,1,"Seleccione nivel");

   while (!PORTB.F3){ //start

       if (PORTB.F4)
       {
        nivel = (++nivel ) % 3; //aumentar pero solo hasta 2
        delay_ms(900);
       }
       if (PORTC.F7)
       {
        if (--nivel < 0 ) nivel = 2; //volvemos al nivel 2 despues del 1
        delay_ms(900);
       }
       switch (nivel)
       {

       case 0 : Lcd_Out(2,1,"Nivel 1");
                break;
       case 1: Lcd_Out(2,1,"Nivel 2");
               break;
       case 2: Lcd_Out(2,1,"Nivel 3");
               break;
       }

    }
  return nivel;
} //fin obternerNivel

unsigned short int numeroVueltas()
{
   unsigned short int vueltas = 20;
   char * auxVueltas="200";

   Lcd_Cmd(Lcd_Clear);
   Lcd_Out(1,1,"Vueltas de juego?");
   delay_ms(1000); //esperamos 1 segundo para evitar los rebotes del pulsador start


    while (!PORTB.F3) //boton star
    {

       if (PORTB.F4)
       {
        vueltas ++; //aumentar
        if (vueltas == 0) vueltas =1;
        delay_ms(900);
       }
       //disminuir si es mayor que 1
       if (PORTC.F7)
       {
        if (vueltas != 1 ) vueltas--;
        delay_ms(900);
       }

        WordToStr(vueltas,auxVueltas);
        Lcd_Out(2,1,auxVueltas);

    }
  return vueltas;
} //fin numeroVueltas



void recalibrar()
{
    while (PORTB.F0) //existe una interrupción y hay que calibrar
  {
   if(PORTA.F0) //calibrar el sensor del jugador
   {
    Lcd_Out(1,1,"Por favor de");
    Lcd_Out(2,1,"una vuelta");
    while(PORTA.F0){}//esperando

   }
   else  //ahora solo hay que recalibrar la pista del jugador
   { Lcd_Cmd(Lcd_Clear);
     Lcd_Out(1,1,"Calibrando el");
     Lcd_Out(2,1,"sistema");
     acelerar(165);
     if(PORTA.F1) // si esta encendido
     { while(PORTA.F1){} //esperamos a que se apague
     }
     else //esta apagado y por lo tanto me sirve la interrupcion de este sensor
     {while(!PORTA.F1) {}
     }
    }
  }

  Pwm_Change_Duty(0); //frenamos el coche
  Lcd_Cmd(Lcd_Clear);
  Lcd_Out(1,1,"Sistema");
  Lcd_Out(2,1,"calibrado");

}

void main() {

  char * textolcd1 = "asfdasfdasfsadf"; //inicializamos valor a texto1lcd
  char * textolcd2="asfasfasfasfsafasd";//inicializamos valor a texto2lcd
  unsigned short int fallo = 0; //inicializamos valor de fallo
  unsigned int nivel =0;  //inicializamos valor de  nivel
  auxpwm=0; //inicializamos valor de auxpwm
  INTCON=0x00;

  numeroVueltasCoche=0; //inicializamos valor de numeroVueltasCoche
  numeroVueltasJugador=0;//inicializamos valor de numeroVueltasCoche
  detInterrupcion=-1; //inicializamos valor de detInterrupcion
  contador=0; //inicializamos valor de contador
  nivel=0;



  delay_ms(2000); // esperamos dos segundos para que tome tensión el LCD de la placa
  configuracion();//lanzamos la configuración inicial
  Lcd_Out(1,1,"Saludos");



  /* Recalibrar los sensores hall de la pista*/
  recalibrar();

  PORTB==0x00;
  PORTA==0x00;
  PORTC==0x00;
  PORTD=0x00;

  delay_ms(2000);

  nivel = seleccionarNivel();
  nivelInterrupt = nivel;

 numeroVueltasCoche=numeroVueltas();
 numeroVueltasJugador=numeroVueltasCoche;
 Lcd_Cmd(Lcd_Clear);
 Lcd_Out(1,1,"Todo correcto");
 Lcd_Out(2,1,"Comienza el juego");
  //Cuenta atras secuencial
  delay_ms(2000);
  Lcd_Cmd(Lcd_Clear);
  Lcd_Out(1,1,"Tres");
  delay_ms(1000);
  Lcd_Cmd(Lcd_Clear);
  Lcd_Out(1,1,"Dos");
  delay_ms(1000);
  Lcd_Cmd(Lcd_Clear);
  Lcd_Out(1,1,"Uno");
  delay_ms(1000);
  Lcd_Out(1,1,"Adelante¡¡¡");

  //Comienza el juego
  INTCON =0xB0;//activamos las interrupciones
  auxpwm=0; //reseteamos el valor para el control de ancho de pulso
  contador=0; //reseteamos el contador para el tiempo.
  detInterrupcion=-1; //reseteamos el valor para la detección de las interrupciones

  tiempovuelta=1;
  interrupcionesvuelta=0;


  delay_ms(1000);
  Lcd_Cmd(Lcd_Clear);
  Lcd_Out(1,8,"Player");
  Lcd_Out(2,8,"Program");
  WordToStr(numeroVueltasJugador,textolcd1);
  WordToStr(numeroVueltasCoche,textolcd2);

    Lcd_Out(1,1,textolcd1);

    Lcd_Out(2,1,textolcd2);
   //VELOCIDAD TERMINALLLLLL¡¡¡¡¡¡
   decision(191,nivel);
   contador = 0;
   fallo = 0;
  while ((numeroVueltasCoche > 0) && (numeroVueltasJugador > 0) && (!fallo))
  {

    //faltaría un if con contador > a mi tiempo fallo en sistema y variable fallo a true para cortar el while
     if (contador >= 200) fallo =1; //contador llega a 100 en cinco segundos

    if(detInterrupcion)
    { switch (detInterrupcion)
    { case 1 : decision(175,nivel);      //sensor 3  180       correcto
               detInterrupcion=-1;

               break;
      case 2 : decision(175,nivel);    //sensor 4    175      correcto
               detInterrupcion=-1;

               break;
      case 3 : decision(172,nivel);    //sensor 5   178      correcto
               detInterrupcion=-1;

               break;
      case 4 : decision(200,nivel);    //sensor 6   200      correcto
               detInterrupcion=-1;

               break;
      case 5 : decision(150,nivel);   //sensor 7    190      correcto
               detInterrupcion=-1;

               break;
      case 6 : decision(165,nivel);   //sendor 8   185 correcto
               detInterrupcion=-1;

               break;
      case 7 : decision(180,nivel);  //sensor 9       180 correcto
               detInterrupcion=-1;

               break;
      case 8 : decision(178,nivel);

               detInterrupcion=-1;    //sensor 10     198 correcto
               break;
      case 9 : IntToStr(numeroVueltasJugador,textolcd1);
                Lcd_Out(1,1,textolcd1);
               detInterrupcion=-1;
               break;
      case 10 : IntToStr(numeroVueltasCoche,textolcd2);
                decision(191,nivel);
                Lcd_Out(2,1,textolcd2);
                detInterrupcion=-1;
               break;


    } //fin switch

  }//fin del if



  }
    Lcd_Cmd(Lcd_Clear);
    Pwm_Change_Duty(0); //freanamos el coche
  if(numeroVueltasCoche ==0)
   Lcd_Out(1,1,"Has perdido");
  if(numeroVueltasJugador ==0)
   Lcd_Out(1,1,"Has ganado");
  if (fallo)
     {
     Lcd_Out (1,1,"Has cometido");
     Lcd_Out(2,1,"falta, pierdes");
     }
}
