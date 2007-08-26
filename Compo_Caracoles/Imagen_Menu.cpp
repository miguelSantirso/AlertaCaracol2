#include <allegro.h>
#include <string>
#include <vector>

#include "Imagen_Menu.h"

using namespace std;

Imagen_Menu::Imagen_Menu(void) : Grafico(NULL)
{
}

Imagen_Menu::Imagen_Menu(int _x, int _y, std::string _Nombre, std::string _Ruta) : x(_x), y(_y), Nombre(_Nombre), Grafico(NULL)
{
	PALETTE palette;	// Variable auxiliar
	
	// Cargamos en memoria el gráfico indicado
	Grafico=load_bitmap((char *)_Ruta.c_str(), palette);

	// <revisar> Controlar mejor error en la carga del fichero </revisar>

	if(!Grafico)
		printf("error");

	x -= Grafico->w/2;
	y -= Grafico->h/2;
}

Imagen_Menu::~Imagen_Menu(void)
{
	// Liberamos la memoria ocupada por el gráfico cargado.
	if(Grafico != NULL)
		destroy_bitmap(Grafico);
}

void Imagen_Menu::Dibujar(BITMAP *bmp)
{
	draw_sprite(bmp, Grafico, x, y);
}

void Imagen_Menu::Actualizar()
{
}