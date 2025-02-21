/*************************************************************/
/*                                                           */
/*          Script Rega Residencial - Rodrigo Bersi          */
/*                                                           */
/*************************************************************/

/************************************************************* PINOS E DEFINICOES */
/* Inclui Bibliotecas */
#include <Wire.h>               //Biblioteca Cristal Liquido I2C
#include <LiquidCrystal_I2C.h>  //Biblioteca Cristal Lquido I2C  
#include <IRremote.h>           //Biblioteca Controle Remoto
#include <virtuabotixRTC.h>    //Biblioteca Relogio
#include <TimerOne.h>          //Biblioteca Timer

/************************************* COMPONENTES - SENSOR UMIDADE E TEMPERATURA  *//*
#include "DHT.h"
# define DHTPIN 2     // Pino digital sensor DHT
# define DHTTYPE DHT11     // DHT 11 - // Escolha o tipo de sensor!
DHT dht(DHTPIN, DHTTYPE);
*/

/* Define Pinos e Constantes */
#define CHUVA A0      //Pino Sensor Chuva (Modulo Chuva)
#define SOL A1        //Pino Sensor Luz (Fotoressistor)
#define CONTROLE 4    //Pino Receptor Controle Remoto
#define VALVULAUM 13    //Pino do Canteiro (Rele e Solenoide) - (PINO 0 DA INTERRUPCAO PWM E PINO 2 NA PLACA ARDUINO UNO - LIGAR NO PINO 2 PARA PWM)
#define VALVULADOIS 12
#define VALVULATRES 11
#define CLK 5         //Pino CLK do Relogio
#define DAT 6         //Pino DAT do Relogio
#define RST 7         //Pino RST do Relogio

/*  Define Comandos do Controle Remoto  */
#define BTDESLIGA 0xFF00FF
#define BTMENU 0xFF40BF
#define BTMAIS 0xFF58A7
#define BTMENOS 0xFF18E7
#define BTENTRA 0xFF9867
#define BTVOLTA 0xFFA857
#define BTMANUAL 0xFF50AF
#define BTUM 0xFF20DF
#define BTDOIS 0xFFA05F
#define BTTRES 0xFF609F
#define BTQUATRO 0xFF10EF
#define BTCINCO 0xFF906F
#define BTAUTOM 0xFF807F

/* Estancia Objeto Display */
LiquidCrystal_I2C display(0x27,16,2);

/* Estancia Objeto Controle Remoto */
IRrecv controle(CONTROLE);
decode_results comando;

/* Estancia Objeto Relogio  */
virtuabotixRTC myRTC(CLK, DAT, RST);


/************************************************************* DECLARACAO DE VARIAVEIS */
/*  Declara Variaveis de Controle*/
bool Automatico = false;    //Liga e Desliga Modo Automatico
bool StatusDisplay = true;      //Verifica Display Ligado/Desligado
bool StatusTela2 = false;     //Controle Status da Tela Inicial
bool StatusRega = false;    //Inicia Status Desligado
bool StatusTempo = false;   //Viabiliza Animacoes dois tempos
bool m = false;        //Controle rega Manual
bool n = false;       //Nova Rega
byte SolDia = 0;         //Leitura do Sol do dia
byte ChuvaDia = 0;       //Leitura da Chuva do dia
byte ContRega = 0;     //Contagem de Tempo de Rega - Em segundos
byte Comando;          //Recebe Comandos do Controle
byte ContTempo = 0;    //Contagem de Tempo total - Ate 60 e reinicia contagem
byte e;                //Rega da Vez em Edicao
byte r;                //Controle laco for
byte VezRega = 0;         //Rega da Vez - Regando
int R = -1;           //Quantidade de regas
byte DiaSem;        //Define Dia da Semana
byte M = 0;        //Item do Menu
byte TelaVez = 0;   //Define Tela a ser exibida
float u;      //Leitura da Umidade
float t;      //Leitura da Temperatura

/*  Declara Variaveis para as Regas Automaticas  */
int HoraRega[5];    //Define Hora da Rega
int MinutoRega[5];   //Define Minuto da Rega
byte TempoRega[5];   //Define Tempo da Rega - Em minutos
byte IndiceDia[5];   //Define Indice dos Dias de Rega
bool CanteiroUm[5];   //Define Rega Canteiro 1
bool CanteiroDois[5];   //Define Rega Canteiro 2
bool CanteiroTres[5];   //Define Rega Canteiro 3
bool Sol[5];          //Define Criterio Sol
bool Chuva[5];        //Define Criterio Chuva
bool TodoDia[5];          //Define Criterio Dia - Todo Dia
bool SegQuaSex[5];          //Define Criterio Dia - Seg Qua e Sex
bool SabDom[5];          //Define Criterio Dia - Sab e Dom

/*  Declara Variaveis para Rega Manual  */
byte TempoRegaMan;     //Define Tempo Rega
bool CanteiroUmMan;   //Define Rega Canteiro 1
bool CanteiroDoisMan;   //Define Rega Canteiro 2
bool CanteiroTresMan;   //Define Rega Canteiro 3

/*    Define Menus e Textos    */   //Alterado de String para Char - verificar funcionalidade
//Menu Dias de Rega - Altera indice
const char* DiaRega[3] = {"Todo Dia     ",
                    "Seg, Qua, Sex",
                    "Sab e Dom    ",
                   };

//Menu Configuracoes Gerais
const char* Menu[2] = {"  NOVA  REGA  ",    //
                  " EDITAR  REGA "};   //



/* Declara Caracteres Especiais */
byte SolLogo[] = {
  B00000,
  B00100,
  B10101,
  B01110,
  B01110,
  B10101,
  B00100,
  B00000
};

byte AutomaticoLogo[] = {
  B01110,
  B10001,
  B10001,
  B10001,
  B11111,
  B10001,
  B10001,
  B10001
};

byte ChuvaLogo[] = {
  B01111,
  B00111,
  B10011,
  B00101,
  B01000,
  B00010,
  B00100,
  B00000
};


/************** FUNCOES AUXILIARES **************/

/************************************************************* FUNCOES DE VERIFICACAO */
/*    Funcoes de Verificacao    */
//Funcao Verifica Dia Rega
bool verificaDiaRega(){
  if (DiaSem == 0){
    if (TodoDia[r] == true);
    else if (SabDom[r] == true);
  } else if (DiaSem == 1) {
    if (TodoDia[r] == true);
    else if (SegQuaSex[r] == true);
  } else if (DiaSem == 2) {
    if (TodoDia[r] == true); 
  } else if (DiaSem == 3) {
    if (TodoDia[r] == true);
    else if (SegQuaSex[r] == true);
  } else if (DiaSem == 4) {
    if (TodoDia[r] == true);
  } else if (DiaSem == 5) {
    if (TodoDia[r] == true);
    else if (SegQuaSex[r] == true);
  } else if (DiaSem == 6) {
    if (TodoDia[r] == true);
    else if (SabDom[r] == true);
  } 
}

