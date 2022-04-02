//
// file: amici.c
//
// @author Ryan Nowak rcn8263
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "hash.h"
#include "table.h"

// The maximum length of any single input command line is 1024 characters, 
// including the trailing newline and NUL characters.
#define BUFFER_SIZE 1024

Table t;
int people;
int friendships;

typedef struct person_s {
    char *firstName;            ///< first name of the person
    char *lastName;             ///< last name of the person
    char *handle;               ///< handle of the person
    struct person_s **friends;  ///< dynamic collection of friends
    size_t friend_count;        ///< current number of friends 
    size_t max_friends;         ///< current limit on friends
} person_t;

/// Add the specified user having the indicated first and last names to the 
/// database with the specified handle. Handles must be unique; names, 
/// however, may be duplicated
///
/// @pre names and handle are non-null and non-empty
/// @param firstName value in struct person_s that represents first name of user
/// @param lastName value in struct person_s that represents last name of user
/// @param handle unique identifier of user
void add(char *firstName, char *lastName, char *handle) {
    //handle already exists in table
    if (ht_has(t, handle)) {
        fprintf(stderr, "error: handle '%s' is already taken. Try another handle.\n", 
            handle);
    }
    else {
        person_t *person = (person_t *) malloc(sizeof(person_t));
        //person_t *person = malloc(sizeof(person_t));
        
        person->firstName = malloc(strlen(firstName) + 1);
        strcpy(person->firstName, firstName);
        
        person->lastName = malloc(strlen(lastName) + 1);
        strcpy(person->lastName, lastName);
        
        person->handle = malloc(strlen(handle) + 1) ;
        strcpy(person->handle, handle);
        
        person->friend_count = 0;
        person->max_friends = 10;
        person->friends = calloc(person->max_friends, 
            sizeof(char *) * person->max_friends);
        ht_put(t, (const void*)person->handle, (const void*)person);
        
        people += 1;
    }
}

/// Checks if there exists a friendship between person1 and person2
///
/// @param person1 pointer to an instance of struct person_s
/// @param person2 pointer to an instance of struct person_s
bool has_friendship(person_t *person1, person_t *person2) {
    int i = 0;
    while (person1->friends[i] != 0) {
        if (!strcmp(person1->friends[i]->handle, person2->handle)) {
            return true;
        }
        i++;
    }
    return false;
}

/// Create a friendship between the two users identified by the indicated 
/// handles. The handles must both exist, must be different (i.e., a user 
/// can't be their own "friend"), and there must not already be a friendship 
/// between these users.
///
/// @pre both handles are non-null and non-empty
/// @param handle1 unique identifier of user 1 
/// @param handle2 unique identifier of user 2
void add_friend(char *handle1, char *handle2) {
    //check if given handles exist in table
    if (!ht_has(t, handle1)) {
        fprintf(stderr, "error: '%s' is not a known handle\n", handle1);
    }
    else if (!ht_has(t, handle2)) {
        fprintf(stderr, "error: '%s' is not a known handle\n", handle2);
    }
    else {
        person_t *person1;
        person_t *person2;
        
        person1 = (person_t *)ht_get(t, (const void*)handle1);
        person2 = (person_t *)ht_get(t, (const void*)handle2);
        
        if (!has_friendship(person1, person2)) {
            person1->friends[person1->friend_count] = person2;
            person1->friend_count += 1;
            person2->friends[person2->friend_count] = person1;
            person2->friend_count += 1;
            
            friendships += 1;
            printf("%s and %s are now friends\n", 
                person1->handle, person2->handle);
        }
        else {
            fprintf(stderr, "error: '%s' and '%s' are already friends.\n", 
                person1->handle, person2->handle);
        }
    }
}

