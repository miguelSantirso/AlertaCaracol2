#include <allegro.h>

#include "Menu.h"
#include "Boton_Menu.h"
#include "Texto_Menu.h"
#include "Imagen_Menu.h"
#include "KeyConfig_Menu.h"

Menu::Menu(void) : Botones(NULL), Numero_Botones(0), Textos(NULL), Numero_Textos(0), Imagenes(NULL), Numero_Imagenes(0),
			Siguiente(-1), Configuracion_Teclas(NULL)
{
	Inicializa_Menu(0);
}

Menu::~Menu(void)
{
	delete [] Botones;
	delete [] Textos;

	for(int i=0; i<Numero_Imagenes; i++)
		delete Imagenes[i];
	delete [] Imagenes;

	delete [] Configuracion_Teclas;
}

void Menu::Liberar_Menu()
{
	delete [] Botones;
	Botones = NULL;
	Numero_Botones = 0;

	delete [] Textos;
	Textos = NULL;
	Numero_Textos = 0;

	for(int i=0; i<Numero_Imagenes; i++)
		delete Imagenes[i];

	delete [] Imagenes;
	Numero_Imagenes = 0;
	Imagenes = NULL;

	delete Configuracion_Teclas;
	Configuracion_Teclas = NULL;
}

void Menu::Inicializa_Menu(int Menu)
{
	Liberar_Menu();

	if(Menu == 0)
	{
		Numero_Botones = 4;
		Botones = new Boton_Menu[Numero_Botones];
#ifdef SPANISH
		Botones[0] = Boton_Menu(320, 187, 200, 38, "start", "Empezar a Jugar", this);
		Botones[1] = Boton_Menu(320, 270, 200, 38, "controles", "Controles", this);
		Botones[2] = Boton_Menu(320, 350, 200, 38, "opciones", "Instrucciones", this);
		Botones[3] = Boton_Menu(320, 440, 200, 38, "exit", "Salir", this);
#endif
#ifdef ENGLISH
		Botones[0] = Boton_Menu(320, 187, 200, 38, "start", "Start game", this);
		Botones[1] = Boton_Menu(320, 270, 200, 38, "controles", "Controls", this);
		Botones[2] = Boton_Menu(320, 350, 200, 38, "opciones", "Help", this);
		Botones[3] = Boton_Menu(320, 440, 200, 38, "exit", "Exit", this);
#endif
		Numero_Textos = 0;
	//	Textos = new Texto_Menu[Numero_Textos];

	//	Textos[0] = Texto_Menu(320, 400, 550, Texto_Menu::COMPLEJO, "explain", "CONTROLES:\n   - Izquierda/Derecha: Disparar e impulsar la nave hacia los lados\n   - Abajo: Impulsar la nave hacia arriba\n   - Arriba: Descender mas rapido\n   - ESC: Salir\n   - P: Pausa\n   - R: Reiniciar Partida");

		Numero_Imagenes = 1;
		Imagenes = new Imagen_Menu *[Numero_Imagenes];
#ifdef SPANISH
		Imagenes[0] = new Imagen_Menu(320, 240, "header", "graficos\\menus\\fondo.pcx");
#endif
#ifdef ENGLISH
		Imagenes[0] = new Imagen_Menu(320, 240, "header", "graficos\\menus\\fondo_en.pcx");
#endif
	}
	else if(Menu == 1)
	{
		Numero_Botones = 1;
		Botones = new Boton_Menu[Numero_Botones];
		Botones[0] = Boton_Menu(320, 440, 200, 38, "volver", "Volver", this);

		Numero_Textos = 2;
		Textos = new Texto_Menu[Numero_Textos];
#ifdef SPANISH
		Textos[0] = Texto_Menu(320, 220, 565, Texto_Menu::COMPLEJO, "explain", "INSTRUCCIONES:\n\n- Controlas una nave que puedes manejar disparando agua a presion en\n tres direcciones diferentes (izquierda, abajo y derecha).\n- Tu objetivo es disparar a los caracoles que suben por las columnas\n para que caigan al agua.\n- Si un caracol llega a lo alto de una columna, esta bajara un poco.");
		Textos[1] = Texto_Menu(320, 340, 565, Texto_Menu::COMPLEJO, "explain", "CONTROLES:\n(los controles se pueden cambiar en la opcion 'controles' del menu)\n- Izquierda/Derecha: Disparar e impulsar la nave hacia los lados\n- Abajo/Arriba: Impulsar la nave hacia arriba/Descender mas rapido\n- ESC: Salir\n- P: Pausa\n- R: Reiniciar Partida");
#endif
#ifdef ENGLISH
		Textos[0] = Texto_Menu(320, 210, 565, Texto_Menu::COMPLEJO, "explain", "INSTRUCTIONS:\n\n- You control a small ship which can be moved by shooting pressured\n water in three different directions (left, down and right).\n- Your objective is to shoot the snails that climb to the top of the\n columns so that they fall to the water.\n- If a snail gets to the top of a column, he will explode itself and\n the column will fall.");
		Textos[1] = Texto_Menu(320, 350, 565, Texto_Menu::COMPLEJO, "explain", "CONTROLS:\n(this controls can be personalized in 'controls', in the main menu)\n- 'left'/'right': Shoot and move the ship horizontally.\n- 'down'/'up': Move the ship up/Go down faster\n- ESC: Exit\n- P: Pause\n- R: Restart Game");
#endif
		Numero_Imagenes = 1;
		Imagenes = new Imagen_Menu *[Numero_Imagenes];
#ifdef SPANISH
		Imagenes[0] = new Imagen_Menu(320, 240, "header", "graficos\\menus\\fondocontroles.pcx");
#endif
#ifdef ENGLISH
		Imagenes[0] = new Imagen_Menu(320, 240, "header", "graficos\\menus\\fondocontroles_en.pcx");
#endif
	}
	else if(Menu == 2)
	{
		Numero_Botones = 1;
		Botones = new Boton_Menu[Numero_Botones];
#ifdef SPANISH
		Botones[0] = Boton_Menu(320, 440, 200, 38, "volver", "Aceptar", this);
#endif
#ifdef ENGLISH
		Botones[0] = Boton_Menu(320, 440, 200, 38, "volver", "Ok", this);
#endif

		Numero_Imagenes = 1;
		Imagenes = new Imagen_Menu *[Numero_Imagenes];
#ifdef SPANISH
		Imagenes[0] = new Imagen_Menu(320, 240, "header", "graficos\\menus\\fondosin.pcx");
#endif
#ifdef ENGLISH
		Imagenes[0] = new Imagen_Menu(320, 240, "header", "graficos\\menus\\fondosin_en.pcx");
#endif

		Configuracion_Teclas = new KeyConfig_Menu(110, 150, 420, 300, this);
	}
}


