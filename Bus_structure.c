#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>

#define length 50
#define companies 4
#define stops 30
#define WHOLE_BUS_FARE 5000   /* flat price for renting a whole bus - change as needed */

void clearInputBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* =========================================================
   MD5 HASHING (written from the MD5 algorithm specification)
   Used to hash owner passwords before saving them to a file,
   so plain-text passwords are never stored on disk.
   Note: this simple implementation assumes a little-endian
   system (which covers normal Windows/PC machines).
   ========================================================= */

uint32_t leftrotate(uint32_t x, uint32_t c)
{
    return (x << c) | (x >> (32 - c));
}

void md5(const uint8_t *initial_msg, size_t initial_len, uint8_t *digest)
{
    /* per-round left-shift amounts, as defined by the MD5 algorithm */
    uint32_t s[] = {
        7,12,17,22, 7,12,17,22, 7,12,17,22, 7,12,17,22,
        5, 9,14,20, 5, 9,14,20, 5, 9,14,20, 5, 9,14,20,
        4,11,16,23, 4,11,16,23, 4,11,16,23, 4,11,16,23,
        6,10,15,21, 6,10,15,21, 6,10,15,21, 6,10,15,21
    };

    /* K constants: built from sin(), as defined by the MD5 algorithm,
       instead of typing out 64 long magic numbers by hand */
    static uint32_t K[64];
    static int built = 0;

    if (!built)
    {
        for (int i = 0; i < 64; i++)
            K[i] = (uint32_t)(fabs(sin(i + 1)) * 4294967296.0);
        built = 1;
    }

    uint32_t h0 = 0x67452301;
    uint32_t h1 = 0xefcdab89;
    uint32_t h2 = 0x98badcfe;
    uint32_t h3 = 0x10325476;

    /* pad the message so its length in bits is 448 (mod 512) */
    size_t newLenBits;
    for (newLenBits = initial_len * 8 + 1; newLenBits % 512 != 448; newLenBits++);
    size_t newLen = newLenBits / 8;

    uint8_t *msg = calloc(newLen + 8, 1);
    memcpy(msg, initial_msg, initial_len);
    msg[initial_len] = 0x80; /* append a single '1' bit */

    uint32_t bitsLen = (uint32_t)(initial_len * 8);
    memcpy(msg + newLen, &bitsLen, 4);

    /* process the message in 512-bit (64-byte) chunks */
    for (size_t offset = 0; offset < newLen; offset += 64)
    {
        uint32_t *w = (uint32_t *)(msg + offset);

        uint32_t a = h0, b = h1, c = h2, d = h3;

        for (uint32_t i = 0; i < 64; i++)
        {
            uint32_t f, g;

            if (i < 16)
            {
                f = (b & c) | ((~b) & d);
                g = i;
            }
            else if (i < 32)
            {
                f = (d & b) | ((~d) & c);
                g = (5 * i + 1) % 16;
            }
            else if (i < 48)
            {
                f = b ^ c ^ d;
                g = (3 * i + 5) % 16;
            }
            else
            {
                f = c ^ (b | (~d));
                g = (7 * i) % 16;
            }

            uint32_t temp = d;
            d = c;
            c = b;
            b = b + leftrotate(a + f + K[i] + w[g], s[i]);
            a = temp;
        }

        h0 += a;
        h1 += b;
        h2 += c;
        h3 += d;
    }

    free(msg);

    memcpy(digest,      &h0, 4);
    memcpy(digest + 4,  &h1, 4);
    memcpy(digest + 8,  &h2, 4);
    memcpy(digest + 12, &h3, 4);
}

/* Converts the 16-byte MD5 digest into a readable 32-character hex string */
void md5ToHexString(uint8_t *digest, char *hexStr)
{
    for (int i = 0; i < 16; i++)
        sprintf(hexStr + (i * 2), "%02x", digest[i]);

    hexStr[32] = '\0';
}

/* Hashes a plain-text password and stores the result (32 hex chars) in hashOut */
void hashPassword(const char *password, char *hashOut)
{
    uint8_t digest[16];
    md5((const uint8_t *)password, strlen(password), digest);
    md5ToHexString(digest, hashOut);
}

