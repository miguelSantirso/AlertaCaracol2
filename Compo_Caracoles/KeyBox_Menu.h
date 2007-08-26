#pragma once

#include <string>

struct FONT;
struct BITMAP;
class KeyConfig_Menu;

class KeyBox_Menu
{
public:
	KeyBox_Menu(void);
	KeyBox_Menu(int _x, int _y, std::string _Nombre, int Id_Tecla, std::string _Label, KeyConfig_Menu *_Puntero_Config);
	~KeyBox_Menu(void);

	void Dibujar(BITMAP *bmp);
	void Actualizar();

	void Borrar();
	
	enum Estado_Boton {NORMAL, HOVER, DOWN, UP, SELECTED};

protected:

	bool Espera_Pulsacion();
	std::string KeyId_To_String(int key);

	int x;
	int y;
	int Ancho;
	int Alto;

	std::string Texto;
	std::string Nombre;
	std::string Label;

	Estado_Boton Estado;

	KeyConfig_Menu * Puntero_Config;
};