//Funcao Configura Criterio Dia
void configuraDia(byte r){     //Define quando regar - Criterio Dia
  /*  Define Dias de Rega */ 
  if (IndiceDia[r] == 0){             //Todo Dia
    TodoDia[r] = true;  
    SegQuaSex[r] = false;
    SabDom[r] = false;         
  } else if (IndiceDia[r] == 1){      //Seg, Qua e Sex
    TodoDia[r] = false;  
    SegQuaSex[r] = true;
    SabDom[r] = false;        
  } else if (IndiceDia[r] == 2){      //Sab e Dom
    TodoDia[r] = false;  
    SegQuaSex[r] = false;
    SabDom[r] = true; 
  }
} //Fecha funcao configuraDia


//Funcao Verifica Tempo - Interrupcao para contagem de tempo
void verificaTempo() {
if (ContTempo < 60)
  ContTempo = ++ContTempo;
else
  ContTempo = 0;
  
if (StatusRega == true)
  ContRega = ++ContRega;

StatusTempo = !StatusTempo;
}


//Funcao verifica se esta na hora de Ligar Rega - Rega Automatica
void verificaRegaAutomatica(){
Serial.println("VerificaAutomatico");
  for (r=0;r<=(R);++r){
    if (verificaDiaRega){   
      if (myRTC.hours == HoraRega[r]){
        if (myRTC.minutes == MinutoRega[r]){
          if (Sol[r] == true){
            if (SolDia > 600){
             VezRega = r+1;
             acionaRega();
            }
          } else {
            VezRega = r+1;
            acionaRega();                         //Chama funcao rega se for hora de regar e automatico ligado
          }
        }
      }
    } 
  } //Encerra laco for
}   //Fecha Funcao




/************************************************************* FUNCOES DE REGA*/

void acionaRega () {

  display.clear();

  StatusRega = !StatusRega;

  if (StatusRega == true) {

    if (VezRega > 0) {

      if (CanteiroUm[VezRega-1] == true)
        digitalWrite(VALVULAUM, HIGH);
      if (CanteiroDois[VezRega-1] == true)
        digitalWrite(VALVULADOIS, HIGH);
      if (CanteiroTres[VezRega-1] == true)
        digitalWrite(VALVULATRES, HIGH);


    ContRega = 0;
      
    } else {

      if (CanteiroUmMan == true)
        digitalWrite(VALVULAUM, HIGH);
      if (CanteiroDoisMan == true)
        digitalWrite(VALVULADOIS, HIGH);
      if (CanteiroTresMan == true)
        digitalWrite(VALVULATRES, HIGH);

    ContRega = 0;
      
    }
    
    
  } else {

    if (VezRega > 0) {

      if (CanteiroUm[VezRega-1] == true)
        digitalWrite(VALVULAUM, LOW);
      if (CanteiroDois[VezRega-1] == true)
        digitalWrite(VALVULADOIS, LOW);
      if (CanteiroTres[VezRega-1] == true)
        digitalWrite(VALVULATRES, LOW);

    ContRega = 0;
      
    } else {

      if (CanteiroUmMan == true)
        digitalWrite(VALVULAUM, LOW);
      if (CanteiroDoisMan == true)
        digitalWrite(VALVULADOIS, LOW);
      if (CanteiroTresMan == true)
        digitalWrite(VALVULATRES, LOW);

    ContRega = 0;
      
    } 
  }
}


//Tela Exibe Tempo de Rega
void telaRegando(){
  StatusTela2 = false;
  display.setCursor(0,0);
  if (StatusTempo == true)
    display.print("Regando.. Rega ");
  else if (StatusTempo == false)
    display.print("Regando.  Rega ");

  //Contagem de Tempo de Rega
  if (VezRega > 0) {   //Rega Automatica

    display.print(VezRega);
    display.setCursor(0,1);
    if (((TempoRega[(VezRega-1)]*60) - ContRega)>120) {
      display.print(((TempoRega[(VezRega-1)]*60) - ContRega)/60);
      display.print("m ");
    } else {
      display.print((TempoRega[(VezRega-1)]*60) - ContRega);
      display.print("s ");
    }
    if (ContRega >= (TempoRega[(VezRega-1)]*60))  {
      acionaRega(); 
      display.clear();
      display.setCursor(0,0);
      display.print("Rega concluida!");
      delay(800);
      display.clear();
    }

 
  } else {    //Rega Manual
     
    display.print("M");
    display.setCursor(0,1);
    if (((TempoRegaMan*60) - ContRega)>120) {
      display.print(((TempoRegaMan*60) - ContRega)/60);
      display.print("m ");
    } else {
      display.print((TempoRegaMan*60) - ContRega);
      display.print("s ");
    }
    if (ContRega >= (TempoRegaMan*60)){
      acionaRega();
      display.clear();
      display.setCursor(0,0);
      display.print("Rega concluida!");
      delay(800);
      display.clear();
    }

  }
} //Fecha Tela Tempo de Rega






/************************************************************* FUNCOES DE TELA */
/*    Funcoes de Tela    */

