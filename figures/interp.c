#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <readline/readline.h>
#include <readline/history.h>

#define MAXCMDS 100

static int done = 0;

int com_help (char* arg);
int com_sleep (char* arg);
int com_quit (char* arg);

typedef struct {
  char *name;
  int (*func) (char*);
  char *doc;
} cmd_t;

cmd_t commands[MAXCMDS] = {
  { "help", com_help, "Display this text" },
  { "?", com_help, "Synonym for `help'" },
  { "sleep", com_sleep, "Wait <n> seconds" },
  { "quit", com_quit, "Quit" },
  { (char *) NULL, NULL, (char *) NULL },
};

char* stripwhite (char *string);
cmd_t* find_command (char *name);
void initialize_readline (void);
int execute_line (char *line);

int
interp_addcmd (char* cmd, int (*cmd_func) (char*), char* doc)
{
  int i;
  for (i = 0; i < (MAXCMDS - 1); ++i) {
    if (! commands[i].name) {
      commands[i].name = cmd;
      commands[i].func = cmd_func;
      commands[i].doc = doc;
      commands[++i].name = NULL;
      return 1;
    }
  }
  return 0;
}

void
interp_run (void)
{
  initialize_readline ();
  while (! done) {
    char* s;
    char* line;

    line = readline (">>> ");
    if (!line)
      break;

    s = stripwhite (line);
    if (*s) {
      add_history (s);
      execute_line (s);
    }

    free (line);
  }
}


int
execute_line (char *line)
{
  register int i;
  cmd_t *command;
  char *word;

  /* Isolate the command word. */
  i = 0;
  while (line[i] && isspace (line[i]))
    i++;
  word = line + i;

  while (line[i] && !isspace (line[i]))
    i++;

  if (line[i])
    line[i++] = '\0';

  command = find_command (word);

  if (!command)
    {
      fprintf (stderr, "%s: No such command.\n", word);
      return (-1);
    }

  /* Get argument to command, if any. */
  while (isspace (line[i]))
    i++;

  word = line + i;

  /* Call the function. */
  return (*(command->func)) (word);
}

/* Look up NAME as the name of a command, and return a pointer to that
   command.  Return a NULL pointer if NAME isn't a command name. */
cmd_t *
find_command (char *name)
{
  int i;
  for (i = 0; commands[i].name; i++)
    if (strcmp (name, commands[i].name) == 0)
      return (&commands[i]);
  return (cmd_t *) NULL;
}

char *
stripwhite (char *string)
{
  register char *s, *t;

  for (s = string; isspace (*s); s++)
    ;
    
  if (*s == 0)
    return (s);

  t = s + strlen (s) - 1;
  while (t > s && isspace (*t))
    t--;
  *++t = '\0';

  return s;
}


/* Interface to Readline Completion */

char *command_generator (const char* text, int state);
char **interp_completion (const char* text, int start, int end);

void
initialize_readline (void)
{
  /* Allow conditional parsing of the ~/.inputrc file. */
  rl_readline_name = "ISEL2014";

  /* Tell the completer that we want a crack first. */
  rl_attempted_completion_function = interp_completion;
}

/* Attempt to complete on the contents of TEXT.  START and END bound the
   region of rl_line_buffer that contains the word to complete.  TEXT is
   the word to complete.  We can use the entire contents of rl_line_buffer
   in case we want to do some simple parsing.  Return the array of matches,
   or NULL if there aren't any. */
char **
interp_completion (const char *text, int start, int end)
{
  char **matches = (char **) NULL;

  /* If this word is at the start of the line, then it is a command
     to complete.  Otherwise it is the name of a file in the current
     directory. */
  if (start == 0)
    matches = rl_completion_matches (text, command_generator);

  return (matches);
}

/* Generator function for command completion.  STATE lets us know whether
   to start from scratch; without any state (i.e. STATE == 0), then we
   start at the top of the list. */
char *
command_generator (const char *text, int state)
{
  static int list_index, len;
  char *name;

  /* If this is a new word to complete, initialize now.  This includes
     saving the length of TEXT for efficiency, and initializing the index
     variable to 0. */
  if (!state) {
    list_index = 0;
    len = strlen (text);
  }

  /* Return the next name which partially matches from the command list. */
  while ((name = commands[list_index].name)) {
    list_index++;

    if (strncmp (name, text, len) == 0)
      return strdup (name);
  }

  /* If no names matched, then return NULL. */
  return (char *) NULL;
}

int
com_help (char *arg)
{
  int i;
  int printed = 0;

  for (i = 0; commands[i].name; i++) {
    if (!*arg || (strcmp (arg, commands[i].name) == 0)) {
      printf ("%s\t\t%s.\n", commands[i].name, commands[i].doc);
      printed++;
    }
  }

  if (!printed) {
    printf ("No commands match `%s'.  Possibilties are:\n", arg);

    for (i = 0; commands[i].name; i++) {
      /* Print in six columns. */
      if (printed == 6) {
        printed = 0;
        printf ("\n");
      }

      printf ("%s\t", commands[i].name);
      printed++;
    }

    if (printed)
      printf ("\n");
  }
  return 0;
}


int
com_sleep (char *arg)
{
  sleep (atoi(arg));
  return 0;
}

int
com_quit (char *arg)
{
  done = 1;
  return 0;
}

