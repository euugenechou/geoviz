#pragma once

#include <string>

extern bool logging;

void _info(std::string msg);

void _err(std::string msg);

void _debug(std::string file, size_t ln, std::string fn, std::string msg);

#define LOGINFO(msg) _info(msg)

#define LOGERR(msg) _err(msg)

#define LOGDEBUG(msg) _debug(__FILE__, __LINE__, __func__, msg)
