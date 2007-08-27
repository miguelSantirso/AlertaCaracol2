/*
* Copyright (c) 2006 Erin Catto http://www.gphysics.com
*
* Permission to use, copy, modify, distribute and sell this software
* and its documentation for any purpose is hereby granted without fee,
* provided that the above copyright notice appear in all copies.
* Erin Catto makes no representations about the suitability
* of this software for any purpose.
* It is provided "as is" without express or implied warranty.
*/

/*
* Adapted to Allegro and slightly changed by Johan Peitz
* http://www.freelunchdesign.com
*/

#define ANCHO_BARRA_AGUA int(15)
#define ALTO_BARRA_AGUA int(40)

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <allegro.h>

#include "World.h"
#include "Body.h"
#include "Joint.h"
#include "Objeto_Fisico.h"
#include "Objeto_Disparo.h"
#include "Objeto_Nave.h"
#include "Objeto_Caracol.h"
#include "Sprite_Efecto.h"
#include "Efecto_Agua.h"
#include "Menu.h"
#include "KeyConfig_Menu.h"

using namespace std;

// ESTRUCTURAS DE ALMACENAMIENTO
vector<Sprite_Efecto *> Sprites_Efectos;	// Almacena los punteros a los efectos (explosiones, salpicaduras de agua, etc...)
vector<Objeto_Fisico *> Objetos_Fisicos;	// Almacena los punteros a todos los objetos que se rigen por las físicas.
vector<Objeto_Fisico *> Columnas;			// Almacena los punteros a las columnas (que son objetos físicos)
vector<Objeto_Caracol *> Caracoles;			// Almacena los punteros a los caracoles
vector<Objeto_Disparo *> Disparos;			// Almacena los punteros a los disparos
vector<Body *> bodies;						// Vector de cuerpos físicos (las cajas de colisiones)
vector<Joint *> joints;						// Vector de enlaces (enlaces entre cuerpos físicos)

// OBJETOS ESPECIALES
Efecto_Agua * Puntero_Agua = NULL;	// Puntero al agua
Body* bomb = NULL;					// Puntero a una bomba (objeto especial que usé para pruebas)
Objeto_Nave *Puntero_Nave=NULL;		// Puntero a la nave del jugador
Objeto_Fisico *Columna_Izquierda;	// Puntero a la columna de la izquierda
Objeto_Fisico *Columna_Derecha;		// Puntero a la columna de la derecha

bool Lado_Oscuro = false;	// Almacena la parte de la pantalla "no relevante"; se supone que es la parte hacia la que
							// el jugador no está mirando. La barra que indica el agua restante irá en ese lado.

// ESTADO DE LA APLICACIÓN (debería haber usado una variable "Estado" en lugar de los tres bool)
bool Pausa = false;			// Indica si el juego está pausado
bool Bienvenida = true;		// Indica si se está en la pantalla de bienvenida
bool Muerto = false;		// Indica si se está en la pantalla de muerto
bool Salir = false;			// Indica que se debe salir del juego

bool Debug = false;			// Indica si se está en modo debug (se dibujan los bodies y los joints)

// ESTADO DE LA PARTIDA
int Puntuacion=0;						// Puntuación del jugador
int Agua_Restante = NIVEL_MAXIMO_AGUA;	// Agua restante (0 - NIVEL_MAXIMO_AGUA)
int Nivel = 0;							// Nivel en que se encuentra el jugador

World world(Vec2(0.0f, 20.0f), 10);		// Inicializa el sistema de físicas

// BITMAPS
BITMAP *swap_screen = NULL;			// Aquí se dibujará todo antes de llevarlo a la pantalla

BITMAP *Imagen_Fondo = NULL;
BITMAP *Sprite_Explosion = NULL;
BITMAP *Sprite_Splash = NULL;
BITMAP *Sprite_Propuls = NULL;

Menu *Puntero_Menu = NULL;

int Alfa_Propuls = 255;

int Vol_Disparo; 

// SAMPLES (para los sonidos cargados)
SAMPLE *Splash_Grande = NULL;
SAMPLE *Propulsor = NULL;
SAMPLE *Disparo = NULL;
SAMPLE *Explosion = NULL;
SAMPLE *Cargando = NULL;
SAMPLE *Caida = NULL;

bool Propulsor_Activo = false;	// Indica si el propulsor está activo
bool Disparo_Activo = false;	// Indica si el disparo está activo

// timer stuff
volatile int frame_count;
volatile int fps;
volatile int logic_count;
volatile int lps;
volatile int cycle_count;
volatile int game_count;

map<string, int> Teclas;

// keeps track of frames each second
void fps_counter(void) {
	fps = frame_count;
	frame_count = 0;
	lps = logic_count;
	logic_count = 0;
}
END_OF_FUNCTION(fps_counter)


// keeps track of internal game speed
void cycle_counter(void) {
	cycle_count++;
	game_count++;
}
END_OF_FUNCTION(game_counter)



void DrawText(BITMAP *p_bmp, int x, int y, char *string) {
    textout_ex(p_bmp, font, string, x, y, 5, -1);
}

void DrawBody(BITMAP *p_bmp, Body* body, int px, int py)
{
	Mat22 R(body->rotation);
	Vec2 x = body->position;
	Vec2 h = 0.5f * body->width;

	Vec2 v1 = x + R * Vec2(-h.x, -h.y);
	Vec2 v2 = x + R * Vec2( h.x, -h.y);
	Vec2 v3 = x + R * Vec2( h.x,  h.y);
	Vec2 v4 = x + R * Vec2(-h.x,  h.y);

    int color = 3;
	if (body == bomb) color = 4;
	if (body->mass == FLT_MAX) color = 1;

    line(p_bmp, px + (int)v1.x, py + (int)v1.y, px + (int)v2.x, py + (int)v2.y, color);
    line(p_bmp, px + (int)v2.x, py + (int)v2.y, px + (int)v3.x, py + (int)v3.y, color);
    line(p_bmp, px + (int)v3.x, py + (int)v3.y, px + (int)v4.x, py + (int)v4.y, color);
    line(p_bmp, px + (int)v4.x, py + (int)v4.y, px + (int)v1.x, py + (int)v1.y, color);

    if (body->mass != FLT_MAX && Debug) {
        textprintf_centre_ex(p_bmp, font, (int)x.x, py + (int)x.y, 5, -1, "%d", (int)body->mass);
    }
}

