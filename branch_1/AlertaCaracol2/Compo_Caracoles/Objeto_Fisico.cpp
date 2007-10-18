#include "Objeto_Fisico.h"

#include <allegro.h>
#include <string>

#include "Body.h"
#include "World.h"

using namespace std;

// Obtener variables globales
extern World world;
extern vector<Body *> bodies;
extern void Aborta_Con_Error(string msg);

// Constructor por defecto (no se usa)
Objeto_Fisico::Objeto_Fisico(void): Puntero_Box(NULL), Grafico_Objeto(NULL)
{
}

// Constructor "útil" de la clase. Recibe:
// Ruta_Fichero	Ruta del gráfico que se utilizará para el objeto
// Peso			Peso que se le dará. Tiene 10.0 como valor por defecto
// Ancho, Alto	Tamaño del gráfico. Si no se indica nada, se le dará el ancho y alto del gráfico.		
Objeto_Fisico::Objeto_Fisico(string Ruta_Fichero, float Peso, int Ancho, int Alto)
{
	Inicializa(Ruta_Fichero, Ancho, Alto);	// Inicializamos según los parámetros indicados
	Puntero_Box->mass=Peso;					// Le damos el peso indicado
}

// Destructor
Objeto_Fisico::~Objeto_Fisico(void)
{
	// Liberamos la memoria ocupada por el gráfico cargado.
	if(Grafico_Objeto != NULL)
		destroy_bitmap(Grafico_Objeto);
}

// Carga los recursos adecuados e inicializa las variables del objeto
void Objeto_Fisico::Inicializa(string Ruta_Fichero, float Peso, int Ancho, int Alto)
{
	PALETTE palette;	// Variable auxiliar
	
	// Cargamos en memoria el gráfico indicado
	Grafico_Objeto=load_bitmap((char *)Ruta_Fichero.c_str(), palette);

	// <revisar> Controlar mejor error en la carga del fichero </revisar>

	if(!Grafico_Objeto)
		Aborta_Con_Error("ERROR:\nError de ejecucion en la funcion Inicializa() de Objeto_Fisico.cpp.\n - Error al cargar el grafico " + Ruta_Fichero);

	// Si no se ha indicado nada, la caja de colisiones tomará el mismo tamaño que el gráfico
	if(Ancho==0)
		Ancho=Grafico_Objeto->w;
	if(Alto==0)
		Alto=Grafico_Objeto->h;

	// Creamos la caja de colisiones asociada
	Puntero_Box = new Body();
	Puntero_Box->Set(Vec2(Ancho, Alto), Peso); // Tamaño
	Puntero_Box->position.Set(320, 240); // Posición
    Puntero_Box->friction = 0.5; // Fricción del objeto
	world.Add(Puntero_Box); // La añadimos al "mundo"
	bodies.push_back(Puntero_Box); // Y la añadimos al vector que almacena todos los cuerpos
}

// Dibuja el objeto en pantalla
void Objeto_Fisico::Dibuja(BITMAP * Pantalla)
{
	// Dibujamos en la pantalla el objeto.
	rotate_sprite(Pantalla,											// Se dibujará en el bitmap indicado
				Grafico_Objeto,										// bitmap a dibujar
				Puntero_Box->position.x-Grafico_Objeto->w/2,		// Posición
				Puntero_Box->position.y-Grafico_Objeto->h/2,
				itofix((256*Puntero_Box->rotation)/(2*3.141516)));	// Rotación
}