//Tela Inicial - Inicializacao - Hora e Data
void telaInicial (){   

/*  Printa Icones  */
//Printa Icone Ensolarado
  if (SolDia < 600){
   display.setCursor(15,1); 
   display.write(byte(0)); 
  }else {
   display.setCursor(15,1); 
   display.print(" ");
  }
  
//Printa Icone Automatico Ligado
  if (Automatico == true) {
    display.setCursor(15,0);
    display.write(byte(1));
  } else {
    display.setCursor(15,0);
    display.print(" ");
  }

//Printa Icone 
    if (ChuvaDia > 600) {
    display.setCursor(14,1);
    display.write(byte(2));   //Icone Chuva Forte
    } else {
    display.setCursor(14,1);  
    display.print(" ");   //Icone Vazio
    } //Fecha IF Chuva


/*  Tela 1  */
if (ContTempo < 30) {
StatusTela2 = false;
//Printa Vamos Regar
  display.setCursor(0,0);
  display.print("Vamos Regar?  ");

//Printa Hora  
  display.setCursor(0,1);
  display.print("Hora: ");
    if (myRTC.hours < 10){
      display.print("0");
      display.print(myRTC.hours);
    } else
      display.print(myRTC.hours);
      
  if (StatusTempo == true)
  display.print(":");
  else if (StatusTempo == false)
  display.print(" ");
  
    if (myRTC.minutes < 10){
     display.print("0");
      display.print(myRTC.minutes);
    } else
      display.print(myRTC.minutes); 
  display.print(" ");

} else if (ContTempo >= 30) {


/*  Tela 2  */
//Printa Data
  if (StatusTela2 == false){
    display.setCursor(0,0);
    switch (DiaSem) {
      case 1:
        display.print("Dom");
      break;
      case 2:
        display.print("Seg");
      break;
      case 3:
        display.print("Ter");
      break;
      case 4:
        display.print("Qua");
      break;
      case 5:
        display.print("Qui");
      break;
      case 6:
        display.print("Sex");
      break;
      case 7:
        display.print("Sab");
      break;
    }
    display.print(" ");
    if (myRTC.dayofmonth < 10){
      display.print("0");
      display.print(myRTC.dayofmonth);
    } else
      display.print(myRTC.dayofmonth);
  display.print("/");
      if (myRTC.month < 10){
      display.print("0");
      display.print(myRTC.month);
    } else
      display.print(myRTC.month);
  display.print("/");
  display.print(myRTC.year);
  display.print("  ");
  StatusTela2 = true;
  }

//Printa Hora
  display.setCursor(0,1);
  display.print("Hora: ");
    if (myRTC.hours < 10){
      display.print("0");
      display.print(myRTC.hours);
    } else
      display.print(myRTC.hours);

  if (StatusTempo == true)
  display.print(":");
  else if (StatusTempo == false)
  display.print(" ");

    if (myRTC.minutes < 10){
     display.print("0");
      display.print(myRTC.minutes);
    } else
      display.print(myRTC.minutes); 
  display.print(" ");
  
} //Finaliza Tela 2
} //Fecha Funcao Tela Inicial


/*    Telas de Configuracao */
//Liga e Desliga Modo Automatico
void telaConfiguraAutomatico(){   
  StatusTela2 = false;
  display.clear();
  display.setCursor(0,0);
  display.print("Modo Automatico"); 
  Automatico = !Automatico;
  if (Automatico == false){
    if (StatusRega == true)
      acionaRega();
    display.setCursor(0,1);
    display.print("Desligado!");
  }
  if (Automatico == true){
    display.setCursor(0,1);
    display.print("Ligado!");
  }
  delay(600);
  display.clear();  
}


//Liga e Desliga Display - Modo Soneca
void telaConfiguraSoneca (){       
  
  StatusTela2 = false;
  display.clear();

  if (StatusDisplay == true){  //Liga Modo Sonega - Desliga Display
    if (StatusRega == false)  {
      StatusDisplay = !StatusDisplay;
      ContRega = 0;
      display.setCursor(0,0);
      display.print("Modo Soneca..");
      delay(600);
      display.clear();
      display.noBacklight();
    } else  {
      Automatico = false;
      acionaRega();
    }
  } else { //Desliga Modo Soneca - Liga Display
    StatusDisplay = !StatusDisplay;
    display.backlight();
  }
} //Fecha Funcao Modo Soneca

/********************************************************* TELAS DE CANTEIROS */


//TELA - CANTEIRO TRES
void telaCanteiroTres(byte r) {
  r = e;
  comando.value = 0;        //Limpa variavel Comando
  controle.resume();  //Limpa controle
  display.clear();
  display.setCursor(0,0);
  display.print("CANTEIRO TRES: ");  
  display.setCursor(0,1);
  display.print("Regar? ");

  if (m == true) {

    if (CanteiroTresMan == true)
      display.print("SIM");
    else
      display.print("NAO");
    
  } else {
        
    if (CanteiroTres[r] == true)
      display.print("SIM");
    else
      display.print("NAO");
            
  } 

  while (comando.value != BTMENU) {  //loop do MENU
    if (controle.decode(&comando)){ //Aguarda comando
      Comando = (comando.value, HEX); //Armazena comando recebido
      Serial.println(comando.value, HEX); //Verificar tecla pressionada
      if (comando.value == BTMAIS) { //Avanca item menu
        display.setCursor(0,1);
        display.print("Regar? ");

        if (m == true) {

          CanteiroTresMan = !CanteiroTresMan;
          if (CanteiroTresMan == true)
            display.print("SIM");
          else
            display.print("NAO");
    
        } else {
        
          CanteiroTres[r] = !CanteiroTres[r];
          if (CanteiroTres[r] == true)
            display.print("SIM");
          else
            display.print("NAO");
            
        }

      } //FECHA BTMAIS
      
      if (comando.value == BTMENOS) { //Volta item menu
        display.setCursor(0,1);
        display.print("Regar? ");
        
        if (m == true) {

          CanteiroTresMan = !CanteiroTresMan;
          if (CanteiroTresMan == true)
            display.print("SIM");
          else
            display.print("NAO");
    
        } else {
        
          CanteiroTres[r] = !CanteiroTres[r];
          if (CanteiroTres[r] == true)
            display.print("SIM");
          else
            display.print("NAO");
            
        }
      } //FECHA BTMENOS
      
      if (comando.value == BTENTRA) { //Entra item menu
        display.setCursor(0,1);
        display.print("FEITO!    ");
        delay(600);
        M = 0;
        n = false;
        m = false;
        TelaVez = 1;
        return;
        //telaMenu();
      } //FECHA BTENTRA
      if (comando.value == BTVOLTA) { //Volta item menu
        TelaVez = 11;
        return;
        //telaCanteiroDois(r);
      }
    controle.resume(); //Limpa Controle
    } // ENCERRA ROTINA
  } //Fecha WHILE
  display.clear();
  if (m == true)
    m = false;
  TelaVez = 1;
  return;
} //FECHA FUNCAO



