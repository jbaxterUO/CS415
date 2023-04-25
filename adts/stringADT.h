#ifndef _STRINGADT_H_
#define _STRINGADT_H_

/*
 * Copyright (c) 2018-2019, 2021, University of Oregon
 * All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:

 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * - Neither the name of the University of Oregon nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * interface definition for Mutable String ADT
 *
 * patterned roughly after Python 3 string class with inclusion of
 * append(), clear(), insert(), remove(), replace(), translate()
 */

#include "ADTs/ADTdefs.h"
#include "ADTs/arraylist.h"

typedef struct string String;		/* forward reference */

/*
 * creates a String from the supplied argument
 *
 * returns pointer to String if successful, NULL otherwise
 */
const String *String_create(char *str);

/*
 * now define dispatch table
 */
struct string {
/*
 * the private data of the String
 */
    void *self;

/*
 * return copy of `str'
 * returns pointer to new String if successful, NULL otherwise (heap errors)
 */
    const String *(*copy)(const String *str);

/*
 * returns new String that is a slice of `str'
 *
 * if end = 0, the last index of the slice is str->length(str)
 *
 * if `begin' or `end' are illegal, NULL is returned
 * otherwise, a new String is returned with a copy of the specified characters
 */
    const String *(*slice)(const String *str, int begin, int end);

/*
 * destroys the String
 */
    void (*destroy)(const String *str);

/*
 * append `suffix' to `str'
 * returns true if successful, false if not (heap error)
 */
    bool (*append)(const String *str, char *suffix);

/*
 * assign `chr' into `str[index]';
 * legal values of `index' are 0 .. str->len(str) - 1
 * if `index' is outside of legal range, return false; otherwise, return true
 */
    bool (*assign)(const String *str, int chr, int index);

/*
 * clear the characters from the string; equivalent to String_create("")
 * without creating a new dispatch table
 */
    void (*clear)(const String *str);
/*
 * insert `substr' into `str' before index `index';
 * legal values of `index' are 0 .. str->len(str)
 * if `index' is outside of legal range, return false; otherwise, return true
 */
    bool (*insert)(const String *str, char *substr, int index);

/*
 * converts all uppercase letters in `str' to lowercase
 */
    void (*lower)(const String *str);

/*
 * removes all leading whitespace in `str'
 */
    void (*lStrip)(const String *str);

/*
 * remove character at `index';
 * legal values of `index' are 0 .. str->len(str)-1
 * if `index' is outside of legal range, return false; otherwise, return true
 */
    bool (*remove)(const String *str, int index);

/*
 * replaces all occurrences of `old' in `str' with `new'
 * returns true if successful, false if not (heap error)
 */
    bool (*replace)(const String *str, char *old, char *new);

/*
 * removes all trailing whitespace in `str'
 */
    void (*rStrip)(const String *str);

/*
 * performs both lStrip() and rStrip() on `str'
 */
    void (*strip)(const String *str);

/*
 * translates all characters in `class' to `chr'
 */
    void (*translate)(const String *str, char *class, int chr);

/*
 * converts all lowercase letters in `str' to uppercase
 */
    void (*upper)(const String *str);

/*
 * compare `str' with `other'; return <0|0|>0 if str < other |
 * str == other | str > other, respectively
 */
    int (*compare)(const String *str, const String *other);

/*
 * return true if `str' contains `substr'; return false if not
 */
    bool (*contains)(const String *str, char *substr);

/*
 * returns true if str->slice(str, begin, end) ends with `suffix'
 * if end = 0, the last index of the slice is str->length(str) 
 * returns false if it does not
 */
    bool (*endsWith)(const String *str, char *suffix, int begin, int end);

/*
 * value of `str[index]' is returned in `*chr';
 * legal values of `index' are 0 .. str->len(str) - 1
 * if `index' is outside of legal range, return false; otherwise, return true
 */
    bool (*get)(const String *str, int index, int *chr);

/*
 * returns index if str->slice(str, begin, end) contains `substr'
 * if end = 0, the last index of the slice is str->length(str) 
 * returns -1 if it does not
 */
    int (*index)(const String *str, char *substr, int begin, int end);

/*
 * returns true if `str' has at least 1 character and all characters are
 * alphanumeric
 * returns false otherwise
 */
    bool (*isAlpha)(const String *str);

/*
 * returns true if `str' has at least 1 character and all characters are digits
 * returns false otherwise
 */
    bool (*isDigit)(const String *str);

/*
 * returns true if `str' has at least 1 character and all characters are
 * lowercase
 * returns false otherwise
 */
    bool (*isLower)(const String *str);

/*
 * returns true if `str' has at least 1 character and all characters are
 * whitespace
 * returns false otherwise
 */
    bool (*isSpace)(const String *str);

/*
 * returns true if `str' has at least 1 character and all characters are
 * uppercase
 * returns false otherwise
 */
    bool (*isUpper)(const String *str);

/*
 * returns the length of `str'
 * returns 0 otherwise
 */
    int (*len)(const String *str);

/*
 * same as index(), but search backwards in `str'
 */
    int (*rindex)(const String *str, char *substr, int begin, int end);

/*
 * splits the string into a list of strings, returning an ArrayList, which
 * can be manipulated by the caller
 *
 * The sep argument is a C string with the characters used to split the string;
 * if it is "", runs of 1 or more white space characters separate words; if
 * it is not "", then the exact sequence of characters is used to separate words
 *
 * the words and the ArrayList are allocated on the heap, so the caller
 * must invoke the destroy() method on the ArrayList when finished to avoid
 * memory leaks
 *
 * returns NULL if memory allocation failure or NO WORDS IN THE String
 */
    const ArrayList *(*split)(const String *str, char *sep);

/*
 * returns true/false if str->slice(str, begin, end) starts with `prefix'
 * if end = 0, the last index of the slice is str->length(str) 
 */
    bool (*startsWith)(const String *str, char *prefix, int begin, int end);

/*
 * returns a char * to the contents of the string
 */
    char *(*convert)(const String *str);
};

#endif /* _STRINGADT_H_ */
