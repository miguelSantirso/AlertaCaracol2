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

#include "World.h"
#include "Arbiter.h"
#include "Body.h"
#include "Joint.h"

#include "Objeto_Nave.h"
#include "Objeto_Paracaidas.h"
#include "Objeto_Fisico.h"

using std::vector;
using std::set;

//////////////////
// <añadido>
extern vector<Objeto_Fisico *> Columnas;
extern Objeto_Nave *Puntero_Nave;
// </añadido>
//////////////////

typedef set<Arbiter>::iterator ArbIter;

void World::Add(Body* body)
{
	bodies.push_back(body);
}

void World::Add(Joint* joint)
{
	joints.push_back(joint);
}

void World::Clear()
{
	bodies.clear();
	joints.clear();
	arbiters.clear();
}

void World::BroadPhase()
{
	// O(n^2) broad-phase
	for (int i = 0; i < (int)bodies.size(); ++i)
	{
		Body* bi = bodies[i];
		if(bi == Puntero_Nave->Puntero_Box)
			continue;

		for (int j = i + 1; j < (int)bodies.size(); ++j)
		{
			Body* bj = bodies[j];
		
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// <añadido>
			// código (chapuza) para que la nave ni el paracaidas choquen con las columnas
			Body* b_jugador=NULL;
			Body* b_paracaidas=NULL;
			Body* b_columna=NULL;
			if(Puntero_Nave!=NULL)
			{
				b_jugador = Puntero_Nave->Puntero_Box;
				b_paracaidas = Puntero_Nave->Puntero_Paracaidas1->Puntero_Box;
			}

			bool Jugador_Columna=false;
			int k;
			for(k=0;k<Columnas.size() && bi!=Columnas[k]->Puntero_Box && bj!=Columnas[k]->Puntero_Box; k++);

			if(k!=Columnas.size())
				if(bi==b_jugador || bj==b_jugador || bi==b_paracaidas || bj==b_paracaidas)
				{
					Jugador_Columna=true;
					b_columna=Columnas[k]->Puntero_Box;
				}

			if(Jugador_Columna && (bi==b_columna || bj==b_columna))
				continue;
// </añadido>
/////////////////////////////////////////////////////////////////////////////////////////////////////////

			if (bi->invMass == 0.0f && bj->invMass == 0.0f)
				continue;

			Arbiter newArb(bi, bj);

			if (newArb.numContacts > 0)
			{
				ArbIter arb = arbiters.find(newArb);
				if (arb == arbiters.end())
				{
					arbiters.insert(newArb);
				}
				else
				{
                    Arbiter fooArb(*arb);
                    fooArb.Update(newArb.contacts, newArb.numContacts);
                    arbiters.erase(arb);
                    arbiters.insert(fooArb);
				}
			}
			else
			{
				arbiters.erase(newArb);
			}
		}
	}
}

void World::Step(float dt)
{
	float inv_dt = dt > 0.0f ? 1.0f / dt : 0.0f;

	BroadPhase();

	for (int i = 0; i < (int)bodies.size(); ++i)
	{
		Body* b = bodies[i];

		if (b->invMass == 0.0f)
			continue;

		b->velocity += dt * (gravity + b->invMass * b->force);
		b->angularVelocity += dt * b->invI * b->torque;
	}

    {
        ArbIter arb = arbiters.begin();
        while (arb != arbiters.end()) {
            Arbiter newArb(*arb);
            assert(newArb.body1 == arb->body1);
            assert(newArb.friction == arb->friction);
            newArb.PreStep(inv_dt);
            arbiters.erase(arb++);
            arbiters.insert(newArb);
        }
    }

	for (int i = 0; i < (int)joints.size(); ++i)
	{
		joints[i]->PreStep(inv_dt);
	}

	for (int i = 0; i < iterations; ++i)
	{
        ArbIter arb = arbiters.begin();
        while (arb != arbiters.end()) {
            Arbiter newArb(*arb);
            newArb.ApplyImpulse();
            arbiters.erase(arb++);
            arbiters.insert(newArb);
        }

		for (int j = 0; j < (int)joints.size(); ++j)
		{
			joints[j]->ApplyImpulse();
		}
	}

	for (int i = 0; i < (int)bodies.size(); ++i)
	{
		Body* b = bodies[i];

		b->position += dt * (b->velocity + b->biasedVelocity);
		b->rotation += dt * (b->angularVelocity + b->biasedAngularVelocity);

		// Bias velocities are reset to zero each step.
		b->biasedVelocity.Set(0.0f, 0.0f);
		b->biasedAngularVelocity = 0.0f;

		b->force.Set(0.0f, 0.0f);
		b->torque = 0.0f;
	}

	if(arbiters.size()>50)
	{
		arbiters.clear();
	}
}
