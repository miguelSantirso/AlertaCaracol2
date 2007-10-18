#include <allegro.h>
#include <string>

#include "Boton_Menu.h"
#include "Menu.h"

extern void drawRectangle(BITMAP *bmp,int x, int y, int w, int h, int color, int style, int bgcolor);
extern void Menu_Listener(std::string msg);

Boton_Menu::Boton_Menu(void)
{
}

Boton_Menu::Boton_Menu(int _x, int _y, int _Ancho, int _Alto, std::string _Nombre, std::string _Texto, Menu * _Puntero_Menu) : x(_x), y(_y), 
		Ancho(_Ancho), Alto(_Alto), Texto(_Texto), Nombre(_Nombre), Estado(NORMAL), Puntero_Menu(_Puntero_Menu)
{
	x -= Ancho/2;
	y -= Alto/2;
}

Boton_Menu::~Boton_Menu(void)
{
}

void Boton_Menu::Dibujar(BITMAP *bmp)
{
	int Estilo_Boton = 1;
	int Color_Borde = makecol(255, 255, 255);
	int Color_Fondo = makecol(140, 220, 220);

	switch(Estado)
	{
	case NORMAL:
		break;
	case HOVER:
		Color_Fondo = makecol(195, 230, 230);
		break;
	case DOWN:
		Color_Fondo = makecol(195, 220, 220);
		break;
	}

	drawRectangle(bmp, x, y, Ancho, Alto, Color_Borde, Estilo_Boton, Color_Fondo);

	textout_centre_ex(bmp, font, (char *)Texto.c_str(), x+Ancho/2, y+Alto/2, 5, -1);
}

void Boton_Menu::Actualizar()
{
	Estado_Boton Estado_Anterior = Estado;
	Estado = NORMAL;

	if(mouse_x>x && mouse_x<x+Ancho && mouse_y>y && mouse_y<y+Alto)
	{
		Estado = HOVER;
		
		if(mouse_b)
			Estado = DOWN;
		else if(Estado_Anterior == DOWN)
			Estado = UP;
	}

	if(Estado != Estado_Anterior)
	{
		std::string msg = Nombre + " ";
		switch(Estado)
		{
		case NORMAL:
			msg += "NORMAL";
			break;
		case HOVER:
			msg += "HOVER";
			break;
		case DOWN:
			msg += "DOWN";
		case UP:
			msg += "UP";
			break;
		}

		if(Puntero_Menu != NULL)
			Puntero_Menu->Listener_Botones(msg);
		Menu_Listener(msg);
	}
}