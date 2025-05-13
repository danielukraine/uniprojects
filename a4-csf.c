//---------------------------------------------------------------------------------------------------------------------
// Assignment 4, Spice pretending card game
//
// Tutor: <Julia Herbsthofer>
//
// Author: <12143302>
//---------------------------------------------------------------------------------------------------------------------

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>


enum Spices
{
  CHILI_SORTING_VALUE = 100,
  PEPPER_SORTING_VALUE = 200,
  WASABI_SORTING_VALUE = 300, 
};

enum ReturnValues
{
  COULDNT_WRITE_RESULTS = -14,
  WRONG_MAGIC_NUMBER = -13,
  MEMORY_ALLOCATION_FAIL = -12,
  COULDNT_OPEN_FILE = -11,
  INVALID_COMMAND = -10,
  INVALID_NUMBER_OF_PARAMETERS = -9,
  INVALID_COMMAND_FOR_TURN = -8,
  INVALID_CARD_FORMAT = -7,
  CARD_NOT_IN_HAND = -6,
  INVALID_VALUE = -5,
  INVALID_SPICE = -4,
  NO_CARDS_IN_THE_PIPE = -3,
  INVALID_CHALLENGE_ARGUMENT = -2,
  INVALID_INPUT = 0,
  CARD_WAS_DROWN = 3,
  CARD_WAS_PLAYED = 6,
  CHALLENGE_VALUE = 7,
  CHALLENGE_SPICE = 8,
  QUIT = 9,
  CHALLENGE_NOT_ALLOWED = 11,
  CHALLENGE_ALLOWED = 12,
  CHALLENGE_ONLY = 13,
  SPICE = 14,
  VALUE = 15,
  LAST_CARD_WAS_PLAYED = 16,
  BONUS_POINTS = 17,
};

struct Card
{
  int value_;
  char spice_;
  int sorting_value_;
};

struct Node
{
  struct Card card_;
  struct Node* next_;
};

struct Result
{
  int points_;
  int game_status_;
};

//---------------------------------------------------------------------------------------------------------------------
/// Copies original string and saves it to a new one 
/// @param str pointer to the original string that has to be copied 
/// @return pointer to the copy of the original string

char* copyString(char* original_string) 
{
  int size = strlen(original_string);
  char* copied_string = malloc((size + 1) * sizeof(char));  

  if (copied_string == NULL)
  { 
    return NULL;
  }

  for (int i = 0; i < size; i++) 
  {
    *(copied_string + i) = *(original_string + i);
  }

  *(copied_string + size) = '\0';

  return copied_string;
}

//---------------------------------------------------------------------------------------------------------------------
/// Prints the error
/// @param error_code error value from ERROR_VALUES enum

void printTheError(int error_code)
{
  if (error_code == INVALID_COMMAND)
  {
    printf("Please enter a valid command!\n");
  }else if (error_code == INVALID_NUMBER_OF_PARAMETERS)
  {
    printf("Please enter the correct number of parameters!\n");
  }else if (error_code == INVALID_COMMAND_FOR_TURN)
  {
    printf("Please enter a command you can use at the moment!\n");
  }else if (error_code == INVALID_CARD_FORMAT)
  {
    printf("Please enter the cards in the correct format!\n");
  }else if (error_code == INVALID_CHALLENGE_ARGUMENT)
  {
    printf("Please choose SPICE or VALUE!\n");
  }
}

//---------------------------------------------------------------------------------------------------------------------
/// Validates if the command is used with valid amount of arguments and if the command can be used in current turn
/// @param valid_arguments_count how much arguments is expected for a valid command to have
/// @param arguments_count  how much arguments the command actually has in the input
/// @param input command as string
/// @param challenge_status CHALLENGE_ALLOWED if "challenge" can be called at the moment, CHALLENGE_NOT_ALLOWED if
/// it cant be played at the moment and CHALLENGE_ONLY if challenge must be played
/// @return 1 if the command is with valid amount of argument and can be played at the turn
/// @return INVALID_NUMBER_OF_PARAMETERS if there are too many or too few arguments
/// @return INVALID_COMMAND_FOR_TURN if command cant be played at the moment

int validateCommand(int valid_arguments_count, int arguments_count, char* input, int challenge_status)
{
    if (valid_arguments_count == arguments_count)
    {
      if (challenge_status == CHALLENGE_ONLY && strcmp(input, "challenge") == 0)
      {
        return 1;
      }else if (challenge_status == CHALLENGE_NOT_ALLOWED && strcmp(input, "challenge") != 0)
      {
        return 1;
      }else if (strcmp(input, "quit") == 0)
      {
        return 1;
      }else if (challenge_status == CHALLENGE_ALLOWED)
      {
        return 1;
      }else{
        return INVALID_COMMAND_FOR_TURN;
      }
    }else{
      return INVALID_NUMBER_OF_PARAMETERS;
    }
}

//---------------------------------------------------------------------------------------------------------------------
/// Validates card for formate "value"_"spice"
/// @param card_as_string card to check in string format
/// @return INVALID_INPUT if the format of the card is invalid
/// @return 1 if the format of the card is valid

