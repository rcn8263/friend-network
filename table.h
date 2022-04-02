/// @file table.h
/// @brief A generic hash table data structure.
///
/// @author Sean Strout (RIT CS)
/// @author bksteele (RIT CS)
/// @author Warren R. Carithers (RIT CS)

#ifndef TABLE_H
#define TABLE_H

#include <stdbool.h>    // bool
#include <stddef.h>     // size_t

/// Initial capacity of table upon creation
#define INITIAL_CAPACITY 16

/// The load at which the table will rehash
#define LOAD_THRESHOLD 0.75

/// The table size will double upon each rehash
#define RESIZE_FACTOR 2

/// The Table data type is a pointer to an opaque structure; clients
/// cannot see all the structure's content.
///
/// General Notes on hash table Operation
///
/// - The operations identified below are the public interface to HashADT.
///   The client supplies functions for hashing the key, checking equality
///   of keys, printing keys, and deleting (key, value) pairs.
///
/// - The table takes ownership of inserted keys and values.  It operates
///   under the assumption that all pointers given to it are dynamically
///   allocated, and should be returned to the free pool by the table.
///
/// - The ht_delete() function can free the (key, value) pair automatically.
///   If this is not desired, the client should pass a NULL pointer for the
///   delete function when ht_create() is called.
///
/// - There is no remove functionality. Entries remain until you call
///   ht_destroy().
///
/// - Wherever a function has a precondition, and the client violates the
///   condition, and the code detects the violation, then the function will
///   assert failure and abort.
///
/// - Wherever a function has a postcondition, and the client violates the
///   condition, and the code detects the violation, then the function will
///   assert failure and abort.
///
typedef struct Table_t * Table;

/// Create a new hash table instance.
/// If delete is NULL, supply no-op function for (key, value) pair deletion.
///
/// @param hash The hash function for key data
/// @param equals The equal function for key comparison
/// @param print The print function for (key, value) pairs is used by dump().
/// @param delete The delete function for (key, value) pairs is used by
////       destroy().
/// @exception Assert fails if it cannot allocate space
/// @pre hash, equals and print are valid function pointers.
/// @return A newly created table
///
Table ht_create(size_t (*hash)(const void* key),
                // equals(k,k)
                bool (*equals)(const void* key1, const void* key2),
                // print(k,v)
                void (*print)(const void* key, const void* value),
                // delete(k,v)
                void (*delete)(void* key, void* value) );

/// Destroy the table instance, and call delete function on each (key, value)
/// pair.
///
/// @param t The table to destroy
/// @pre t is a valid instance of table.
/// @post t is not a valid instance of table.
///
void ht_destroy( Table t );

/// Print information about the hash table (size, capacity, collisions,
/// rehashes).  If 'full' is true, also print the entire contents of the
/// hash table using the registered print function with each non-null entry.
///
/// @param t The table to display
/// @param full Do a full dump of entire table contents
/// @pre t is a valid instance of table.
///
void ht_dump( const Table t, bool full );

/// Get the value associated with a key from the table.  This function
/// uses the registered hash function to locate the key, and the
/// registered equals function to check for equality.
///
/// @pre The table must have the key, or the function will assert failure
/// @param t The table
/// @param key The key
/// @pre has( t, key) is true.
/// @pre t is a valid instance of table, and key is not NULL.
/// @return The value associated with the key
///
const void* ht_get( const Table t, const void* key );

/// Check if the table has a key.  This function uses the registered hash
/// function to locate the key, and the registered equals function to
/// check for equality.
///
/// @param t The table
/// @param key The key
/// @pre t is a valid instance of table, and key is not NULL.
/// @return Whether the key exists in the table.
///
bool ht_has( const Table t, const void* key );

/// Add a (key, value) pair to the table, or update an existing key's value.
/// This function uses the registered hash function to locate the key,
/// and the registered equals function to check for equality.
///
/// @param t The table
/// @param key The key
/// @param value The value
/// @exception Assert fails if it cannot allocate space
/// @pre t is a valid instance of table. key is not NULL. value is not NULL.
/// @post if size reached the LOAD_THRESHOLD, table has grown by RESIZE_FACTOR.
/// @return The old value associated with the key, if one exists.
///
void* ht_put( Table t, const void* key, const void* value );

/// Get the collection of keys from the table.  This function allocates
/// space to store the keys, which the caller is responsible for freeing.
///
/// @param t The table
/// @exception Assert fails if it cannot allocate space
/// @pre t is a valid instance of table.
/// @post client is responsible for freeing the returned array.
/// @return A dynamic array of keys
///
void** ht_keys( const Table t );

/// Get the collection of values from the table.  This function allocates
/// space to store the values, which the caller is responsible for freeing.
///
/// @param t The table
/// @exception Assert fails if it cannot allocate space
/// @pre t is a valid instance of table.
/// @post client is responsible for freeing the returned array.
/// @return A dynamic array of values
///
void** ht_values( const Table t );

#endif // TABLE_H
