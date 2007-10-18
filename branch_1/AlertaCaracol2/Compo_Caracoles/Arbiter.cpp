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

#include "Arbiter.h"
#include "Body.h"


#define BIAS_PRESERVES_MOMENTUM	1

Arbiter::Arbiter(Body* b1, Body* b2)
{
	if (b1 < b2)
	{
		body1 = b1;
		body2 = b2;
	}
	else
	{
		body1 = b2;
		body2 = b1;
	}

	numContacts = Collide(contacts, body1, body2);

	friction = sqrtf(body1->friction * body2->friction);

}

void Arbiter::Update(Contact* newContacts, int numNewContacts)
{
	Contact mergedContacts[2];

	for (int i = 0; i < numNewContacts; ++i)
	{
		Contact* cNew = newContacts + i;
		int k = -1;
		for (int j = 0; j < numContacts; ++j)
		{
			Contact* cOld = contacts + j;
			if (cNew->feature.key == cOld->feature.key)
			{
				k = j;
				break;
			}
		}

		if (k > -1)
		{
			Contact* c = mergedContacts + i;
			Contact* cOld = contacts + k;
			*c = *cNew;
			c->Pn = cOld->Pn;
			c->Pt = cOld->Pt;
		}
		else
		{
			mergedContacts[i] = newContacts[i];
		}
	}

	for (int i = 0; i < numNewContacts; ++i)
		contacts[i] = mergedContacts[i];

	numContacts = numNewContacts;
}


void Arbiter::PreStep(float inv_dt)
{
	const float k_allowedPenetration = 0.01f;
	const float k_biasFactor = 0.8f;

	for (int i = 0; i < numContacts; ++i)
	{
		Contact* c = contacts + i;

		Vec2 r1 = c->position - body1->position;
		Vec2 r2 = c->position - body2->position;

		// Precompute normal mass, tangent mass, and bias.
		float rn1 = Dot(r1, c->normal);
		float rn2 = Dot(r2, c->normal);
		float kNormal = body1->invMass + body2->invMass;
		kNormal += body1->invI * (Dot(r1, r1) - rn1 * rn1) + body2->invI * (Dot(r2, r2) - rn2 * rn2);
		c->massNormal = 1.0f / kNormal;

		Vec2 tangent = Cross(c->normal, 1.0f);
		float rt1 = Dot(r1, tangent);
		float rt2 = Dot(r2, tangent);
		float kTangent = body1->invMass + body2->invMass;
		kTangent += body1->invI * (Dot(r1, r1) - rt1 * rt1) + body2->invI * (Dot(r2, r2) - rt2 * rt2);
		c->massTangent = 1.0f /  kTangent;

		c->bias = -k_biasFactor * inv_dt * Min(0.0f, c->separation + k_allowedPenetration);

		// Apply normal + friction impulse
		Vec2 P = c->Pn * c->normal + c->Pt * tangent;

		body1->velocity -= body1->invMass * P;
		body1->angularVelocity -= body1->invI * Cross(r1, P);

		body2->velocity += body2->invMass * P;
		body2->angularVelocity += body2->invI * Cross(r2, P);

		// Initialize bias impulse to zero.
		c->Pnb = 0.0f;
	}
}

void Arbiter::ApplyImpulse()
{
	Body* b1 = body1;
	Body* b2 = body2;

	for (int i = 0; i < numContacts; ++i)
	{
		Contact* c = contacts + i;
		c->r1 = c->position - b1->position;
		c->r2 = c->position - b2->position;

		// Relative velocity at contact
		Vec2 dv = b2->velocity + Cross(b2->angularVelocity, c->r2) - b1->velocity - Cross(b1->angularVelocity, c->r1);

		// Compute normal impulse
		float vn = Dot(dv, c->normal);
#if BIAS_PRESERVES_MOMENTUM == 0
		float dPn = c->massNormal * (-vn + c->bias);
#else
		float dPn = c->massNormal * (-vn);
#endif
		// Clamp the accumulated impulse
		float Pn0 = c->Pn;
		c->Pn = Max(Pn0 + dPn, 0.0f);
		dPn = c->Pn - Pn0;

		// Apply contact impulse
		Vec2 Pn = dPn * c->normal;

		b1->velocity -= b1->invMass * Pn;
		b1->angularVelocity -= b1->invI * Cross(c->r1, Pn);

		b2->velocity += b2->invMass * Pn;
		b2->angularVelocity += b2->invI * Cross(c->r2, Pn);

#if BIAS_PRESERVES_MOMENTUM == 1
		// Compute bias impulse
		dv = b2->biasedVelocity + Cross(b2->biasedAngularVelocity, c->r2) - b1->biasedVelocity - Cross(b1->biasedAngularVelocity, c->r1);
		float vnb = Dot(dv, c->normal);

		float dPnb = c->massNormal * (-vnb + c->bias);
		float Pnb0 = c->Pnb;
		c->Pnb = Max(Pnb0 + dPnb, 0.0f);
		dPnb = c->Pnb - Pnb0;

		Vec2 Pb = dPnb * c->normal;

		b1->biasedVelocity -= b1->invMass * Pb;
		b1->biasedAngularVelocity -= b1->invI * Cross(c->r1, Pb);

		b2->biasedVelocity += b2->invMass * Pb;
		b2->biasedAngularVelocity += b2->invI * Cross(c->r2, Pb);
#endif

		// Relative velocity at contact
		dv = b2->velocity + Cross(b2->angularVelocity, c->r2) - b1->velocity - Cross(b1->angularVelocity, c->r1);

		// Compute friction impulse
		float maxPt = friction * c->Pn;

		Vec2 tangent = Cross(c->normal, 1.0f);
		float vt = Dot(dv, tangent);
		float dPt = c->massTangent * (-vt);

		// Clamp friction
		float oldTangentImpulse = c->Pt;
		c->Pt = Clamp(oldTangentImpulse + dPt, -maxPt, maxPt);
		dPt = c->Pt - oldTangentImpulse;

		// Apply contact impulse
		Vec2 Pt = dPt * tangent;

		b1->velocity -= b1->invMass * Pt;
		b1->angularVelocity -= b1->invI * Cross(c->r1, Pt);

		b2->velocity += b2->invMass * Pt;
		b2->angularVelocity += b2->invI * Cross(c->r2, Pt);
	}
}
