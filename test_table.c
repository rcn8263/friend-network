/// \file test_table.c
/// \brief A test program for the hash table.
/// These tests contain some redundancies but are fairly thorough.
///
/// @author Sean Strout (RIT CS)
/// @author bksteele (bks@cs.rit.edu)

#define _DEFAULT_SOURCE  // strdup

#include <assert.h>  // assert
#include <stdio.h>   // printf, fprintf
#include <stdlib.h>  // rand, srand, EXIT_SUCCESS
#include <string.h>  // strcmp
#include <stdbool.h> // bool
#include <string.h>  // strdup
#include <time.h>    // time
#include "hash.h"    // long_hash, long_equals, long_str_print, str_hash, str_equals
                     // str_long_print, longlong_print
#include "table.h"   // ht_create, ht_destroy, ht_dump, ht_get, ht_has, ht_put

/// Test function for long keys with c-string values.
/// @param no_rehash set to true to stop rehashing
void test_long_str( bool no_rehash ) {
    // The test data
    long elements[] = {
          10, 20, 30
        , 40, 50, 60
        , 70, 80, 90
        , 100, 110, 120
        , 130, 140, 150
        , 160};
    char* names[] = {
          "ten", "twenty", "thirty"
        , "forty", "fifty", "sixty"
        , "seventy", "eighty", "ninety"
        , "one hundred", "one hundred ten", "one hundred twenty"
        , "one hundred thirty", "one hundred forty", "one hundred fifty"
        , "one hundred sixty"};
    int NUM_ELEMENTS = INITIAL_CAPACITY ;
    if ( no_rehash ) NUM_ELEMENTS = INITIAL_CAPACITY * LOAD_THRESHOLD - 1;
    long missing = 0;
    char* ten = "NEW TEN VALUE";

    printf("========== test_long_str()...putting %d elements.\n", NUM_ELEMENTS);
    Table t = ht_create(long_hash, long_equals, long_str_print, NULL);

    // the hash table will rehash when the 13th key, 130, is ht_put
    for (int i=0; i<NUM_ELEMENTS; ++i) {
        ht_put( t, (void*)elements[i], (void*)names[i]);
    }
    // test update: the new value will not be found later.
    char* old_value = (char*)ht_put( t, (void*)elements[0], ten);
    if (old_value == NULL || strcmp(old_value, names[0]) != 0) {
        printf("ERROR: ht_put did NOT return correct old value for key upon update.\n");
    } else {
        printf("OK: ht_put returned correct old value for key upon update.\n");
    }
    ht_dump( t, true);

    // check for existence of all keys
    for (int i=0; i<NUM_ELEMENTS; ++i) {
        printf("ht_has(%lu)? %d\n", elements[i], false != ht_has( t, (void*)elements[i]));
    }
    printf("ht_has(0)? %d (%s)\n", ht_has( t, (void*)missing), "expected to be 0");  // missing key

    // check that all values match for all keys
    for (int i=0; i<NUM_ELEMENTS; ++i) {
        printf( "ht_get(%lu): %s\n", elements[i]
              , (char*) ht_get( t, (void*)elements[i]));
    }

    // grab all keys and make sure they were all found
    long** key_data = (long**) ht_keys(t);
    for (int i=0; i<NUM_ELEMENTS; ++i) {
        bool found = false;
        for (int j=0; j<NUM_ELEMENTS; ++j) {
            if ((long)key_data[i] == elements[j]) {
                found = true;
                break;
            }
        }
        printf("key: %lu,%sfound\n", (long)key_data[i], found ? " " : " (updated key) not ");
    }
    free(key_data);  // must do this, we claimed ownership by calling ht_keys()

    // grab all values and make sure they were all found
    char** val_data = (char**) ht_values(t);
    for (int i=0; i<NUM_ELEMENTS; ++i) {
        bool found = false;
        for (int j=0; j<NUM_ELEMENTS; ++j) {
            if (strcmp(val_data[i], names[j]) == 0) {
                found = true;
                break;
            }
        }
        printf("value: %s,%sfound\n", val_data[i], found ? " " : " (updated value expected to be) not ");
    }
    free(val_data);  // must do this, we claimed ownership by calling ht_values()

    ht_dump( t, false);
    ht_destroy(t);
}