/// Removes the friendship that person1 has to person2. Must be called again
/// with the same people in the opposite order to remove the friendship between
/// them.
/// 
/// @param person1 pointer to an instance of struct person_s
/// @param person2 pointer to an instance of struct person_s
void remove_friend(person_t *person1, person_t *person2) {
    int i = 0;
    //char *temp;
    bool tmp = false; 
    while (person1->friends[i] != 0) {
        if (tmp) {
            person1->friends[i] = person1->friends[i+1];
        }
        if (!strcmp(person1->friends[i]->handle, person2->handle)) {
            person1->friends[i] = person1->friends[i+1];
            tmp = true;
        } 
        i++;
    }
    person1->friends[i-1] = 0;
    person1->friend_count -= 1;
}

/// Dissolve the friendship that exists between the specified users. The two 
/// handles must exist, and there must be a friendship between the users.
/// 
/// @pre both handles are non-null and non-empty
/// @param handle1 unique identifier of user 1
/// @param handle2 unique identifier of user 2
void unfriend(char *handle1, char *handle2) {
    //check if given handles exist in table
    if (!ht_has(t, handle1)) {
        fprintf(stderr, "error: '%s' is not a known handle\n", handle1);
    }
    else if (!ht_has(t, handle2)) {
        fprintf(stderr, "error: '%s' is not a known handle\n", handle2);
    }
    else {
        person_t *person1;
        person_t *person2;
        
        person1 = (person_t *)ht_get(t, (const void*)handle1);
        person2 = (person_t *)ht_get(t, (const void*)handle2);
    
        if (has_friendship(person1, person2)) {
            //Remove friendship from person 1
            remove_friend(person1, person2);
            //Remove friendship from person 2
            remove_friend(person2, person1);
            
            friendships -= 1;
            printf("%s and %s are no longer friends\n", 
                person1->handle, person2->handle);
        }
        else {
            fprintf(stderr, "error: '%s' and '%s' are were not friends.\n", 
                person1->handle, person2->handle);
        }
    }
}

/// prints out the data of the user in the format
/// firstName lastName ('handle')
/// 
/// @param handle unique identifier of user
void print_user(char *handle) {
    person_t *person;
    person = (person_t *)ht_get(t, (const void*)handle);
    
    printf("%s %s ('%s')", person->firstName, person->lastName, person->handle);
}

/// Count the number of existing friendships for the specified user, and 
/// report that. The specified handle must be in the system.
///
/// @pre handle is non-null and non-empty
/// @param handle unique identifier of user
void size(char *handle) {
    //check if given handles exist in table
    if (!ht_has(t, handle)) {
        fprintf(stderr, "error: '%s' is not a known handle\n", handle);
    }
    else {
        person_t *person;
        person = (person_t *)ht_get(t, (const void*)handle);
        
        printf("User ");
        print_user(handle);
        if (person->friend_count == 0) {
            printf(" has no friends\n");
        }
        else if (person->friend_count == 1) {
            printf(" has 1 friend\n");
        }
        else {
            printf(" has %ld friends\n", person->friend_count);
        }
    }
}

/// Find the entry for the specified user, and print the user's name and 
/// handle, followed by a list of the user's current friendships. The specified 
/// handle must be in the system.
///
/// @pre handle is non-null and non-empty
/// @param handle unique identifier of user
void print(char *handle) {
    //check if given handles exist in table
    if (!ht_has(t, handle)) {
        fprintf(stderr, "error: '%s' is not a known handle\n", handle);
    }
    else {
        size(handle);
        
        person_t *person;
          person = (person_t *)ht_get(t, (const void*)handle);
        
        int i = 0;
        while (person->friends[i] != 0) {
            printf("\t");
            print_user(person->friends[i]->handle);
            printf("\n");
            i++;
        }
    }
}

/// Report on the current contents of the network by printing the number of 
/// users in the system and the number of unique friendships
void stats() {
    printf("Statistics: ");
    if (people == 1) {
        printf("%d person, ", people);
    }
    else {
        printf("%d people, ", people);
    }
    if (friendships == 1) {
        printf("%d friendship\n", friendships);
    }
    else {
        printf("%d friendships\n", friendships);
    }
}


