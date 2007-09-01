#include <fstream>
#include <string>

#include "KeyConfig_Menu.h"
#include "KeyBox_Menu.h"

using namespace std;

extern void Aborta_Con_Error(string msg);

KeyConfig_Menu::KeyConfig_Menu(int _x, int _y, int _Ancho, int _Alto, Menu *_Puntero_Menu) : x(_x),
		y(_y), Ancho(_Ancho), Alto(_Alto), Puntero_Menu(Puntero_Menu), KeyBoxes(NULL)
{
	Lee_Configuracion();

	Numero_KeyBoxes = Config_Teclas.size();
	KeyBoxes = new KeyBox_Menu [Numero_KeyBoxes];
	
	string Aux;
	for(int i=0; i<Numero_KeyBoxes; i++)
	{
		string Nombre = Config_Teclas[i].Nombre;
		string Label = Config_Teclas[i].Label;
		KeyBoxes[i] = KeyBox_Menu(150, 200+40*i, Config_Teclas[i].Nombre, Config_Teclas[i].Id_Tecla, Label, this);
	}
}

KeyConfig_Menu::~KeyConfig_Menu(void)
{
	Escribe_Configuracion();

	delete [] KeyBoxes;
}

void KeyConfig_Menu::Actualizar()
{
	for(int i=0; i<Numero_KeyBoxes; i++)
		KeyBoxes[i].Actualizar();
}

void KeyConfig_Menu::Dibujar(BITMAP *bmp)
{
	for(int i=0; i<Numero_KeyBoxes; i++)
		KeyBoxes[i].Dibujar(bmp);
}

void KeyConfig_Menu::Lee_Configuracion()
{
	fstream Fichero;
	Fichero.open("config\\keyboard.conf");

	if(!Fichero.is_open())
	{
		/*
		Key_Struct Tecla;
		Tecla.Nombre = "up";
		Tecla.Id_Tecla = 51;
		Config_Teclas.push_back(Tecla);
		Tecla.Nombre = "down";
		Config_Teclas.push_back(Tecla);
		Tecla.Nombre = "left";
		Config_Teclas.push_back(Tecla);
		Tecla.Nombre = "right";
		Config_Teclas.push_back(Tecla);

		Escribe_Configuracion();*/
		Aborta_Con_Error("ERROR:\nError de ejecucion en la funcion Lee_Configuracion() de KeyConfig_Menu.cpp.\n - Error al cargar el fichero de configuracion de teclado.");
		return;
	}

	Key_Struct Tecla;
	Fichero.read((char *)(&Tecla), sizeof(Tecla));
	while(!Fichero.eof())
	{
		Config_Teclas.push_back(Tecla);

		Fichero.read((char *)(&Tecla), sizeof(Tecla));
	}

	Fichero.close();
}

void KeyConfig_Menu::Escribe_Configuracion()
{
	ofstream Fichero;
	Fichero.open("config\\keyboard.conf");

	if(!Fichero.is_open())
	{
		Aborta_Con_Error("ERROR:\nError de ejecucion en la funcion Escribe_Configuracion() de KeyConfig_Menu.cpp.\n - Error al escribir en el fichero de configuracion de teclado.");
		return;
	}

	for(int i=0; i<Config_Teclas.size(); i++)
	{
		Fichero.write((char *)(&Config_Teclas[i]), sizeof(Key_Struct));
	}
	Fichero.close();
}

bool KeyConfig_Menu::Validar_Tecla(std::string Clave, int Nuevo_Valor)
{
	for(int i=0; i<Config_Teclas.size(); i++)
	{
		if(Config_Teclas[i].Id_Tecla == Nuevo_Valor)
		{
			Config_Teclas[i].Id_Tecla = -1;
			KeyBoxes[i].Borrar();
		}
	}

	int i;
	for(i=0; i<Config_Teclas.size() && strcmp((char *)Config_Teclas[i].Nombre,Clave.c_str()) != 0; i++);
	
	if(i<Config_Teclas.size() && strcmp((char *)Config_Teclas[i].Nombre,Clave.c_str()) == 0)
		Config_Teclas[i].Id_Tecla = Nuevo_Valor;
	return true;
}