/// Test function for c-string keys with long values. 
/// It doesn't re-test a ht_put update, ht_keys, or ht_values
/// @param no_rehash set to true to stop rehashing
void test_str_long( bool no_rehash ) {
    // Same data as the first test, but associated the other way
    const long elements[] = {
          10, 20, 30
        , 40, 50, 60
        , 70, 80, 90
        , 100, 110, 120
        , 130, 140, 150
        , 160};
    const char* names[] = {
          "ten", "twenty", "thirty"
        , "forty", "fifty", "sixty"
        , "seventy", "eighty", "ninety"
        , "one hundred", "one hundred ten", "one hundred twenty"
        , "one hundred thirty", "one hundred forty", "one hundred fifty"
        , "one hundred sixty"};

    int NUM_ELEMENTS = INITIAL_CAPACITY;
    if ( no_rehash ) NUM_ELEMENTS = INITIAL_CAPACITY * LOAD_THRESHOLD - 1;
    char* missing = "missing";

    printf("========== test_str_long()...putting %d elements.\n", NUM_ELEMENTS);
    Table t = ht_create(str_hash, str_equals, str_long_print, NULL);

    // rehash happens with 13th name, "one hundred thirty"
    for (int i=0; i<NUM_ELEMENTS; ++i) {
        ht_put( t, (void*)names[i], (void*)elements[i]);
    }
    ht_dump( t, true);

    // check that all keys are found
    for (int i=0; i<NUM_ELEMENTS; ++i) {
        printf("ht_has(%s)? %d\n", names[i], 0 != ht_has( t, (void*)names[i]));
    }
    printf("ht_has(missing)? %d (%s)\n", ht_has( t, missing), "expected to be 0");  // missing key

    // check that all values are retrievable for all keys
    for (int i=0; i<NUM_ELEMENTS; ++i) {
        printf("ht_get(%s): %lu\n", names[i], (long)ht_get( t, (void*)names[i]));
    }

    ht_dump( t, false);
    ht_destroy(t);
}

/// test_stress uses a hash table of long keys and NEGATED long values.
/// @param seed a seed for the random number generator
void test_stress(int seed) {

    printf("========== test_stress()...\n");
    const size_t NUM_ELEMENTS = 1000000;  // 1 million elements
    long* elements = (long*) malloc(NUM_ELEMENTS * sizeof(long));
    if (elements == NULL) {
        fprintf(stderr, "ERROR: test_stress failed.\n");
        assert(NULL);
    }

    srand(seed);  // seed the rng

    // populate elements with random integers
    for (size_t i=0; i<NUM_ELEMENTS; ++i) {
        elements[i] = rand();
    }

    // create hash table, key=int, value=int
    Table t = ht_create(long_hash, long_equals, long_long_print, NULL);

    // put all elements
    for (size_t i=0; i<NUM_ELEMENTS; ++i) {
        ht_put( t, (void*)elements[i], (void*)-elements[i]);
    }
    // put a negative key
    ht_put( t, (void*)-37, (void*)-37);

    // check that has finds each element
    for (size_t i=0; i<NUM_ELEMENTS; ++i) {
        if (ht_has( t, (void*)elements[i]) != true) {
            fprintf(stderr, "ERROR: test_stress, ht_has() check failed.\n");
            free(elements);
            assert(NULL);
        }
    }

    // check that ht_get works
    for (size_t i=0; i<NUM_ELEMENTS; ++i) {
        if (-(long)ht_get( t, (void*)elements[i]) != elements[i]) {
            fprintf(stderr, "ERROR: test_stress, ht_get() check failed.\n");
            free(elements);
            assert(NULL);
        }
    }
    if ( (long)ht_get( t, (void*)-37) != -37 ) {
        printf("ERROR: ht_get(-37): %ld.\n", (long)ht_get( t, (void*)-37));
    } else {
        printf("OK: ht_get(-37): %ld.\n", (long)ht_get( t, (void*)-37));
    }

    // these results depend on whether any duplicate values are randomly
    // generated
    ht_dump( t, false);
    ht_destroy(t);
    free(elements);
}

