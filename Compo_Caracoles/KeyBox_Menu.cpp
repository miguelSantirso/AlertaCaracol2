#include <allegro.h>
#include <string>

#include "KeyBox_Menu.h"
#include "KeyConfig_Menu.h"

extern void drawRectangle(BITMAP *bmp,int x, int y, int w, int h, int color, int style, int bgcolor);
extern void Menu_Listener(std::string msg);

KeyBox_Menu::KeyBox_Menu(void)
{
}

KeyBox_Menu::KeyBox_Menu(int _x, int _y, std::string _Nombre, int Id_Tecla, std::string _Label, KeyConfig_Menu * _Puntero_Config) : x(_x), y(_y), 
		Nombre(_Nombre), Estado(NORMAL), Puntero_Config(_Puntero_Config), Label(_Label)
{
	Texto = KeyId_To_String(Id_Tecla);

	Ancho = 92;
	Alto = 10*3;
	y -= Alto/2;
}

KeyBox_Menu::~KeyBox_Menu(void)
{
}

void KeyBox_Menu::Dibujar(BITMAP *bmp)
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
	case SELECTED:
		Color_Fondo = makecol(195, 220, 220);
		Color_Borde = makecol(0, 0, 0);
		break;
	}

	drawRectangle(bmp, x, y, Ancho, Alto, Color_Borde, Estilo_Boton, Color_Fondo);

	textout_centre_ex(bmp, font, (char *)Texto.c_str(), x+Ancho/2, y+Alto/2, 5, -1);
	textout_ex(bmp, font, (char *)Label.c_str(), x+Ancho+15, y+Alto/2, 5, -1);
}

bool KeyBox_Menu::Espera_Pulsacion()
{
	if(keypressed())
	{
		int val;
		val = readkey();
		if(Puntero_Config->Validar_Tecla(Nombre, val))
		{
			Texto = KeyId_To_String(val);
			
		}
		return true;
	}
	else
		return false;
}

void KeyBox_Menu::Borrar()
{
	Texto = "-";
}

std::string KeyBox_Menu::KeyId_To_String(int key)
{
	std::string String;
#ifdef SPANISH
	switch(key)
	{
	case -1:
		String = "-";
		break;
	case 21504:
		String = "'arriba'";
		break;
	case 21760:
		String = "'abajo'";
		break;
	case 20992:
		String = "'izquierda'";
		break;
	case 21248:
		String = "'derecha'";
		break;
	default:
		String = key;
		break;
	}
#endif
#ifdef ENGLISH
	switch(key)
	{
	case -1:
		String = "-";
		break;
	case 21504:
		String = "'up'";
		break;
	case 21760:
		String = "'down'";
		break;
	case 20992:
		String = "'left'";
		break;
	case 21248:
		String = "'right'";
		break;
	default:
		String = key;
		break;
	}
#endif
	return String;
}

void KeyBox_Menu::Actualizar()
{
	if(Estado == SELECTED && !Espera_Pulsacion())
		return;

	Estado_Boton Estado_Anterior = Estado;
	Estado = NORMAL;

	if(mouse_x>x && mouse_x<x+Ancho && mouse_y>y && mouse_y<y+Alto)
	{
		Estado = HOVER;
		
		if(mouse_b)
			Estado = DOWN;
		else if(Estado_Anterior == DOWN)
			Estado = SELECTED;
		else if(Estado_Anterior == SELECTED)
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

/*		if(Puntero_Menu != NULL)
			Puntero_Menu->Listener_Botones(msg);*/
		Menu_Listener(msg);
	}
}