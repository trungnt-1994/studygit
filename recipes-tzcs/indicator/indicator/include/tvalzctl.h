/** 
Copyright (C) 2012 TOSHIBA Corporation

Module Name:
    tvalzctl.h

Abstract:
   This module is a header file for the call sides which uses a Toshiba peculiar driver (tvalz.ko).

Author:
   TIH/SWT
   ZYQ

Environment:
   Operates only in the kernel mode of ACPI OS.

Notes:
   Note.

Revision History:
   18-sep-2012 Version 1.0.0.1 ZYQ Creation.
*/

#ifndef TVALZCTL_H
#define TVALZCTL_H

#define ULONG unsigned int
/** 

#define CTL_CODE(DeviceType, Function, Method, Access) (                 \
        ((DeviceType)<<16) | ((Access)<<14) | ((Function)<<2) | (Method) \
        )

#define IOCTL_TVALZ_INFO        (ULONG)CTL_CODE(0x22, 0xA10, 0, 0)
#define IOCTL_TVALZ_CANCEL_INFO (ULONG)CTL_CODE(0x22, 0xA11, 0, 0)
#define IOCTL_TVALZ_GHCI        (ULONG)CTL_CODE(0x22, 0xA20, 0, 0)
#define IOCTL_TVALZ_ENAB        (ULONG)CTL_CODE(0x22, 0xA30, 0, 0)
#define IOCTL_TVALZ_RUN_METHOD  (ULONG)CTL_CODE(0x22, 0xA50, 0, 0)
#define IOCTL_TVALZ_RETRIEVE_EVENT (ULONG)CTL_CODE(0x22, 0xA51, 0, 0)
#define IOCTL_TVALZ_CTL_EXT        (ULONG)CTL_CODE(0x22, 0xA60, 0, 0)*/


#ifndef _GHCI_INTERFACE_
#define _GHCI_INTERFACE_
// Define GHCI buffer struct.
typedef struct _ghci_interface {
        ULONG ghci_eax;
        ULONG ghci_ebx;
        ULONG ghci_ecx;
        ULONG ghci_edx;
        ULONG ghci_esi;
        ULONG ghci_edi;
}ghci_interface, *pghci_interface;
#endif


// Read mode of input device status.
enum
{   
        TVALZ_HCI_RAW_MODE,
        TVALZ_INFO_MODE,
};

// Define GHCI buffer struct.
typedef struct _info_interface {
        ULONG info_arg0;
        ULONG info_arg1;
        ULONG info_arg2;
        ULONG info_arg3;
        ULONG info_arg4;
        ULONG info_arg5;
        ULONG info_arg6;
        ULONG info_arg7;
        ULONG info_arg8;
        ULONG info_arg9;
}info_interface, *pinfo_interface;

// Define scan code and key code struct.
typedef struct _scan_key_code{
        unsigned int scancode;
        unsigned int keycode;
}scan_key_code, *pscan_key_code;

#define IOCTL_TVALZ_DISABLE_INPUTDEV         _IOR(0x2A,  0x8, int)
#define IOCTL_TVALZ_ENABLE_INPUTDEV          _IOR(0x2A,  0x9, int)
#define IOCTL_TVALZ_INFO                     _IOR(0x2A,  0x10, info_interface)
#define IOCTL_TVALZ_CANCEL_INFO              _IOR(0x2A,  0x11, int)
#define IOCTL_TVALZ_GHCI                     _IOWR(0x2A, 0x20, ghci_interface)
#define IOCTL_TVALZ_ENAB                     _IOR(0x2A,  0x30, int)
#define IOCTL_TVALZ_RUN_METHOD               _IOWR(0x2A, 0x50, ghci_interface)
#define IOCTL_TVALZ_RETRIEVE_EVENT           _IOWR(0x2A, 0x51, ghci_interface)
#define IOCTL_TVALZ_GET_RETRIEVE_EVENT       _IOWR(0x2A, 0x52, ghci_interface)
#define IOCTL_TVALZ_CTL_EXT                  _IOWR(0x2A, 0x60, ghci_interface)
#define IOCTL_TVALZ_SET_KEYCODE              _IOW(0x2A,  0x61, scan_key_code)


#endif
