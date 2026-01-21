#ifndef STRING_H
#define STRING_H

#include "types.h"

/**
 * @brief Compute the length of a null-terminated string
 * 
 * @param s String to measure
 * @return val Length of s
 */
val strlen(val* s);

/**
 * @brief Append one string to the end of another, followed by a null terminator
 * 
 * @param dst Base string
 * @param src String to append onto `dst`
 */
void strcat(val* dst, val* src);

/**
 * @brief Copy a specified amount of one string into a buffer, up to the null terminator
 * 
 * @param dst String to copy into
 * @param src String to copy from
 * @param n Number of characters to copy
 */
void strncpy(val* dst, val* src, val n);

/**
 * @brief Copy a string into another, up to the null terminator
 * 
 * @param dst String to copy into
 * @param src String to copy from
 */
void strcpy(val* dst, val* src);

/**
 * @brief Convert an integer value to a null-terminated string containing its base-10 representation in ASCII
 * 
 * @param n The value to parse
 * @param str A buffer to fill - must be large enough
 * @return Pointer to the string
 */
val* itoa(val n, val* str);

/**
 * @brief Fill a specified amount of a buffer with a specified character
 * 
 * @param ptr Buffer to fill
 * @param v Value to place in buffer
 * @param n Number of elements to write
 */
void memset(val *ptr, val v, val n);

#endif
