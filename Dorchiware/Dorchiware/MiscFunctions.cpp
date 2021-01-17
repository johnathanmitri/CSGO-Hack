#include "MiscFunctions.h"
#include "cHack.h"

void MiscFunctions::increaseTriggerbotFov()
{
	if (hack->settings.halfTriggerbotFov >= 45)  //if at or greater than maximum, dont go anymore
		return;

	if (hack->settings.halfTriggerbotFov >= 1)
	{
		hack->settings.halfTriggerbotFov += 1;
	}
	else
		hack->settings.halfTriggerbotFov *= 2;

}

void MiscFunctions::decreaseTriggerbotFov()
{
	if (hack->settings.halfTriggerbotFov <= 0.125)  //if at or lower than minimum, dont go anymore
		return;

	if (hack->settings.halfTriggerbotFov > 1)
		hack->settings.halfTriggerbotFov -= 1;
	else
		hack->settings.halfTriggerbotFov /= 2;
}

void MiscFunctions::test()
{

}

