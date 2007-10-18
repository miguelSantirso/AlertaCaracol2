#pragma once

#include <string>

struct FONT;
struct BITMAP;
class Menu;

class Boton_Menu
{
public:
	Boton_Menu(void);
	Boton_Menu(int _x, int _y, int _Ancho, int _Alto, std::string _Nombre, std::string _Texto, Menu *_Puntero_Menu);
	~Boton_Menu(void);

	void Dibujar(BITMAP *bmp);
	void Actualizar();
	
	enum Estado_Boton {NORMAL, HOVER, DOWN, UP};

protected:

	int x;
	int y;
	int Ancho;
	int Alto;

	std::string Texto;
	std::string Nombre;

	Estado_Boton Estado;

	Menu * Puntero_Menu;
};