void DrawJoint(BITMAP *p_bmp, Joint* joint, int px, int py)
{
	Body* b1 = joint->body1;
	Body* b2 = joint->body2;

	Mat22 R1(b1->rotation);
	Mat22 R2(b2->rotation);

	Vec2 x1 = b1->position;
	Vec2 p1 = x1 + R1 * joint->localAnchor1;

	Vec2 x2 = b2->position;
	Vec2 p2 = x2 + R2 * joint->localAnchor2;

	int color = 2;
	line(p_bmp, px + (int)x1.x, py + (int)x1.y, px + (int)p1.x, py + (int)p1.y, color);
	line(p_bmp, px + (int)p1.x, py + (int)p1.y, px + (int)x2.x, py + (int)x2.y, color);
	line(p_bmp, px + (int)x2.x, py + (int)x2.y, px + (int)p2.x, py + (int)p2.y, color);
	line(p_bmp, px + (int)p2.x, py + (int)p2.y, px + (int)x1.x, py + (int)x1.y, color);
}

void LaunchBomb()
{
    if (bomb != NULL) {
        delete bomb;
        bomb = new Body();
    }
    else {
        bomb = new Body();
        bodies.push_back(bomb);
        world.Add(bomb);
    }

    bomb->Set(Vec2(10, 10), 250);
	bomb->position.Set(320 + Random(-250.0f, 250.0f), 15.0f);
	bomb->rotation = Random(-1.5f, 1.5f);
	bomb->friction = 0.2;
	bomb->velocity = Vec2((320 - bomb->position.x)/4, 50);
	bomb->angularVelocity = Random(-2.0f, 2.0f);
}

void Lee_Configuracion_Teclado()
{
	fstream Fichero;
	Fichero.open("config\\keyboard.conf");

	if(!Fichero.is_open())
	{
		vector<KeyConfig_Menu::Key_Struct> Config_Teclas;

		KeyConfig_Menu::Key_Struct Tecla;
		strcpy((char *)&Tecla.Nombre, (char *)"up");
#ifdef SPANISH
		strcpy((char *)&Tecla.Label, (char *)"Tecla 'arriba'");
#endif
#ifdef ENGLISH
		strcpy((char *)&Tecla.Label, (char *)"'UP' key");
#endif
		Tecla.Id_Tecla = 21504;
		Config_Teclas.push_back(Tecla);
		strcpy((char *)&Tecla.Nombre, (char *)"down");
#ifdef SPANISH
		strcpy((char *)&Tecla.Label, (char *)"Tecla 'abajo'");
#endif
#ifdef ENGLISH
		strcpy((char *)&Tecla.Label, (char *)"'DOWN' key");
#endif
		Tecla.Id_Tecla = 21760;
		Config_Teclas.push_back(Tecla);
		strcpy((char *)&Tecla.Nombre, (char *)"left");
#ifdef SPANISH
		strcpy((char *)&Tecla.Label, (char *)"Tecla 'izquierda'");
#endif
#ifdef ENGLISH
		strcpy((char *)&Tecla.Label, (char *)"'LEFT' key");
#endif
		Tecla.Id_Tecla = 20992;
		Config_Teclas.push_back(Tecla);
		strcpy((char *)&Tecla.Nombre, (char *)"right");
#ifdef SPANISH
		strcpy((char *)&Tecla.Label, (char *)"Tecla 'derecha'");
#endif
#ifdef ENGLISH
		strcpy((char *)&Tecla.Label, (char *)"'RIGHT' key");
#endif
		Tecla.Id_Tecla = 21248;
		Config_Teclas.push_back(Tecla);

		ofstream new_f;
		new_f.open("config\\keyboard.conf");

		if(!new_f.is_open())
		{
			printf("error");
			return;
		}

		for(int i=0; i<Config_Teclas.size(); i++)
		{
			new_f.write((char *)(&Config_Teclas[i]), sizeof(Config_Teclas[i]));
		}
		new_f.close();
		Lee_Configuracion_Teclado();
		return;
	}

	while(!Fichero.eof())
	{
		KeyConfig_Menu::Key_Struct Tecla;
		Fichero.read((char *)(&Tecla), sizeof(Tecla));

		if(Tecla.Nombre != "")
			Teclas[Tecla.Nombre] = Tecla.Id_Tecla;
	}

	Fichero.close();
}


