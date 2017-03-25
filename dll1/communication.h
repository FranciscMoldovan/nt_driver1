#ifndef _COMMUNICATION_H_
#define _COMMUNICATION_H_


void
InitializeCommunication(
    __int32 NrOfDllConnThreads
);

void
UninitializeCommunication();

__int32
TestCommand();

__int32
ToggleProcessMonitoring(unsigned char onOrOff);

#endif //_COMMUNICATION_H_
