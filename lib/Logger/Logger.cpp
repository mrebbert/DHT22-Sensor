#include <Logger.h>

Logger::Logger() {
  init("LOG", log_info);
}

Logger::Logger(const char *name) {
  init(name, log_info);
}

Logger::Logger(const char *name, int loglevel) {
  init(name, loglevel);
}

void Logger::init(const char *name, int loglevel) {
  _name = name;
  _loglevel = loglevel;
}

void Logger::log(int loglevel, String message) {
  if (_loglevel <= loglevel) {
    Serial.printf("%s \t %s: %s\n", getLoglevel(loglevel).c_str(), _name, message.c_str());
  }
}

void Logger::debug(String message) {
  log(log_debug, message);
}

void Logger::info(String message) {
  log(log_info, message);
}
void Logger::error(String message) {
  log(log_error, message);
}

String Logger::getLoglevel(int loglevel) {
  String _str = "ERROR";
  switch (loglevel) {
    case 1:
      _str = "DEBUG";
      break;
    case 2:
      _str = "INFO";
      break;
  }
  return (_str);
}
