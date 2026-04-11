#pragma once

#ifdef SCREEN_DEBUG
#define DEBUG_PRINT(...) Serial.print("DEBUG: "); Serial.print(__VA_ARGS__);
#define DEBUG_PRINT_DEC(...) Serial.print("DEBUG: "); Serial.print(__VA_ARGS__, DEC);
#else
#define DEBUG_PRINT
#define DEBUG_PRINT_DEC
#endif