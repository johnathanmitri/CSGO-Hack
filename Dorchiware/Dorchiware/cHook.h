#pragma once

class cHook
{
	//uintptr_t 

	




private:
	static cHook* hookTable[20];// = new cHook[20];
	static int hookTableSize;
	bool hooked;
	char jumpInstruction[5];

public:
	static void unHookAll();

	int stolenByteCount;
	
	char* detourFunctionPtr;
	char* targetFunctionPtr;

	char* jumpBackGateway;

	char* stolenBytes;

	cHook(char* _targetFunctionPtr, char* _detourFunctionPtr, int _stolenByteCount);

	void applyHook();

	void unHook();

};

