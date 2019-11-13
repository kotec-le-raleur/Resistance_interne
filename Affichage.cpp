// affichage pour la mesure de résistance interne des accus Nimh et Li-ion
//
//

#include <SPI.h>
#include "Ucglib.h"
#include "Common.h"


//---------------------------------------------
//-----   Prototypes des fonctions    ---------
//---------------------------------------------
void InitDisplay(char *);
void Message (int , int , char*, int, int, int);
void Couleur (int,int,int);
void ClearDisplay (void);
void bbox(int a,int b ,int c, int d);

//---------------------------------------------
/*
  Hardware SPI Pins:
    Arduino Uno		sclk=13, data=11
*/

Ucglib_ILI9163_18x128x128_HWSPI ucg(/*cd=*/ 8, /*cs=*/ 9, /*reset=*/ 7);	/* HW SPI Adapter */

//---------------------------------------------
void InitDisplay(char *txt)
 {

  ucg.begin(UCG_FONT_MODE_TRANSPARENT);
  //ucg.begin(UCG_FONT_MODE_SOLID);
  ucg.clearScreen();
  ucg.setColor (OLIVE);
  ucg.drawRFrame (0,0,127,127,3);
  
  ucg.setColor (TEAL);
  ucg.setFont(ucg_font_ncenR18_tr);
  ucg.setPrintPos(4,22);
  ucg.print(txt);
 
 }
 
 
//---------------------------------------------
void ClearDisplay (void)
{
	ucg.clearScreen();
}

//---------------------------------------------
void bbox(int a,int b ,int c, int d)
{
	ucg.setColor(BLACK);
    ucg.drawBox(a, b, c, d);
}

//---------------------------------------------
void Message (int HPos, int VPos, char* txt,int rg,int vt,int bl)
{
 ucg.setFont(ucg_font_ncenR12_tr);
 ucg.setColor(rg,vt,bl);
 ucg.setPrintPos(HPos,VPos);
 ucg.print(txt);
	
return;
}