/* =========================================================
   OWNER REGISTER / LOGIN SYSTEM
   Stored in owners.txt as:  username  passwordHash  ownerName
   This file is what lets an owner log back in at any time -
   their account is saved here permanently the moment they sign up.
   ========================================================= */

/* Returns 1 on success and fills loggedInUser (auto-login),
   returns 0 if the account could not be created */
int registerOwner(char *loggedInUser)
{
    char username[length], password[length], name[length];
    char hashedPassword[33];

    printf("\nChoose a username: ");
    scanf("%s", username);

    /* check if username is already taken */
    FILE *checkFile = fopen("owners.txt", "r");
    if (checkFile != NULL)
    {
        char existingUser[length], existingHash[33], existingName[length];

        while (fscanf(checkFile, "%s %s", existingUser, existingHash) == 2)
        {
            fscanf(checkFile, " %[^\n]", existingName);

            if (strcmp(existingUser, username) == 0)
            {
                printf("This username is already taken. Please try logging in instead.\n");
                fclose(checkFile);
                return 0;
            }
        }
        fclose(checkFile);
    }

    printf("Choose a password: ");
    scanf("%s", password);

    printf("Enter your full name: ");
    clearInputBuffer();
    scanf(" %[^\n]", name);

    hashPassword(password, hashedPassword);

    FILE *fp = fopen("owners.txt", "a");
    if (fp == NULL)
    {
        printf("Could not save your account. Please try again.\n");
        return 0;
    }

    fprintf(fp, "%s %s %s\n", username, hashedPassword, name);
    fclose(fp);

    printf("\nAccount created and saved successfully! Logging you in...\n");

    /* auto-login right after signup, since the account is now
       permanently saved in owners.txt and can be logged into anytime */
    strcpy(loggedInUser, username);
    return 1;
}

/* Returns 1 and fills loggedInUser if login succeeds, otherwise returns 0 */
int loginOwner(char *loggedInUser)
{
    char username[length], password[length];
    char hashedPassword[33];

    printf("\nUsername: ");
    scanf("%s", username);

    printf("Password: ");
    scanf("%s", password);

    hashPassword(password, hashedPassword);

    FILE *fp = fopen("owners.txt", "r");
    if (fp == NULL)
    {
        printf("No owner accounts found yet. Please register first.\n");
        return 0;
    }

    char fileUser[length], fileHash[33], fileName[length];

    while (fscanf(fp, "%s %s", fileUser, fileHash) == 2)
    {
        fscanf(fp, " %[^\n]", fileName);

        if (strcmp(fileUser, username) == 0 && strcmp(fileHash, hashedPassword) == 0)
        {
            fclose(fp);
            strcpy(loggedInUser, fileUser);
            printf("\nLogin successful. Welcome, %s!\n", fileName);
            return 1;
        }
    }

    fclose(fp);
    printf("Invalid username or password.\n");
    return 0;
}

/* =========================================================
   BUS REGISTRATION (saved by logged-in owners)

   Every registered bus is saved to TWO files:

   1. registered_buses.txt - a human-readable, labeled block
      per bus. Used by "View Info" (choice 4) to display
      everything nicely on screen.

   2. bus_records.txt - ONE line per bus, fields separated by
      the "|" character. This format is much easier for code
      to search, edit, or delete a single bus later (for
      future Update Bus Info / Delete Bus Info features),
      since a human-readable multi-line block is awkward to
      rewrite programmatically.
   ========================================================= */

