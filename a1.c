//---------------------------------------------------------------------------------------------------------------------
// Assignment 1: voting simulation
// Takes votes for 5 candidates and calculates the percentage that each of the candidates received, then determines the winner
// Author: <12143302>
//---------------------------------------------------------------------------------------------------------------------

#include <stdio.h>
#include <ctype.h>

struct candidate
{
    char candidates_id;
    int amount_of_votes;
};

//---------------------------------------------------------------------------------------------------------------------
/// Function to get confirmation from the user. Confirms that the
///      entered votes are correct and restarts the process if they are not.
/// @return 1 if the input is "Yes"
/// @return 0 if the input is "No"

int confirmation(void)
{
    int choice; // Variable `choice` that will be assigned value 0 or 1

    while (1)
    {
        printf("\nAre you sure the input is correct?\n 1. Yes\n 2. No\n > ");
        scanf(" %d", &choice);
        while (getchar() != '\n'); // Clear input buffer

        // Returning 1 if the input is "Yes"
        if (choice == 1)
        {
            return 1;    
        }
        // Returning 0 if the input is "No"
        else if (choice == 2)
        {
            return 0;
        }
        else
        {
            printf("[Error] This input is out of bounds!\n");
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
/// Function to get the votes from the user. Updates `amount_of_votes` for each candidate.
/// @param candidates[5] - list of candidates who participate in voting

void votingResults(struct candidate candidates[5])
{
    int total_votes = 0;

    // Start of the loop that will continue until the votes input is confirmed
    while (1)
    {
        int candidate_votes_updated;

        for (int i = 0; i <= 4; i++)
        {
            printf("\nPlease enter the amount of votes for candidate \"%c\"! (1 to 1000)\n > ", candidates[i].candidates_id);

            // Creating a variable that will return 0 if it is not an integer and 1 if it is an integer
            int is_integer = scanf(" %d", &candidate_votes_updated);

            if (is_integer == 1 && candidate_votes_updated > 0 && candidate_votes_updated <= 1000)
            {
                candidates[i].amount_of_votes = candidate_votes_updated;
                total_votes += candidate_votes_updated;
            }
            else
            {
                printf("[Error] This input is out of bounds!\n");
                i -= 1;

                while (getchar() != '\n'); // Clear input buffer
            }
        }

        int confirm = confirmation();
        
        // If confirmation returns 1 (input is "Yes"), breaks the "while" loop, if it returns 0 ("No"), the loop starts over
        if (confirm == 1)
        {
            break;
        }
        total_votes = 0;
    }

    printf("\nThe results of the election are:\n");
    
    for (int i = 0; i <= 4; i++)
    {
        float candidate_percentage = (float)candidates[i].amount_of_votes / total_votes * 100;
        printf(" - Candidate \"%c\": %.2f %% (%d votes).\n", candidates[i].candidates_id, candidate_percentage, candidates[i].amount_of_votes);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/// Function to get IDs for each candidate. Updates `candidates_id`
/// @param candidates[5] - list of candidates who participate in voting

void candidatesIdUpdate(struct candidate candidates[5])
{
    char used_ids[5] = {0};

    for (int i = 0; i <= 4; i++)
    {
        printf("\nPlease enter an ID for candidate %d! (single letter only)\n > ", i+1);
        
        int duplicate_found = 0; // Initializing the flag to avoid updating an ID that is a duplicate
        char candidates_new_id;

        scanf(" %c", &candidates_new_id);

        // Loop to check if the entered ID has already been used
        for (int j = 0; j <= i; j++)
        {
            // If the entered ID has already been used, 
            if (candidates_new_id == used_ids[j])
            {
                printf("[Error] This ID is already in use!\n");

                // Setting `i` one step back so the loop repeats until the input is not a duplicate
                i -= 1;
                duplicate_found = 1;
                break;
            }
        }

        // Checking if the entered ID is a character (a-z, A-Z) and hasn’t been used before
        if (isalpha(candidates_new_id) && duplicate_found == 0)
        {
            used_ids[i] = candidates_new_id;
            candidates[i].candidates_id = candidates_new_id;
        }
        else if (!isalpha(candidates_new_id))
        {
            printf("[Error] This input is out of bounds!\n");
            i -= 1;
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
/// Function to find the candidate(s) with the highest number of votes. Prints out the results of the election.
/// @param candidates[5] - list of candidates who participate in voting

void winnersDeclaration(struct candidate candidates[5])
{
    int max_votes = candidates[0].amount_of_votes;

    for (int i = 0; i <= 4; i++)
    {
        // If a candidate's vote count is higher than the current max, update the max vote count
        if (candidates[i].amount_of_votes > max_votes)
        {
            max_votes = candidates[i].amount_of_votes;
        }
    }

    printf("\nPlease congratulate the winner(s) of the election:\n");
    
    int winners_count = 0;
    for (int i = 0; i <= 4; i++)
    {
        if (candidates[i].amount_of_votes == max_votes)
        {
            printf(" - Candidate \"%c\"!\n", candidates[i].candidates_id);
            winners_count += 1;
        }
    }

    if (winners_count != 1)
    {
        printf("\nThat's a %d-way tie!\n", winners_count);
    }
}

int main(void)
{
    struct candidate candidate_1_ = {'1', 0};
    struct candidate candidate_2_ = {'2', 0};
    struct candidate candidate_3_ = {'3', 0};
    struct candidate candidate_4_ = {'4', 0};
    struct candidate candidate_5_ = {'5', 0};

    struct candidate candidates[5] = {candidate_1_, candidate_2_, candidate_3_, candidate_4_, candidate_5_};

    printf("Welcome to the Election Simulator Program!\n");

    candidatesIdUpdate(candidates);
    
    votingResults(candidates);

    winnersDeclaration(candidates);

    printf("\nThank you for using the Election Simulator Program!\n");

    return 0;
}
