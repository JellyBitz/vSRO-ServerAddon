#include "CPosition.h"

CPosition::CPosition()
{

}
CPosition::CPosition(uint16_t RegionId, uint16_t UnkUShort01, float PosX, float PosY, float PosZ)
{
	this->RegionId = RegionId;
	this->UnkUShort01 = UnkUShort01;
	this->PosX = PosX;
	this->PosY = PosY;
	this->PosZ = PosZ;
}