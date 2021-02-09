#include "pch.h"
#include "cHook.h"

//cHook::hookTable = new cHook[20];
int cHook::hookTableSize = 0;

cHook* cHook::hookTable[20];

cHook::cHook(char* _targetFunctionPtr, char* _detourFunctionPtr, int _stolenByteCount)
{
	hookTable[hookTableSize] = this;
	hookTableSize++;

	hooked = false;
	stolenByteCount = _stolenByteCount;
	detourFunctionPtr = _detourFunctionPtr;
	targetFunctionPtr = _targetFunctionPtr;

	jumpInstruction[0] = 0xE9; //jmp opcode


	int test = (int)(detourFunctionPtr - (targetFunctionPtr + 5));    //destination - (source + 5)
	char* test2 = new char[4];//*(char*)(&test);
	*(int*)(test2) = test;

	*(int*)(jumpInstruction + 1) = (int)(detourFunctionPtr - (targetFunctionPtr + 5)); //relative jump address

	stolenBytes = new char[stolenByteCount]; //SIZE
	memcpy(stolenBytes, _targetFunctionPtr, stolenByteCount);     //copy orignal code into stolenbytes	

	jumpBackGateway = new char[stolenByteCount + 5];
	memcpy(jumpBackGateway, stolenBytes, stolenByteCount);  //copy the stolen bytes into the gateway, so that we can add the jump back to original code after it.

	DWORD oProc;
	
	int relJmpBackAddr = (int)(targetFunctionPtr - (jumpBackGateway + 5)); //relative offset to jmp back to original code
	jumpBackGateway[stolenByteCount] = 0xE9;                            //put a jmp instruction at the byte following the stolen ones

	*(int*)(&jumpBackGateway[stolenByteCount + 1]) = relJmpBackAddr;    //follow the jmp with the relative offset

	VirtualProtect(jumpBackGateway, stolenByteCount + 5, PAGE_EXECUTE_READWRITE, &oProc);  //set the jump gateway to the same virtualprotect as the other code
}


void cHook::applyHook()
{
	DWORD oldProtect;
	VirtualProtect(targetFunctionPtr, stolenByteCount, PAGE_EXECUTE_READWRITE, &oldProtect); //make it writeable, and remember its old protection

	memset(targetFunctionPtr, 0x90, stolenByteCount); //nop the original stolenBytes so that so that no junk code is left

	memcpy(targetFunctionPtr, &jumpInstruction, 5);  //5 bytes for jmp + 4 byte address

	VirtualProtect(targetFunctionPtr, stolenByteCount, oldProtect, &oldProtect);

	hooked = true;
}

void cHook::unHook()
{
	if (!hooked)
		return;
	DWORD oldProtect;
	VirtualProtect(targetFunctionPtr, stolenByteCount, PAGE_EXECUTE_READWRITE, &oldProtect); //make it writeable, and remember its old protection

	memcpy(targetFunctionPtr, stolenBytes, stolenByteCount);  //set the original bytes back

	VirtualProtect(targetFunctionPtr, stolenByteCount, oldProtect, &oldProtect); //reset the virtual protect

	hooked = false;
}


void cHook::unHookAll()
{
	for (cHook* hook : hookTable)
	{
		if (hook == nullptr)
			break;
		hook->unHook();
	}

}