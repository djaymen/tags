#ifndef SYSTPROJET_CMD_H
#define SYSTPROJET_CMD_H
#include <argp.h>

const char *argp_program_version =
  "tags 1.0";
const char *argp_program_bug_address =
  "\n<ga_benamara@esi.dz>\n<aaknoun.mehdi11@gmail.com>\n<ga_djelid@esi.dz>";

/* Program documentation. */
static char doc[] =
  "\nCmd example for tags managing -- Manual\n Examples :\n tag -a Red -c Color file.txt\n tag -d Green green_file.txt";

/* A description of the arguments we accept. */
static char args_doc[] = "FILE";

/* The options we understand. */
static struct argp_option options[] = {
  {"add",  'a',"TAG", 0  ,  "Add TAG to FILE" },
  {"delete",    'd', "TAG",      0,  "Remove TAG from FILE" },
  {"category",   'c', "CATEGORY",0,      "Add a new category" },
  {"output",   'o', "FILE", 0,
   "Output to FILE instead of standard output" },
  { 0 }
};

/* Used by main to communicate with parse_opt. */
struct arguments
{
  char *args[1];                /* FILE PATH */
  char *add_tag; 
  char *rm_tag;
  char *add_category;
  char *output_file;
};


/* Functions prototype ! */
static error_t parse_opt (int key, char *arg, struct argp_state *state);




#endif //SYSTPROJET_CMD_H