// Cargador de la partida
void Iniciar_Partida()
{
	// Variables auxiliares
	PALETTE paleta;
	Body * b;

	// Inicializr el estado de la partida
	Puntuacion = 0;
	Agua_Restante = NIVEL_MAXIMO_AGUA;
	Nivel = 1;

	// Cargar diversos gráficos
	Imagen_Fondo = load_pcx("graficos\\fondo.pcx", paleta);
	Sprite_Explosion = load_pcx("graficos\\Explosion.pcx", paleta);
	Sprite_Splash = load_pcx("graficos\\Splash.pcx", paleta);
	Sprite_Propuls = load_pcx("graficos\\propulsion.pcx", paleta);

	Alfa_Propuls = 255;

    // Carga y creación del suelo
/*	Objeto_Fisico *Suelo = new Objeto_Fisico("graficos\\agua.pcx", FLT_MAX);
	Suelo->Puntero_Box->Set(Vec2(640, 40), FLT_MAX);
	Suelo->Puntero_Box->position.Set(320, 460);
	Objetos_Fisicos.push_back(Suelo);*/
   
   // Carga y creación del techo
	Objeto_Fisico *Techo = new Objeto_Fisico("graficos\\techo.pcx", FLT_MAX);
	Techo->Puntero_Box->Set(Vec2(1200, 400), 500.0f);
	Techo->Puntero_Box->friction=500.0f;
	Techo->Puntero_Box->position.Set(320, 20 - 200);
	Objetos_Fisicos.push_back(Techo);

	//CARGA Y CREACIÓN DE LAS COLUMNAS
	Columnas.resize(5);	// Preparamos para almacenar las columnas

    // Una columna
	Objeto_Fisico *Columna = new Objeto_Fisico("graficos\\columna.pcx", FLT_MAX);
	Columna->Puntero_Box->Set(Vec2(24, 480), FLT_MAX);
	Columna->Puntero_Box->position.Set(320, 280);
	Objetos_Fisicos.push_back(Columna);
	Columnas[0]=(Columna);

    // Una columna
	Columna = new Objeto_Fisico("graficos\\columna.pcx", FLT_MAX);
	Columna->Puntero_Box->Set(Vec2(24, 480), FLT_MAX);
	Columna->Puntero_Box->position.Set(320-640/4, 280);
	Objetos_Fisicos.push_back(Columna);
	Columnas[1]=(Columna);

    // Una columna
	Columna = new Objeto_Fisico("graficos\\columna.pcx", FLT_MAX);
	Columna->Puntero_Box->Set(Vec2(24, 480), FLT_MAX);
	Columna->Puntero_Box->position.Set(320+640/4, 280);
	Objetos_Fisicos.push_back(Columna);
	Columnas[2]=(Columna);

    // Columna
	Columna = new Objeto_Fisico("graficos\\columna.pcx", FLT_MAX);
	Columna->Puntero_Box->Set(Vec2(24, 480), FLT_MAX);
	Columna->Puntero_Box->position.Set(20, 280);
	Objetos_Fisicos.push_back(Columna);
	Columnas[3]=(Columna);

	// Columna
	Columna = new Objeto_Fisico("graficos\\columna.pcx", FLT_MAX);
	Columna->Puntero_Box->Set(Vec2(24, 480), FLT_MAX);
	Columna->Puntero_Box->position.Set(620, 280);
	Objetos_Fisicos.push_back(Columna);
	Columnas[4]=(Columna);

    // Limite lateral izquierdo
	Columna_Izquierda = new Objeto_Fisico("graficos\\limite_lateral.pcx", FLT_MAX);
	Columna_Izquierda->Puntero_Box->Set(Vec2(30, 480), FLT_MAX);
	Columna_Izquierda->Puntero_Box->position.Set(5, 280);
	Objetos_Fisicos.push_back(Columna_Izquierda);
    // Limite lateral derecho
	Columna_Derecha = new Objeto_Fisico("graficos\\limite_lateral.pcx", FLT_MAX);
	Columna_Derecha->Puntero_Box->Set(Vec2(30, 480), FLT_MAX);
	Columna_Derecha->Puntero_Box->position.Set(635, 280);
	Objetos_Fisicos.push_back(Columna_Derecha);

	// Crear el agua
	Puntero_Agua = new Efecto_Agua(0, 425, 640, 480);

	// Crear la nave
	Puntero_Nave = new Objeto_Nave();
	Puntero_Agua->Nuevo_Cuerpo(Puntero_Nave->Puntero_Box);
	Objetos_Fisicos.push_back((Objeto_Fisico *)Puntero_Nave);

	// Carga de los SONIDOS
     Splash_Grande = load_wav("sonidos\\splash_grande.wav");
	 Propulsor = load_wav("sonidos\\propulsor.wav");
	 Disparo = load_wav("sonidos\\propulsor.wav");
	 Explosion = load_wav("sonidos\\Explosion.wav");
	 Cargando = load_wav("sonidos\\Cargando.wav");
	 Caida = load_wav("sonidos\\Caida.wav");

	 Lee_Configuracion_Teclado();

	 if(Bienvenida)
		Puntero_Menu = new Menu();
}

void Inicializar(int index)
{
	// ELIMINAR TODOS LOS OBJETOS Y VACIAR TODAS LAS ESTRUCTURAS DE ALMACENAMIENTO

    for(int i = 0; i < (int)joints.size(); i ++) {
        delete joints[i];
    }
    joints.clear();

    for(int i = 0; i < (int)bodies.size(); i ++) {
        delete bodies[i];
    }
    bodies.clear();

    for(int i = 0; i < (int)Objetos_Fisicos.size(); i ++) {
        delete Objetos_Fisicos[i];
    }
	Objetos_Fisicos.clear();

    for(int i = 0; i < (int)Disparos.size(); i ++) {
		if(Disparos[i] != NULL)
			delete Disparos[i];
    }
	Disparos.clear();

    for(int i = 0; i < (int)Caracoles.size(); i ++) {
		if(Caracoles[i] != NULL)
			delete Caracoles[i];
    }
	Caracoles.clear();

	 for(int i = 0; i < (int)Sprites_Efectos.size(); i ++) {
		if(Sprites_Efectos[i] != NULL)
			delete Sprites_Efectos[i];
    }
	Sprites_Efectos.clear();
	
	Puntero_Nave=NULL;
	bomb = NULL;

	if(Puntero_Agua != NULL)
	{
		delete Puntero_Agua;
		Puntero_Agua = NULL;
	}

	if(Puntero_Menu != NULL)
		delete Puntero_Menu;
	Puntero_Menu = NULL;

	// DESCARGAR RECURSOS (muy importante para no dejar memoria ocupada e inutilizable)
	// Solo hace falta los bitmaps cargados explícitamente; la clase Objeto_Fisico ya se ocupa
	// de descargar el resto de gráficos cargados

	destroy_bitmap(Imagen_Fondo);	// Imagen de fondo
	Imagen_Fondo=NULL;
	destroy_bitmap(Sprite_Explosion);	// Explosión
	Sprite_Explosion=NULL;
	destroy_bitmap(Sprite_Splash);	// Salpicadura del agua
	Sprite_Splash=NULL;
	destroy_bitmap(Sprite_Propuls);	// Propulsor de agua
	Sprite_Propuls=NULL;

	world.Clear();

	// SONIDOS
    destroy_sample(Splash_Grande);
	destroy_sample(Propulsor);
	destroy_sample(Disparo);
	destroy_sample(Explosion);
	destroy_sample(Cargando);
	destroy_sample(Caida);

	// Ponemos a NULL todos los punteros a sonidos para evitar problemas
    Splash_Grande = NULL;
	Propulsor = NULL;
	Disparo = NULL;
	Explosion = NULL;
	Cargando = NULL;
	Caida = NULL;

	// Reiniciamos la partida
	Iniciar_Partida();
}