/// helper function used by Table t that will delete the given user and free all
/// of its data
/// 
/// @param key in the table represented by handle
/// @param value is the person_t that is associated with the given key
void delete_1_ptr_str(void *key, void *value) {
    person_t *person;
    person = (person_t *) value;
    free(person->firstName);
    free(person->lastName);
    free(key);
    free(person->friends);
    free(person);
}

/// creates a new hash table that the users will be stored in
void init_table(void) {
	t = ht_create(str_hash, str_equals, str_long_print, delete_1_ptr_str);
	people = 0;
	friendships = 0;
}

/// deletes the current table. Also goes through each user in the table and 
/// frees the data associated with them using the function delete_1_ptr_str.
void delete_table(void) {
    ht_destroy(t);
    people = 0;
    friendships = 0;
}

/// Delete the current collection of people and friendships in the network, 
/// returning it to an empty state.
void init() {
    delete_table();
    init_table();
    printf("system re-initialized");
}

/// Delete the current collection of people and friendships in the network, 
/// and exit from the program.
void quit() {
    delete_table();
}

/// prints out the contents of the current table
void print_table() {
    ht_dump(t, true);
}

int main(void) {
    
    char in[BUFFER_SIZE];
    const char *delim = " \n";
    char *token;
    
    init_table();
    
    do {
        printf("amici> ");
        if(fgets(in, BUFFER_SIZE, stdin) == NULL) {
            break;
        }
        if (!strcmp(in, "\n")) {
            continue;
        }
        
        //parses input into a char array of strings
        char *cmd[BUFFER_SIZE] = {NULL};
        int numArgs = 0;
        token = strtok(in, delim);
        while (token != NULL) {
            cmd[numArgs] = token;
            numArgs++;
            token = strtok(NULL, delim);
        }

        // goes through and performs the given command
        //add
        if (!strcmp(cmd[0], "add")) {
            if (numArgs == 4) {
                add(cmd[1], cmd[2], cmd[3]);
            }
            else {
                fprintf(stderr, 
                    "error: add command usage: first-name last-name handle\n");
            }
        }
        //friend
        else if (!strcmp("friend", cmd[0])) {
            if (numArgs == 3) {
                add_friend(cmd[1], cmd[2]);
            }
            else {
                fprintf(stderr, 
                    "error: friend command usage: handle1 handle2\n");
            }
        }
        //unfriend
        else if (!strcmp("unfriend", cmd[0])) {
            if (numArgs == 3) {
                unfriend(cmd[1], cmd[2]);
            }
            else {
                fprintf(stderr, 
                    "error: unfriend command usage: handle1 handle2\n");
            }
        }
        //print
        else if (!strcmp("print", cmd[0])) {
            if (numArgs == 2) {
                print(cmd[1]);
            }
            else {
                fprintf(stderr, 
                    "error: print command usage: handle\n");
            }
        }
        //size
        else if (!strcmp("size", cmd[0])) {
            if (numArgs == 2) {
                size(cmd[1]);
            }
            else {
                fprintf(stderr, 
                    "error: size command usage: handle\n");
            }
        }
        //stats
        else if (!strcmp("stats", cmd[0])) {
            if (numArgs == 1) {
                stats();
            }
            else {
                fprintf(stderr, 
                    "error: stats command usage: No arguments must be given\n");
            }
        }
        //init
        else if (!strcmp("init", cmd[0])) {
            if (numArgs == 1) {
                init();
            }
            else {
                fprintf(stderr, 
                    "error: init command usage: No arguments must be given\n");
            }
        }
        //quit
        else if (!strcmp("quit", cmd[0])) {
            if (numArgs == 1) {
                quit();
                return 0;
            }
            else {
                fprintf(stderr, 
                    "error: quit command usage: No arguments must be given\n");
            }
        }
        
    }
    while (1);
    
    quit();
    return 0;
}
