/*
  
 Tentive de déterminer la résistance interne des accus courants
 Copyright  René Lefebvre  dit "Kotec le Râleur"

*/

#include "Common.h"

// #define  DEBUG_ON           1    // mettre en commentaire pour supprimer le debug sur RX
#define  RELAIS_NIMH       	2
#define  RELAIS_LION        3
#define  INP_NIMH     	    A0
#define  INP_LION      	    A1
#define  RELAIS_NIMH_ON    	digitalWrite(RELAIS_LION,LOW);digitalWrite(RELAIS_NIMH,HIGH) 
#define  RELAIS_NIMH_OFF   	digitalWrite(RELAIS_NIMH,LOW) 
#define  RELAIS_LION_ON     digitalWrite(RELAIS_NIMH,LOW);digitalWrite(RELAIS_LION,HIGH) 
#define  RELAIS_LION_OFF    digitalWrite(RELAIS_LION,LOW) 

#define  READ_V_NIMH    	analogRead(INP_NIMH)
#define  READ_V_LION        analogRead(INP_LION)
#define  RESISTANCE   	    7.57
#define  NB_ACQ       	    16
#define  NB_CALC       	    6
#define  TIME_INTER_ACQ     15   /* milisecondes */
#define  COEF_MUL           1.00
#define  T_MINIMUM          300
#define  MAXNIMH            2152    // ajusté en cherchant la tension qui donne 1023 sur le ADC
#define  MAXLION            4578

float Rint              = 0.0;
unsigned long V_NO_Load = 0L;        // value lue sans charge
unsigned long V_ON_Load = 0L;        // value read lue charge
unsigned long Tbat      = 0L;

enum Status {S_RIEN, S_NIMH, S_LION, S_2PILES };

//----------------------------------------------------
//-----  Prototypes des fonctions  externes  ---------
//----------------------------------------------------
extern void InitDisplay(char *);
extern void Message (int , int , char*,int,int,int);
extern void ClearDisplay (void);
extern void bbox(int a,int b ,int c, int d);


//-------------------------------------------------------------------
//-----   Routine de lecture sur les entrées analogiques    ---------
//    on fait la somme de NB_ACQ mesures pour lisser les résultats
//-------------------------------------------------------------------
unsigned long Acquisition(int canal)
{
  unsigned long Vtmp = 0L;
  for (int x=0; x<NB_ACQ;  x++)
	{   
		delay(TIME_INTER_ACQ);
		Vtmp += analogRead(canal);
	} 
	return(Vtmp);
}


//-------------------------------------------------------------------
//--- Routine pour savoir s'il y a une ou deux piles et leur type ---
//-------------------------------------------------------------------
int GetStatus (void)
{
int etat = S_RIEN;
int TN,TL;
  RELAIS_LION_OFF;
  RELAIS_NIMH_OFF;
  delay(200); 
  TN = READ_V_NIMH;
  TL = READ_V_LION;
 
  if ((TN < T_MINIMUM) && (TL < T_MINIMUM))  etat = S_RIEN;
  if ((TN > T_MINIMUM) && (TL < T_MINIMUM))  etat = S_NIMH;
  if ((TN < T_MINIMUM) && (TL > T_MINIMUM))  etat = S_LION;
  if ((TN > T_MINIMUM) && (TL > T_MINIMUM))  etat = S_2PILES;
  
#ifdef DEBUG_ON  
  Serial.print(" NIMH=");  Serial.print (TN); 
  Serial.print(" LION=");  Serial.print (TL); 
  Serial.print(" etat=");  Serial.println (etat); 
#endif  
  return (etat);
}

//-------------------------------------------------------------------
//-----                 Routine  principale                 ---------
//-------------------------------------------------------------------
void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  pinMode(RELAIS_NIMH, OUTPUT);
  RELAIS_NIMH_OFF;
  pinMode(RELAIS_LION, OUTPUT);
  RELAIS_LION_OFF;

  InitDisplay("R Interne");
// on utilise la référence interne de 1.1V sur ATMega328
  analogReference(INTERNAL);
  delay(200); 
}