// Calcula el nivel actual
void Controla_Nivel()
{
	// Cargamos las puntuaciones "límite" en un vector
	int Cambios[5] = {700, 1700, 2700, 4000, 5000};

	int i;
	for(i = 0; i<5 && Puntuacion > Cambios[i]; i++); // Buscamos el rango de puntuaciones en que se encuentra

	// Modificamos el nivel
	Nivel = i+1;
}

// Función que se ocupa de actualizar la lógica y la física del juego
void Simulate()
{
	// Si se está en uno de esos estados, no hacemos nada.
	if(Pausa || Bienvenida || Muerto)
		return;

	if(Puntero_Menu != NULL)
	{
		delete Puntero_Menu;
		Puntero_Menu = NULL;
	}

	// Gestión de la parte de teclado que controla la nave del jugador
	// (esta parte no es nada recomendable hacerla así; todo lo de teclado debería estar junto)

	if (key[Teclas["down"] >> 8])	// Propulsión hacia arriba
	{
		if(Puntero_Nave!=NULL)
		{
			if(!Propulsor_Activo)
			{
				play_sample(Propulsor, 255, 0, 500, 1);
				Propulsor_Activo=true;
			}
		}
	}
	else
	{
		if(Propulsor_Activo)
		{
			stop_sample(Propulsor);
			Propulsor_Activo = false;
		}
	}
	if (key[Teclas["left"] >> 8] || key[Teclas["right"] >> 8])	// Movimiento lateral
	{
		if(Puntero_Nave!=NULL)
		{
			if(!Disparo_Activo)
			{
				Vol_Disparo=240;
				play_sample(Disparo, Vol_Disparo, 0, 1000, 1);
				Disparo_Activo=true;
			}
		}
	}
	else
	{
		if(Disparo_Activo)
		{
			if(Vol_Disparo > 10)
			{
				Vol_Disparo-=2;
				adjust_sample(Disparo, Vol_Disparo, 0, 1000, 1);
			}
			else
			{
				stop_sample(Disparo);
				Disparo_Activo = false;
			}
		}
	}

	Controla_Nivel();	// Actualizar el nivel actual en función de la puntuación.
	world.Step(0.05);	// Actualizar las físicas

	if(Puntero_Agua != NULL)
		Puntero_Agua->Actualiza();	// Actualizar la lógica del agua

	if(Puntero_Nave!=NULL)
		Puntero_Nave->Actualiza();	// Actualizar la lógica de la nave.
	
	// Actualizar la lógica de los caracoles
	for(vector<Objeto_Caracol *>::iterator i = Caracoles.begin(); i!=Caracoles.end(); i++)
	{
		if((*i)!=NULL)	// Si no ha sido destruido
		{
			(*i)->Actualiza();	// Actualizar
		}
	}

	// LÓGICA DE LAS COLUMNAS

// <revisar> El valor inicial de Altura_Maxima podría fallar al cambiar la resolución </revisar>

	int Altura_Maxima=1000;	// Altura de la columna más alta; 
							// se utilizará para calcular la altura del techo.

	// Actualizar valor de Altura_Maxima (tener en cuenta que el origen de coordenadas está en la esquina superior izquierda)
	for(int i=0; i<Columnas.size(); i++) // Para todas las columnas
		if(Columnas[i]->Puntero_Box->position.y<Altura_Maxima)	// Si la columna i está más alta que la máxima
			Altura_Maxima=Columnas[i]->Puntero_Box->position.y;	// Actualizar Altura_Maxima

	// SI es necesario, bajamos las columnas de los extremos (que harán que el techo baje por su propio peso)
	if(Columna_Izquierda->Puntero_Box->position.y < Altura_Maxima)
	{
		play_sample(Caida, 255, 0, 1000, 0);	// Sonido del techo bajando
		Columna_Izquierda->Puntero_Box->position.y=Altura_Maxima;
		Columna_Derecha->Puntero_Box->position.y=Altura_Maxima;
	}

// <revisar> El valor inicial de Altura_Maxima podría fallar al cambiar la resolución </revisar>

	// Comprobamos si el jugador ha muerto en función de la posición del techo
	if(Altura_Maxima > 600)
		Muerto = true;

	// Creación de caracoles
	if(rand()%(210-Nivel*10)==0) // Aleatorio, siendo más probable que aparezca cuanto más alto es el nivel
	{
		Objeto_Caracol *Puntero_Caracol = new Objeto_Caracol(); // Crear un Objeto_Caracol
		Caracoles.push_back(Puntero_Caracol);					// Lo almacenamos
		Puntero_Agua->Nuevo_Cuerpo(Puntero_Caracol->Puntero_Box);
	}
}

