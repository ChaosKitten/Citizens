#ifndef CCLIENT_ENGINE_HPP
#define CCLIENT_ENGINE_HPP

#if __linux
#include "CClientEngineLinux.hpp"
#elif __apple__
#include "CClientEngineMacOSX.hpp"
#elif __WIN32 || __WIN64
#include "CClientEngineWindows.hpp"
#else
#include "CClientEngineGeneric.hpp"
#endif

#endif
