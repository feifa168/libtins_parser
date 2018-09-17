#ifndef SNIFF_PROTO_CROSS_PLATERORM_H
#define SNIFF_PROTO_CROSS_PLATERORM_H

#define VIRTUAL_METHOD   virtual

#if (defined WIN32 || defined WIN64)
#define WINDOWS
#endif

#ifdef WINDOWS
    #define CALL_METHOD __stdcall
    #define DLL_EXPORT __declspec(dllexport)
    #define DLL_IMPORT __declspec(dllimport)
#else
    #define CALL_METHOD
    #define DLL_EXPORT __declspec(dllexport)
    #define DLL_IMPORT __declspec(dllimport)
#endif

#define PURE_VIRTUAL    =0

#endif  // SNIFF_PROTO_CROSS_PLATERORM_H