// Función que destruye un disparo y lo elimina de todas las estructuras de almacenamiento
void Destruye_Efecto(vector<Sprite_Efecto *>::iterator Efecto)
{
	delete *Efecto; // Eliminamos el objeto
	*Efecto=NULL; // Ponemos a NULL este elemento del vector hasta que se resetee.
}

// Función que destruye un disparo y lo elimina de todas las estructuras de almacenamiento
void Destruye_Disparo(vector<Objeto_Disparo *>::iterator Disparo)
{
	vector<Body *>::iterator i;	// iterador para recorrer las diferentes estructuras de almacenamiento
	Body *Puntero_Box=(*Disparo)->Puntero_Box; // Puntero a la caja de colisiones del disparo

	for(i = bodies.begin(); i != bodies.end() && (*i) != Puntero_Box; i ++); // buscamos el elemento en el vector de cuerpos
	if(i != bodies.end())
		bodies.erase(i);// Lo eliminamos del vector de cuerpos de main (para que no se dibuje)

	for(i = world.bodies.begin(); i != world.bodies.end() && (*i) != Puntero_Box; i ++); // buscamos el elemento en el vector de cuerpos del world
	if(i != world.bodies.end())
		world.bodies.erase(i); // lo eliminamos para que no se tenga en cuenta en los calculos

	delete *Disparo;	// Eliminamos el objeto
	*Disparo=NULL;		// Ponemos a NULL este elemento del vector hasta que se resetee.
						// (me daba problemas eliminar tantos disparos tan rápido, así que opté por ponerlos a NULL temporalmente
						// y eliminarlos todos juntos cada cierto tiempo)
}

// Función que destruye un caracol y lo elimina de todas las estructuras de almacenamiento
// el funcionamiento es completamente análogo al de Destruye_Disparo
void Destruye_Caracol(vector<Objeto_Caracol *>::iterator Caracol)
{
	vector<Body *>::iterator i;
	Body *Puntero_Box=(*Caracol)->Puntero_Box;

	for(i = bodies.begin(); i != bodies.end() && (*i) != Puntero_Box; i ++);
	if(i != bodies.end())
		bodies.erase(i);// Lo eliminamos del vector de cuerpos de main (para que no se dibuje)

	for(i = world.bodies.begin(); i != world.bodies.end() && (*i) != Puntero_Box; i ++);
	if(i != world.bodies.end())
		world.bodies.erase(i); // y del de world (para que no se tenga en cuenta en los calculos)

	Puntero_Agua->Eliminar_Cuerpo((*Caracol)->Puntero_Box);
	delete *Caracol; // Eliminamos el objeto
	*Caracol=NULL;	// Ponemos a NULL este elemento del vector hasta que se resetee.
					// (me daba problemas eliminar tantos disparos tan rápido, así que opté por ponerlos a NULL temporalmente
					// y eliminarlos todos juntos cada cierto tiempo)
}

// Vacía completamente el vector que almacena los disparos
void Resetear_Vector_Disparos()
{
	for(vector<Objeto_Disparo *>::iterator i = Disparos.begin(); i!=Disparos.end(); i++)
	{	
			if((*i)!=NULL)
				Destruye_Disparo(i);
	}
	Disparos.clear();
}

// Vacía completamente el vector que almacena los efectos especiales
void Resetear_Vector_Efectos()
{
	for(vector<Sprite_Efecto *>::iterator i = Sprites_Efectos.begin(); i!=Sprites_Efectos.end(); i++)
	{	
			if((*i)!=NULL)
				Destruye_Efecto(i);
	}
	Sprites_Efectos.clear();
}

// Vacía completamente el vector que almacena los caracoles
void Resetear_Vector_Caracoles()
{
	for(vector<Objeto_Caracol *>::iterator i = Caracoles.begin(); i!=Caracoles.end(); i++)
	{	
			if((*i)!=NULL)
				Destruye_Caracol(i);
	}
	Caracoles.clear();
}

// Dibuja la barra que indica el agua restante
void Dibuja_Barra_Agua()
{
	Body *Box=Puntero_Nave->Puntero_Box; // Obtener el puntero a la caja de colisiones de la nave

	// Posición horizontal de la barra de agua restante
	int X_Origen_MarcadorAgua=Box->position.x+(Lado_Oscuro ? 1 : -1)*Box->width.x; // La colocamos en el lado oscuro
	int Y_Origen_MarcadorAgua=Box->position.y-Box->width.x;	// Posición vertical de la barra

	// Calculamos la cantidad de relleno de la barra
	int Altura_Barra = (float(Agua_Restante)/NIVEL_MAXIMO_AGUA)*ALTO_BARRA_AGUA; // Porcentaje de agua restante

	// dibujar el relleno del rectángulo
	rectfill(swap_screen,												// se dibujará en la swap_screen
		X_Origen_MarcadorAgua,											// Esquina inferior izquierda
		Y_Origen_MarcadorAgua+ALTO_BARRA_AGUA-Altura_Barra, 
		X_Origen_MarcadorAgua+(Lado_Oscuro ? 1 : -1)*ANCHO_BARRA_AGUA,	// Esquina superior derecha
		// Y_Origen_MarcadorAgua+ALTO_BARRA_AGUA, makecol(0,169,255));
		Y_Origen_MarcadorAgua+ALTO_BARRA_AGUA, 
		makecol(255*(1-float(Agua_Restante)/NIVEL_MAXIMO_AGUA),169*(float(Agua_Restante)/NIVEL_MAXIMO_AGUA),255*(float(Agua_Restante)/NIVEL_MAXIMO_AGUA)));

	// dibujar el contorno del rectángulo
	rect(swap_screen,
		X_Origen_MarcadorAgua, 
		Y_Origen_MarcadorAgua, 
		X_Origen_MarcadorAgua+(Lado_Oscuro ? 1 : -1)*ANCHO_BARRA_AGUA,
		Y_Origen_MarcadorAgua+ALTO_BARRA_AGUA, makecol(0,90,255));
}

