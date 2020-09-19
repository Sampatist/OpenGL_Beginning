#include "PlayerController.h"
#include "Camera.h"
#include "inputs.h"
#include <iostream>
#include <algorithm>
#include "Chunk/blockEdit.h"
#include "PhysicsEngine/PhysicsObject.h"

constexpr float PI = 3.14f;

constexpr float SENSITIVITY = 0.0005f;

constexpr float SPEED = 0.6f; //0.2f feels good
constexpr float SPEED_MULTIPLIER = 5.5f;  //dont know

constexpr float YAW_ANGLE_UPPER_LIMIT = PI/2 - 0.1f;
constexpr float YAW_ANGLE_LOWER_LIMIT = -PI/2 + 0.05f;

static	glm::vec<3, double, glm::packed_highp> up_vector(0.0f, 1.0f, 0.0f);
static	glm::vec<3, double, glm::packed_highp> right_vector(0.0f);
static	glm::vec<3, double, glm::packed_highp> forward_vector(0.0f);

static float mouseX = 0.0f;
static float mouseY = 0.0f;

static PhysicsObject player(Camera::GetPosition(), 50, {0.0f, 0.0f, 0.0f, 0.8f, 1.7f, 0.8f});

static bool isCreative = false;

void PlayerController::update()
{
	//CAMERA
	double deltaMouseX = inputManager::getInput(inputManager::Input::MOUSEX);
	double deltaMouseY = inputManager::getInput(inputManager::Input::MOUSEY);

	//MOVEMENT
	double right = inputManager::getInput(inputManager::Input::RIGHT);
	double forward = inputManager::getInput(inputManager::Input::FORWARD);
	double fly = inputManager::getInput(inputManager::Input::FLY);
	double shift = inputManager::getInput(inputManager::Input::SHIFT);

	//MOUSE ACTION
	double mouseLeft = inputManager::getInput(inputManager::Input::MOUSELEFT);
	double mouseRight = inputManager::getInput(inputManager::Input::MOUSERIGHT);

	if(mouseLeft)
	{
		RayCast::Info info = BlockEdit::getCurrentRayInfo();

		if (info.hit)
		{
			ChunkManager::addBlockUpdate({ info.block.chunkLocation, info.block.x, info.block.z, info.block.y, 0 });
		}
		/*if(info.hit)
		{
			for (int x = -1; x <= 1; x++)
			{
				for (int z = -1; z <= 1; z++)
				{
					auto actualBlockX = info.block.x - x * CHUNK_WIDTH;
					auto actualBlockZ = info.block.z - z * CHUNK_LENGTH;
					auto actualBlockY = info.block.y;
					std::pair<int, int> location(info.block.chunkLocation.first + x, info.block.chunkLocation.second + z);
					for (int k = 0; k < CHUNK_HEIGHT; k++)
					{
						for (int j = 0; j < CHUNK_LENGTH; j++)
						{
							for (int i = 0; i < CHUNK_WIDTH; i++)
							{
								auto a = (actualBlockX - i) * (actualBlockX - i);
								auto b = (actualBlockY - k) * (actualBlockY - k);
								auto c = (actualBlockZ - j) * (actualBlockZ - j);
								if (a + b + c < 100)
								{
									ChunkManager::addBlockUpdate({location, i, j, k, 0});
								}
							}
						}
					}
				}
			}
		}*/
	}
	if (mouseRight)
	{
		RayCast::Info info = BlockEdit::getCurrentRayInfo();
		
		//std::pair<int, int> location(info.block.chunkLocation.first, info.block.chunkLocation.second);
		std::cout << "Chunk: (" << info.block.chunkLocation.first << ", " << info.block.chunkLocation.second << ")\n"<< "Block pos: " << info.block.x << " " << info.block.z << " " <<  info.block.y << std::endl;
		

		//if (info.hit)
		//{
		//	ChunkManager::addBlockUpdate({ info.block.chunkLocation, info.block.x, info.block.z, info.block.y, info.block.blockID});
		//}


		//RayCast::Info info = BlockEdit::getCurrentRayInfo();

		//if (info.hit)
		//{
		//	for (int x = -1; x <= 1; x++)
		//	{
		//		for (int z = -1; z <= 1; z++)
		//		{
		//			auto actualBlockX = info.block.x - x * CHUNK_WIDTH;
		//			auto actualBlockZ = info.block.z - z * CHUNK_LENGTH;
		//			auto actualBlockY = info.block.y;
		//			std::pair<int, int> location(info.block.chunkLocation.first + x, info.block.chunkLocation.second + z);
		//			for (int k = 0; k < CHUNK_HEIGHT; k++)
		//			{
		//				for (int j = 0; j < CHUNK_LENGTH; j++)
		//				{
		//					for (int i = 0; i < CHUNK_WIDTH; i++)
		//					{
		//						auto a = (actualBlockX - i) * (actualBlockX - i);
		//						auto b = (actualBlockY - k) * (actualBlockY - k);
		//						auto c = (actualBlockZ - j) * (actualBlockZ - j);
		//						if (a + b + c < 100)
		//						{
		//							ChunkManager::addBlockUpdate({ location, i, j, k, 3});
		//						}
		//					}
		//				}
		//			}
		//		}
		//	}
		//}
	}
	mouseX += deltaMouseX * SENSITIVITY;
	mouseY += deltaMouseY * SENSITIVITY;
	mouseY = std::clamp(mouseY, YAW_ANGLE_LOWER_LIMIT, YAW_ANGLE_UPPER_LIMIT);

	Camera::setAngle(mouseY, mouseX);

	right_vector = glm::normalize(glm::cross(Camera::GetCameraAngle(), up_vector));
	forward_vector = glm::normalize(glm::cross(up_vector, right_vector));
	
	if (shift == 1.0f)
	{
		player.setGravity(false);
		isCreative = true;
	}
	else
	{
		player.setGravity(true);
		isCreative = false;
	}

	glm::vec<3, double, glm::packed_highp> force(0.0f, 0.0f, 0.0f);
	double jump = fly * 10.0f;
	if (player.groundTime < 5 && !isCreative)
		jump = 0;
	if (player.isOnGround || isCreative)
		force = right_vector * right + forward_vector * forward + up_vector * jump - (29.0 / 30.0) * up_vector * jump * double(isCreative);
	else
		force = (right_vector * right + forward_vector * forward) / 5.0;
	if (isCreative)
		force *= 3;
	player.addForce(force);
	player.update();
	Camera::setPosition(player.getPosition() + glm::vec<3, double, glm::packed_highp>(0.0, 0.75, 0.0));
}