//-------------------------------------------------------------------
//-----                 Routine  en boucle                  ---------
//-----  Test de la présence d'une pile et de son type
//-------------------------------------------------------------------
void loop() {

long int Tvide, Tch;
long int TNimh, TLion;
int      Status = S_RIEN;
char     buff[15];
float    f_tvide = 0.0;
float    f_tch   = 0.0;
  
// initialisation	
	Rint      = 0.0;
	V_NO_Load = 0L;        // tension  sans charge
	V_ON_Load = 0L;        // tension  avec charge
  
   Status = GetStatus();
   switch (Status)
   {
//-------------------------------------------------------------------
   case S_RIEN:
     InitDisplay("* Attente *");
     Message (4,50,"Pas de batterie",FUCHSIA);
	 delay(1000);
     bbox(3,38,120,86);  // effacement de l'ecran
   break;
//-------------------------------------------------------------------
   case S_2PILES:
     InitDisplay("* Attente *");
     Message (4,50,"2 batteries !!",RED);
	 delay(1000);
     bbox(3,38,120,86);  // effacement de l'ecran
   break;
//-------------------------------------------------------------------
   case S_NIMH:
     InitDisplay(" *Ni_MH*");  // detection d'une pile ou d'un accus Cd-Ni ou Ni-Mh

      Message (4,50,"Batterie Ni-MH",GREEN);
      delay(500);
      bbox(3,38,120,18);
	  Rint    = 0.0; 
	  Tvide   = 0L;
      Tch     = 0L;
      f_tvide = 0.0;
      f_tch   = 0.0;

// On fait la moyenne sur NB_CALC mesures 
  	 for (int ix=0; ix<NB_CALC; ix++) 
  	{ 
  		RELAIS_NIMH_OFF; RELAIS_LION_OFF;
  		delay(200);  
  		V_NO_Load =  Acquisition(INP_NIMH);  // lecture de la tension à vide
  		RELAIS_NIMH_ON;
  		delay(200);  
  		V_ON_Load =  Acquisition(INP_NIMH);  // lecture de la tension en charge sur 7.5 Ohms
  		Tvide += map(V_NO_Load,0,1023*NB_ACQ,0,MAXNIMH);   // mise à l'echelle en Volt
  		Tch   += map(V_ON_Load,0,1023*NB_ACQ,0,MAXNIMH);
  		Rint  += RESISTANCE * (((V_NO_Load - V_ON_Load) * COEF_MUL)  /  V_ON_Load); 
   	}	 
  	Tvide = Tvide / NB_CALC;  f_tvide = (float)Tvide/1000.00;
  	Tch   = Tch   / NB_CALC;  f_tch   = (float)Tch/1000.00;
  	Rint  = Rint  / (float)NB_CALC;
#ifdef DEBUG_ON	  
      Serial.print("T vide = ");  Serial.print(V_NO_Load); Serial.print("  T ch = ");  Serial.print(V_ON_Load); 
      Serial.print(" Ri = ");     Serial.println (Rint,4); 
#endif	  
  
    Message (4,50,  "T vide :",GREEN);
  //  itoa(Tvide,buff,10);
    dtostrf(f_tvide,5, 3, buff);  //4 is mininum width, 6 is precision
    Message (70,50,buff ,WHITE);
    
    Message (4,75,  "T chg  : ",YELLOW);
 //   itoa(Tch,buff,10);
    dtostrf(f_tch,5, 3, buff);  //4 is mininum width, 6 is precision
    Message (70,75,buff ,WHITE);
  
    Message (4,100, "R int : ",RED);
    dtostrf(Rint/COEF_MUL,5, 3, buff);  //4 is mininum width, 6 is precision
    Message (70,100,buff,RED);
  	// bouclage tant que la batterie reste en place   
  	while (GetStatus() == S_NIMH) delay (100);

   break;
//-------------------------------------------------------------------
   case S_LION:
     InitDisplay(" * Li-Ion *");
     delay(700);
     bbox(3,38,120,18);
     Rint = 0.0; 
     Tvide = 0L;
     Tch   = 0L;
     f_tvide = 0.0;
     f_tch   = 0.0;

// On fait la moyenne sur NB_CALC mesures 
     for (int ix=0; ix<NB_CALC; ix++) 
    { 
      RELAIS_NIMH_OFF; RELAIS_LION_OFF;
      delay(200);  
      V_NO_Load =  Acquisition(INP_LION);  // lecture de la tension à vide de la batterie Li-Ion
      
      RELAIS_LION_ON;
      delay(200);  
      V_ON_Load =  Acquisition(INP_LION);  // lecture de la tension en charge sur 7.5 Ohms
      Tvide += map(V_NO_Load,0,1023*NB_ACQ,0,MAXLION);   // mise à l'echelle en Volt
      Tch   += map(V_ON_Load,0,1023*NB_ACQ,0,MAXLION);
      Rint  += RESISTANCE * (((V_NO_Load - V_ON_Load) * COEF_MUL)  /  V_ON_Load); 
    }  
    Tvide = Tvide / NB_CALC;  f_tvide = (float)Tvide/1000.00;
    Tch   = Tch   / NB_CALC;  f_tch   = (float)Tch/1000.00;
    Rint  = Rint  / (float)NB_CALC;
#ifdef DEBUG_ON	  
      Serial.print("T vide = ");  Serial.print(V_NO_Load); Serial.print("  T ch = ");  Serial.print(V_ON_Load); 
      Serial.print(" Ri = ");     Serial.println (Rint,4); 
#endif	  
  
    Message (4,50,  "T vide :",GREEN);
  //  itoa(Tvide,buff,10);
    dtostrf(f_tvide,5, 3, buff);  //4 is mininum width, 6 is precision
    Message (70,50,buff ,WHITE);
    
    Message (4,75,  "T chg  : ",YELLOW);
 //   itoa(Tch,buff,10);
    dtostrf(f_tch,5, 3, buff);  //4 is mininum width, 6 is precision
    Message (70,75,buff ,WHITE);
  
    Message (4,100, "R int : ",RED);
    dtostrf(Rint/COEF_MUL,5, 3, buff);  //4 is mininum width, 6 is precision
    Message (70,100,buff,RED);
    // bouclage tant que la batterie reste en place   
    while (GetStatus() == S_LION) delay (100);

   break;
//-------------------------------------------------------------------
   }
  delay( 700);
}