void registerBusDetails(char *ownerUsername)
{
    char busNumber[length], driverName[length], driverContact[length];
    char regDate[length], busType[length];

    printf("\nEnter Bus Number (e.g. Dhaka Metro Ga 11-1234): ");
    scanf(" %[^\n]", busNumber);

    printf("Enter Driver Name: ");
    scanf(" %[^\n]", driverName);

    printf("Enter Driver Contact Number: ");
    scanf(" %[^\n]", driverContact);

    printf("Enter Bus Registration Date (DD-MM-YYYY): ");
    scanf(" %[^\n]", regDate);

    printf("Enter Bus Type (AC / Non-AC / Double Decker): ");
    scanf(" %[^\n]", busType);

    /* ---- Save 1: human-readable block (for View Info) ---- */
    FILE *fp = fopen("registered_buses.txt", "a");

    if (fp == NULL)
    {
        printf("Could not save bus details. Please try again.\n");
        return;
    }

    fprintf(fp, "Owner Username    : %s\n", ownerUsername);
    fprintf(fp, "Bus Number        : %s\n", busNumber);
    fprintf(fp, "Driver Name       : %s\n", driverName);
    fprintf(fp, "Driver Contact    : %s\n", driverContact);
    fprintf(fp, "Registration Date : %s\n", regDate);
    fprintf(fp, "Bus Type          : %s\n", busType);
    fprintf(fp, "--------------------------------------------\n");

    fclose(fp);

    /* ---- Save 2: single-line record (for future update/delete) ---- */
    FILE *recFp = fopen("bus_records.txt", "a");

    if (recFp == NULL)
    {
        printf("Bus details saved, but the update-ready record file could not be created.\n");
        return;
    }

    fprintf(recFp, "%s|%s|%s|%s|%s|%s\n",
            ownerUsername, busNumber, driverName, driverContact, regDate, busType);

    fclose(recFp);

    printf("\nBus details saved successfully!\n");
}

/* =========================================================
   DRIVER & HELPER INFORMATION  (NEW)

   Lets a logged-in owner attach a driver AND a helper record
   to a specific bus number. Saved to TWO files, same pattern
   as bus registration above:

   1. driver_helper.txt - human-readable block per entry.
      Used by "View Driver & Helper Info" so ANYONE (owner or
      passenger, no login required) can look up who is driving
      / helping on a given bus.

   2. driver_helper_records.txt - ONE line per entry, fields
      separated by "|", for easy future search/update/delete.
   ========================================================= */

void addDriverHelperInfo(char *ownerUsername)
{
    char busNumber[length];
    char driverName[length], driverContact[length], driverLicense[length];
    char helperName[length], helperContact[length];

    printf("\n----- Add Driver & Helper Info -----\n");

    printf("Enter Bus Number this info belongs to: ");
    scanf(" %[^\n]", busNumber);

    printf("Enter Driver Name: ");
    scanf(" %[^\n]", driverName);

    printf("Enter Driver Contact Number: ");
    scanf(" %[^\n]", driverContact);

    printf("Enter Driver License Number: ");
    scanf(" %[^\n]", driverLicense);

    printf("Enter Helper Name: ");
    scanf(" %[^\n]", helperName);

    printf("Enter Helper Contact Number: ");
    scanf(" %[^\n]", helperContact);

    /* ---- Save 1: human-readable block (for viewing) ---- */
    FILE *fp = fopen("driver_helper.txt", "a");

    if (fp == NULL)
    {
        printf("Could not save driver & helper details. Please try again.\n");
        return;
    }

    fprintf(fp, "Added By (Owner)  : %s\n", ownerUsername);
    fprintf(fp, "Bus Number        : %s\n", busNumber);
    fprintf(fp, "Driver Name       : %s\n", driverName);
    fprintf(fp, "Driver Contact    : %s\n", driverContact);
    fprintf(fp, "Driver License    : %s\n", driverLicense);
    fprintf(fp, "Helper Name       : %s\n", helperName);
    fprintf(fp, "Helper Contact    : %s\n", helperContact);
    fprintf(fp, "--------------------------------------------\n");

    fclose(fp);

    /* ---- Save 2: single-line record (for future search/update/delete) ---- */
    FILE *recFp = fopen("driver_helper_records.txt", "a");

    if (recFp == NULL)
    {
        printf("Driver & helper details saved, but the record file could not be created.\n");
        return;
    }

    fprintf(recFp, "%s|%s|%s|%s|%s|%s|%s\n",
            ownerUsername, busNumber, driverName, driverContact,
            driverLicense, helperName, helperContact);

    fclose(recFp);

    printf("\nDriver & Helper details saved successfully!\n");
}

/* Anyone (no login needed) can view all saved driver & helper info,
   or filter it down to just one bus number. */
