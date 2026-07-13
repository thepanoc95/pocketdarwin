/* Availability.h - Linux stub for Apple SDK compatibility */

#ifndef __AVAILABILITY__
#define __AVAILABILITY__

#ifndef __OSX_AVAILABLE_STARTING
#define __OSX_AVAILABLE_STARTING(_osx, _ios)
#endif

#ifndef __OSX_AVAILABLE_BUT_DEPRECATED
#define __OSX_AVAILABLE_BUT_DEPRECATED(_osxIntro, _osxDep, _iosIntro, _iosDep)
#endif

#ifndef API_AVAILABLE
#define API_AVAILABLE(...)
#endif

#ifndef API_AVAILABLE_BUT_DEPRECATED
#define API_AVAILABLE_BUT_DEPRECATED(...)
#endif

#ifndef API_DEPRECATED
#define API_DEPRECATED(...)
#endif

#ifndef API_UNAVAILABLE
#define API_UNAVAILABLE(...)
#endif

#ifndef NS_AVAILABLE
#define NS_AVAILABLE(_osx, _ios)
#endif

#ifndef NS_AVAILABLE_IOS
#define NS_AVAILABLE_IOS(_ios)
#endif

#ifndef NS_AVAILABLE_MAC
#define NS_AVAILABLE_MAC(_osx)
#endif

#endif /* __AVAILABILITY__ */