//TELA - CANTEIRO DOIS
void telaCanteiroDois(byte r) {
  r = e;
  comando.value = 0;        //Limpa variavel Comando
  controle.resume();  //Limpa controle
  display.clear();
  display.setCursor(0,0);
  display.print("CANTEIRO DOIS: "); 
  display.setCursor(0,1);
  display.print("Regar? ");

  if (m == true) {

    if (CanteiroDoisMan == true)
      display.print("SIM");
    else
      display.print("NAO");
    
  } else {
        
    if (CanteiroDois[r] == true)
      display.print("SIM");
    else
      display.print("NAO");
            
  } 

  while (comando.value != BTMENU) {  //loop do MENU
    if (controle.decode(&comando)){ //Aguarda comando
      Comando = (comando.value, HEX); //Armazena comando recebido
      Serial.println(comando.value, HEX); //Verificar tecla pressionada
      if (comando.value == BTMAIS) { //Avanca item menu
        display.setCursor(0,1);
        display.print("Regar? ");

        if (m == true) {

          CanteiroDoisMan = !CanteiroDoisMan;
          if (CanteiroDoisMan == true)
            display.print("SIM");
          else
            display.print("NAO");
    
        } else {
        
          CanteiroDois[r] = !CanteiroDois[r];
          if (CanteiroDois[r] == true)
            display.print("SIM");
          else
            display.print("NAO");
            
        } 

      } //FECHA BTMAIS
      if (comando.value == BTMENOS) { //Volta item menu
        display.setCursor(0,1);
        display.print("Regar? ");
        
        if (m == true) {

          CanteiroDoisMan = !CanteiroDoisMan;
          if (CanteiroDoisMan == true)
            display.print("SIM");
          else
            display.print("NAO");
    
        } else {
        
          CanteiroDois[r] = !CanteiroDois[r];
          if (CanteiroDois[r] == true)
            display.print("SIM");
          else
            display.print("NAO");
            
        }
      } //FECHA BTMENOS
      if (comando.value == BTENTRA) { //Entra item menu
        TelaVez = 12;
        return;
        //telaCanteiroTres(r);
      } //FECHA BTENTRA
      if (comando.value == BTVOLTA) { //Volta item menu
        TelaVez = 10;
        return;
        //telaCanteiroUm(r);
      }
    controle.resume(); //Limpa Controle
    } // ENCERRA ROTINA
  } //Fecha WHILE
  display.clear();
  TelaVez = 1;
  return;
} //FECHA FUNCAO


//TELA - CANTEIRO UM
void telaCanteiroUm(byte r) {
  r = e;
  comando.value = 0;        //Limpa variavel Comando
  controle.resume();  //Limpa controle
  display.clear();
  display.setCursor(0,0);
  display.print("CANTEIRO UM: ");  
  display.setCursor(0,1);
  display.print("Regar? ");

  if (m == true) {

  if (CanteiroUmMan == true)
    display.print("SIM");
  else
    display.print("NAO");
    
  } else {
  
  if (CanteiroUm[r] == true)
    display.print("SIM");
  else
    display.print("NAO");
    
  }
    
  while (comando.value != BTMENU) {  //loop do MENU
    if (controle.decode(&comando)){ //Aguarda comando
      Comando = (comando.value, HEX); //Armazena comando recebido
      Serial.println(comando.value, HEX); //Verificar tecla pressionada
      if (comando.value == BTMAIS) { //Avanca item menu
        display.setCursor(0,1);
        display.print("Regar? ");

        if (m == true) {

          CanteiroUmMan = !CanteiroUmMan;
          if (CanteiroUmMan == true)
            display.print("SIM");
          else
            display.print("NAO");
    
        } else {
        
          CanteiroUm[r] = !CanteiroUm[r];
          if (CanteiroUm[r] == true)
            display.print("SIM");
          else
            display.print("NAO");
            
        }  
        
      } //FECHA BTMAIS
      if (comando.value == BTMENOS) { //Volta item menu
        display.setCursor(0,1);
        display.print("Regar? ");

        if (m == true) {

          CanteiroUmMan = !CanteiroUmMan;
          if (CanteiroUmMan == true)
            display.print("SIM");
          else
            display.print("NAO");
    
        } else {
        
          CanteiroUm[r] = !CanteiroUm[r];
          if (CanteiroUm[r] == true)
            display.print("SIM");
          else
            display.print("NAO");
            
        } 
      } //FECHA BTMENOS
      if (comando.value == BTENTRA) { //Entra item menu
        TelaVez = 11;
        return;
        //telaCanteiroDois(r);
      } //FECHA BTENTRA
      if (comando.value == BTVOLTA) { //Volta item menu
        if (m == true)
          TelaVez = 7;
        else
          TelaVez = 9;
        return;
        //telaTempoRega(r);
      }
    controle.resume(); //Limpa Controle
    } // ENCERRA ROTINA
  } //Fecha WHILE
  display.clear();
  TelaVez = 1;
  return;
} //FECHA FUNCAO



//TELA - CANTEIRO UM
void telaChuva(byte r) {
  r = e;
  comando.value = 0;        //Limpa variavel Comando
  controle.resume();  //Limpa controle
  display.clear();
  display.setCursor(0,0);
  display.print("C Chuva - Rega "); 
  display.print(r+1);
  display.setCursor(0,1);
  display.print("Regar? ");

  if (Chuva[r] == true)
    display.print("SIM");
  else
    display.print("NAO");
    
  while (comando.value != BTMENU) {  //loop do MENU
    if (controle.decode(&comando)){ //Aguarda comando
      Comando = (comando.value, HEX); //Armazena comando recebido
      Serial.println(comando.value, HEX); //Verificar tecla pressionada
      if (comando.value == BTMAIS) { //Avanca item menu
        display.setCursor(0,1);
        display.print("Regar? ");
        Chuva[r] = !Chuva[r];
        if (Chuva[r] == true)
          display.print("SIM");
        else
          display.print("NAO");  
      } //FECHA BTMAIS
      if (comando.value == BTMENOS) { //Volta item menu
        display.setCursor(0,1);
        display.print("Regar? ");
        Chuva[r] = !Chuva[r];
        if (Chuva[r] == true)
          display.print("SIM");
        else
          display.print("NAO");
      } //FECHA BTMENOS
      if (comando.value == BTENTRA) { //Entra item menu
        TelaVez = 10;
        return;
        //telaCanteiroDois(r);
      } //FECHA BTENTRA
      if (comando.value == BTVOLTA) { //Volta item menu
        TelaVez = 8;
        return;
        //telaTempoRega(r);
      }
    controle.resume(); //Limpa Controle
    } // ENCERRA ROTINA
  } //Fecha WHILE
  display.clear();
  TelaVez = 1;
  return;
} //FECHA FUNCAO


