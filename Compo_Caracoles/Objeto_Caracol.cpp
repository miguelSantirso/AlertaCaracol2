#include "Objeto_Caracol.h"

#include <allegro.h>

#include <vector>
#include "Sprite_Efecto.h"
#include "Body.h"
#include "Objeto_Fisico.h"
#include "MathUtils.h"
#include "Efecto_Agua.h"

using namespace std;

// Necesitamos obtener algunas variables globales.
// Esto no es muy elegante, pero lo hice así por ahorrar tiempo
extern vector<Objeto_Fisico *> Columnas;
extern vector<Sprite_Efecto *> Sprites_Efectos;
extern int Nivel;
extern int Puntuacion;
extern void Baja_Columna(int Columna);
extern SAMPLE *Splash_Grande;
extern SAMPLE *Explosion;
extern BITMAP *Sprite_Splash;
extern BITMAP *Sprite_Explosion;
extern Efecto_Agua * Puntero_Agua;

// Constructor por defecto
Objeto_Caracol::Objeto_Caracol(void) : Destruir(false)
{
	// Sortear la columna en la que aparece el caracol hasta que encontramos una válida 
	// (que no esté demasiado baja)
	do
	{
		if(Nivel < 3) // Si el nivel es menor que tres, solo puede aparecer en ciertas columnas
			Columna=rand()%Nivel;
		else
			Columna = rand()%5;

	} while(Columnas[Columna]->Puntero_Box->position.y > 620);

	Parte_Derecha=rand()%2;	// Sortear el lado de la columna en que aparecerá

	// Las columnas de los extremos solo tienen una parte válida
	if(Columna==4) Parte_Derecha = false;
	if(Columna==3) Parte_Derecha = true;

	// Cargar el gráfico adecuado (seguro que hay forma de hacer un mirror al mismo gráfico, pero no es el mejor momento para investigar)
	if(Parte_Derecha)
		Inicializa("graficos\\Caracol-der.pcx");
	else
		Inicializa("graficos\\Caracol-izq.pcx");

	// Inicializamos las propiedades físicas del caracol (masa y fricción)
	Puntero_Box->mass=2000.0f;
	Puntero_Box->friction = 0.20;

	// Situamos el caracol en la base de la columna
	Vec2 Posicion = Columnas[Columna]->Puntero_Box->position;
	Posicion.y = 400;
	Posicion.x+=(Parte_Derecha ? 1 : -1)*Columnas[Columna]->Puntero_Box->width.x/2;
	Puntero_Box->position = Posicion;
}

// Destructor, llama implícitamente al del padre así que no hay que hacer nada aquí
Objeto_Caracol::~Objeto_Caracol(void)
{
}

// Actualiza la lógica del caracol
void Objeto_Caracol::Actualiza()
{
	if(Destruir)	// Nunca se debería llegar a llamar a esta función con Destruir==true
		return;		// pero aseguramos que no haga nada en ese caso

	Body *b_col=Columnas[Columna]->Puntero_Box; // Puntero a la caja de colisiones
	
	if(abs(b_col->position.x-Puntero_Box->position.x)<30) // Si está pegado a la columna
	{
		Puntero_Box->AddForce((Parte_Derecha ? 1 : -1)*800*Vec2 (-1, 0));	// Fuerza horizontal que mantiene pegado al caracol
		Puntero_Box->AddForce((361+int(Puntuacion/5000))*Vec2 (0, -1));		// Fuerza vertical hacia arriba para que suba (depende de la puntuación)
	}

	if(Puntero_Box->position.y > Puntero_Agua->Obtener_Altura(Puntero_Box->position.x)/* Puntero_Agua->Obtener_Altura_Media()*/)	// Si el caracol ha caído al agua
	{
		// Creamos el efecto de la salpicadura de agua
		Sprite_Efecto *S = new Sprite_Efecto(Sprite_Splash, Puntero_Box->position.x, Puntero_Agua->Obtener_Altura(Puntero_Box->position.x), 5);
		Sprites_Efectos.push_back(S);
		// Reproducimos el sonido de la caída
		play_sample(Splash_Grande, 255, 0, 1000, 0);

		Destruir=true;		// Indicar que se debe destruir este objeto en cuanto se pueda
		Puntuacion+=100;	// Aumentar la puntuación
	}

	if(Puntero_Box->position.y<b_col->position.y-b_col->width.y/2+20)	// Si ha llegado a lo alto de la columna
	{
		// Creamos la explosión
		Sprite_Efecto *S = new Sprite_Efecto(Sprite_Explosion, Puntero_Box->position.x, Puntero_Box->position.y, 2);
		Sprites_Efectos.push_back(S);
		// Reproducir el sonido de la explosión
		play_sample(Explosion, 255, 0, 1000, 0);

		Baja_Columna(Columna);	// Bajar la altura de la columna
		Destruir=true;			// Indicar que se debe destruir este objeto
	}
}