void Menu::Listener_Botones(std::string msg)
{
	if(msg == "controles UP")
		Siguiente = 2;
	else if(msg == "opciones UP")
		Siguiente = 1;
	else if(msg == "volver UP")
		Siguiente = 0;
}

void Menu::Dibujar(BITMAP *bmp)
{
	for(int i=0; i<Numero_Imagenes; i++)
		Imagenes[i]->Dibujar(bmp);

	for(int i=0; i<Numero_Botones; i++)
		Botones[i].Dibujar(bmp);

	for(int i=0; i<Numero_Textos; i++)
		Textos[i].Dibujar(bmp);

	if(Configuracion_Teclas != NULL)
		Configuracion_Teclas->Dibujar(bmp);

	circle(bmp, mouse_x, mouse_y, 5, makecol(50, 50, 100));
}

void Menu::Actualizar()
{
	for(int i=0; i<Numero_Botones; i++)
		Botones[i].Actualizar();

	if(Configuracion_Teclas != NULL)
		Configuracion_Teclas->Actualizar();

	if(Siguiente != -1)
	{
		Inicializa_Menu(Siguiente);
		Siguiente = -1;
	}
}


// draw a rectangle
// since we use it three times in the editor, it would be nice to have
// a function for it.
// Esta función la encontré por internet; aunque está muy bien hecha, tiene un funcionamiento bastante sencillo
void Menu::drawRectangle(BITMAP *bmp,int x, int y, int w,int h,int color, int style, int bgcolor)
{
	w--;
	h--;
	if (style==0)
	{
		vline(bmp,x,y,y+h,color);
		vline(bmp,x+w,y,y+h,color);
		hline(bmp,x,y,x+w,color);
		hline(bmp,x,y+h,x+w,color);
	}
	if (style==1)
	{
		rectfill(bmp,x,y,x+w,y+h,bgcolor);
		vline(bmp,x,y,y+h,color);
		vline(bmp,x+w,y,y+h,color);
		hline(bmp,x,y,x+w,color);
		hline(bmp,x,y+h,x+w,color);
	}


	// this is for our dialog. This draws a box with rounded corners.
	if (style==2)
	{									
		arc(bmp,x+10,y+10,itofix(64),itofix(128),10,255);
		arc(bmp,x+10,y+h-10,itofix(128),itofix(192),10,255);
		arc(bmp,x+w-10,y+10,itofix(0),itofix(64),10,255);
		arc(bmp,x+w-10,y+h-10,itofix(192),itofix(256),10,255);


		hline(bmp,x+10,y,x+w-10,255);
		hline(bmp,x+10,y+h,x+w-10,255);
		vline(bmp,x,y+10,y+h-10,255);
		vline(bmp,x+w,y+10,y+h-10,255);
	
		floodfill(bmp,x+w/2,y+h/2,255);

		arc(bmp,x+10,y+10,itofix(64),itofix(128),8,bgcolor);
		arc(bmp,x+10,y+h-10,itofix(128),itofix(192),8,bgcolor);
		arc(bmp,x+w-10,y+10,itofix(0),itofix(64),8,bgcolor);
		arc(bmp,x+w-10,y+h-10,itofix(192),itofix(256),8,bgcolor);


		hline(bmp,x+10,y+2,x+w-10,bgcolor);
		hline(bmp,x+10,y+h-2,x+w-10,bgcolor);
		vline(bmp,x+2,y+10,y+h-10,bgcolor);
		vline(bmp,x+w-2,y+10,y+h-10,bgcolor);

		floodfill(bmp,x+w/2,y+h/2,bgcolor);
	}
}