//TELA - CRITERIO SOL
void telaSol(byte r) {
  r = e;
  comando.value = 0;        //Limpa variavel Comando
  controle.resume();  //Limpa controle
  display.clear();
  display.setCursor(0,0);
  display.print("Com Sol - Rega "); 
  display.print(r+1);
  display.setCursor(0,1);
  display.print("Regar? ");

  if (Sol[r] == true)
    display.print("NAO");
  else
    display.print("SIM");
    
  while (comando.value != BTMENU) {  //loop do MENU
    if (controle.decode(&comando)){ //Aguarda comando
      Comando = (comando.value, HEX); //Armazena comando recebido
      Serial.println(comando.value, HEX); //Verificar tecla pressionada
      if (comando.value == BTMAIS) { //Avanca item menu
        display.setCursor(0,1);
        display.print("Regar? ");
        Sol[r] = !Sol[r];
        if (Sol[r] == true)
          display.print("NAO");
        else
          display.print("SIM");  
      } //FECHA BTMAIS
      if (comando.value == BTMENOS) { //Volta item menu
        display.setCursor(0,1);
        display.print("Regar? ");
        Sol[r] = !Sol[r];
        if (Sol[r] == true)
          display.print("NAO");
        else
          display.print("SIM"); 
      } //FECHA BTMENOS
      if (comando.value == BTENTRA) { //Entra item menu
        TelaVez = 9;
        return;
        //telaCanteiroDois(r);
      } //FECHA BTENTRA
      if (comando.value == BTVOLTA) { //Volta item menu
        TelaVez = 7;
        return;
        //telaTempoRega(r);
      }
    controle.resume(); //Limpa Controle
    } // ENCERRA ROTINA
  } //Fecha WHILE
  display.clear();
  TelaVez = 1;
  return;
} //FECHA FUNCAO



//TELA - EDITA REGA - TEMPO REGAR (TEMPO)
void telaTempoRega (byte r) {

  comando.value = 0;        //Limpa variavel Comando
  controle.resume();  //Limpa controle
  display.clear();
  if (m == true) {
    display.setCursor(0,0);
    display.print("Rega Manual"); 
    display.setCursor(0,1);
    display.print("Tempo: ");
    display.print(TempoRegaMan);
    display.print(" min ");
  } else {
    r = e;
    display.setCursor(0,0);
    display.print("Tempo Rega ");
    display.print(r+1);
    display.setCursor(0,1);
    display.print("Tempo: ");
    display.print(TempoRega[r]);
    display.print(" min ");
  }
  while (comando.value != BTMENU) {  //loop do MENUr = 6;
    if (controle.decode(&comando)){ //Aguarda comando
      Comando = (comando.value, HEX); //Armazena comando recebido
      Serial.println(comando.value, HEX); //Verificar tecla pressionada
    if (comando.value == BTMAIS) { //Avanca item menu

      if (m == true) {

        TempoRegaMan = ++TempoRegaMan;
        display.setCursor(0,1);
        display.print("Tempo: ");
        display.print(TempoRegaMan);
        display.print(" min  ");
        
      } else {
      
      TempoRega[r] = ++TempoRega[r];
      display.setCursor(0,1);
      display.print("Tempo: ");
      display.print(TempoRega[r]);
      display.print(" min  ");
      }
    }
    if (comando.value == BTMENOS) { //Volta item menu
    
      if (m == true) {

        TempoRegaMan = --TempoRegaMan;
        if (TempoRegaMan < 0)
          TempoRegaMan = 0;
        display.setCursor(0,1);
        display.print("Tempo: ");
        display.print(TempoRegaMan);
        display.print(" min  ");
        
      } else {
            
      TempoRega[r] = --TempoRega[r];
      if (TempoRega[r] < 0)
        TempoRega[r] = 0;
      display.setCursor(0,1);
      display.print("Tempo: ");
      display.print(TempoRega[r]);
      display.print(" min  ");
      }
    }
      if (comando.value == BTENTRA) { //Entra item menu
        display.clear();
        if (m == true)
          TelaVez = 10;
        else
          TelaVez = 8;
        return;
        //telaCanteiroUm(r);  
      } //Fecha BTENTRA
      if (comando.value == BTVOLTA) { //Volta item menu
        if (m == true){
          m = false;
          TelaVez = 2;
        } else {
          TelaVez = 6;
        return;
        }
        //telaMinutoRega(r);
      }
    controle.resume(); //Limpa Controle
    } //
  } //Fecha WHILE
  display.clear();
  if (m == true)
    m = false;
  TelaVez = 1;
  return;
}


//TELA - EDITA REGA - MINUTO REGAR (MINUTO)
void telaMinutoRega (byte r) {
  r = e;
  comando.value = 0;        //Limpa variavel Comando
  controle.resume();  //Limpa controle
  display.clear();
  display.setCursor(0,0);
  display.print("Min Rega ");
  display.print(r+1);
  display.setCursor(0,1);
  display.print("Minuto: ");
  if (HoraRega[r] < 10) {
    display.print("0");
    display.print(HoraRega[r]);
  } else
    display.print(HoraRega[r]);
  display.print(":");
  if (MinutoRega[r] < 10){
    display.print("0");
    display.print(MinutoRega[r]);
  } else
    display.print(MinutoRega[r]);

  while (comando.value != BTMENU) {  //loop do MENU
    if (controle.decode(&comando)){ //Aguarda comando
      Comando = (comando.value, HEX); //Armazena comando recebido
      Serial.println(comando.value, HEX); //Verificar tecla pressionada
    if (comando.value == BTMAIS) { //Avanca item menu
        MinutoRega[r] = ++MinutoRega[r];
        if (MinutoRega[r] > 59) 
          MinutoRega[r] = 0;
        display.setCursor(0,1);
        display.print("Minuto: ");
        if (HoraRega[r] < 10) {
          display.print("0");
          display.print(HoraRega[r]);
        } else
          display.print(HoraRega[r]);
        display.print(":");
        if (MinutoRega[r] < 10){
          display.print("0");
          display.print(MinutoRega[r]);
        } else
          display.print(MinutoRega[r]);
    }
    if (comando.value == BTMENOS) { //Volta item menu
        MinutoRega[r] = --MinutoRega[r];
        if (MinutoRega[r] < 0) 
          MinutoRega[r] = 59;
        display.setCursor(0,1);
        display.print("Minuto: ");
        if (HoraRega[r] < 10) {
          display.print("0");
          display.print(HoraRega[r]);
        } else
          display.print(HoraRega[r]);
        display.print(":");
        if (MinutoRega[r] < 10){
          display.print("0");
          display.print(MinutoRega[r]);
        } else
          display.print(MinutoRega[r]);
    }
      if (comando.value == BTENTRA) { //Entra item menu
        display.clear();
        TelaVez = 7;
        return;
        //telaTempoRega(r);  
      } //Fecha BTENTRA
      if (comando.value == BTVOLTA) { //Volta item menu
        TelaVez = 5;
        return;
        //telaHoraRega(r);
      }
    controle.resume(); //Limpa Controle
    } //
  } //Fecha WHILE
  display.clear();
  TelaVez = 1;
  return;
}


