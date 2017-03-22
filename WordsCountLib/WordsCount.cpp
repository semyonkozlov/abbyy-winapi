#include "WordsCount.h"

int WordsCount( const wchar_t* text )
{
    int wordsCounter = 0;
    int i = 0;

    while( text[i] != L'\0' ) {
        while( IsDelimiter( text[i] ) ) {
            ++i;
        }
        if( text[i] == L'\0' ) {
            return wordsCounter;
        }

        ++i;
        if( IsDelimiter( text[i] ) ) {
            ++wordsCounter;
        }
    }

    return wordsCounter + 1;
}

bool IsDelimiter( wchar_t symbol )
{
    return symbol == L' ' || symbol == L'\n' || symbol == L'\t';
}
