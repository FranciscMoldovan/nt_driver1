#ifndef _SERVER_COMMUNICATION_H_
#define _SERVER_COMMUNICATION_H_

#include <ntstatus.h>

NTSTATUS
InitializeCommunication();

void
UninitializeCommunication();


#endif //_SERVER_COMMUNICATION_H_