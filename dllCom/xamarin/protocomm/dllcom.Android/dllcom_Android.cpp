#include "dllcom_Android.h"
#include "android\log.h"

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "dllcom_Android", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "dllcom_Android", __VA_ARGS__))

extern "C" {
	/* Cette fonction triviale retourne l'ABI de la plateforme pour laquelle cette bibliothèque native dynamique est compilée.*/
	const char * dllcom_Android::getPlatformABI()
	{
	#if defined(__arm__)
	#if defined(__ARM_ARCH_7A__)
	#if defined(__ARM_NEON__)
		#define ABI "armeabi-v7a/NEON"
	#else
		#define ABI "armeabi-v7a"
	#endif
	#else
		#define ABI "armeabi"
	#endif
	#elif defined(__i386__)
		#define ABI "x86"
	#else
		#define ABI "unknown"
	#endif
		LOGI("This dynamic shared library is compiled with ABI: %s", ABI);
		return "This native library is compiled with ABI: %s" ABI ".";
	}

	void dllcom_Android()
	{
	}

	dllcom_Android::dllcom_Android()
	{
	}

	dllcom_Android::~dllcom_Android()
	{
	}
}
