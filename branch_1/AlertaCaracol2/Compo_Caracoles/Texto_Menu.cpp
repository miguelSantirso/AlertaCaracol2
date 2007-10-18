#include <allegro.h>
#include <string>
#include <vector>

#include "Texto_Menu.h"

#define TAMANO_FUENTE int(13)

extern void drawRectangle(BITMAP *bmp,int x, int y, int w, int h, int color, int style, int bgcolor);
extern void Menu_Listener(std::string msg);

using namespace std;

Texto_Menu::Texto_Menu(void)
{
}

Texto_Menu::Texto_Menu(int _x, int _y, int _Ancho, formato _Formato_Texto, std::string _Nombre, std::string _Texto) : x(_x), y(_y), Ancho(_Ancho), 
		Formato_Texto(_Formato_Texto), Nombre(_Nombre)
{
	Lineas = Dividir_Lineas(_Texto);
	Alto = (Lineas.size()+2)*TAMANO_FUENTE;

	x -= Ancho/2;
	y -= Alto/2;
}

Texto_Menu::~Texto_Menu(void)
{
}

void Texto_Menu::Dibujar(BITMAP *bmp)
{
	int Estilo_Boton = 1;
	int Color_Borde = makecol(255, 255, 255);
	int Color_Fondo = makecol(140, 220, 220);

	drawRectangle(bmp, x, y, Ancho, Alto, Color_Borde, Estilo_Boton, Color_Fondo);

	for(int i=0; i<Lineas.size(); i++)
	{
		int Aux_x;
		if(Formato_Texto == IZQUIERDA)
			textout_ex(bmp, font, (char *)Lineas[i].c_str(), x+TAMANO_FUENTE, y+TAMANO_FUENTE+i*TAMANO_FUENTE, 5, -1);
		else if(Formato_Texto == CENTRADO)
			textout_centre_ex(bmp, font, (char *)Lineas[i].c_str(), x+Ancho/2, y+TAMANO_FUENTE+i*TAMANO_FUENTE, 5, -1);
		else if(Formato_Texto == COMPLEJO)
		{
			if(i==0)
				textout_centre_ex(bmp, font, (char *)Lineas[i].c_str(), x+Ancho/2, y+TAMANO_FUENTE+i*TAMANO_FUENTE, 5, -1);
			else
				textout_ex(bmp, font, (char *)Lineas[i].c_str(), x+TAMANO_FUENTE, y+TAMANO_FUENTE+i*TAMANO_FUENTE, 5, -1);
		}
	}
}

void Texto_Menu::Actualizar()
{
}

vector<string> Texto_Menu::Dividir_Lineas(string T)
{
	vector<string> Result;

	std::string::size_type Pos_A = 0;
	std::string::size_type Pos_B = T.find('\n');
	while (Pos_A != string::npos)
	{
		Result.push_back(T.substr(Pos_A, Pos_B - Pos_A));
		Pos_A = Pos_B;
		Pos_B = T.find('\n', Pos_A+1);
		if(Pos_A != string::npos)
			Pos_A++;
	}
	return Result;
}