void viewDriverHelperInfo()
{
    FILE *fp = fopen("driver_helper.txt", "r");

    if (fp == NULL)
    {
        printf("\nNo driver & helper information has been added yet.\n");
        return;
    }

    int filterChoice;
    char searchBus[length];

    printf("\n1. View ALL driver & helper info\n");
    printf("2. Search by Bus Number\n");
    printf("Enter your choice: ");
    scanf("%d", &filterChoice);
    clearInputBuffer();

    if (filterChoice == 2)
    {
        printf("Enter Bus Number to search: ");
        scanf(" %[^\n]", searchBus);
    }

    char line[200];
    char currentBlock[1000];
    currentBlock[0] = '\0';
    int blockHasBus = 0;
    int found = 0;

    if (filterChoice == 2)
    {
        /* Read block by block (each block ends with the dashed line)
           and only print blocks whose Bus Number line matches. */
        while (fgets(line, sizeof(line), fp) != NULL)
        {
            strcat(currentBlock, line);

            if (strncmp(line, "Bus Number", 10) == 0 && strstr(line, searchBus) != NULL)
                blockHasBus = 1;

            if (strncmp(line, "----", 4) == 0)
            {
                if (blockHasBus)
                {
                    printf("%s", currentBlock);
                    found = 1;
                }
                currentBlock[0] = '\0';
                blockHasBus = 0;
            }
        }

        if (!found)
            printf("\nNo driver/helper info found for bus \"%s\".\n", searchBus);
    }
    else
    {
        printf("\n===== ALL DRIVER & HELPER INFO =====\n\n");

        while (fgets(line, sizeof(line), fp) != NULL)
        {
            printf("%s", line);
        }
    }

    fclose(fp);
}

/* =========================================================
   TICKET BOOKING (feature 3)
   Lets a passenger search a route, pick a company, book
   either a normal seat ticket or the whole bus, see an
   invoice, confirm, and have the booking saved to
   ticket_bookings.txt
   ========================================================= */