//TELA - EDITA REGA - HORA REGAR (HORA)
void telaHoraRega (byte r) {
  r = e;
  comando.value = 0;        //Limpa variavel Comando
  controle.resume();  //Limpa controle
  display.clear();
  display.setCursor(0,0);
  display.print("Hora Rega ");
  display.print(r+1);
  display.setCursor(0,1);
  display.print("Hora: ");
  if (HoraRega[r] < 10) {
    display.print("0");
    display.print(HoraRega[r]);
  } else
    display.print(HoraRega[r]);
  display.print(":");
  if (MinutoRega[r] < 10){
    display.print("0");
    display.print(MinutoRega[r]);
  } else
    display.print(MinutoRega[r]);

  while (comando.value != BTMENU) {  //loop do MENU
    if (controle.decode(&comando)){ //Aguarda comando
      Comando = (comando.value, HEX); //Armazena comando recebido
      Serial.println(comando.value, HEX); //Verificar tecla pressionada
    if (comando.value == BTMAIS) { //Avanca item menu
        HoraRega[r] = ++HoraRega[r];
        if (HoraRega[r] > 24) 
          HoraRega[r] = 0;
        display.setCursor(0,1);
        display.print("Hora: ");
        if (HoraRega[r] < 10) {
          display.print("0");
          display.print(HoraRega[r]);
        } else
          display.print(HoraRega[r]);
        display.print(":");
        if (MinutoRega[r] < 10){
          display.print("0");
          display.print(MinutoRega[r]);
        } else
          display.print(MinutoRega[r]);
    }
    if (comando.value == BTMENOS) { //Volta item menu
        HoraRega[r] = --HoraRega[r];
        if (HoraRega[r] < 1) 
          HoraRega[r] = 24;
        display.setCursor(0,1);
        display.print("Hora: ");
        if (HoraRega[r] < 10) {
          display.print("0");
          display.print(HoraRega[r]);
        } else
          display.print(HoraRega[r]);
        display.print(":");
        if (MinutoRega[r] < 10){
          display.print("0");
          display.print(MinutoRega[r]);
        } else
          display.print(MinutoRega[r]);
    }
      if (comando.value == BTENTRA) { //Entra item menu
        display.clear();
        TelaVez = 6;
        return;
        //telaMinutoRega(r);  
      } //Fecha BTENTRA
      if (comando.value == BTVOLTA) { //Volta item menu
        TelaVez = 4;
        return;
        //telaQuandoRega(r);
      }
    controle.resume(); //Limpa Controle
    } //
  } //Fecha WHILE
  display.clear();
  TelaVez = 1;
  return;
}


//TELA - EDITA REGA - QUANDO REGAR (DIA)
void telaQuandoRega (byte r) {
  r = e;
  comando.value = 0;        //Limpa variavel Comando
  controle.resume();  //Limpa controle
  display.clear();
  display.setCursor(0,0);
  display.print("Quando? Rega - ");
  display.print(r+1);
  display.setCursor(0,1);
  display.print(DiaRega[IndiceDia[r]]);

  while (comando.value != BTMENU) {  //loop do MENU
    if (controle.decode(&comando)){ //Aguarda comando
      Comando = (comando.value, HEX); //Armazena comando recebido
      Serial.println(comando.value, HEX); //Verificar tecla pressionada
      if (comando.value == BTMAIS) { //Avanca item menu
        IndiceDia[r] = ++IndiceDia[r];
        if (IndiceDia[r] > 2)
          IndiceDia[r] = 0;
      display.setCursor(0,1);
      display.print(DiaRega[IndiceDia[r]]);
      }
      if (comando.value == BTMENOS) { //Volta item menu
        IndiceDia[r] = --IndiceDia[r];
        if (IndiceDia[r] < 0)
          IndiceDia[r] = 2;
      display.setCursor(0,1);
      display.print(DiaRega[IndiceDia[r]]);
      }
      if (comando.value == BTENTRA) { //Entra item menu
        display.clear();
        TelaVez = 5;
        return;
        //telaHoraRega(r);  
      } //Fecha BTENTRA
      if (comando.value == BTVOLTA) { //Volta item menu
    if (n == true) {
          n = false;
          R = --R;
          TelaVez = 1;
          return;
          //telaMenu();  
        } else
          TelaVez = 3;
          return;
          //telaEscolheRega();
      }
    controle.resume(); //Limpa Controle
    } //
  } //Fecha WHILE
  display.clear();
  if (n == true) {
    n = false;
    R = --R;
    TelaVez = 1;
    return;
  }
  TelaVez = 1;
  return;
}



//Decide qual rega editar
void telaEscolheRega () {
  comando.value = 0;        //Limpa variavel Comando
  controle.resume();  //Limpa controle
  r = 0;
  display.clear();
  display.setCursor(0,0);
  display.print("Editar Rega: ");
  display.setCursor(0,1);
  display.print("Rega: ");
  display.print(r+1);
  display.print(" (");
  
  if (HoraRega[r] < 10) {
    display.print("0");
    display.print(HoraRega[r]);
  } else
    display.print(HoraRega[r]);
  display.print(":");
  if (MinutoRega[r] < 10){
    display.print("0");
    display.print(MinutoRega[r]);
  } else
    display.print(MinutoRega[r]);
  
  display.print(")");
  
  while (comando.value != BTMENU) {  //loop do MENU
    if (controle.decode(&comando)){ //Aguarda comando
      Comando = (comando.value, HEX); //Armazena comando recebido
      Serial.println(comando.value, HEX); //Verificar tecla pressionada
      if (comando.value == BTMAIS) { //Avanca item menu
        r = ++r;
        if (r > R)
          r = 0;
        display.setCursor(0,1);
        display.print("Rega: ");
        display.print(r+1);
        display.print(" (");
        if (HoraRega[r] < 10) {
        display.print("0");
        display.print(HoraRega[r]);
      } else
        display.print(HoraRega[r]);
      display.print(":");
      if (MinutoRega[r] < 10){
        display.print("0");
        display.print(MinutoRega[r]);
      } else
        display.print(MinutoRega[r]);
        display.print(")");
      }
      if (comando.value == BTMENOS) { //Volta item menu
        r = --r;
        if (r < 0)
          r = R;
        display.setCursor(0,1);
        display.print("Rega: ");
        display.print(r+1);
        display.print(" (");
        if (HoraRega[r] < 10) {
        display.print("0");
        display.print(HoraRega[r]);
      } else
        display.print(HoraRega[r]);
      display.print(":");
      if (MinutoRega[r] < 10){
        display.print("0");
        display.print(MinutoRega[r]);
      } else
        display.print(MinutoRega[r]);
        display.print(")");
      }
      if (comando.value == BTENTRA) { //Entra item menu
        display.clear();
        e = r; 
        TelaVez = 4;
        return;
        //telaQuandoRega(r);  
        

      } //Fecha BTENTRA
      if (comando.value == BTVOLTA) { //Volta item menu
        TelaVez = 2;
        return;
        //telaManAut();
      }
    controle.resume(); //Limpa Controle
    } //
  } //Fecha WHILE
  display.clear();
  TelaVez = 1;
  return;
}


