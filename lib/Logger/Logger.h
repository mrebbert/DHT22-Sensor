#ifndef Logger_h
#define Logger_h

#include <Arduino.h>

const int log_debug=1;
const int log_info=2;
const int log_error=3;

class Logger {
  public:
    Logger();
    Logger(const char *name);
    Logger(const char *name, int loglevel);
    void log(int loglevel, String message);
    void debug(String message);
    void info(String message);
    void error(String message);
  private:
    const char *_name;
    int  _loglevel;
    void init(const char *name, int loglevel);
    String getLoglevel(int loglevel);
};
#endif
