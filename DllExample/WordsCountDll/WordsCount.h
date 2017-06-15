#pragma once

#ifndef WORDS_COUNT_API
#define WORDS_COUNT_API extern "C" __declspec( dllimport )
#endif

WORDS_COUNT_API int WordsCount( const wchar_t* text );
WORDS_COUNT_API bool IsDelimiter( wchar_t symbol );