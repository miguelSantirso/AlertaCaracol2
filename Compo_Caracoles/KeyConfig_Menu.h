#pragma once
#include <string>
#include <vector>

struct BITMAP;
class KeyBox_Menu;
class Menu;

class KeyConfig_Menu
{
public:

/*	struct Key_Struct
	{
		std::string Nombre;
		std::string Label;
		int Id_Tecla;
	};*/
	struct Key_Struct
	{
		char Nombre[50];
		char Label[200];
		int Id_Tecla;
	};

	KeyConfig_Menu(int _x, int _y, int _Ancho, int _Alto, Menu *_Puntero_Menu);
	~KeyConfig_Menu(void);

	void Dibujar(BITMAP *bmp);
	void Actualizar();

	bool Validar_Tecla(std::string Clave, int Nuevo_Valor);
protected:

	void Lee_Configuracion();
	void Escribe_Configuracion();

	int x, y;
	int Ancho, Alto;
	Menu *Puntero_Menu;

	std::vector<Key_Struct> Config_Teclas;

	KeyBox_Menu * KeyBoxes; // Array de keyboxes
	int Numero_KeyBoxes;
};
