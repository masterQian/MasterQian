#pragma once

#if BUILD_TEST
#include "../include/MasterQian.Data.h"
#include "../include/MasterQian.DB.Sqlite.h"
#include "../include/MasterQian.Media.GDI.h"
#include "../include/MasterQian.Parser.Ini.h"
#include "../include/MasterQian.Parser.Config.h"
#include "../include/MasterQian.Storage.Path.h"
#include "../include/MasterQian.Storage.Zip.h"
#include "../include/MasterQian.System.AutoDevice.h"
#include "../include/MasterQian.System.Clipboard.h"
#include "../include/MasterQian.System.Info.h"
#include "../include/MasterQian.System.Log.h"
#include "../include/MasterQian.System.Process.h"
#include "../include/MasterQian.System.Reg.h"
#include "../include/MasterQian.System.Window.h"
#include "../include/MasterQian.Tool.Benchmark.h"
#include "../include/MasterQian.WinRT.h"

using namespace MasterQian;
using namespace DB;
using namespace Media;
using namespace Parser;
using namespace Storage;
using namespace System;
using namespace Tool;
#endif

#include "../include/MasterQian.System.Log.h"

using namespace MasterQian;
inline System::ConsoleLogger logger;