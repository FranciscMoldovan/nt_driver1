#ifndef _DLL1_H_
#define _DLL1_H_

#define NOTHING

#include <ntstatus.h>
#define WIN32_NO_STATUS         // avoid redefining the already defined NTSTATUS values
//#define WIN32_LEAN_AND_MEAN     
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <strsafe.h>
#include <fltuser.h>

// type definitions for the interface
typedef unsigned __int64 QWORD;

#include "dll1_public.h"
#include "debug.h"


#endif // _DLL1_H_