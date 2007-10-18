#pragma once

#include <string>
#include <vector>

struct BITMAP;

class Texto_Menu
{
public:
	enum formato {IZQUIERDA, CENTRADO, COMPLEJO};
	Texto_Menu(void);
	Texto_Menu(int _x, int _y, int _Ancho, formato _Formato_Texto, std::string _Nombre, std::string _Texto);
	~Texto_Menu(void);

	void Dibujar(BITMAP *bmp);
	void Actualizar();

protected:

	std::vector<std::string> Dividir_Lineas(std::string T);

	int x;
	int y;
	int Ancho;
	int Alto;
	formato Formato_Texto;

	std::vector<std::string> Lineas;
	std::string Nombre;
};
