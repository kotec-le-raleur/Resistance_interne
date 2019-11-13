# Resistance_interne
mesure de la résistance interne d'un élément Ni_Mh et/ou Li-Ion

L'affichage est un 128X128 LCD couleur avec un circuit ILI9163.

Il faut la lib ucglib    :  #include "Ucglib.h"
et la lib spi

le cablage de l'afficheur se déduit facilement du code 
La mesure des tensions sont en Volts et celle de la résistance en Ohm.
J'ai testé avec une résistance de 1 Ohm en série avec la pile et la valeur lue est bien augmentée
de 1 ohm .

La qualité des contacts est primordiale, sur mon proto, ce n'est pas idéal
mais je vais me faire des supports plus sérieux en 4 fils, avec des pointes en argent.

L'alimentation est directe par un connecteur mini USB présent sur l'arduino micro.
Les mesures de tensions sont faite par une centaine de mesures individuelles et un moyenage.

Voir le code et les constantes pour modifier tout cela
#define  NB_ACQ       	    16
#define  NB_CALC       	    6
ici, on a 16 mesures analogiques moyennées et 6 tours de mesures soit 16*6 acquisitions .

Bon amusement !
Miaou




