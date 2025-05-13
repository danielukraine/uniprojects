//---------------------------------------------------------------------------------------------------------------------
// Assignment 3, Shopping list
//
// Tutor: <Julia Herbsthofer>
//
// Author: <12143302>
//---------------------------------------------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct Item
{
  char *item_name_;
  int amount_;
};

//---------------------------------------------------------------------------------------------------------------------
/// Compare function for qsort(). Compares 2 strings in terms of alphabetical order.
/// In case if both strings are equal, compares the amount of their items.
/// @param a pointer to the element of the Shopping list array
/// @param b pointer to the next element of the Shopping List array
/// @return negative number if the string a is lower in alphabetical order than b or amount a < b
/// @return positive number if the string a is higher in alphabetical order than b or amount a > b
/// @return 0 if the strings and their amount are the same

int compareString(const void *a, const void *b)
{
  const struct Item *item_1 = (const struct Item *)a;
  const struct Item *item_2 = (const struct Item *)b;

  int comparedStrings = strcmp(item_1->item_name_, item_2->item_name_);

  if (comparedStrings == 0)
  {
    return item_1->amount_ - item_2->amount_;
  }

  return comparedStrings;
}

//---------------------------------------------------------------------------------------------------------------------
/// Handles input of arbitrary length and saves the string on the heap.
/// @return pointer to the entered string 
/// @return NULL, if memory allocation failed
/// @return "", if the string is empty

