#pragma once
#include "objeto_fisico.h"


// Clase que representa los caracoles
class Objeto_Caracol :
	public Objeto_Fisico // Hereda de la clase Objeto_Fisico que dota al objeto de propiedades físicas
{
public:
	// Constructores y destructores
	Objeto_Caracol(void);
	~Objeto_Caracol(void);
	
	void Actualiza(); // Actualiza la lógica del objeto

	int Columna;		// Indica la columna en que está el caracol
	bool Parte_Derecha;	// Booleano para almacenar la parte de la columna en que se ha situado el caracol
	bool Destruir;		// Indica si el objeto se debe destruir tan pronto como sea posible
};