/// Test creating 2 tables at the same time.
void test2Tables() {
    // The test data
    const long elements[] = {
          10, 20, 30
        , 40, 50, 60
        , 70, 80, 90
        };
    const char* names[] = {
          "ten", "twenty", "thirty"
        , "forty", "fifty", "sixty"
        , "seventy", "eighty", "ninety"
        };
    int NUM_ELEMENTS = sizeof( names ) / sizeof( char* ) ;
    long missing = 0;
    char* ten = "NEW TEN VALUE";

    printf("========== test2Tables()...\n" );
#define NTABLES 2
    Table t[NTABLES] = { NULL, NULL };
    t[0] = ht_create(long_hash, long_equals, long_str_print, NULL);
    t[1] = ht_create(long_hash, long_equals, long_str_print, NULL);

    for (int i=0; i<NUM_ELEMENTS; ++i) {
        ht_put( t[0], (void*)elements[i], (void*)names[i]);
        ht_put( t[1]
           , (void*)elements[NUM_ELEMENTS - 1 - i]
           , (void*)names[NUM_ELEMENTS - 1 -i ]);
    }
    // only update one table: new value won't be found later in one table.
    char* old_value = (char*)ht_put( t[0], (void*)elements[0], ten);
    if (old_value == NULL || strcmp(old_value, names[0]) != 0) {
        printf("ERROR: update did not return old value.\n");
    }
    printf( "table 0 ht_get(%lu): %s\n", elements[0]
          , (char*) ht_get( t[0], (void*)elements[0]));
    printf( "table 1 ht_get(%lu): %s\n", elements[0]
          , (char*) ht_get( t[1], (void*)elements[0]));
    
    for (int j=0; j<NTABLES; ++j) {
        // check for existence of all keys
        for (int i=0; i<NUM_ELEMENTS; ++i) {
            printf("ht_has(%lu)? %d\n"
                  , elements[i], ht_has( t[j], (void*)elements[i]));
        }
        printf("ht_has(0)? %d\n", ht_has( t[j], (void*)missing));  // missing key
    
        // check that all values match for all keys
        for (int i=0; i<NUM_ELEMENTS; ++i) {
            printf( "ht_get(%lu): %s\n", elements[i]
                  , (char*) ht_get( t[j], (void*)elements[i]));
        }
    
        // grab all keys and make sure they were all found
        long** key_data = (long**) ht_keys(t[j]);
        for (int i=0; i<NUM_ELEMENTS; ++i) {
            bool found = false;
            for (int j=0; j<NUM_ELEMENTS; ++j) {
                if ((long)key_data[i] == elements[j]) {
                    found = true;
                    break;
                }
            }
            printf("key: %lu,%sfound\n"
                  , (long)key_data[i], found ? " " : " not ");
        }
        free(key_data);  // must do because this code gets ht_keys() ownership.
    
        // grab all values and make sure they were all found
        char** val_data = (char**) ht_values(t[j]);
        for (int i=0; i<NUM_ELEMENTS; ++i) {
            bool found = false;
            for (int j=0; j<NUM_ELEMENTS; ++j) {
                if (strcmp(val_data[i], names[j]) == 0) {
                    found = true;
                    break;
                }
            }
            // a match to the updated value expected to be not found
            printf("value: %s,%sfound%s\n", val_data[i], found ? " " : " not ",
                strcmp(ten, val_data[i]) == 0 ? ", expected." : "" );
        }
        free(val_data);  // must do because this code gets values() ownership.
    } // end 2Table loop

    ht_dump( t[0], false);
    ht_destroy(t[0]);
    ht_dump( t[1], false);
    ht_destroy(t[1]);

    return;
}

/// l_ptr_hash interprets the element as a pointer to the long value and
/// hashes by simply casting to size_t.
/// The cast solves the problem that X % N is negative if X is negative, and
/// a hash value needs to be non-negative for proper array indexing.
/// @param element the item to hash
static size_t l_ptr_hash( const void* element) {
    return (size_t)*(long*)element;
}

/// l_ptr_equals interprets elements as pointers to long values to compare.
/// @param element1 the first item to check for equality
/// @param element2 the second item to check for equality
static bool l_ptr_equals( const void* element1, const void* element2) {
    return *(long*)element1 == *(long*)element2;
}