//Tela - Decide Automatico ou Manual
void telaManAut () {
  comando.value = 0;
  controle.resume();  //Limpa controle
  display.clear();
  display.setCursor(0,0);
  display.print("Editar Rega:"); 
  display.setCursor(0,1);
  if (m == true)
    display.print("<    MANUAL    >");
  else
    display.print("<  AUTOMATICA  >");
  while (comando.value != BTMENU) {  //loop do MENU
    if (controle.decode(&comando)){ //Aguarda comando
      Comando = (comando.value, HEX); //Armazena comando recebido
      Serial.println(comando.value, HEX); //Verificar tecla pressionada
      if (comando.value == BTMAIS) { //Avanca item menu
        display.setCursor(0,1);
        m = !m;
        if (m == true)
          display.print("<    MANUAL    >");
        else
          display.print("<  AUTOMATICA  >");
      }
      if (comando.value == BTMENOS) { //Volta item menu
        display.setCursor(0,1);
        m = !m;
        if (m == false)
          display.print("<  AUTOMATICA  >");
        else
          display.print("<    MANUAL    >");
      }
      if (comando.value == BTENTRA) { //Entra item menu
        display.clear();
        if (m == true){
          TelaVez = 7;  
          return;
        } else {
          if (R == -1) {
            M = 0;
            display.setCursor(0,0);
            display.print("NENHUMA REGA");
            delay(600);
            TelaVez = 1;
            return;
            //telaMenu();
          } else
            TelaVez = 3;
            return;
        } 
      } //Fecha BTENTRA
      if (comando.value == BTVOLTA) { //Volta item menu
        TelaVez = 1;
        //telaMenu();
      }
    controle.resume(); //Limpa Controle
    } //
  } //Fecha WHILE
  display.clear();
  TelaVez = 1;
  return;
}




//Funcao Tela do Menu
void telaMenu(){
  comando.value = 0;        //Limpa variavel Comando
  controle.resume();  //Limpa controle
  display.setCursor(0,0);
  display.print("      MENU      ");
  display.setCursor(0,1);
  display.print("<");
  display.print(Menu[M]);
  display.setCursor(15,1);
  display.print(">");
  while (comando.value != BTMENU) {  //loop do MENU
    if (controle.decode(&comando)){ //Aguarda comando
      Comando = (comando.value, HEX); //Armazena comando recebido
      Serial.println(comando.value, HEX); //Verificar tecla pressionada
      if (comando.value == BTMAIS) { //Avanca item menu
          M = ++M;
          if (M > 1)
            M = 0;
       display.setCursor(0,1);
       display.print("<");
       display.print(Menu[M]);
       display.setCursor(15,1);
       display.print(">");
      }
      if (comando.value == BTMENOS) { //Volta item menu
        M = --M;
        if (M < 0)
          M = 1;
        display.setCursor(0,1);
        display.print("<");
        display.print(Menu[M]);
        display.setCursor(15,1);
        display.print(">");
      }

      if (comando.value == BTENTRA) { //Entra item menu
        display.clear();
        switch (M) {  
          case 0:
            R = ++R;
            n = true;
            TelaVez = 4;
            e = R;
            return;
            //telaQuandoRega(R);
          break;
          case 1:
            m = true;
            TelaVez = 2;
            return;
            //telaManAut();
          break;
          case 2:

          break;       
          case 3:

          break;         
        } 
      } //Fecha BTENTRA
      if (comando.value == BTVOLTA) { //Volta item menu
        TelaVez = 0;
        return;
      }
    controle.resume(); //Limpa Controle
    } //
  } //Fecha WHILE
  display.clear();
  TelaVez = 0;
  //return;
} //Fecha Funcao 






/************************************************************* FUNCAO INICIALIZACAO - SETUP */
//Funcao de Inicializacao do Sistema - SETUP
void setup() {
//Configuracoes dos Modos dos Pinos
  //pinMode(SOL, INPUT);  //Nao necessario em entradas analogicas
  pinMode(CONTROLE, INPUT);
  
  pinMode(VALVULAUM, OUTPUT);
  digitalWrite(VALVULAUM, LOW);
  pinMode(VALVULADOIS, OUTPUT);
  digitalWrite(VALVULADOIS, LOW);
  pinMode(VALVULATRES, OUTPUT);
  digitalWrite(VALVULATRES, LOW);
  
  //Configuracoes do Displaybool Automatico = false;    //Liga e Desliga Modo Automatico
  display.init(); //Inicia comunicação com o Display
  display.backlight(); //Ascende a tela do Display
  display.createChar(0,SolLogo);    //Cria Char Sol
  display.createChar(1,AutomaticoLogo); //Cria Char Automatico
  display.createChar(2,ChuvaLogo);    //Cria Char Chuva
  display.clear();            //Limpa Display
  display.setCursor(0,0);       //Posiciona Cursor
  
  //Inicia o Controle Remoto
  controle.enableIRIn();

  //Inicia Sensor Umidade e Temperatura
  //dht.begin();

  //Configurar Relogio - Tirar comentario da linha abaixo para setar data-hora
  //Segundo-Minuto-Hora-DiaSemana-DiaMes-Mes-Ano
  //myRTC.setDS1302Time(00, 27, 03, 2, 25, 04, 2022); //Define data-hora

  //Inicia Timer
  Timer1.initialize(1000000);
  Timer1.attachInterrupt(verificaTempo);

  //Inicia Monitor Serial
  Serial.begin(9600);


}

