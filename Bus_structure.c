#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define length 50
#define companies 4
#define stops 30
void clearInputBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main()
{
    int choice;
    int flag = 0;
    int fare = 1;
    int selectedBus = -1;

    char student;

    char company[companies][length];
    char route[companies][stops][length];
    int totalstops[companies];
    
    printf("\n\n");
    printf("                ██████╗ ██╗   ██╗███████╗        ███████╗██╗███╗   ██╗██████╗ ███████╗██████╗ ██╗  ██╗\n");
    printf("                ██╔══██╗██║   ██║██╔════╝        ██╔════╝██║████╗  ██║██╔══██╗██╔════╝██╔══██╗╚██╗██╔╝\n");
    printf("                ██████╔╝██║   ██║███████╗        █████╗  ██║██╔██╗ ██║██║  ██║█████╗  ██████╔╝ ╚███╔╝ \n");
    printf("                ██╔══██╗██║   ██║╚════██║        ██╔══╝  ██║██║╚██╗██║██║  ██║██╔══╝  ██╔══██╗ ██╔██╗ \n");
    printf("                ██████╔╝╚██████╔╝███████║███████╗██║     ██║██║ ╚████║██████╔╝███████╗██║  ██║██╔╝ ██╗\n");
    printf("                ╚═════╝  ╚═════╝ ╚══════╝╚══════╝╚═╝     ╚═╝╚═╝  ╚═══╝╚═════╝ ╚══════╝╚═╝  ╚═╝╚═╝  ╚═╝\n");

    while (1)
    {
        printf("\n===== MAIN MENU =====\n");
        printf("1. Find Bus\n");
        printf("2. Register Bus\n");
        printf("3. Book Ticket\n");
        printf("4. View Info\n");
        printf("5. Send SOS\n");
        printf("0. Exit\n");

        printf("Enter your choice: ");
        scanf("%d", &choice);

        if (choice == 1) // feature 1
        {
             FILE *fp = fopen("bus_data.txt", "r");

            if (fp == NULL)
            {
                printf("Could not open bus_data.txt\n");
                return 1;
            }

            for (int i = 0; i < companies; i++)
            {
                fscanf(fp, " %[^\n]", company[i]);
                fscanf(fp, "%d", &totalstops[i]);

                for (int j = 0; j < totalstops[i]; j++)
                {
                    fscanf(fp, " %[^\n]", route[i][j]);
                }
            }

            fclose(fp);
            
            char from[length], to[length];

            printf("\nWhere do you want to go?\n");
            scanf(" %[^\n]", to);
            clearInputBuffer();

            printf("From where do you want to go %s?\n", to);
            scanf(" %[^\n]", from);
            clearInputBuffer();

            printf("Are you a Student? (Y/N): ");
            scanf(" %c", &student);
            clearInputBuffer();

            printf("\nAvailable Buses are:\n");

            for (int i = 0; i < companies; i++)
            {
                int fromIndex = -1;
                int toIndex = -1;

                for (int j = 0; j < totalstops[i]; j++)
                {
                    if (strcmp(route[i][j], from) == 0)
                        fromIndex = j;

                    if (strcmp(route[i][j], to) == 0)
                        toIndex = j;
                }

                if (fromIndex != -1 && toIndex != -1)
                {
                    flag = 1;
                    selectedBus = i;

                    printf("\n%s\n", company[i]);
                    printf("Route: ");

                    if (fromIndex < toIndex)
                    {
                        fare = abs(toIndex - fromIndex) * 5;

                        if (student == 'Y' || student == 'y')
                            fare = fare / 2;

                        for (int k = fromIndex; k <= toIndex; k++)
                        {
                            printf("%s", route[i][k]);

                            if (k != toIndex)
                                printf(" -> ");
                        }
                    }
                    else if (fromIndex > toIndex)
                    {
                        fare = abs(fromIndex - toIndex) * 5;

                        if (student == 'Y' || student == 'y')
                            fare = fare / 2;

                        for (int m = fromIndex; m >= toIndex; m--)
                        {
                            printf("%s", route[i][m]);

                            if (m != toIndex)
                                printf(" -> ");
                        }
                    }
                }
            }

            if (flag == 1)
            {
                printf("\n\nFare : %d Taka\n", fare);
            }
            
            printf("\nPress Enter to go back to the main menu...");
            getchar(); // Removes the '\n' left by scanf
            
        }

        else if (choice == 2)// feature 2
        {
            // ===================================
            // PASTE ALL OF FEATURE 2 CODE HERE
            // ===================================

            printf("\nPress Enter to go back to the main menu...");
            getchar();
            
        }

        else if (choice == 3)
        {
            // ===================================
            // PASTE ALL OF FEATURE 3 CODE HERE
            // ===================================

            printf("\nPress Enter to go back to the main menu...");
            getchar();
            
        }
        else if (choice == 4)
        {
            //feature 4
            
            printf("\nPress Enter to go back to the main menu...");
            getchar();
        }
        else if (choice == 5)
        {
            //feature 5
            
            printf("\nPress Enter to go back to the main menu..");
            getchar();
        }

        else if (choice == 0)
        {
            printf("Goodbye!\n");
            break;
        }

        else
        {
            printf("Invalid choice!\n");
        }
    }

    return 0;
}