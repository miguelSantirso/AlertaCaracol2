#include "Sprite_Efecto.h"

#include <allegro.h>

Sprite_Efecto::Sprite_Efecto(void)
{
}

Sprite_Efecto::Sprite_Efecto(BITMAP *Sprite_Origen, int x, int y, int D) : Sprite(Sprite_Origen), 
	Pos_X(x), Pos_Y(y), Alfa(255), Decremento(D)
{
}

Sprite_Efecto::~Sprite_Efecto(void)
{
}

void Sprite_Efecto::Dibuja(BITMAP * Pantalla)
{
	Alfa-=Decremento;
	if(Alfa < 10)
		Destruir=true;
	else
	{
		set_trans_blender(128, 128, 128, Alfa);
		// Dibujamos en la pantalla el objeto.
		draw_trans_sprite(Pantalla, Sprite, Pos_X-Sprite->w/2, Pos_Y-Sprite->h/2);
		set_trans_blender(0, 0, 0, 255);
	}

}
