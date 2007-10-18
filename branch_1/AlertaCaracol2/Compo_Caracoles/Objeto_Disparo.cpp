#include "Objeto_Disparo.h"

#include <allegro.h>
#include <string>

#include "Body.h"
#include "Joint.h"
#include "World.h"
#include "Objeto_Paracaidas.h"
#include "Objeto_Disparo.h"

using namespace std;

// Obtener variables globales
extern vector<Joint *> joints;
extern vector<Body *> bodies;
extern vector<Objeto_Disparo *> Disparos;
extern vector<Objeto_Fisico *> Objetos_Fisicos;
extern World world;

// Constructor por defecto, este no se usa para nada en principio
Objeto_Disparo::Objeto_Disparo(void): Destruir(false)
{
}

// Constructor que se usa de forma habitual
Objeto_Disparo::Objeto_Disparo(int X, int Y, Vec2 Direccion) : Destruir(false)
{
	// Cargar el gráfico adecuado (la función Inicializa es de la clase padre)
	Inicializa("graficos\\disparo.pcx");

	// Situamos la caja de colisiones en la posición indicada
	Puntero_Box->position.x=X;
	Puntero_Box->position.y=Y;

	// Le damos un impulso inicial en la dirección indicada
	Puntero_Box->AddForce(8000.0f*Direccion);
}

Objeto_Disparo::~Objeto_Disparo(void)
{
}

void Objeto_Disparo::Actualiza()
{
	if(Destruir)	// Nunca se debería llegar a llamar a esta función con Destruir==true
		return;		// pero aseguramos que no haga nada en ese caso

	if(Puntero_Box->position.y>436 || Puntero_Box->velocity.y<0) // Si el disparo ha llegado al suelo
	{
		Destruir=true;
	}

}
