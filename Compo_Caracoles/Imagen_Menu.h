#pragma once

#include <string>
#include <vector>

struct BITMAP;

class Imagen_Menu
{
public:
	Imagen_Menu(void);
	Imagen_Menu(int _x, int _y, std::string _Nombre, std::string _Ruta);
	~Imagen_Menu(void);

	void Dibujar(BITMAP *bmp);
	void Actualizar();

protected:

	int x;
	int y;
	int Ancho;
	int Alto;

	std::string Nombre;
	BITMAP * Grafico;
};
