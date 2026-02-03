// rtc_include.h
#ifndef RTC_INCLUDE_H
#define RTC_INCLUDE_H

struct SiiRtcInfo;
struct Time;

void RtcGetInfoFake(struct SiiRtcInfo *rtc);
void RtcCalcTimeDifferenceFake(struct SiiRtcInfo *rtc, struct Time *result, struct Time *t);
void RtcAdvanceTime(int hours, int minutes, int seconds);

#endif // RTC_INCLUDE_H
