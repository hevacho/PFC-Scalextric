#line 1 "C:/Documents and Settings/bla/Escritorio/programa final/display.c"

unsigned short int numeroVueltasJugador;
unsigned short int numeroVueltasCoche;
unsigned long int contador;
int auxpwm;
int detInterrupcion;

int nivelInterrupt;
double tiempovuelta;
unsigned int interrupcionesvuelta;

void mostrarCadena(char * cadena)
{ int i =0;

 while (cadena[i] != '\0')
 Usart_Write(cadena[i++]);
 Usart_Write(' ');
 Usart_Write(13);

}



void acelerar(int pwmConcreto)
{



 int cincoporciento = (pwmConcreto - auxpwm) *5/100;
 if (cincoporciento ==0) cincoporciento = 5;

 while (auxpwm < (pwmConcreto- cincoporciento))
 { auxpwm+=cincoporciento;
 Pwm_Change_Duty(auxpwm);


 }
 auxpwm = pwmConcreto;
 Pwm_Change_Duty(auxpwm);



}


void decelerar(int pwmConcreto)
{




 int cincoporciento = ((auxpwm - pwmConcreto) *2/100);
 if (cincoporciento == 0) cincoporciento = 5;

 while(auxpwm > (pwmConcreto+cincoporciento))
 {
 auxpwm -= cincoporciento;
 Pwm_Change_Duty(auxpwm);



 }

 auxpwm = pwmConcreto;


 Pwm_Change_Duty(auxpwm);


}

void decision(int pwmConcreto, int nivel)
{ pwmConcreto = pwmConcreto - (pwmConcreto * 2 * nivel /100);
 if (tiempoVuelta > 1) pwmConcreto = pwmConcreto * tiempoVuelta;

 if (auxpwm > pwmConcreto)
 decelerar(pwmConcreto);
 else if (auxpwm < pwmConcreto)
 acelerar(pwmConcreto);

}


void interrupt(void)
 {



 INTCON.GIE=0;




 if (PORTA.F0){
 numeroVueltasJugador--;
 detInterrupcion=9;
 delay_ms(200);

 }
 if (PORTA.F1) {
 numeroVueltasCoche--;
 contador=0;
 switch (nivelInterrupt)
 {
 case 0:
 tiempovuelta = interrupcionesvuelta/80;
 break;
 case 1:
 tiempovuelta = interrupcionesvuelta/120;
 break;

 case 2:
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
 contador++;
 TMR0L = 61;
 }
 INTCON = 0xB0;

 }




void configuracion(void)
{ TRISA=0x3F;
 TRISA=0x2F;
 ADCON1=0x87;
 TRISD = 0x00;
 TRISB= 0xDF;
 TRISC =0x80;


 Lcd_Config(&PORTD, 0, 1, 2, 7, 6, 5, 4);
 Lcd_Cmd(Lcd_CLEAR);
 Lcd_Cmd(Lcd_CURSOR_OFF);


 T0CON = 0xC7;
 TMR0L = 61;
 INTCON =0x00;


 Pwm_Init(15000);
 Pwm_Start();
 Pwm_Change_Duty(0);





}




unsigned short int seleccionarNivel(){
 short int nivel = 0;
 Lcd_Cmd(Lcd_Clear);
 Lcd_Out(1,1,"Seleccione nivel");

 while (!PORTB.F3){

 if (PORTB.F4)
 {
 nivel = (++nivel ) % 3;
 delay_ms(900);
 }
 if (PORTC.F7)
 {
 if (--nivel < 0 ) nivel = 2;
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
}

unsigned short int numeroVueltas()
{
 unsigned short int vueltas = 20;
 char * auxVueltas="200";

 Lcd_Cmd(Lcd_Clear);
 Lcd_Out(1,1,"Vueltas de juego?");
 delay_ms(1000);


 while (!PORTB.F3)
 {

 if (PORTB.F4)
 {
 vueltas ++;
 if (vueltas == 0) vueltas =1;
 delay_ms(900);
 }

 if (PORTC.F7)
 {
 if (vueltas != 1 ) vueltas--;
 delay_ms(900);
 }

 WordToStr(vueltas,auxVueltas);
 Lcd_Out(2,1,auxVueltas);

 }
 return vueltas;
}



void recalibrar()
{
 while (PORTB.F0)
 {
 if(PORTA.F0)
 {
 Lcd_Out(1,1,"Por favor de");
 Lcd_Out(2,1,"una vuelta");
 while(PORTA.F0){}

 }
 else
 { Lcd_Cmd(Lcd_Clear);
 Lcd_Out(1,1,"Calibrando el");
 Lcd_Out(2,1,"sistema");
 acelerar(165);
 if(PORTA.F1)
 { while(PORTA.F1){}
 }
 else
 {while(!PORTA.F1) {}
 }
 }
 }

 Pwm_Change_Duty(0);
 Lcd_Cmd(Lcd_Clear);
 Lcd_Out(1,1,"Sistema");
 Lcd_Out(2,1,"calibrado");

}

void main() {

 char * textolcd1 = "asfdasfdasfsadf";
 char * textolcd2="asfasfasfasfsafasd";
 unsigned short int fallo = 0;
 unsigned int nivel =0;
 auxpwm=0;
 INTCON=0x00;

 numeroVueltasCoche=0;
 numeroVueltasJugador=0;
 detInterrupcion=-1;
 contador=0;
 nivel=0;



 delay_ms(2000);
 configuracion();
 Lcd_Out(1,1,"Saludos");




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


 INTCON =0xB0;
 auxpwm=0;
 contador=0;
 detInterrupcion=-1;

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

 decision(191,nivel);
 contador = 0;
 fallo = 0;
 while ((numeroVueltasCoche > 0) && (numeroVueltasJugador > 0) && (!fallo))
 {


 if (contador >= 200) fallo =1;

 if(detInterrupcion)
 { switch (detInterrupcion)
 { case 1 : decision(175,nivel);
 detInterrupcion=-1;

 break;
 case 2 : decision(175,nivel);
 detInterrupcion=-1;

 break;
 case 3 : decision(172,nivel);
 detInterrupcion=-1;

 break;
 case 4 : decision(200,nivel);
 detInterrupcion=-1;

 break;
 case 5 : decision(150,nivel);
 detInterrupcion=-1;

 break;
 case 6 : decision(165,nivel);
 detInterrupcion=-1;

 break;
 case 7 : decision(180,nivel);
 detInterrupcion=-1;

 break;
 case 8 : decision(178,nivel);

 detInterrupcion=-1;
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


 }

 }



 }
 Lcd_Cmd(Lcd_Clear);
 Pwm_Change_Duty(0);
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
