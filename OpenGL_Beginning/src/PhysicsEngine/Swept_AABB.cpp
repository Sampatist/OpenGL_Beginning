#include "Swept_AABB.h"
#include <limits>
#include <algorithm>
#include "PhysicsObject.h"
#include <iostream>

// döndü = collision time , normal info
CollisionInfo SweptAABB(const PhysicsObject& dynamic, const HitBox& staTHICC)
{
	float xDisEntry, yDisEntry, zDisEntry;
	float xDisExit,  yDisExit,  zDisExit;

	glm::vec3 v = dynamic.getVelocity();
	HitBox dynaMHICC = dynamic.getHitBox();

	// find the distance between the objects on the near and far sides for both x and y 
	if (v.x > 0.0f)
	{
		xDisEntry = staTHICC.x - (dynaMHICC.x + dynaMHICC.w);
		xDisExit  = (staTHICC.x + staTHICC.w) - dynaMHICC.x;
	}
	else
	{
		xDisEntry = (staTHICC.x + staTHICC.w) - dynaMHICC.x;
		xDisExit  = staTHICC.x - (dynaMHICC.x + dynaMHICC.w);
	}

	if (v.y > 0.0f)
	{
		yDisEntry = staTHICC.y - (dynaMHICC.y + dynaMHICC.h);
		yDisExit  = (staTHICC.y + staTHICC.h) - dynaMHICC.y;
	}
	else
	{
		yDisEntry = (staTHICC.y + staTHICC.h) - dynaMHICC.y;
		yDisExit  = staTHICC.y - (dynaMHICC.y + dynaMHICC.h);
	}

	if (v.z > 0.0f)
	{
		zDisEntry = staTHICC.z - (dynaMHICC.z + dynaMHICC.d);
		zDisExit  = (staTHICC.z + staTHICC.d) - dynaMHICC.z;
	}
	else
	{
		zDisEntry = (staTHICC.z + staTHICC.d) - dynaMHICC.z;
		zDisExit  = staTHICC.z - (dynaMHICC.z + dynaMHICC.d);
	}

	//calculate times 

	float xEntry, yEntry, zEntry;
	float xExit,  yExit,  zExit;

	if (v.x == 0.0f)
	{
		xEntry = -std::numeric_limits<float>::infinity();
		xExit  =  std::numeric_limits<float>::infinity();
	}
	else
	{
		xEntry = xDisEntry / v.x;
		xExit  = xDisExit  / v.x;
	}

	if (v.y == 0.0f)
	{
		yEntry = -std::numeric_limits<float>::infinity();
		yExit  =  std::numeric_limits<float>::infinity();
	}
	else
	{
		yEntry = yDisEntry / v.y;
		yExit  = yDisExit  / v.y;
	}

	if (v.z == 0.0f)
	{
		zEntry = -std::numeric_limits<float>::infinity();
		zExit  =  std::numeric_limits<float>::infinity();
	}	
	else
	{
		zEntry = zDisEntry / v.z;
		zExit  = zDisExit  / v.z;
	}

	//calculate last entry time and first exit time

	float entryTime = std::max({ xEntry, yEntry, zEntry });

	float exitTime = std::min({ xExit, yExit, zExit });
	//printf("Times: %.2f %.2f\n",entryTime, exitTime);
	float normalx, normaly, normalz;

	// if there was no collision
	if (entryTime > exitTime || xEntry < 0.0f && yEntry < 0.0f && zEntry < 0.0f || xEntry > 1.0f || yEntry > 1.0f || zEntry > 1.0f)
	{
		normalx = 0.0f;
		normaly = 0.0f;
		normalz = 0.0f;

		return {1.0f, glm::vec3(normalx, normaly, normalz)};
	}
	else // if there was a collision 
	{
		// calculate normal of collided surface
		if (xEntry == entryTime)
		{
			if (xDisEntry < 0.0f)
			{
				normalx = 1.0f;
				normaly = 0.0f;
				normalz = 0.0f;
			}
			else
			{
				normalx = -1.0f;
				normaly = 0.0f;
				normalz = 0.0f;
			}
		}
		else if (yEntry == entryTime)
		{
			if (yDisEntry < 0.0f)
			{
				normalx = 0.0f;
				normaly = 1.0f;
				normalz = 0.0f;
			}
			else
			{
				normalx = 0.0f;
				normaly = -1.0f;
				normalz = 0.0f;
			}
		} 
		else
		{
			if (zDisEntry < 0.0f)
			{
				normalx = 0.0f;
				normaly = 0.0f;
				normalz = 1.0f;
			}
			else
			{
				normalx = 0.0f;
				normaly = 0.0f;
				normalz = -1.0f;
			}
		}

		return {entryTime, glm::vec3(normalx, normaly, normalz)};
	}
}