/// l_ptr_str_print interprets key, value pair as (long*, c-string) types.
/// @param key interpreted as a pointer to a long key value
/// @param value interpreted as a pointer to a c-string value
/// @pre both key and value must be non-NULL, heap-allocated items.
static void l_ptr_str_print( const void* key, const void* value) {
    printf( "%lu : %s", *(long*)key, (char*)value);
}

/// delete_l_ptr_str deletes an entry of type long*,char* pair.
/// @param key interpreted as a pointer to a long key value
/// @param value interpreted as a pointer to a c-string value
/// @pre both key and value must be non-NULL, heap-allocated items.
/// @post both key and value are invalid
static void delete_l_ptr_str( void* key, void* value ) {
    free( key );
    free( value );
}

/// Test a Hashtable( long*, char*) where both key and value are heap allocated.
void test_deletes() {
    // The test data
    const long elements[] = {
          10, 20, 30
        , 40, 50, 60
        , 70, 80, 90
        };
    const char* names[] = {
          "ten", "twenty", "thirty"
        , "forty", "fifty", "sixty"
        , "seventy", "eighty", "ninety"
        };
    int NUM_ELEMENTS = sizeof( names ) / sizeof( char* ) ;

    printf("========== test_deletes()...\n" );
    // Hashtable( long*, char*) destructs entry key, value pairs on heap
    Table t = ht_create(l_ptr_hash, l_ptr_equals, l_ptr_str_print, delete_l_ptr_str);

    // put dynamically
    for (int i=0; i<NUM_ELEMENTS; ++i) {
        // allocate a pointer to a long for a key 
        long* hky = malloc( sizeof( long));
        *hky = 10 * (i+1);
        ht_put( t, (void*)hky, (void*)strdup( names[i]) );
    }
    
    // check for existence of all keys
    for (int i=0; i<NUM_ELEMENTS; ++i) {
        printf("ht_has(%lu)? %d\n"
              , elements[i], ht_has( t, (void*)&elements[i]));
    }

    // check that all values match for all keys
    for (int i=0; i<NUM_ELEMENTS; ++i) {
        printf( "ht_get(%lu): %s\n", elements[i]
              , (char*) ht_get( t, (void*)&elements[i]));
    }

    // grab all keys and make sure they were all found
    long** key_data = (long**) ht_keys(t);
    for (int i=0; i<NUM_ELEMENTS; ++i) {
        bool found = false;
        for (int j=0; j<NUM_ELEMENTS; ++j) {
            if (*(long*)key_data[i] == elements[j]) {
                found = true;
                break;
            }
        }
        printf("key: %lu,%sfound\n"
              , *(long*)key_data[i], found ? " " : " not ");
    }
    free(key_data);  // must do because this code gets ht_keys() ownership.

    // grab all values and make sure they were all found
    char** val_data = (char**) ht_values(t);
    for (int i=0; i<NUM_ELEMENTS; ++i) {
        bool found = false;
        for (int j=0; j<NUM_ELEMENTS; ++j) {
            if (strcmp(val_data[i], names[j]) == 0) {
                found = true;
                break;
            }
        }
        printf("value: %s,%sfound\n", val_data[i], found ? " " : " not ");
    }
    free(val_data);  // must do because this code gets values() ownership.

    ht_dump( t, false);
    ht_destroy(t);

    return;
}

/// The main function runs the three test functions.
/// Usage: test_table #
/// @param argc command line argument count
/// @param argv command line arguments
/// @return EXIT_SUCCESS
int main(int argc, char* argv[]) {

    test_long_str( true);  // first test for long keys and c-string values
    test_str_long( true);  // second test for c-string keys and long values

    test_long_str( false);  // first test with rehashing
    test_str_long( false);  // second test with rehashing
    test2Tables();
    test_deletes();

#ifdef NOSTRESS
    printf("========== test_stress not done.\n");
    (void)argc; (void)argv; // ignore
#else
    // If no number is specified on the command line, seed on current time
    int seed = time(0);
    if (argc == 2) {
        seed = atoi(argv[1]); // deprecated, but what the heck; it's a test.
    }
    test_stress(seed);  // third test for int keys and int values
#endif

    return EXIT_SUCCESS;
}