int validateCard(char *card_as_string)
{
  //If a card is missing in the players turn
  if (card_as_string == NULL)
  {
    return INVALID_INPUT;
  }

  int i = 1;
  int length = strlen(card_as_string);
  if (length < 3 || length > 5)
  {
    return INVALID_INPUT;
  } 

  //If the card doesnt start with a digit or starts with 0
  if (!isdigit(card_as_string[0]) || card_as_string[0] == '0')
  {
    return INVALID_INPUT;
  }

  //If the value is 10
  if (card_as_string[1] == '0')
  {
    if (card_as_string[0] == '1')
    {
      i = 2;
    }else{
      return INVALID_INPUT;
    }
  }

  //Check fot an underscore after the value
  if ( card_as_string[i] != '_')
  {
    return INVALID_INPUT;
  }

  //Check for value after underscore
  if (!isalpha((card_as_string[i + 1])))
  {
    return INVALID_INPUT;
  }

  if (card_as_string[i + 1] != 'c' && card_as_string[i + 1] != 'w' && card_as_string[i + 1] != 'p' )
  { 
    return INVALID_INPUT;
  }
  
  //Check for extra characters when the card is expected to end
  if (card_as_string[i + 2] != '\0')
  {
    return INVALID_INPUT;
  }

  return 1;
}

//---------------------------------------------------------------------------------------------------------------------
/// Adds whitespaces between arguments of the command and deletes all unnecessary whitespaces 
/// @param input original user string of arbitrary length
/// @return pointer to the new string of format "command (argument_1) (argument_2)"
/// @return NULL if memory allocation failed

char* inputWithWhiteSpaces(char* input)
{
  char* new_string = malloc((strlen(input)*2 + 1) * sizeof(char));

  if (new_string == NULL)
  {
    free(input);
    return NULL;
  }

  int j = 0;
  int word_started = 0;

  for (int i = 0; input[i] != '\0'; i++)
  {
    //Skip spaces at the beggin of the string
    if (word_started == 0 && isspace(input[i]))
    {
      continue;
    }

    //Skip trailing space at the end
    if (isspace(input[i]) && input[i+1] == '\0')
    {
      continue;
    }

    //We add the char to the new string only if it is not a space or is space and the next char is not a space
    if (!isspace(input[i]) || (word_started == 1 && isspace(input[i]) && !isspace(input[i + 1])))
    {
      word_started = 1;
      new_string[j++] = input[i];
    }

  }
   new_string[j] = '\0';

  free(input);
  return new_string;

}
//---------------------------------------------------------------------------------------------------------------------
/// Gets the input of arbitrary length entered by user
/// @return pointer to the entered string  
/// @return NULL if memory allocation failed

char *getTheInput(void)
{
  int buffer_size = 50;

  char *new_string = malloc(buffer_size * sizeof(char));
  int char_seen = 0;

  if (!new_string) 
  {
    return NULL;
  }

  char *character = new_string;
  int char_count = 0;

  while (1) 
  {
    char ch = getchar();

    if ((ch == '\n' && char_count == 0) || (ch == '\n' && char_seen == 0))
    {
      free(new_string);
      return "";
    }

    if (ch == '\n') 
    {
      break;
    }

    char_count++;
    
    if (!isspace(ch))
    {
      char_seen = 1;
    }

    // Resize buffer if necessary
    if (char_count > buffer_size) 
    {
      buffer_size *= 2;
      char *temp = realloc(new_string, buffer_size * sizeof(char));
      if (!temp) 
      {
        free(new_string);
        return NULL;
      }

      new_string = temp;
      character = new_string + char_count - 1;
    }

    *character++ = tolower(ch); 
  }

  // Null-terminate the buffer
  *character = '\0';

  char *temp = realloc(new_string, (char_count + 1) * sizeof(char));

  if (!temp) 
  {
    free(new_string);
    return NULL;
  }

  new_string = temp;

  return new_string;
}

//---------------------------------------------------------------------------------------------------------------------
/// Checks, if the users input is in valid format "command (argument_1) (argument_2)"
/// @param input user's input with whitespaces between arguments
/// @param cards_played pointer to the list of cards as strings where we save cards in case "play"
/// @param challange_status parameter to check if challanged is a valid command for the turn
/// @return CARD_WAS_PLAYED if valid "play" command was entered
/// @return CARD_WAS_DROWN if valid "draw" command was entered
/// @return CHALLENGE_VALUE if valid "challenge value" command was entered
/// @return CHALLENGE_SPICE if valid "challenge spice" command was entered
/// @return QUIT if valid "quit" command was entered
/// @return INVALID_CARD_FORMAT if invalid card format was entered in "play" command
/// @return INVALID_CHALLENGE_ARGUMENT if the argument for challenge is not "spice" or "value"
/// @return INVALID_NUMBER_OF_PARAMETERS (saved in variables valid_for_quit, valid_test_result, valid_for_draw,
/// valid_for_challange) in case if the number of arguments for specific command is invalid
///@return INVALID_COMMAND_FOR_TURN (saved in variables valid_for_quit, valid_test_result, valid_for_draw,
/// valid_for_challange) in case if the command can't be played at the moment