/************************************************************* FUNCOES PRINCIPAL - LOOP */
//Funcao principal do sistema - LOOP
void loop() {

/* Leituras e Sensores */
  myRTC.updateTime();   //Faz leitura do Modulo Relogio
  SolDia = analogRead(SOL);  //Faz a leitura da Luz
  ChuvaDia = analogRead(CHUVA); //Faz a leitura da chuva
  DiaSem = myRTC.dayofweek;
  //u = dht.readHumidity();       // Leitura da Umidade
  //t = dht.readTemperature();    //Leitura da Temperatura em graus celsius 

    
if (StatusDisplay == true && StatusRega == false){
  switch (TelaVez) {  
    case 0:
      telaInicial();
    break;
    case 1:
      telaMenu();
    break;
    case 2:
      telaManAut ();  
    break;       
    case 3:
      telaEscolheRega ();
    break; 
    case 4:
      telaQuandoRega(r);
    break;  
    case 5:
      telaHoraRega (r);
    break; 
    case 6:
      telaMinutoRega (r);
    break;
    case 7:
      telaTempoRega (r);
    break;
    case 8:
      telaSol (r);  
    break;   
    case 9:
      telaChuva (r);
    break;  
    case 10:
      telaCanteiroUm (r);
    break;    
    case 11:
      telaCanteiroDois (r);
    break;  
    case 12:
      telaCanteiroTres (r);
    break; 
  }
}

/* Comandos do Controle Remoto */  
  //Inicia Controle Remoto
  if (controle.decode(&comando)){ //Aguarda comando
    Comando = (comando.value, HEX); //Armazena comando recebido
      
    if (comando.value == BTAUTOM && StatusDisplay == true) //Liga e Desliga Modo Automatico
      telaConfiguraAutomatico();

    if (comando.value == BTDESLIGA)              //Liga Modo Soneca - Desliga Display
     telaConfiguraSoneca();   

    if (comando.value == BTMENU){              //Chama Menu 
     M = 0;
     TelaVez = 1;
     //telaMenu(); 
    }

    //Aciona Rega Manual - Botao SEIS
    if (comando.value == BTMANUAL && StatusDisplay == true){    //Inicia Rega Manual
      if (StatusRega == false)
        VezRega = 0;
      acionaRega();
      
    }
    //Aciona Manualmente Regas Automaticas Cadastradas - Botao de UM a CINCO
    if (comando.value == BTUM && StatusDisplay == true){    //Inicia Rega Manual
      if (StatusRega == true){
        if (VezRega == 1)
          acionaRega();
        else {
          display.clear();
          display.setCursor(0,0);
          display.print("REGA ACIONADA: ");
            if (VezRega == 0)
              display.print("M");
            else
              display.print(VezRega);
          delay(1000);
          display.clear();
        }   
      } else {
        if (R < 0) {
          display.clear();
          display.setCursor(0,0);
          display.print("SEM REGA CRIADA");
          delay(600);
          TelaVez = 0;
        display.clear();
        } else {
          VezRega = 1;
          acionaRega();
        }
      }   
    }
    
    //Aciona Manualmente Regas Automaticas Cadastradas - Botao de UM a CINCO
    if (comando.value == BTDOIS && StatusDisplay == true){    //Inicia Rega Manual
      if (StatusRega == true){
        if (VezRega == 2)
          acionaRega();
        else {
          display.clear();
          display.setCursor(0,0);
          display.print("REGA ACIONADA: ");
            if (VezRega == 0)
              display.print("M");
            else
              display.print(VezRega);
          delay(1000);
          display.clear();
        }   
      } else {
        if (R < 1) {
          display.clear();
          display.setCursor(0,0);
          display.print("SEM REGA CRIADA");
          delay(600);
          TelaVez = 0;
        display.clear();
        } else {
          VezRega = 2;
          acionaRega();
        }
      }   
    }
    //Aciona Manualmente Regas Automaticas Cadastradas - Botao de UM a CINCO
    if (comando.value == BTTRES && StatusDisplay == true){    //Inicia Rega Manual
      if (StatusRega == true){
        if (VezRega == 3)
          acionaRega();
        else {
          display.clear();
          display.setCursor(0,0);
          display.print("REGA ACIONADA: ");
            if (VezRega == 0)
              display.print("M");
            else
              display.print(VezRega);
          delay(1000);
          display.clear();
        }   
      } else {
        if (R < 2) {
          display.clear();
          display.setCursor(0,0);
          display.print("SEM REGA CRIADA");
          delay(600);
          TelaVez = 0;
        display.clear();
        } else {
          VezRega = 3;
          acionaRega();
        }
      }  
    }
    //Aciona Manualmente Regas Automaticas Cadastradas - Botao de UM a CINCO
    if (comando.value == BTQUATRO && StatusDisplay == true){    //Inicia Rega Manual
      if (StatusRega == true){
        if (VezRega == 4)
          acionaRega();
        else {
          display.clear();
          display.setCursor(0,0);
          display.print("REGA ACIONADA: ");
            if (VezRega == 0)
              display.print("M");
            else
              display.print(VezRega);
          delay(1000);
          display.clear();
        }   
      } else {
        if (R < 3) {
          display.clear();
          display.setCursor(0,0);
          display.print("SEM REGA CRIADA");
          delay(600);
          TelaVez = 0;
        display.clear();
        } else {
          VezRega = 4;
          acionaRega();
        }
      } 
    }
    //Aciona Manualmente Regas Automaticas Cadastradas - Botao de UM a CINCO
    if (comando.value == BTCINCO && StatusDisplay == true){    //Inicia Rega Manual
      if (StatusRega == true){
        if (VezRega == 5)
          acionaRega();
        else {
          display.clear();
          display.setCursor(0,0);
          display.print("REGA ACIONADA: ");
            if (VezRega == 0)
              display.print("M");
            else
              display.print(VezRega);
          delay(1000);
          display.clear();
        }   
      } else {
        if (R < 4) {
          display.clear();
          display.setCursor(0,0);
          display.print("SEM REGA CRIADA");
          delay(600);
          TelaVez = 0;
        display.clear();
        } else {
          VezRega = 5;
          acionaRega();
        }
      } 
    }

    
    //Encerra Comandos
    controle.resume(); //Encerra o Controle
  } //Encerra Controle Remoto


/*  Verifica Rega Automatica  */
if (Automatico == true && StatusRega == false)
  verificaRegaAutomatica();

/*  Exibe Tempo de Rega  */
  if (StatusRega == true)
    telaRegando();

} // Encerra LOOP

/********************************************************************** FIM DO PROGRAMA */
