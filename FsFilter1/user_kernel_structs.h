#ifndef _USER_KERNEL_STRUCTS_H_
#define _USER_KERNEL_STRUCTS_H_

#include <fltUserStructures.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define DRIVER_PORT   L"\\DSMKPort"
	// TODO
    typedef enum _COMMAND_CODE
    {
        cmdGetLibraryVersion = 0,                                    // driver --> library --> driver
        cmdTestWithReply,	     		                             // library --> driver --> 
		cmdEnableProcmon,
		cmdDisableProcmon,
		cmdGiveProcname
    } COMMAND_CODE, *PCOMMAND_CODE;

    typedef struct _CMD_TEST_WITH_REPLY
    {
        COMMAND_CODE    Command;
        __int32         ReplyFromKernel;
    }CMD_TEST_WITH_REPLY, *PCMD_TEST_WITH_REPLY;

	typedef struct _CMD_PROCMON_WITH_REPLY
	{
		COMMAND_CODE	Command;									
		__int32			ReplyFromKernel;							
	}CMD_PROCMON_WITH_REPLY, *PCMD_PROCMON_WITH_REPLY;

    typedef struct _COMMAND_REQUEST
    {
        FILTER_MESSAGE_HEADER   MessageHeader;
        COMMAND_CODE            Command;
    }COMMAND_REQUEST, *PCOMMAND_REQUEST;

    typedef struct _VERSION_INFORMATION
    {
        COMMAND_CODE            Command;
        __int32                 Major;
        __int32                 Minor;
        __int32                 Revision;
        __int32                 Build;
    }VERSION_INFORMATION, *PVERSION_INFORMATION;


	typedef struct _PROC_INFO
	{
		COMMAND_CODE		Command;
		WCHAR				ImageFileName[200];
	}PROC_INFO, *PPROC_INFO;


    typedef struct _VERSION_INFORMATION_REPLY
    {
        FILTER_REPLY_HEADER     ReplyHeader;
        VERSION_INFORMATION     VersionInformation;

    }VERSION_INFORMATION_REPLY, *PVERSION_INFORMATION_REPLY;

	typedef struct _PROCESS_INFORMATION_REPLY
	{
		FILTER_REPLY_HEADER		ReplyHeader;
		PROC_INFO				ProcessInformation;
	}PROCESS_INFORMATION_REPLY, *PPROCESS_INFORMATION_REPLY;

#ifdef __cplusplus
}
#endif

#endif //_USER_KERNEL_STRUCTS_H_