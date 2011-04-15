
#include <windows.h>

typedef struct {
    LARGE_INTEGER start;
    LARGE_INTEGER stop;
} stopWatch;

#ifdef __cplusplus
extern "C" {
#endif

void startTimer( stopWatch *timer);
void stopTimer( stopWatch *timer);
double LIToSecs( LARGE_INTEGER * L);
double getElapsedTime( stopWatch *timer);

#ifdef __cplusplus
}
#endif