int validateTurnsFormat(char* input, char* cards_played[], int challenge_status) {
    int argument_count = 1;
    for (int i = 0; *(input + i) != '\0'; i++)
    {
      if (*(input + i) == ' ')
      {
        argument_count++;
      }
    }

    char* command = strtok(input, " ");
    char* first_argument = strtok(NULL, " ");
    char* second_argument = strtok(NULL, " ");

    if (strcmp(command, "play") == 0)
    {
      int valid_test_result = validateCommand(3, argument_count, input, challenge_status);
      if (valid_test_result == 1)
      {
        if (validateCard(first_argument) && validateCard(second_argument))
        {
          cards_played[0] = copyString(first_argument);
          cards_played[1] = copyString(second_argument);

          if (cards_played[0] == NULL || cards_played[1] == NULL)
          {
            free(cards_played[0]);
            free(cards_played[1]);         
            return MEMORY_ALLOCATION_FAIL;
          }    
          return CARD_WAS_PLAYED;
        }else{
          return INVALID_CARD_FORMAT;
        }
      }
      return valid_test_result;
    }

    if (strcmp(command, "quit") == 0)
    {
      int valid_for_quit = validateCommand(1, argument_count, input, challenge_status);

      if (valid_for_quit == 1)
      {
        return QUIT;
      }
      return valid_for_quit;
    }

    if (strcmp(command, "draw") == 0)
    {
      int valid_for_draw = validateCommand(1, argument_count, input, challenge_status);
      if (valid_for_draw == 1)
      {
        return CARD_WAS_DROWN;
      }
      return valid_for_draw;
    }

    if (strcmp(command, "challenge") == 0)
    {
      int valid_for_challenge = validateCommand(2, argument_count, input, challenge_status);

      if (valid_for_challenge == 1)
      {
        if (strcmp(first_argument, "value") == 0)
        {
          return CHALLENGE_VALUE;
        }else if (strcmp(first_argument, "spice") == 0)
        {
          return CHALLENGE_SPICE;
        }else{
          return INVALID_CHALLENGE_ARGUMENT;
        }
      }

      return valid_for_challenge;
    }

    return INVALID_COMMAND;
}

//---------------------------------------------------------------------------------------------------------------------
/// Adds node to the linked list sorted(1) or unsorted(0) just as the last element
/// @param head pointer to the first element of the linked list
/// @param card card, that has to be added to the linked list
/// @param sorted (1) if we want to add it sorted by its sorting value and (0) is we add just at the end
/// @return 0 if memory allocation failed
/// @return 1 if success 

int addNodeSorted(struct Node* head, struct Card card, int sorted)
{
  struct Node* new_node = malloc(sizeof(struct Node));

  if (!new_node)
  {
    return MEMORY_ALLOCATION_FAIL;
  }

  new_node->card_ = card;
  struct Node* temp = head;
  new_node->next_ = NULL;

  if (sorted == 1)
  {
    while (temp->next_ != NULL && card.sorting_value_ > temp->next_->card_.sorting_value_)
    {
      temp = temp->next_;
    }

    new_node->next_ = temp->next_;
    temp->next_ = new_node;    
    return 1;

  }else{

    while (temp->next_ != NULL)
    {
      temp = temp->next_;
    }
    temp->next_ = new_node;
    return 1;
  }
}

//---------------------------------------------------------------------------------------------------------------------
/// Checks if the specific card exists in the players hand
/// @param head  pointer to the first element of the linked list that represents players hand
/// @param card_value_to_check sorting value of the card
/// @return INVALID_INPUT if card doesnt exist in the hand
/// @return 1 if card exists in the hand

int checkNodeExistance(struct Node* head, int card_value_to_check)
{
  struct Node* temp = head;

  while (temp != NULL)
  {

    if(temp->card_.sorting_value_ == card_value_to_check)
    {
      return 1;
    }
    temp = temp->next_;
  } 
  
  return INVALID_INPUT;

}

//---------------------------------------------------------------------------------------------------------------------
/// Deletes a node (card) from a linked list (hand). Before exucuting needs a check if card exists.
/// @param head pointer to the first element of the linked list (hand)
/// @param card_value_to_delete sorting value of the card we want to delete

void deleteNode(struct Node* head, int card_value_to_delete)
{
    struct Node* temp = head;
    while (temp->next_->card_.sorting_value_ != card_value_to_delete)
    {
      temp = temp->next_;
    }

    struct Node* node_to_delete = temp->next_;
    temp->next_ = temp->next_->next_;
    free(node_to_delete);
}

//---------------------------------------------------------------------------------------------------------------------
/// Prints the linked list
/// @param head pointer to the first element of linked list (hand)