void bookTicket(char company[companies][length],
                char route[companies][stops][length],
                int totalstops[companies])
{
    char passengerName[length], from[length], to[length];
    char student;

    printf("\nEnter your name: ");
    scanf(" %[^\n]", passengerName);

    printf("From where do you want to travel? ");
    scanf(" %[^\n]", from);

    printf("To where do you want to go? ");
    scanf(" %[^\n]", to);

    printf("Are you a Student? (Y/N): ");
    scanf(" %c", &student);
    clearInputBuffer();

    /* search every company for this route, same matching logic as Find Bus */
    int matchCompany[companies];
    int matchFromIndex[companies];
    int matchToIndex[companies];
    int matchFare[companies];
    int matchCount = 0;

    for (int i = 0; i < companies; i++)
    {
        int fromIndex = -1, toIndex = -1;

        for (int j = 0; j < totalstops[i]; j++)
        {
            if (strcmp(route[i][j], from) == 0)
                fromIndex = j;

            if (strcmp(route[i][j], to) == 0)
                toIndex = j;
        }

        if (fromIndex != -1 && toIndex != -1 && fromIndex != toIndex)
        {
            int fare = abs(toIndex - fromIndex) * 5;

            if (student == 'Y' || student == 'y')
                fare = fare / 2;

            matchCompany[matchCount] = i;
            matchFromIndex[matchCount] = fromIndex;
            matchToIndex[matchCount] = toIndex;
            matchFare[matchCount] = fare;
            matchCount++;
        }
    }

    if (matchCount == 0)
    {
        printf("\nNo buses run directly between %s and %s.\n", from, to);
        return;
    }

    printf("\nAvailable Buses:\n");
    for (int k = 0; k < matchCount; k++)
        printf("%d. %-30s Fare: %d Taka (per seat)\n", k + 1, company[matchCompany[k]], matchFare[k]);

    int selection = 0;
    while (selection < 1 || selection > matchCount)
    {
        printf("Select a bus company (1-%d): ", matchCount);
        scanf("%d", &selection);
        clearInputBuffer();
    }

    int idx = selection - 1;
    int companyIndex = matchCompany[idx];
    int fromIndex = matchFromIndex[idx];
    int toIndex = matchToIndex[idx];
    int perSeatFare = matchFare[idx];

    printf("\n1. Book a single/regular ticket\n");
    printf("2. Book the whole bus\n");
    int bookingType = 0;
    while (bookingType != 1 && bookingType != 2)
    {
        printf("Enter your choice: ");
        scanf("%d", &bookingType);
        clearInputBuffer();
    }

    int quantity = 1;
    int totalFare = 0;
    char ticketType[20];

    if (bookingType == 1)
    {
        do
        {
            printf("How many tickets do you want to book? ");
            scanf("%d", &quantity);
            clearInputBuffer();
        } while (quantity < 1);

        totalFare = perSeatFare * quantity;
        strcpy(ticketType, "Regular");
    }
    else
    {
        quantity = 1;
        totalFare = WHOLE_BUS_FARE;
        strcpy(ticketType, "Whole Bus");
    }

    /* build the route path string, same direction logic as Find Bus */
    char routePath[600] = "";
    if (fromIndex < toIndex)
    {
        for (int k = fromIndex; k <= toIndex; k++)
        {
            strcat(routePath, route[companyIndex][k]);
            if (k != toIndex)
                strcat(routePath, " -> ");
        }
    }
    else
    {
        for (int m = fromIndex; m >= toIndex; m--)
        {
            strcat(routePath, route[companyIndex][m]);
            if (m != toIndex)
                strcat(routePath, " -> ");
        }
    }

    /* simple booking ID from the current time so each invoice is unique */
    int bookingID = (int)(time(NULL) % 100000);
    time_t now = time(NULL);
    char bookingTime[64];
    strcpy(bookingTime, ctime(&now));
    bookingTime[strcspn(bookingTime, "\n")] = '\0'; /* strip trailing newline from ctime */

    printf("\n============================================\n");
    printf("               TICKET INVOICE\n");
    printf("============================================\n");
    printf("Booking ID     : %05d\n", bookingID);
    printf("Date/Time      : %s\n", bookingTime);
    printf("Passenger Name : %s\n", passengerName);
    printf("Bus Company    : %s\n", company[companyIndex]);
    printf("Route          : %s\n", routePath);
    printf("Ticket Type    : %s\n", ticketType);
    if (bookingType == 1)
        printf("Seats Booked   : %d\n", quantity);
    printf("Total Fare     : %d Taka\n", totalFare);
    printf("============================================\n");

    char confirm;
    printf("Confirm this booking? (Y/N): ");
    scanf(" %c", &confirm);
    clearInputBuffer();

    if (confirm != 'Y' && confirm != 'y')
    {
        printf("\nBooking cancelled. Nothing was saved.\n");
        return;
    }

    FILE *fp = fopen("ticket_bookings.txt", "a");
    if (fp == NULL)
    {
        printf("\nCould not save your booking. Please try again.\n");
        return;
    }

    fprintf(fp, "Booking ID     : %05d\n", bookingID);
    fprintf(fp, "Date/Time      : %s\n", bookingTime);
    fprintf(fp, "Passenger Name : %s\n", passengerName);
    fprintf(fp, "From           : %s\n", from);
    fprintf(fp, "To             : %s\n", to);
    fprintf(fp, "Bus Company    : %s\n", company[companyIndex]);
    fprintf(fp, "Route          : %s\n", routePath);
    fprintf(fp, "Ticket Type    : %s\n", ticketType);
    if (bookingType == 1)
        fprintf(fp, "Seats Booked   : %d\n", quantity);
    fprintf(fp, "Total Fare     : %d Taka\n", totalFare);
    fprintf(fp, "--------------------------------------------\n");

    fclose(fp);

    printf("\nBooking confirmed! Your invoice has been saved.\n");
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
        printf("6. Driver & Helper Info\n");
        printf("0. Exit\n");

        printf("Enter your choice: ");
        scanf("%d", &choice);

        if (choice == 1) // feature 1
        {

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

        else if (choice == 2)// feature 2 - Owner Register / Login, then Register Bus
        {
            int ownerChoice;
            char loggedInUser[length];
            int loggedIn = 0;

            printf("\n===== BUS OWNER PORTAL =====\n");
            printf("1. Login\n");
            printf("2. Create New Owner Account\n");
            printf("Enter your choice: ");
            scanf("%d", &ownerChoice);
            clearInputBuffer();

            if (ownerChoice == 1)
            {
                loggedIn = loginOwner(loggedInUser);
            }
            else if (ownerChoice == 2)
            {
                loggedIn = registerOwner(loggedInUser);
            }
            else
            {
                printf("Invalid choice.\n");
            }

            if (loggedIn == 1)
            {
                printf("\n--------------------------------------------\n");
                printf("You are now logged in as: %s\n", loggedInUser);
                printf("--------------------------------------------\n");

                // ===================================
                // BUS REGISTRATION FORM
                // ===================================
                registerBusDetails(loggedInUser);
            }

            printf("\nPress Enter to go back to the main menu...");
            getchar();
            
        }

        else if (choice == 3)
        {
            // ===================================
            // FEATURE 3 - BOOK TICKET
            // ===================================
            bookTicket(company, route, totalstops);

            printf("\nPress Enter to go back to the main menu...");
            getchar();
            
        }
        else if (choice == 4)
        {
            //feature 4 - View Info (shows every registered bus)

            FILE *viewFp = fopen("registered_buses.txt", "r");

            if (viewFp == NULL)
            {
                printf("\nNo buses have been registered yet.\n");
            }
            else
            {
                printf("\n===== ALL REGISTERED BUSES =====\n\n");

                char line[200];

                while (fgets(line, sizeof(line), viewFp) != NULL)
                {
                    printf("%s", line);
                }

                fclose(viewFp);
            }
            
            printf("\nPress Enter to go back to the main menu...");
            getchar();
        }
        else if (choice == 5)
        {
            //feature 5 - Send SOS

            if (selectedBus == -1)
            {
                printf("\nYou haven't searched for a bus yet.\n");
                printf("Please use 'Find Bus' first so we know which bus you are on.\n");
            }
            else
            {
                int sos;

                printf("\n============================================\n");
                printf(" Government Passenger Safety Service\n");
                printf("============================================\n");
                printf("Do you want to send an Emergency SOS?\n");
                printf("1. Yes\n");
                printf("2. No\n");
                printf("Enter Choice: ");
                scanf("%d", &sos);
                clearInputBuffer();

                if (sos == 1)
                {
                    printf("\n============================================\n");
                    printf("        EMERGENCY SOS ACTIVATED\n");
                    printf("============================================\n");
                    printf("Emergency Alert Sent Successfully!\n\n");

                    printf("Bus           : %s\n", company[selectedBus]);

                    printf("\nEmergency Helpline : 999\n");
                    printf("Nearest Police Control Room has been notified.\n");
                    printf("Women Safety Cell has also been notified.\n");
                    printf("Stay Calm. Help is on the way.\n");
                    printf("============================================\n");
                }
                else
                {
                    printf("\n============================================\n");
                    printf("No Emergency Alert Sent.\n");
                    printf("============================================\n");
                }
            }
            
            printf("\nPress Enter to go back to the main menu..");
            getchar();
        }

        else if (choice == 6)
        {
            // ===================================
            // FEATURE 6 (NEW) - DRIVER & HELPER INFO
            // Adding requires an owner login, same as
            // Register Bus. Viewing is open to anyone -
            // no login required - so passengers can also
            // check who is driving/helping on a bus.
            // ===================================

            int dhChoice;

            printf("\n===== DRIVER & HELPER INFO =====\n");
            printf("1. Add Driver & Helper Info (Bus Owner Login Required)\n");
            printf("2. View Driver & Helper Info (Anyone)\n");
            printf("Enter your choice: ");
            scanf("%d", &dhChoice);
            clearInputBuffer();

            if (dhChoice == 1)
            {
                int ownerChoice2;
                char loggedInUser2[length];
                int loggedIn2 = 0;

                printf("\n===== BUS OWNER PORTAL =====\n");
                printf("1. Login\n");
                printf("2. Create New Owner Account\n");
                printf("Enter your choice: ");
                scanf("%d", &ownerChoice2);
                clearInputBuffer();

                if (ownerChoice2 == 1)
                {
                    loggedIn2 = loginOwner(loggedInUser2);
                }
                else if (ownerChoice2 == 2)
                {
                    loggedIn2 = registerOwner(loggedInUser2);
                }
                else
                {
                    printf("Invalid choice.\n");
                }

                if (loggedIn2 == 1)
                {
                    addDriverHelperInfo(loggedInUser2);
                }
            }
            else if (dhChoice == 2)
            {
                viewDriverHelperInfo();
            }
            else
            {
                printf("Invalid choice.\n");
            }

            printf("\nPress Enter to go back to the main menu...");
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
