RTC_TimeTypeDef rtcTime;
RTC_DateTypeDef rtcDate;

extern bool setRTC(const char*, const char*);
extern void getRTC(RTC_DateTypeDef&, RTC_TimeTypeDef&);
extern void showDateTime(const RTC_DateTypeDef&, const RTC_TimeTypeDef&);

void getRTC(RTC_DateTypeDef& d, RTC_TimeTypeDef& t) {
  M5.Rtc.GetDate(&d);
  M5.Rtc.GetTime(&t);
}
/*
void printDateTime(const RTC_DateTypeDef& d, const RTC_TimeTypeDef& t) {
  static const char *wd[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

  //M5.Lcd.printf("Date   : %04d/%02d/%02d(%s)\n", d.Year, d.Month, d.Date, wd[d.WeekDay]);
  //M5.Lcd.printf("Time   : %02d:%02d:%02d\n", t.Hours, t.Minutes, t.Seconds);
  canvas.printf("Date   : %04d/%02d/%02d(%s)\n", d.Year, d.Month, d.Date, wd[d.WeekDay]);
  canvas.printf("Time   : %02d:%02d:%02d\n", t.Hours, t.Minutes, t.Seconds);
}
*/

void updateRTC(){
  getRTC(rtcDate, rtcTime);
  //M5.Lcd.setCursor(0, FONT_SIZE * 1);
  //showDateTime(rtcDate, rtcTime);
  month = rtcDate.Month;
  hour = rtcTime.Hours;
  minute = rtcTime.Minutes;
  second = rtcTime.Seconds;
}