void printLinkedList(struct Node* head)
{
  struct Node* temp = head->next_;
  while (temp != NULL)
  {
    printf("%d_%c", temp->card_.value_, temp->card_.spice_); // Print without trailing space first
    temp = temp->next_;
    if (temp != NULL) // If not the last node, print a space
    {
      printf(" ");
    }else{
      printf("\n");
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------
/// Frees the linked list
/// @param head pointer to the first element of linked list

void freeLinkedList(struct Node* head)
{
  struct Node* temp = head; 
  while (temp != NULL)
  {
    struct Node* next = temp->next_;
    free(temp);
    temp = next;
  }
}

//---------------------------------------------------------------------------------------------------------------------
/// Calculates the sorting value (value, that we use to sort cards and also acceses them)
/// @param value value of the card (1-10)
/// @param spice spice of the card (w, p, c)
/// @return sorting value of the card 

int calculateSortingValue(int value, char spice)
{
  if (spice == 'c')
  {
    return CHILI_SORTING_VALUE + value;
  }else if (spice == 'w')
  {
    return WASABI_SORTING_VALUE + value;
  }else{
    return PEPPER_SORTING_VALUE + value;
  }
}

//---------------------------------------------------------------------------------------------------------------------
/// Converts a card from string format into card format. Function is being called only on valid cards.
/// @param card_as_string  
/// @return new_card card in struct Card format

struct Card getTheCard(char card_as_string[])
{
  int digets = 0;
  struct Card new_card = {0, 0, 0};
  char* character = card_as_string;
  for (int i = 0; *(character + i) != '\0'; i++)
  {
    if (isdigit(*(character + i)))
    {
      digets++;
      if (digets == 2)
      {
        new_card.value_ = 10;
      }else{
        //If we substract ASCII value of a number (1-9) from ASCII (0), we get this number as integer
        new_card.value_ =  *(character + i) - '0';
      }
      
    }

    if (isalpha(*(character + i)))
    {
      new_card.spice_ = *(character + i);
    } 

  }
  new_card.sorting_value_ = calculateSortingValue(new_card.value_, new_card.spice_);   
  return new_card;
}


//---------------------------------------------------------------------------------------------------------------------
/// Reads the cards from the file and devides them into players hands and cards pipe
/// @param file_name path to file as string
/// @param first_hand  linked list representing first players hand
/// @param second_hand linked list representing second players hand
/// @param cards_pipe linked list representing cards pipe
/// @return COULDNT_OPEN_FILE if file doesnt exist
/// @return WRONG_MAGIC_NUMBER if there is no "ESP\n" as the first line
/// @return 1 if everything correct

int readTheCards(char* file_name, struct Node* first_hand, struct Node* second_hand, struct Node* cards_pipe)
{
  int cards_given = 0;
  int turn = 1;
  int first_line = 1;
  FILE *file = fopen(file_name, "r");
  
  if (file == NULL)
  {
    return COULDNT_OPEN_FILE;
  }

  char ch;

  //5 to be able to contain value 10 and + "\0"
  char card_as_string[5] = {0};
  int index = 0;
  int result;


  while ((ch = fgetc(file)) != EOF)
  {
    if (index > 4)
    {
      fclose(file);
      return WRONG_MAGIC_NUMBER;
    }

    card_as_string[index++] = ch;
    

    if (ch == '\n')
    {
      card_as_string[index] = '\0';

      if (first_line == 1)
      {
        if (strcmp(card_as_string, "ESP\n") == 0)
        {
          memset(card_as_string, 0, sizeof(card_as_string));
          index = 0;
          first_line = 0;
          continue;
        }else{
          fclose(file);
          return WRONG_MAGIC_NUMBER;
        }

      }

      struct Card new_card = getTheCard(card_as_string);
      if (turn == 1 && cards_given < 12)
      {
        result = addNodeSorted(first_hand, new_card, 1);
      }else if (turn == -1 && cards_given <= 12)
      {
        result = addNodeSorted(second_hand, new_card, 1);
      }else{
        result = addNodeSorted(cards_pipe, new_card, 0);
      }

      if (result == MEMORY_ALLOCATION_FAIL)
      {
        return MEMORY_ALLOCATION_FAIL;
      }
      
      turn = -turn;
      cards_given++;

      memset(card_as_string, 0, sizeof(card_as_string));
      index = 0;
    }
  }

  // To handle cases, where the cards file is empty
  if (first_line == 1)
  {
    fclose(file);
    return WRONG_MAGIC_NUMBER;
  }

  fclose(file);
  return 1;
  
}

//---------------------------------------------------------------------------------------------------------------------
/// Logic for "play" command
/// @param cards_played list of 2 cards as strings that will be converted into card format
/// @param hand hand of a player that used the command
/// @param real_card real card played in previous turn
/// @param displayed_card displated card plater in previous turn
/// @return CARD_WAS_PLAYER if no error occured
/// @return code of the error if something went wrong

int playCommand(char* cards_played[], struct Node* hand, struct Card* real_card, struct Card* displayed_card)
{

    //Displayed card isnt changed yet so it saves the value of the last displayed card
    struct Card last_displayed_card = *displayed_card;
    struct Card last_real_card = *real_card;

    *real_card = getTheCard(cards_played[0]);    // Update the real card
    *displayed_card = getTheCard(cards_played[1]); // Update the displayed card


    int exists = checkNodeExistance(hand, real_card->sorting_value_);

    if (!exists) {
        //Reset the cards back to previous if error
        *real_card = last_real_card; 
        *displayed_card = last_displayed_card;
        printf("Please enter a card in your hand cards!\n");
        return CARD_NOT_IN_HAND;
    }
    if (((last_displayed_card.value_ == 10 || last_displayed_card.value_ == 0) && displayed_card->value_ > 3) || (last_displayed_card.value_ != 10 && displayed_card->value_ < last_displayed_card.value_))
    {
      *displayed_card = last_displayed_card;
      *real_card = last_real_card;
        printf("Please enter a valid VALUE!\n");
        return INVALID_VALUE;
    }

    if (displayed_card->spice_ != last_displayed_card.spice_ && last_displayed_card.spice_ != 0)
    {
      *displayed_card = last_displayed_card;
      *real_card = last_real_card;
      printf("Please enter a valid SPICE!\n");
      return INVALID_SPICE;
    }

    deleteNode(hand, real_card->sorting_value_);
    
    if (hand->next_ == NULL)
    {
      return LAST_CARD_WAS_PLAYED;
    }

    return CARD_WAS_PLAYED; 
}

//---------------------------------------------------------------------------------------------------------------------
/// Challenges the last played card when the command is “challenge”
/// @param real_card the last real played card
/// @param displayed_card the last displayed card
/// @param value_or_spice (VALUE_CHALLANGED) Or ((SPICE_CHALLENGED) determines if values or spices will be compared
/// @return 1 if the player who called challenge won
/// @return -1 if the player who accepted challenge won

int challengeCard(struct Card *real_card, struct Card *displayed_card, int value_or_spice)
{
  if (value_or_spice == CHALLENGE_VALUE)
  {
    if (real_card->value_ != displayed_card->value_)
    {
      printf("Challenge successful: %d_%c's value does not match the real card %d_%c.\n", displayed_card->value_, displayed_card->spice_, real_card->value_, real_card->spice_);
      return 1;
    }else{
      printf("Challenge failed: %d_%c's value matches the real card %d_%c.\n", displayed_card->value_, displayed_card->spice_, real_card->value_, real_card->spice_);
      return -1;
    }
  }

  if (value_or_spice == CHALLENGE_SPICE)
  {
    if (real_card->spice_ != displayed_card->spice_)
    {
      printf("Challenge successful: %d_%c's spice does not match the real card %d_%c.\n", displayed_card->value_, displayed_card->spice_, real_card->value_, real_card->spice_);
      return 1;
    }else{
      printf("Challenge failed: %d_%c's spice matches the real card %d_%c.\n", displayed_card->value_, displayed_card->spice_, real_card->value_, real_card->spice_);
      return -1;
    }
  }

  return 0;
}

//---------------------------------------------------------------------------------------------------------------------
/// Function for drawing a card from the pipe
/// @param players_hand pointer to the hand of player who draws a card
/// @param cards_pipe pointer to the pipe of cards
/// @return CARD_WAS_DROWN if player successfully drew a card 
/// @return NO_CARDS_IN_THE_PIPE if the last card in the pipe was drawn

int drawCard(struct Node* players_hand, struct Node* cards_pipe)
{
  //So we skip the dummy head
  struct Node* drawn_card = cards_pipe->next_;
  cards_pipe->next_ = drawn_card->next_;
  int result;

  if (cards_pipe->next_ != NULL)
  {

    result = addNodeSorted(players_hand, drawn_card->card_, 1);
    
    if (result == MEMORY_ALLOCATION_FAIL)
    {
      return MEMORY_ALLOCATION_FAIL;
    }

    free(drawn_card);

    return CARD_WAS_DROWN;
    
  }

  free(drawn_card);
  return NO_CARDS_IN_THE_PIPE;
}

//---------------------------------------------------------------------------------------------------------------------
/// Refills hands of the players after challenge was played
/// @param winner pointer to the challenge winners hand
/// @param looser pointer to the chellenge loosers hand
/// @param card_pike pointer to the pipe of cards
/// @return NO_CARDS_IN_THE_PIPE if the last card from the pipe was drown
/// @return 1 if there are cards in pipe and game continues 

int refillHands(struct Node* winner, struct Node* looser, struct Node* card_pipe)
{
  for (int i = 0; i < 2; i++)
  {
    if (drawCard(looser, card_pipe) == NO_CARDS_IN_THE_PIPE)
    {
      return NO_CARDS_IN_THE_PIPE;
    }
  }
  if (winner->next_ == NULL)
  {
    for (int i = 0; i < 6; i++)
    {
      if (drawCard(winner, card_pipe) == NO_CARDS_IN_THE_PIPE)
      {
        return NO_CARDS_IN_THE_PIPE;
      }
    }
  }
  
  if (card_pipe->next_ == NULL)
  {
    return NO_CARDS_IN_THE_PIPE;
  }

  return 1;
}

//---------------------------------------------------------------------------------------------------------------------
/// Prints the information message before every turn
/// @param game_start_message (1) to write the whole message, (0) to write only players turn
/// @param players_number number of the player that has the turn
/// @param challenge_status to add LAST CARD message if only challenge is allowed
/// @param displayed_card last played card
/// @param cards_played_in_round amount of the cards played in current round
/// @param current_player hand of the player that has the turn

void printGameMessage(int game_start_message, int players_number, int challenge_status, struct Card displayed_card,
                      int cards_played_in_round, struct Node* current_player)
{
  if (game_start_message == 1)
  {
    printf("\nPlayer %d:\n", players_number);
    if (displayed_card.value_ == 0)
    {
      printf("    latest played card:\n");
    }else if (challenge_status != CHALLENGE_ONLY){
      printf("    latest played card: %d_%c\n", displayed_card.value_, displayed_card.spice_);
    }else{
      printf("    latest played card: %d_%c LAST CARD\n", displayed_card.value_, displayed_card.spice_);
    }
    
    printf("    cards played this round: %d\n", cards_played_in_round);
    printf("    hand cards: ");
    printLinkedList(current_player);
  }
  printf("P%d > ", players_number);
}

//---------------------------------------------------------------------------------------------------------------------
/// Main logic for turn. Handles input from user and calls the used command or prints the error
/// @param current_player hand of the player who has the turn
/// @param real_card last real played card
/// @param displayed_card last displayed card
/// @param card_pipe pipe of cards
/// @param challenge_status to determine if challenge can or must be played at the moment
/// @return result of the executed command

int playTurn(struct Node* current_player, struct Card* real_card, struct Card* displayed_card, struct Node* card_pipe,
            int challenge_status)
{
  char* cards_played[2] = {0};

  char* input = getTheInput();

  if (input == NULL)
  {
    return MEMORY_ALLOCATION_FAIL;
  }

  if (strcmp(input, "") == 0)
  {
    printTheError(INVALID_COMMAND);
    return INVALID_COMMAND;
  }

  char* players_turn = inputWithWhiteSpaces(input);

  if (players_turn == NULL)
  {
    return MEMORY_ALLOCATION_FAIL;
  }

  int turns_command = validateTurnsFormat(players_turn, cards_played, challenge_status);

  if (turns_command == CARD_WAS_PLAYED)
  {

    free(players_turn);
    int result = playCommand(cards_played, current_player, real_card, displayed_card);
    free(cards_played[0]);
    free(cards_played[1]);
    return result;
  }

  if (turns_command == CARD_WAS_DROWN)
  {
    free(players_turn);
    return drawCard(current_player, card_pipe);
  }

  if (turns_command == CHALLENGE_SPICE || turns_command == CHALLENGE_VALUE) 
  {
    free(players_turn);
    return challengeCard(real_card, displayed_card, turns_command);
  }

  printTheError(turns_command);
  free(players_turn);
  return turns_command;
}

//---------------------------------------------------------------------------------------------------------------------
/// Returns the hand of the player that currently has the turn
/// @param first_player hand of the first player
/// @param second_plyaer hand of the second player
/// @param players_turn which player is currently on the turn
/// @return hand of the player that currently has turn

struct Node* getCurrentPlayer(struct Node* first_player, struct Node* second_player, int players_turn) {
  if (players_turn == 1)
  {
    return first_player;
  }else{
    return second_player;
  }
}

//---------------------------------------------------------------------------------------------------------------------
/// Switches the turn
/// @param players_turn who had the turn in previous turn
/// @return who has the turn in next turn

int turnSwitch(int players_turn)
{
  if (players_turn == 1)
  {
    return 2;
  }else{
    return 1;
  }
}

//---------------------------------------------------------------------------------------------------------------------
/// Main logic for round. Handles the result of the turn and passes result in the final function.
/// @param first_player hand of the first player
/// @param second_player hand of the second player
/// @param card_pipe pipe of cards
/// @param players_turn which player is at the turn
/// @return result_of_round struct element, that contains information about the action made in the turn 
/// and amount of the points gained in the round

struct Result playRound(struct Node* first_player, struct Node* second_player, struct Node* card_pipe, int *players_turn)
{

  printf("\n-------------------\n");
  printf("ROUND START\n");
  printf("-------------------\n");

  struct Card real_card = {0,0,0};
  struct Card displayed_card = {0,0,0};
  struct Result result_of_round = {0, 0};

  int cards_played_in_round = 0;
  struct Node* current_player = NULL;
  int challenge_status = CHALLENGE_NOT_ALLOWED;
  int game_start_message = 1;

  while(1)
  {

    current_player = getCurrentPlayer(first_player, second_player, *players_turn);
    printGameMessage(game_start_message, *players_turn, challenge_status, displayed_card, cards_played_in_round, current_player);
    int valid_turn = playTurn(current_player, &real_card, &displayed_card, card_pipe, challenge_status);

    if (valid_turn == MEMORY_ALLOCATION_FAIL)
    {
      result_of_round.game_status_ = MEMORY_ALLOCATION_FAIL;
      return result_of_round;
    }

    if (valid_turn > 0)
    {
      game_start_message = 1;
    }else{
      game_start_message = 0;
    }

    //If quit 
    if (valid_turn == QUIT)
    {
      result_of_round.game_status_ = QUIT;
      return result_of_round;
    }

    //If a card was played or drawed
    if (valid_turn == CARD_WAS_DROWN)
    {
      challenge_status = CHALLENGE_NOT_ALLOWED;
      *players_turn = turnSwitch(*players_turn);
      continue;
    }
    if (valid_turn == CARD_WAS_PLAYED)
    {
      challenge_status = CHALLENGE_ALLOWED;
      cards_played_in_round++;
      *players_turn = turnSwitch(*players_turn);
      continue;
    }

    if (valid_turn == 1 || valid_turn == -1)
    {
      if (challenge_status == CHALLENGE_ONLY && valid_turn == -1)
      {
        result_of_round.game_status_ = BONUS_POINTS;
        result_of_round.points_ = valid_turn * cards_played_in_round;
      }else{
        result_of_round.points_ = valid_turn * cards_played_in_round;
      }
      return result_of_round;
    }
    
    if (valid_turn == LAST_CARD_WAS_PLAYED)
    {
      *players_turn = turnSwitch(*players_turn);
      challenge_status = CHALLENGE_ONLY;
      cards_played_in_round++;
    }

    if (valid_turn == NO_CARDS_IN_THE_PIPE)
    {
      result_of_round.game_status_ = NO_CARDS_IN_THE_PIPE;
      return result_of_round;
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------
/// Updates overall points of a player who won the challenge
/// @param points amount of the points that has to be given
/// @param players_turn which player was at the moment when the points are being updated
/// @param points_player_1 points of the first player at the moment 
/// @param points_player_2 points of the second player at the moment 
/// @param bonus_points (0) is no bonus points assigned, (BONUS_POINTS) If 10 bonus points assigned
/// @return 1 if points were assigned to the first player
/// @return 2 if points were assigned to the second player

int updatePoints(int points, int *players_turn, int *points_player_1, int* points_player_2, int bonus_points)
{
  //The player who called the challenge won
  if(points > 0)
  {
    //If it was the first player
    if (*players_turn == 1)
    {
      *points_player_1 += points;
      printf("Player 1 gets %d points.\n", points);
      return 1;
    }else{
      //If it was the second player
      *points_player_2 += points;
      printf("Player 2 gets %d points.\n", points);
      return 2;
    }
    //If the player who accepted challenge won
  }else{
    if (*players_turn == 1)
    {
      //The points at this moment are negative so we multiply it by -1 to bring it back to positive
      *points_player_2 += -points;
      printf("Player 2 gets %d points.\n", -points);

      if (bonus_points == BONUS_POINTS)
      {
        *points_player_2 += 10;
        printf("Player 2 gets 10 bonus points (last card).\n");
      }
      return 2;
    }else{
      *points_player_1 += -points;
      printf("Player 1 gets %d points.\n", -points);
      if (bonus_points == BONUS_POINTS)
      {
        *points_player_1 += 10;
        printf("Player 1 gets 10 bonus points (last card).\n");
      }
      return 1;
    }
  } 
}

//---------------------------------------------------------------------------------------------------------------------
/// Prints results of the game and writes them into the file
/// @param points_player_1 final points of the first players
/// @param points_player_2 final points of the second player
/// @param file_name path to the config file
/// @return 0 if result was written in the file
/// @return COULDNT_WRITE_RESULT if an error occurred while writing results into file

int winnerDeclaring(int points_player_1, int points_player_2, char* file_name)
{
  FILE* file = fopen(file_name, "a");
  int valid_file = 1;

  if (file == NULL)
  {
    valid_file = 0;
  }

  if (points_player_2 > points_player_1) {
      printf("\nPlayer 2: %d points\n", points_player_2);
      printf("Player 1: %d points\n", points_player_1);
      printf("\nCongratulations! Player 2 wins the game!\n");
      
      if (valid_file == 1){
        fprintf(file, "\nPlayer 2: %d points\n", points_player_2);
        fprintf(file, "Player 1: %d points\n", points_player_1);
        fputs("\nCongratulations! Player 2 wins the game!\n", file);
      }
  } else {
      printf("\nPlayer 1: %d points\n", points_player_1);
      printf("Player 2: %d points\n", points_player_2);
      printf("\nCongratulations! Player 1 wins the game!\n");

      if (valid_file == 1){
        fprintf(file, "\nPlayer 1: %d points\n", points_player_1);
        fprintf(file, "Player 2: %d points\n", points_player_2);
        fputs("\nCongratulations! Player 1 wins the game!\n", file);
      }
  }

    
    if (points_player_1 == points_player_2)
    {
      printf("Congratulations! Player 2 wins the game!\n");
      if (valid_file == 1){
        fputs("Congratulations! Player 2 wins the game!\n", file);
      }
    }
  
  if (valid_file == 1)
  {
    fclose(file);
    return 0;
  }else{
    return COULDNT_WRITE_RESULTS;
  }
}

//---------------------------------------------------------------------------------------------------------------------
/// Main logic for the game. Starts and finishes the game, keeps track of the points.
/// @param file_name path to the file with cards
/// @param first_player hand of the first player
/// @param second_player hand of the second player
/// @param card_pipe pipe of cards
/// @return QUIT if "quit" was entered
/// @return COULDNT_WRITE_RESULTS if result wasnt written into file
/// @return winner number of the player who won

int startGame(char* file_name, struct Node* first_player, struct Node* second_player, struct Node* card_pipe)

{


  int reading_cards_result = readTheCards(file_name, first_player, second_player, card_pipe);

  if (reading_cards_result == COULDNT_OPEN_FILE || reading_cards_result == WRONG_MAGIC_NUMBER || reading_cards_result == MEMORY_ALLOCATION_FAIL)
  {

      return reading_cards_result;
  }

  int players_turn = 1;
  int points_player_1 = 0;
  int points_player_2 = 0;
  int refill;
  int winner = 0;
  int game_winner = 0;
  struct Result round_result = {0};

  printf("Welcome to Entertaining Spice Pretending!\n"); 

  while (1)
  {
    round_result = playRound(first_player, second_player, card_pipe, &players_turn);

    if (round_result.game_status_ == MEMORY_ALLOCATION_FAIL)
    {
      return MEMORY_ALLOCATION_FAIL;
    }

    if (round_result.game_status_ == NO_CARDS_IN_THE_PIPE)
    {
      break;
    }

    if (round_result.game_status_ == QUIT)
    {
 
      return QUIT;
    }
    
    if (round_result.game_status_ == BONUS_POINTS)
    {
      winner = updatePoints(round_result.points_, &players_turn, &points_player_1, &points_player_2, BONUS_POINTS);
    }else{
      winner = updatePoints(round_result.points_, &players_turn, &points_player_1, &points_player_2, 0);
    }
    
    if (winner == 1)
    {
      refill = refillHands(first_player, second_player, card_pipe);
      players_turn = 2;
    }else if (winner == 2){
      refill = refillHands(second_player, first_player, card_pipe);
      players_turn = 1;
    } 
    
    if (refill == NO_CARDS_IN_THE_PIPE)
    {
      break;
    }
  }

  int result = winnerDeclaring(points_player_1, points_player_2, file_name);
  if (result == COULDNT_WRITE_RESULTS)
  {
    return COULDNT_WRITE_RESULTS;
  }else{
    return game_winner;
  }
  
    
}

//---------------------------------------------------------------------------------------------------------------------
/// Main function
/// @param argc amount of the arguments entered
/// @param argv list of the arguments as strings
/// @return 0 if "quit was entered", results werent written into file or program was executed succesfully 
/// @return 1 if invalid amount of arguments was used
/// @return 2 if file couldnt be opened
/// @return 3 if file contains wrong magic number
/// @return 4 if Memory allocation failed

int main(int argc, char* argv[])
{ 
  if (argc != 2)
  {
    printf("Usage: ./a4-csf <config file>\n");
    return 1;
  }

  char* file_name = argv[1];

  struct Node* first_player = malloc(sizeof(struct Node));

  //Create a "dummy head" for each linked list
  first_player->card_.sorting_value_ = -1; 
  first_player->next_ = NULL;

  struct Node* second_player = malloc(sizeof(struct Node));

  second_player->card_.sorting_value_ = -1;
  second_player->next_ = NULL;

  struct Node* card_pipe = malloc(sizeof(struct Node));

  card_pipe->card_.sorting_value_ = -1;
  card_pipe->next_ = NULL;

  if (first_player == NULL || second_player == NULL || card_pipe == NULL)
  {
    free(first_player);
    free(second_player);
    free(card_pipe);
    printf("Error: Out of memory\n");
    return 4;
  }



  int game = startGame(file_name, first_player, second_player, card_pipe);

  freeLinkedList(first_player);
  freeLinkedList(second_player);
  freeLinkedList(card_pipe);

  if (game == COULDNT_OPEN_FILE)
  {
    printf("Error: Cannot open file: %s\n", file_name);
    return 2;
  }

  if (game == WRONG_MAGIC_NUMBER)
  {
    printf("Error: Invalid file: %s\n", file_name);
    return 3;
  }

  if (game == COULDNT_WRITE_RESULTS)
  {
    printf("Warning: Results not written to file!\n");
    return 0;
  }

  if (game == MEMORY_ALLOCATION_FAIL)
  {
    printf("Error: Out of memory\n");
    return 4;
  }

  return 0;
}

