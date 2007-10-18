#pragma once

//#define ENGLISH

#include <string>

struct BITMAP;
class Boton_Menu;
class Texto_Menu;
class Imagen_Menu;
class KeyConfig_Menu;

class Menu
{
public:
	
	Menu(void);
	~Menu(void);

	void Dibujar(BITMAP *bmp);
	void Actualizar();

	void Listener_Botones(std::string msg);
protected:
	void Liberar_Menu();
	void Inicializa_Menu(int Menu);

	void drawRectangle(BITMAP *bmp,int x, int y, int w,int h,int color, int style, int bgcolor);

	Boton_Menu * Botones; // Array de botones
	int Numero_Botones;

	Texto_Menu * Textos; // Array de textos
	int Numero_Textos;

	Imagen_Menu ** Imagenes; // Array de imagenes
	int Numero_Imagenes;

	KeyConfig_Menu * Configuracion_Teclas;

	int Siguiente; // Si es distinto de -1, indica que se debe cambiar al menú #Siguiente.
};