// draw a rectangle
// since we use it three times in the editor, it would be nice to have
// a function for it.
// Esta función la encontré por internet; aunque está muy bien hecha, tiene un funcionamiento bastante sencillo
void drawRectangle(BITMAP *bmp,int x, int y, int w,int h,int color, int style, int bgcolor)
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

// Realiza todas las tareas de dibujo.
void Render()
{
	char Cadena_Numero[20];
	string Aux;
    clear_to_color(swap_screen, makecol(245,245,255));

	if(Imagen_Fondo!=NULL)
		draw_sprite(swap_screen, Imagen_Fondo, 0, 0);

	// Dibujar objetos físicos
    for(int i = 0; i < (int)Objetos_Fisicos.size(); i ++) {
		Objetos_Fisicos[i]->Dibuja(swap_screen);
    }
	// Dibujar Efectos
    for(int i = 0; i < (int)Sprites_Efectos.size(); i ++) {
		Sprites_Efectos[i]->Dibuja(swap_screen);
    }

	Body *P_N = Puntero_Nave->Puntero_Box;
	if(key[Teclas["down"] >> 8] && Agua_Restante > 0)
	{
		Alfa_Propuls = 255;
		set_trans_blender(0, 0, 0, Alfa_Propuls);
		// Dibujamos en la pantalla el objeto.
		draw_trans_sprite(swap_screen, Sprite_Propuls, P_N->position.x-Sprite_Propuls->w/2,
			P_N->position.y+Sprite_Propuls->h/2-2);
		set_trans_blender(0, 0, 0, 255);
	}
	else
	{
		Alfa_Propuls-=4;
		if(Alfa_Propuls>10)
		{
			set_trans_blender(128, 128, 128, Alfa_Propuls);
			// Dibujamos en la pantalla el objeto.
			draw_trans_sprite(swap_screen, Sprite_Propuls, P_N->position.x-Sprite_Propuls->w/2,
				P_N->position.y+Sprite_Propuls->h/2-2);
			set_trans_blender(0, 0, 0, 255);
		}
	}

	if(Puntero_Nave!=NULL)
	{
		Dibuja_Barra_Agua();
	}
	// Actualizar disparos
	for(vector<Objeto_Disparo *>::iterator i = Disparos.begin(); i!=Disparos.end(); i++)
	{	
		// Si no ha sido destruido
		if((*i)!=NULL)
		{
			(*i)->Actualiza();	// Lo Actualizar
			(*i)->Dibuja(swap_screen);	// Lo dibujamos
			if((*i)->Destruir)	// Si el objeto se ha automarcado para ser destruido
			{
				Destruye_Disparo(i); // Eliminamos el objeto y ponemos a NULL este elemento del vector
			}
		}
	}
	// Dibujar caracoles
	for(vector<Objeto_Caracol *>::iterator i = Caracoles.begin(); i!=Caracoles.end(); i++)
	{	
		// Si no ha sido destruido
		if((*i)!=NULL)
		{
			(*i)->Dibuja(swap_screen);	// Lo dibujamos
			if((*i)->Destruir)	// Si el objeto se ha automarcado para ser destruido
			{
				Destruye_Caracol(i); // Eliminamos el objeto y ponemos a NULL este elemento del vector
			}
		}
	}

	if(Puntero_Agua!=NULL)
		Puntero_Agua->Dibuja(swap_screen);

	// Por un problema con las estructuras de almacenamiento STL, tengo que vaciar el vector 
	// entero cada cierto tiempo 
	if(Disparos.size()>100)
	{
		Resetear_Vector_Disparos();
	}
/*	if(Caracoles.size()>200)
	{
		Resetear_Vector_Caracoles();
	}*/
	if(Sprites_Efectos.size()>100)
	{
		Resetear_Vector_Efectos();
	}

	// Acciones especiales del modo debug
    if (Debug) {
		// Dibujar cajas de colisiones
		for(int i = 0; i < (int)bodies.size(); i ++) {
			DrawBody(swap_screen, bodies[i], 0, 0);
		}
		// Dibujar uniones
        for(int i = 0; i < (int)joints.size(); i ++) {
            DrawJoint(swap_screen, joints[i], 0, 0);
        }

		// Escribir diferentes estadísticas de control
		DrawText(swap_screen, 5, 25, "Proyecto Caracol para compo 48h");
		Aux="Numero de Arbitros: ";
		_itoa_s(world.arbiters.size(), Cadena_Numero, 10);
		Aux+=Cadena_Numero;
		DrawText(swap_screen, 5, 395, (char *)Aux.c_str());
		Aux="Numero de Objetos Fisicos: ";
		_itoa_s(Objetos_Fisicos.size(), Cadena_Numero, 10);
		Aux+=Cadena_Numero;
		DrawText(swap_screen, 5, 405, (char *)Aux.c_str());
		Aux="Numero de bodies en main (dibujo): ";
		_itoa_s(bodies.size(), Cadena_Numero, 10);
		Aux+=Cadena_Numero;
		DrawText(swap_screen, 5, 415, (char *)Aux.c_str());
		Aux="Numero de bodies en world (calculos): ";
		_itoa_s(world.bodies.size(), Cadena_Numero, 10);
		Aux+=Cadena_Numero;
		DrawText(swap_screen, 5, 425, (char *)Aux.c_str());
		Aux="Numero de disparos: ";
		_itoa_s(Disparos.size(), Cadena_Numero, 10);
		Aux+=Cadena_Numero;
		DrawText(swap_screen, 5, 435, (char *)Aux.c_str());
		Aux="Numero de caracoles: ";
		_itoa_s(Caracoles.size(), Cadena_Numero, 10);
		Aux+=Cadena_Numero;
		DrawText(swap_screen, 5, 445, (char *)Aux.c_str());
		Aux="Numero de efectos: ";
		_itoa_s(Sprites_Efectos.size(), Cadena_Numero, 10);
		Aux+=Cadena_Numero;
		DrawText(swap_screen, 5, 455, (char *)Aux.c_str());
	}

	// Dibujar textos:
#ifdef SPANISH
	DrawText(swap_screen, 5, 5, "Pulsa 'R' para reiniciar la partida.");
	Aux="Puntuacion: ";
#endif
#ifdef ENGLISH
	DrawText(swap_screen, 5, 5, "Press 'R' to restart the game.");
	Aux="Score: ";
#endif
	_itoa_s(Puntuacion, Cadena_Numero, 10);  // Convierte un entero a un array de caracteres
	Aux+=Cadena_Numero;
	DrawText(swap_screen, 450, 440, (char *)Aux.c_str());
#ifdef SPANISH
	Aux="Deposito de Agua: ";
#endif
#ifdef ENGLISH
	Aux="Water Tank: ";
#endif
	float Agua=float(Agua_Restante)/NIVEL_MAXIMO_AGUA;
	_itoa_s(100*Agua, Cadena_Numero, 10);
	Aux+=Cadena_Numero;
	Aux+="%";
	DrawText(swap_screen, 450, 450, (char *)Aux.c_str());
#ifdef SPANISH
	Aux="Nivel: ";
#endif
#ifdef ENGLISH
	Aux="Level: ";
#endif
	_itoa_s(Nivel, Cadena_Numero, 10);
	Aux+=Cadena_Numero;
	DrawText(swap_screen, 450, 460, (char *)Aux.c_str());

	// Esto no debería ir aquí, pero cuando lo hice ya faltaban pocas horas para el final del concurso xD

	/// MENU DE PAUSA
	if(Pausa)
	{
		float Opacidad = 0.7;
		const int val = (int)(Opacidad * 0xff);
		set_trans_blender(0xff, 0xff, 0xff, val);
		drawing_mode(DRAW_MODE_TRANS, 0, 0, 0);
		drawRectangle(swap_screen, 0, 0, 640, 480, makeacol(255, 255, 255, 0), 1, makeacol(255, 255, 255, 255*Opacidad));
		drawRectangle(swap_screen, 640/4, 480/4+48, 640/2, 48, makeacol(0, 99, 168, 0), 1, makeacol(0, 122, 207, 255*0.8));
		drawing_mode(DRAW_MODE_SOLID, 0, 0, 0);
#ifdef SPANISH
		DrawText(swap_screen, 300, 180, "PAUSA");
		DrawText(swap_screen, 200, 195, "Pulsa P para volver al juego.");
		DrawText(swap_screen, 200, 205, "Pulsa ESC para salir al menú.");
#endif
#ifdef ENGLISH
		DrawText(swap_screen, 300, 180, "PAUSE");
		DrawText(swap_screen, 200, 195, "Press 'P' to resume the game.");
		DrawText(swap_screen, 200, 205, "Press 'ESC' to go to the menu.");
#endif
	}
	// MENÚ DE BIENVENIDA
	if(Bienvenida)
	{
		float Opacidad = 1.0;
		const int val = (int)(Opacidad * 0xff);
		set_trans_blender(0xff, 0xff, 0xff, val);
		drawing_mode(DRAW_MODE_TRANS, 0, 0, 0);
		drawRectangle(swap_screen, 0, 0, 640, 480, makeacol(255, 255, 255, 0), 1, makeacol(255, 255, 255, 255*Opacidad));
//		drawRectangle(swap_screen, 640/4, 480/4, 640/2, 480/2, makeacol(0, 99, 168, 0), 1, makeacol(0, 122, 207, 255*0.8));
		drawing_mode(DRAW_MODE_SOLID, 0, 0, 0);
/*		DrawText(swap_screen, 280, 180-48, "BIENVENIDO");
		DrawText(swap_screen, 175, 195, "Pulsa C para empezar a jugar.");
		DrawText(swap_screen, 175, 205, "Pulsa ESC para salir del juego.");

		DrawText(swap_screen, 175, 225, "CONTROLES:");
		DrawText(swap_screen, 175, 233, "==========");

		DrawText(swap_screen, 175, 245, "  - Izquierda/Derecha > Disparar.");
		DrawText(swap_screen, 175, 255, "  - Abajo > Impulsarse hacia arriba.");
		DrawText(swap_screen, 175, 265, "  - Arriba > Dejarse caer.");
		DrawText(swap_screen, 175, 275, "  - P > Pausa.");
		DrawText(swap_screen, 175, 285, "  - Recarga el agua aterrizando.");

		DrawText(swap_screen, 175, 295, "OBJETIVO Y REGLAS:");
		DrawText(swap_screen, 175, 303, "==================");

		DrawText(swap_screen, 175, 315, "  - Evita que los caracoles suban.");
		DrawText(swap_screen, 175, 325, "  - Si el techo baja demasiado,");
		DrawText(swap_screen, 175, 335, "     perderas.");*/
	}
	// MENÚ DE MUERTO
	if(Muerto)
	{
		float Opacidad = 0.7;
		const int val = (int)(Opacidad * 0xff);
		set_trans_blender(0xff, 0xff, 0xff, val);
		drawing_mode(DRAW_MODE_TRANS, 0, 0, 0);
		drawRectangle(swap_screen, 0, 0, 640, 480, makeacol(255, 255, 255, 0), 1, makeacol(255, 255, 255, 255*Opacidad));
		drawRectangle(swap_screen, 640/4, 480/4, 640/2, 480/2, makeacol(0, 99, 168, 0), 1, makeacol(0, 122, 207, 255*0.8));
		drawing_mode(DRAW_MODE_SOLID, 0, 0, 0);
#ifdef SPANISH
		DrawText(swap_screen, 280, 180-48, "HAS PERDIDO");
		Aux="Puntuacion: ";
#endif
#ifdef ENGLISH
		DrawText(swap_screen, 280, 180-48, "YOU LOST");
		Aux="Score: ";
#endif
		_itoa_s(Puntuacion, Cadena_Numero, 10);
		Aux+=Cadena_Numero;
		DrawText(swap_screen, 200, 200, (char *)Aux.c_str());
#ifdef SPANISH
	DrawText(swap_screen, 5, 5, "Pulsa 'R' para reiniciar la partida.");
	Aux="Puntuacion: ";
#endif
#ifdef ENGLISH
	DrawText(swap_screen, 5, 5, "Press 'R' to restart the game.");
	Aux="Score: ";
#endif
	}

	if(Puntero_Menu != NULL)
	{
		Puntero_Menu -> Dibujar(swap_screen);
		Puntero_Menu -> Actualizar();
	}

    acquire_screen();	// Bloquear la pantalla antes de dibujar
    blit(swap_screen, screen, 0, 0, 0, 0, 640, 480); // Copiar todo desde swap_screen hasta la pantalla (screen)
    release_screen();	// La dejamos libre de nuevo
}

