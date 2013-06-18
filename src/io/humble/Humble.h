#ifndef HUMBLE_H_
#define HUMBLE_H_

#ifdef __cplusplus
namespace io {
namespace humble {}
}
#endif // __cplusplus
#define VS_STRINGIFY(__arg) #__arg
#if (__GNUC__ >= 4) || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
#  ifndef GCC_HASCLASSVISIBILITY
#    define GCC_HASCLASSVISIBILITY
#  endif
#endif
#ifndef VS_API_EXPORT
# if defined(_WIN32) || defined(__WIN32__) || defined(__CYGWIN__) || defined(VS_OS_WINDOWS)
#   ifdef VS_API_COMPILING
#     define VS_API_EXPORT __declspec(dllexport)
#   else
#     define VS_API_EXPORT __declspec(dllimport)
#   endif
#   define VS_API_CALL __stdcall
# else
#   if defined(__GNUC__) && defined(GCC_HASCLASSVISIBILITY)
#     if VS_API_COMPILING
#       define VS_API_EXPORT __attribute__ ((visibility("default")))
#     else
#       define VS_API_EXPORT
#     endif
#   else
#     define VS_API_EXPORT
#   endif
#   define VS_API_CALL
# endif
#endif
#ifdef JNIEXPORT
#undef JNIEXPORT
#define JNIEXPORT "_____ERROR___USE_VS_API_EXPORT_INSTEAD_OF_JNIEXPORT____________"
#endif
#ifdef JNICALL
#undef JNICALL
#define JNICALL VS_API_CALL
#endif
#endif // HUMBLE_H_
