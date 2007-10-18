#pragma once

// Declaraciones anticipadas
class Body;
struct BITMAP;

#include <string>

// Clase Objeto_Fisico. Esta clase es muy importante ya que será la clase de la que podrán heredar todos los objetos
// que estén regidos por las normas de la física del juego. Dota de comportamiento físico a cualquier objeto que 
// herede de esta clase.
// Proporciona además funciones para la carga/descarga del gráfico del objeto y para su dibujado en pantalla
class Objeto_Fisico
{
public:
	// Constructores
	Objeto_Fisico(void);
	Objeto_Fisico(std::string Ruta_Fichero, float Peso=10.0f, int Ancho=0, int Alto=0);

	// Destructor
	~Objeto_Fisico(void);

	// Este método permite que las clases hijas se inicialicen
	void Inicializa(std::string Ruta_Fichero, float Peso=10.0f, int Ancho=0, int Alto=0);

	void Dibuja(BITMAP * Pantalla); // Dibuja el objeto en pantalla

	Body * Puntero_Box; // Puntero a la caja de colisiones asociada.
protected:
	BITMAP * Grafico_Objeto; // Puntero al sprite que representa al objeto en pantalla
};