// Reduce la altura de la columna indicada
void Baja_Columna(int Columna)
{
	if(Columna>Columnas.size())	// Si la columna indicada no es correcta
		// <revisar> Error, columnas no válido </revisar>
		return;

	// Bajamos la altura de la columna

// <revisar> La cantidad que baja la columna debería ir en función de la resolución </revisar>
	Columnas[Columna]->Puntero_Box->position.y+=40;
}

// Controla el teclado
void Keyboard()
{
	// Lectura de teclas sin bloqueo
	//int Aux = Config_Teclas[0].Id_Tecla >> 8;
	if (key[Teclas["down"] >> 8])
		if(Puntero_Nave!=NULL)
			Puntero_Nave->Sube();


	// Disparo
	if (key[Teclas["left"] >> 8] && Puntero_Nave!=NULL)
	{
		Lado_Oscuro=true;
		Puntero_Nave->Disparo_Lateral(false);
	}
	if (key[Teclas["right"] >> 8] && Puntero_Nave!=NULL)
	{
		Lado_Oscuro=false;
		Puntero_Nave->Disparo_Lateral(true);
	}

	// Lecturas de teclas con bloqueo
    int kp = readkey() >> 8;

	if (kp == KEY_ESC && !Bienvenida) {Bienvenida = true; Muerto = false; Pausa = false; Inicializar(9); }
	if (kp == KEY_TAB) Debug = !Debug;
	if (kp == KEY_P) Pausa = !Pausa;
	if (kp == KEY_R) { Inicializar(9); Muerto = false; Pausa = false; Bienvenida = false; }
	if (kp == KEY_SPACE) Puntero_Agua->Modifica_Amplitud(50);
}

