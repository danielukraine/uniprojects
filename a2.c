//---------------------------------------------------------------------------------------------------------------------
// Assignment 2, wordle game
//
// Tutor: <Julia Herbsthofer>
//
// Author: <12143302>
//---------------------------------------------------------------------------------------------------------------------

#define ANSI_COLOR_RESET "\x1b[0m"
#define ANSI_COLOR_BLUE "\x1b[94m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define LENGTH_OF_WORD 5
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

struct Attemp 
{
  char attemp_guess_[16];
};

//---------------------------------------------------------------------------------------------------------------------
/// Displays all the guesses, that were made during the game. It has 2 modes: displaying with color and without.
/// @param attemps pointer to the first element of array of all the attempts that were made
/// @param attemp current number of attempts
/// @param color_enabled 0 for displaying the attempts without colors, 1 for displaying attempts with color

void showingGuesses(struct Attemp *attemps, int attemp, int color_enabled) 
{
  for (int i = 0; i <= attemp; i++) 
  {
    char *current_try = attemps[i].attemp_guess_;

    //Printing just the initial string if the color is off
    if (color_enabled == 0) 
    {
      printf("|%s\n", current_try);
    } 
    else 
    {
      printf("|");

      //Printing each element of the given string deleting the mark and adding the color
      for (int j = 0; current_try[j] != '\0'; j++) 
      {
        if (current_try[j] == '*') 
        {
          printf(" ");
          printf("%s%c%s", ANSI_COLOR_BLUE, current_try[j + 1], ANSI_COLOR_RESET);
          j++;
        } 
        else if (current_try[j] == '~') 
        {
          printf(" ");
          printf("%s%c%s", ANSI_COLOR_MAGENTA, current_try[j + 1], ANSI_COLOR_RESET);
          j++;
        } 
        else 
        {
          printf("%c", current_try[j]);
        }
      }
      printf("\n");
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------
/// Gets the input for a guess from user and checks if it is valid
/// @param user_input string, where the input will be stored
/// @return 0 if the input is EOF
/// @return 1 if the input is longer than 5 characters or contains invalid characters
/// @return 2 if the input is correct

int getTheGuess(char *user_input) 
{
  int wrong_character = 0;
  int added_characters = 0;

  printf("\nPlease enter your guess:\n > ");

  while (1) 
  {
    int ch = getchar();

    if (ch == EOF) 
    {
      return 0;
    }

    if (ch == '\n') 
    {
      break;
    }

    if (isspace(ch)) 
    {
      continue;
    }

    if (!isalpha(ch)) 
    {
      wrong_character = 1;
    }

    if (added_characters < LENGTH_OF_WORD) 
    {
      *user_input++ = toupper(ch);
      added_characters++;
    } 
    else 
    {
      printf("ERROR: Word is not 5 characters long.\n");
      while (getchar() != '\n');
      return 1;
    }
  }

  if (added_characters < LENGTH_OF_WORD) 
  {
    printf("ERROR: Word is not 5 characters long.\n");
    return 1;
  }

  if (wrong_character == 1) 
  {
    printf("ERROR: Word contains invalid characters.\n");
    return 1;
  }

  return 2;
}

//---------------------------------------------------------------------------------------------------------------------
/// Changes all the letters in the solution to uppercase
/// @param solution pointer to the first character of the solution string

void solutionToUpper(char *solution) 
{
  for (int i = 0; *(solution + i) != '\0'; i++) 
  {
    *(solution + i) = toupper(*(solution + i));
  }
}

//---------------------------------------------------------------------------------------------------------------------
/// Checks if the given solution is correct
/// @param solution pointer to the first character of the solution string
/// @return 0 if the solution is not 5 characters long or contains invalid characters
/// @return 1 if the solution is correct

int solutionCheck(char *solution) 
{
  int length = 0;
  int wrong_character = 0;

  for (int i = 0; *(solution + i) != '\0'; i++) 
  {

    //Check for invalid characters
    if (!isalpha(*(solution + i))) 
    {
      wrong_character = 1;
    }
    length++;
  }

  if (length != LENGTH_OF_WORD) 
  {
    printf("ERROR: Solution argument must be 5 characters long.\n");
    return 0;
  }

  //If there is an invalid character and input is 5 characters
  if (wrong_character == 1) 
  {
    printf("ERROR: Solution argument contains invalid characters.\n");
    return 0;
  }

  //If solution is valid, making it to uppercase
  solutionToUpper(solution);
  return 1;
}

//---------------------------------------------------------------------------------------------------------------------
/// Compares the user input and the solution. Creates a string containing the information about the guess
/// @param answer pointer to the first character of the solution string
/// @param user_guess pointer to the first character of users guess
/// @param attemps pointer to the first element of the attempts array
/// @param attemp current number of the attempt
/// @return amount of matches between user guess and solution

int compareGuessAndSolution(char *answer, char *user_guess, struct Attemp *attemps, int attemp) 
{
  char *current_character = attemps[attemp].attemp_guess_;
  int matches = 0;

  //Start of the loop to find matches. Every iterration moves pointer by one to acceses the next element and saves the result to attemp
  for (int i = 0; i < LENGTH_OF_WORD; i++) 
  {
    int is_match_in_wrong_place = 0;

    if (*(answer + i) == *(user_guess + i)) 
    {
      *current_character++ = ' ';
      *current_character++ = '*';
      *current_character++ = *(user_guess + i);
      matches++;
    } 
    else 
    {
      //Start of the loop to find matching element in wrong place
      for (int j = 0; j < LENGTH_OF_WORD; j++) 
      {
        if (*(answer + j) == *(user_guess + i) && i != j) 
        {
          is_match_in_wrong_place = 1;
          break;
        }
      }

      if (is_match_in_wrong_place) 
      {
        *current_character++ = ' ';
        *current_character++ = '~';
        *current_character++ = *(user_guess + i);
      } 
      //If no matches were found
      else 
      {
        *current_character++ = ' ';
        *current_character++ = ' ';
        *current_character++ = *(user_guess + i);
      }
    }
  }

  return matches;
}

//---------------------------------------------------------------------------------------------------------------------
/// Main logic of the game. Keeps track of attempts, gives feedback about the guess and checks the winning condition
/// @param answer solution as string
/// @param color_enabled 0 for displaying feedback to guesses without color, 1 - with color
/// @return 0 if the input is EOF - terminates the program
/// @return 1 if the user guessed the word
/// @return 2 if the user used all the attempts and didn’t get the solution

int gameStart(char answer[], int color_enabled) 
{
  struct Attemp attemps[6] = {{{0}}, {{0}}, {{0}}, {{0}}, {{0}}, {{0}}};
  char user_input[LENGTH_OF_WORD + 1] = {0};
  int attemp = 0;

  printf("Welcome to Evolving Spelling Puzzle!\n");

  while (1) 
  {
    int valid = getTheGuess(user_input);
    int matches = 0;

    //Break the game if the input is EOF
    if (valid == 0) 
    {
      return 0;
    }

    //Compare the string to solution and display the feedback if the input is valid
    if (valid == 2) 
    {
      matches = compareGuessAndSolution(answer, user_input, attemps, attemp);
      showingGuesses(attemps, attemp, color_enabled);
      attemp++;
    }

    //If no attemps left
    if (attemp == 6) 
    {
      printf("\nOh no! You did not guess the word correctly! :(\n");
      printf("The solution was: '%s'!\n", answer);
      return 2;
    }

    //If user won
    if (matches == 5) 
    {
      printf("\nGood job! You solved this puzzle!\n");
      return 1;
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------
/// Parses the flag for enabling the colorful displaying
/// @param argv pointer to the list of pointers of the arguments to the main function
/// @return -1 if the color argument is not 0 or 1
/// @return 0 if the color argument is NULL or 0, no color
/// @return 1 if the color argument is 1, enables color

int colorEnabling(char *argv[]) 
{
  //Check if the color argument was given
  if (argv[2] != NULL) 
  {
    //Check if it was given correctly
    if ((argv[2][0] != '1' && argv[2][0] != '0') || argv[2][1] != '\0') 
    {
      printf("ERROR: (Optional) color output parameter can either be enabled (1) or not (0).\n");
      return -1;
    }

    if (argv[2][0] == '1') 
    {
      return 1;
    }
  }

  return 0;
}

//---------------------------------------------------------------------------------------------------------------------
/// Main function <SOLUTION> <color_mode>(optionally)
/// @param argc count of arguments given to the main function
/// @param argv pointer to the list of pointers of the arguments to the main function
/// @return -1 if the are more than 3 argument or one of the arguments was given wrong
/// @return 1 if the word was guessed correctly
/// @return 2 if the word wasn`t guessed and the user has no attemp legt
int main(int argc, char *argv[]) {

  int color_enabled = 0;

  //Check for the right amount of arguments
  if (argc < 2 || argc > 3) 
  {
    printf("Usage: ./a2 <solution> [optional: color_enabled]\n");
    return -1;
  }

  color_enabled = colorEnabling(argv);

  if (color_enabled == -1)
  {
    return -1;
  }

  //Getting the answer from the second argument
  char *answer = argv[1];
  int valid_solution = solutionCheck(answer);

  if (!valid_solution) {
    return -1;
  }
  
  //Start the game if solution is valid
  if (valid_solution) {
    int result = gameStart(answer, color_enabled);
    return result;
  }
}