char *handleInput(void)
{
  int buffer_size = 50;

  char *new_string = malloc(buffer_size * sizeof(char));

  if (!new_string) 
  {
    printf("[ERROR] Memory allocation failed!\n\n");
    return NULL;
  }

  char *character = new_string;
  int char_count = 0;

  while (1) 
  {
    char ch = getchar();

    if (char_count == 0 && ch == '\n')
    {
      free(new_string);
      return "";
    }

    if (ch == '\n') 
    {
      break;
    }

    char_count++;

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
      character = new_string + char_count - 1; // Adjust pointer after realloc
    }

    *character++ = ch; // Store character in buffer
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
/// Sorts the list using qsort() and prints out the shopping list in alphabetical order and frees the memory
/// @param shopping_list pointer to the first Item of the shopping list
/// @param items_count amount of the items in the shopping list
/// @param list_name pointer to the name of the list

void printSortedList(struct Item *shopping_list, int items_count, char *list_name)
{
  qsort(shopping_list, items_count, sizeof(struct Item), compareString);

  printf("\nShopping list: %s\n", list_name);
  for (int i = 0; i < items_count; i++)
  {
    printf(" * %d %s\n", shopping_list[i].amount_, shopping_list[i].item_name_);
    free(shopping_list[i].item_name_);
  }

  free(shopping_list);
}

//---------------------------------------------------------------------------------------------------------------------
/// Handles the input for the amount. Saves the integer to the shopping list if all conditions are met.
/// @param current_index the number of the Item we are assigning amount for.
/// @param current_item pointer to the shopping list
/// @return -1 if memory allocation failed
/// @return 0 if the input contains non digits (except leading + or -)
/// @return 1 if the input is valid and the number was assigned to the amount
/// @return 2 if quit was entered

int getTheNumber(int counter, struct Item *current_item)
{
  int i = 0;
  char *number = handleInput();
  if (!number)
  {
    printf("[ERROR] Memory allocation failed!\n\n");
    return -1;
  }

  if (strcmp(number, "quit") == 0)
  {
    free(number);
    return 2;
  }

  if (number[0] == '\0')
  {
    printf("[ERROR] The quantity must be an integer!\n\n");
    return 0;
  }

  if (number[0] == '+' || number[0] == '-')
  {
    i++;
  }

  //In case if there is no leading + or -, checking if the first character is digit
  if (!isdigit(number[i]))
  {
    printf("[ERROR] The quantity must be an integer!\n\n");
    free(number);
    return 0;
  }

  //Loop through the string to check if it contains not digits
  for (; number[i] != '\0'; i++)
  {
    if (!isdigit(number[i]))
    {
      printf("[ERROR] The quantity must be an integer!\n\n");
      free(number);
      return 0;
    }
  }

  //Converting the string to integer after validation and saving it in the list
  int amount = atoi(number);
  if (amount > 0)
  {
    (*(current_item + counter)).amount_ = amount;
    free(number);
    return 1;
  }
  else
  {
    printf("[ERROR] The quantity must be greater than 0!\n\n");
    free(number);
    return 0;
  }
}

//---------------------------------------------------------------------------------------------------------------------
/// Frees the allocated memory for every item in the list and for the list itself.
/// @param shopping_list pointer to the first Item of the shopping list
/// @param items_count amount of the items in the shopping list

void freeMemory(struct Item *shopping_list, int items_count)
{
  for (int i = 0; i < items_count; i++)
  {
    free(shopping_list[i].item_name_);
  }
  free(shopping_list);    
}


//---------------------------------------------------------------------------------------------------------------------
/// Gets the item and saves it on the shopping list. Handles all the cases of possible inputs.
/// @param shopping_list pointer to the first Item of the shopping list
/// @param items_count amount of the items in the shopping list
/// @return -1 if memory allocation failed
/// @return 0 if "done" was entered
/// @return 1 if "quit" was entered
/// @return 2 if the item was added to the shopping list

int getTheItem(struct Item *shopping_list, int items_count)
{
  int empty = 0;
  while (1)
  {
    if (empty == 0)
    {
      printf("\nItem %d:\n", items_count + 1);
    }
    printf("What do you want to add?\n > ");

    char *item = handleInput();

    if (!item)
    {
      printf("[ERROR] Memory allocation failed.\n");
      return -1;
    }

    if (strcmp(item, "quit") == 0)
    {
      free(item);
      return 1;
    }

    if (strcmp(item, "") == 0)
    {
      printf("[ERROR] Item name cannot be empty!\n\n");
      empty = 1;
      continue;
    }

    empty = 0;

    if (strcmp(item, "done") == 0)
    {
      free(item);
      return 0;
    }

    shopping_list[items_count].item_name_ = item;
    return 2;
  }
}

//---------------------------------------------------------------------------------------------------------------------
/// Processes the result of get_the_number function. 
/// @param shopping_list pointer to the first Item of the shopping list
/// @param items_count amount of the items in the shopping list
/// @return -1 if memory allocation failed
/// @return 0 if the amount was assigned to the item
/// @return 1 if "quit" was entered

int getAmount(struct Item *shopping_list, int items_count)
{
  while (1)
  {
    printf("How many pieces/grams/milliliters of this item do you want to get?\n > ");

    int result = getTheNumber(items_count, shopping_list);

    //Quit was entered
    if (result == 2)
    {
      return 1;
    }

    //Memory allocation failed
    if (result == -1)
    {
      return -1;
    }

    //Normal integer
    if (result == 1)
    {
      return 0;
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------
/// Main logic of the program. Keeps track of the amount of items, saves items and their amounts in the list.
/// Allocates memory for the shopping list, frees memory for the shopping list itself and all the items in it.
/// @param list_name pointer to the name of the list
/// @return 0 if "quit" or "done" was entered
/// @return 1 if memory allocation failed at any step

int addItemToTheList(char *list_name)
{
  int items_count = 0;
  int list_size = 3;
  struct Item *shopping_list = malloc(list_size * sizeof(struct Item));

  if (!shopping_list)
  {
    printf("[ERROR] Memory allocation failed.\n");
    return 1;
  }

  while (1)
  {
    if (items_count >= list_size)
    {
      list_size *= 2;
      struct Item *temp = realloc(shopping_list, list_size * sizeof(struct Item));

      if (!temp)
      {
        freeMemory(shopping_list, items_count);
        return 1;
      }

      shopping_list = temp;
    }

    int added_item = getTheItem(shopping_list, items_count);

    //If memory allocation failed
    if (added_item == -1)
    {
      freeMemory(shopping_list, items_count);
      return 1;
    //If quit was entered
    } else if (added_item == 1)
    {
      freeMemory(shopping_list, items_count);
      return 0;
    //If done was entered
    } else if (added_item == 0)
    {
      printSortedList(shopping_list, items_count, list_name);
      return 0;
    }

    int add_amount = getAmount(shopping_list, items_count);

    //If memory allocation failed
    if (add_amount == -1)
    {
      freeMemory(shopping_list, items_count);
      return 0;

    //If quit was entered
    }else if (add_amount == 1)
    {
      free(shopping_list[items_count].item_name_);
      freeMemory(shopping_list, items_count);
      return 0;
    }

    items_count++;
  }
}

//---------------------------------------------------------------------------------------------------------------------
/// Allocates memory for the shopping list name and saves the pointer to it.
/// @return pointer to the name of the list if all conditions are met
/// @return "" if "quit" was entered
/// @return NULL if memory allocation failed at any step

char *getTheName(void)
{
  printf("\nWelcome to the\nElectronic Shopping Process!\n\n");
  while (1)
  {
    printf("What do you want to name your shopping list?\n > ");
    char *list_name = handleInput();

    if (list_name == NULL)
    {
      printf("[ERROR] Memory allocation failed.\n");
      return NULL;
    }

    if (strcmp(list_name, "quit") == 0)
    {
      free(list_name);
      return "";
    }

    if (strcmp(list_name, "") == 0)
    {
      printf("[ERROR] The name cannot be empty!\n\n");
      continue;
    }

    return list_name;
  }
}

//---------------------------------------------------------------------------------------------------------------------
/// Main function. 
/// @return 0 if "quit" or "done" was entered
/// @return 1 if memory allocation failed at any step

int main(void)
{
  char *list_name = getTheName();

  if (!list_name)
  {
    free(list_name);
    return 1;
  }

  if (strcmp(list_name, "") == 0)
  {
    return 0;
  }

  int result = addItemToTheList(list_name);

  free(list_name);
  return result;
}
