
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
    int flag = 0;
    int fare = 1;
    int selectedBus = -1;

    char student;

    char company[companies][length]=
    {
        "Achim Paribahan",
        "Active Paribahan",
        "Agradut",
        "Airport Bangabandhu Avenue Paribahan"
    };

    char route[companies][stops][length]=
    {
        {"Gabtoli", "Technical", "Ansar Camp", "Mirpur 1", "Sony Cinema Hall", "Mirpur 2",
        "Mirpur 10","Mirpur 11","Purobi","Kalshi","ECB Square","MES","Shewra",
        "Kuril Bishwa Road","Jamuna Future Park","Bashundhara","Nadda","Notun Bazar","Bashtola",
        "Shahjadpur","Uttar Badda","Badda","Madhya","Badda","Merul Badda","Rampura Bridge",
        "Banasree","Demra Staff Quarter","",""},

        {"Shia Masjid","Adabor","Shyamoli","Technical",
        "Ansar Camp","Mirpur 1","Sony Cinema Hall","Mirpur 2",
        "Mirpur 10","Mirpur 11","Purobi","Kalshi",
        "ECB Square","MES","Shewrapara","Kuril Bishwa Road","Khilkhet","Airport","Jashimuddin",
        "Rajlakshmi","Azampur","House Building","Abdullahpur","","","","","",""},

        {"Savar","Hemayetpur","Amin Bazar","Gabtoli",
        "Technical","Kallyanpur","Shyamoli",
        "Shishu Mela","Agargaon","Zia Uddyan",
        "Bijoy Sarani","Jahangir Gate","Mohakhali","Wireless","Gulshan 1",
        "Badda Link Road","Bashtola","Shahjadpur","Uttar Badda","Notun Bazar",
        "","","","","","","","",""},

        {"Fulbaria","Golap Shah Mazar",
        "GPO","Paltan","Press Club","High Court","Matsya Bhaban",
        "Shahbag","Bangla Motor","Kawran Bazar",
        "Farmgate","Bijoy Sarani","Jahangir Gate",
        "Mohakhali","Chairman Bari",
        "Sainik Club","Banani","Kakali","Staff Road","MES",
        "Kurmitola","Shewra","Kuril Bishwa Road",
        "Khilkhet","Airport","Jashimuddin",
        "Rajlakshmi","House Building","Abdullahpur"}
    };

    printf("\n\n");
    printf("                ██████╗ ██╗   ██╗███████╗        ███████╗██╗███╗   ██╗██████╗ ███████╗██████╗ ██╗  ██╗\n");
    printf("                ██╔══██╗██║   ██║██╔════╝        ██╔════╝██║████╗  ██║██╔══██╗██╔════╝██╔══██╗╚██╗██╔╝\n");
    printf("                ██████╔╝██║   ██║███████╗        █████╗  ██║██╔██╗ ██║██║  ██║█████╗  ██████╔╝ ╚███╔╝ \n");
    printf("                ██╔══██╗██║   ██║╚════██║        ██╔══╝  ██║██║╚██╗██║██║  ██║██╔══╝  ██╔══██╗ ██╔██╗ \n");
    printf("                ██████╔╝╚██████╔╝███████║███████╗██║     ██║██║ ╚████║██████╔╝███████╗██║  ██║██╔╝ ██╗\n");
    printf("                ╚═════╝  ╚═════╝ ╚══════╝╚══════╝╚═╝     ╚═╝╚═╝  ╚═══╝╚═════╝ ╚══════╝╚═╝  ╚═╝╚═╝  ╚═╝\n");

    int totalstops[companies]={27,23,20,29};

    char from[length],to[length];

    printf("\nWhere do you want to go?\n");
    scanf(" %[^\n]",to);
    clearInputBuffer();

    printf("From where do you want to go %s?\n",to);
    scanf(" %[^\n]",from);
    clearInputBuffer();

    printf("Are you a Student? (Y/N): ");
    scanf(" %c",&student);
    clearInputBuffer();

    printf("\nAvailable Buses are:\n");

    for(int i=0;i<companies;i++)
    {
        int fromIndex=-1;
        int toIndex=-1;

        for(int j=0;j<totalstops[i];j++)
        {
            if(strcmp(route[i][j],from)==0)
                fromIndex=j;

            if(strcmp(route[i][j],to)==0)
                toIndex=j;
        }

        if(fromIndex!=-1 && toIndex!=-1)
        {
            flag=1;
            selectedBus=i;

            printf("\n%s\n",company[i]);
            printf("Route: ");

            if(fromIndex<toIndex)
            {
                fare=abs(toIndex-fromIndex)*5;

                if(student=='Y'||student=='y')
                    fare=fare/2;

                for(int k=fromIndex;k<=toIndex;k++)
                {
                    printf("%s",route[i][k]);

                    if(k!=toIndex)
                        printf(" -> ");
                }
            }

            else if(fromIndex>toIndex)
            {
                fare=abs(fromIndex-toIndex)*5;

                if(student=='Y'||student=='y')
                    fare=fare/2;

                for(int m=fromIndex;m>=toIndex;m--)
                {
                    printf("%s",route[i][m]);

                    if(m!=toIndex)
                        printf(" -> ");
                }
            }
        }
    }

    if(flag==1)
    {
        printf("\n\nFare : %d Taka\n",fare);

        int sos = 0;
        int validInput = 0;

        printf("\n============================================\n");
        printf(" Government Passenger Safety Service\n");
        printf("============================================\n");
        printf("Do you want to send an Emergency SOS?\n");
        printf("1. Yes\n");
        printf("2. No\n");


        while(!validInput)
        {
            printf("Enter Choice: ");

            if(scanf("%d",&sos) != 1)
            {

                printf("Invalid input. Please enter 1 or 2.\n");
                clearInputBuffer();
                continue;
            }

            clearInputBuffer();

            if(sos==1 || sos==2)
            {
                validInput = 1;
            }
            else
            {
                printf("Invalid choice. Please enter 1 or 2.\n");
            }
        }

        if(sos==1)
        {
            printf("\n============================================\n");
            printf("        EMERGENCY SOS ACTIVATED\n");
            printf("============================================\n");
            printf("Emergency Alert Sent Successfully!\n\n");

            printf("Bus           : %s\n",company[selectedBus]);
            printf("From          : %s\n",from);
            printf("Destination   : %s\n",to);

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
            printf("Thank you for using Dhaka Local Transport Bus Finder System.\n");
            printf("Have a Safe Journey!\n");
            printf("============================================\n");
        }
    }

    else
    {
        printf("\nNo Bus found in this Route.\n");
    }

    return 0;
}