// Función que recibe los mensajes del menú (pulsaciones en botones por ejemplo)
// Ante ciertos mensajes podrá actuar en consecuencia
void Menu_Listener(string msg)
{
	if(msg == "start UP")
	{
		if (Bienvenida){ Bienvenida = false; Lee_Configuracion_Teclado(); }
	}
	else if(msg == "exit UP")
	{
		Salir = true;
	}
}
/*
int main()
{
  GLYPH_FACE *face;
  GLYPH_REND *rend;

  allegro_init();
  set_color_depth(32);
  set_gfx_mode(GFX_AUTODETECT,800,600,0,0);
  clear_to_color(screen,makecol(255,255,255));
  install_keyboard();

  face = gk_load_face_from_file("arial.ttf",0);
  if (!face) { allegro_message("Can't load font!"); exit(1); }
  rend = gk_create_renderer(face,0);
  if (!rend) { allegro_message("Can't create renderer!"); exit(1); }
  gk_rend_set_size_pixels(rend,32,32);
  gk_rend_set_text_color(rend,29,102,185);

  gk_render_line_utf8(screen,rend,"Hello, World!",50,50);

  readkey();
  return 0;
}
END_OF_MAIN()
*/
// Función main
void main(int argc, char** argv)
{
    allegro_init();
	set_color_depth(16);
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0);
	set_window_title("Snail Alert 2: The Revenge");

	srand ( time(NULL) );

	PALETTE palette;

	load_bitmap("graficos\\nave.pcx", palette);
	set_palette(palette);

	COLOR_MAP global_trans_table;
    create_trans_table(&global_trans_table, palette, 128, 128, 128, NULL);

	install_keyboard();
	install_mouse();
	char * p=NULL;
	install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, p);

	// install timers
	install_timer();
	LOCK_VARIABLE(cycle_count);
	LOCK_VARIABLE(logic_count);
	LOCK_VARIABLE(lps);
	LOCK_VARIABLE(fps);
	LOCK_VARIABLE(frame_count);
	srand(time(NULL));

	LOCK_FUNCTION(fps_counter);
	install_int(fps_counter, 1000);
	fps = 0;
	frame_count = 0;
	cycle_count = 0;

	LOCK_FUNCTION(cycle_counter);
	install_int(cycle_counter, 10);
	game_count = 0;

	swap_screen = create_bitmap(640, 480);
	if (!swap_screen) {
	    allegro_message("failed to create swap screen");
	    exit(-1);
	}

	Inicializar(9);


    clear_keybuf();
    int needToDraw = 1;
    cycle_count = 0;

	// BUCLE PRINCIPAL
	while(!Salir) {
		while(cycle_count > 0) {
			logic_count ++;

            // input
            if (keypressed()) Keyboard();

            // do logic
            Simulate();

            // yield
            rest(0);

			needToDraw = 1;
			cycle_count --;
		}
		// draw
		if (needToDraw) {
			frame_count ++;

			Render();

			needToDraw = 0;
		}
	}

	allegro_exit();
}
END_OF_MAIN()
