#pragma once
#include "objeto_fisico.h"

#define NIVEL_MAXIMO_AGUA int(500)

// Declaraciones anticipadas
class Objeto_Paracaidas;
class World;

// Clase que representa la nave del jugador
class Objeto_Nave :
	public Objeto_Fisico
{
friend class World;
public:
	Objeto_Nave(void);
	~Objeto_Nave(void);

	void Sube(); // M�todo que se llama cuando se pulsa la tecla de subir
	void Disparo_Lateral(bool Derecha); // M�todo que se llama cuando se pulsa la tecla de subir
	void Actualiza(); // M�todo que actuliza ciertos comportamientos de la nave

protected:
	void Crea_Cuerda(int Desplazamiento_Horizontal);	// Funci�n auxiliar que crea una uni�n entre la nave y el paraca�das
	Objeto_Paracaidas * Puntero_Paracaidas1;			// Puntero al paracaidas de la nave
};
