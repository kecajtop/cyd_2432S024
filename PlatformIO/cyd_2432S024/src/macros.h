#ifndef macros_h
#define macros_h

#define DEBUG_LEVEL  DBG_LEVEL                                               // manages most of the print and println debug, not all but most
		//Serial.print("USR ");\
		//Serial.print(": ");\
//    	//Serial.print(__FILENAME__);\

#if (DEBUG_LEVEL > 0U)
#define UsrLogln(...)\
		Serial.println(__VA_ARGS__)
		
#define UsrLog(...)\
		Serial.print(__VA_ARGS__)
		
#define print_k(...)\
		Serial.print(__VA_ARGS__)
		
#define print_kln(...)\
		Serial.println(__VA_ARGS__)
#else
#define UsrLogln(...)
#define UsrLog(...)
#define print_k(...)
#define print_kln(...)
#endif

#if (DEBUG_LEVEL > 1U)
#define ErrLogln(...)\
		Serial.println(__VA_ARGS__)
		
#define ErrLog(...)\
		Serial.print(__VA_ARGS__)
#else
#define ErrLogln(...)
#define ErrLog(...)
#endif

#if (DEBUG_LEVEL > 2U)
#define DbgLogln(...)\
		Serial.println(__VA_ARGS__)
		
#define DbgLog(...)\
		Serial.print(__VA_ARGS__)
#else
#define DbgLogln(...)
#define DbgLog(...)
#endif

#define ST(A) #A
#define STR(A) ST(A)

#endif