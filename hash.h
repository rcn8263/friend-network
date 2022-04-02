/// @file hash.h
/// @brief Definitions for some common hashing functions, along with
///    equality checking and debug printing. 
///    This is used by test_table.c.
///
/// @author Sean Strout (RIT CS)
/// @author bksteele (RIT CS)
/// @author Warren R. Carithers (RIT_CS)

#ifndef HASH_H
#define HASH_H

#include <stdbool.h>  // bool
#include <stdio.h>    // printf

/// The hash function for long keys produces size_t cast of the long value.
/// The cast solves the problem that X % N is negative if X is negative, and
/// a hash value needs to be non-negative for proper array indexing.
///
/// @param element The long value to hash
/// @return The hash code is the size_t, or unsigned long, value
///
size_t long_hash( const void* element);

/// long_equals function checks equality of the elements as two long values.
///
/// @param element1 The first long
/// @param element2 The second long
/// @return whether the longs are equal or not
///
bool long_equals( const void* element1, const void* element2);

/// long_str_print function interprets keys as long and values as C-string.
///
/// @param key the long integer key
/// @param value the C-string value
///
void long_str_print( const void* key, const void* value);

/// str_hash function returns the hash of a native C-string.  
///
/// @param element the C-string to hash
/// @return the hash value of the C-string
///
size_t str_hash( const void* element);

/// str_equals function checks equality of the elements as two C-strings.
///
/// @param element1 first C-string
/// @param element2 second C-string
/// @return Whether the C-strings are equal or not
///
bool str_equals( const void* element1, const void* element2);

/// str_long_print function interprets keys as C-string and values as long.
///
/// @param key the C-string key
/// @param value the long value
///
void str_long_print( const void* key, const void* value);

/// long_long_print print function interprets keys as long and values as long.
///
/// @param key the long key
/// @param value the long value
///
void long_long_print( const void* key, const void* value);

#endif

