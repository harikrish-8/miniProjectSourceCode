// Bank-account program reads a random-access file sequentially,
// updates data already written to the file, creates new data to
// be placed in the file, and deletes data previously in the file.
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
// clientData structure definition
struct clientData
{
    int acctNum; // account number
    char lastName[15];    // account last name
    char firstName[10];   // account first name
    char lastAccessed[30]; // last accessed date and time
    double balance;       // account balance
    char phnum[15];  // kyc: Mobile 
    char Aanum[13];  // kyc: ID
    int pincode; //security pin 
    int isActive ; // status:  1 for actv ,0 for inActv

};                        // end structure clientData

// prototypes
int enterChoice(void);
void textFile(FILE *fPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);
void viewRecord(FILE *fPtr);

int main(){
    FILE *cfPtr;         // credit.dat file pointer
    int choice; // user's choice

    cfPtr = fopen("credit.dat", "rb+"); // open for reading and writing

    if (cfPtr == NULL)
    {
        printf("Build a secure database.\n");
        cfPtr = fopen("credit.dat", "wb+"); // create file for reading and writing
        struct clientData blankClient = {0, "", "", "", 0.0, "", "", 0, 0}; // blank client
        for (int i = 0; i < 100; i++)
        {
            fwrite(&blankClient, sizeof(struct clientData), 1, cfPtr);
        }
        printf("Database created successfully.\n");
        rewind(cfPtr);
       
    } // end if

    // enable user to specify action
    choice = enterChoice();
    while (choice != 6){
        switch (choice){
        case 1:// create record
            newRecord(cfPtr);
            break;
        case 2:// update record
            updateRecord(cfPtr);
            break;
        case 3:// create text file from record file
            textFile(cfPtr);
            break;
        case 4:// delete existing record
            deleteRecord(cfPtr);
            break;
        case 5:// view record
            viewRecord(cfPtr);
            break;
        default:// display if user does not select valid choice
            puts("Incorrect choice");
            break;
        } // end switch
        choice = enterChoice(); // ask again
    }     // end while
    printf("\nProgram safely closed.Welcome !\n");
    fclose(cfPtr);// fclose closes the file
    return 0; 
} // end main
//--------------Helper functions----------------

int enterChoice(void){
    int menuChoice; // variable to store user's choice
    // display available options
    printf("\n1 - Create a new account\n");
    printf("2 - Update an account\n");
    printf("3 - Store a text file\n");
    printf("4 - Delete an existing account\n");
    printf("5 - View an account details\n");
    printf("6 - End program\n");
    printf("%s", "Enter your choice: ");
    scanf("%d", &menuChoice);
    return menuChoice;
}

// create formatted text file for printing
void textFile(FILE *readPtr)
{
    FILE *writePtr = fopen("accounts.txt","w"); // accounts.txt file pointer
    struct clientData client = {0,"","","",0.0,"","",0,0}; //Initialization
    
    if (writePtr == NULL){printf("File could not be opened.");} // end if
    else{
        rewind(readPtr); // sets pointer to beginning of file
        fprintf(writePtr, "%-6s%-8s%-16s%-11s%-15s%-10s\n", "Acct", "Status", "Last Name", "First Name", "Balance", "Last Accessed");

        // copy all records from random-access file into text file
        while (!feof(readPtr)){
            // write single record to text file
            if (fread(&client, sizeof(struct clientData), 1, readPtr) == 1 && client.acctNum != 0){
                char status[10];
                if (client.isActive == 1){
                    strcpy(status, "Active");
                }
                else{
                    strcpy(status, "Inactive");
                }
                fprintf(writePtr, "%-6d%-8s%-16s%-11s%-15.2f%-10s\n", client.acctNum, status, client.lastName, client.firstName, client.balance, client.lastAccessed);
            } // end if
        }     // end while
        fclose(writePtr); // fclose closes the file
        printf("Exported to accounts.txt\n");
    }                     
} // end function textFile

// update balance in record
void updateRecord(FILE *fPtr)
{
    int account; // account number
    double transaction;   // transaction amount
    struct clientData client = {0, "", "", "", 0.0, "", "", 0, 0}; // create clientData with no information
    time_t currentTime; // variable to store current time
    char *timeString;

    printf("%s", "Enter account to update ( 1 - 100 ): ");// obtain number of account to update
    scanf("%d", &account);

    // move file pointer to correct record in file
    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    // read record from file
    fread(&client, sizeof(struct clientData), 1, fPtr);
    // display error if account does not exist
    if (client.acctNum == 0 || client.isActive == 0)
    {
        printf("Error: Account is either empty or Closed.\n");
    }
    else{
        //----sec check----
        int enteredpin;
        printf("Entered 4-digit PIN for verification: ");
        scanf("%d", &enteredpin);
        
        if (enteredpin != client.pincode){
            printf("Error: Incorrect PIN. Transaction cancelled.\n");
            return;
        }

        printf("Verification successful.Current Balance : %.2f\n", client.balance);
        printf("Entered deposit (+) or withdrawl (-)");
        scanf("%lf", &transaction);

        // prevent the over drawing account
        if(transaction < 0 && (client.balance + transaction < 0)){
            printf("Error: Insufficient funds. Transaction cancelled.\n");
            return;
        }
        client.balance = client.balance + transaction;
        timeString = ctime(&currentTime); // convert to readable text
        timeString[strcspn(timeString, "\n")]= 0; 
        strcpy(client.lastAccessed, timeString);
        // move file pointer to correct record in file
        // move back by 1 record length
        fseek(fPtr, -(long)sizeof(struct clientData), SEEK_CUR);
        // write updated record over old record in file
        fwrite(&client, sizeof(struct clientData), 1, fPtr);
        printf("Record updated Balance: %.2f\n", client.balance);
    } // end else
} // end function updateRecord

// delete an existing record
void deleteRecord(FILE *fPtr)
{
    int accountNum;                                 // account number
    struct clientData client;                       // stores record read from file
    struct clientData blankClient = {0, "", "", "", 0.0, "", "", 0, 0}; // blank client

    // obtain number of account to delete
    printf("Enter account number to delete ( 1 - 100 ): ");
    scanf("%d", &accountNum);

    // move file pointer to correct record in file
    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    // read record from file
    fread(&client, sizeof(struct clientData), 1, fPtr);
    // display error if record does not exist
    if (client.acctNum == 0 || client.isActive == 0)
    {
        printf("Account %d does not exist. or already closed\n", accountNum);
    } // end if
    else
    { // delete record
        int enteredpin;
        printf("Warning: you are to close this account.\n");
        printf("Enter 4-digit PIN to confirm closure: ");
        scanf("%d", &enteredpin);

        if(enteredpin != client.pincode){
            printf("Security alert: Incorrect PIN.\n");
            return;
        }

        // move file pointer to correct record in file
        fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
        // replace existing record with blank record
        fwrite(&blankClient, sizeof(struct clientData), 1, fPtr);
        printf("Account deleted.\n");
        printf("Account successfully CLOSED.\n");
    } // end else
} // end function deleteRecord

// create and insert record
void newRecord(FILE *fPtr){
    // create clientData with default information
    int accountNum; // account number
    struct clientData client = {0, "", "", "", 0.0, "", "", 0, 0};
    time_t currentTime; // variable to store current time
    char *timeString;
    
    // obtain number of account to create
    printf("Enter new account number ( 1 - 100 ): ");
    scanf("%d", &accountNum);
    // move file pointer to correct record in file
    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    // read record from file
    fread(&client, sizeof(struct clientData), 1, fPtr);
    // display error if account already exists
    if (client.acctNum != 0){
        printf("Account #%d already contains information.\n", client.acctNum);
    }
    else{
        // create record
        // user enters last name, first name and balance
        printf("Enter lastname, firstname, balance\n? ");
        scanf("%14s %9s %lf", client.lastName, client.firstName, &client.balance);
        printf("Enter phonenumber, Aadharnumber, 4-D-PIN, Depamount\n? ");
        scanf("%14s %12s %d", client.phnum, client.Aanum, &client.pincode);
        client.acctNum = accountNum;
        client.isActive = 1; // set account status to active

        time(&currentTime);
        // convert current time to string and store in client.lastAccessed
        timeString = ctime(&currentTime); // convert to readable text
        timeString[strcspn(timeString, "\n")]= 0; // THE magic fix the chop of the hidden 'enter' key that breaks files
        strcpy(client.lastAccessed, timeString);

        // move file pointer to correct record in file
        fseek(fPtr, (client.acctNum - 1) * sizeof(struct clientData), SEEK_SET);
        // insert record in file
        fwrite(&client, sizeof(struct clientData), 1, fPtr);
        printf("Account created.\n");
    } // end else
} // end function newRecord
void viewRecord(FILE *fPtr){
    int accountNum; // account number
    struct clientData client = {0, "", "", "", 0.0, "", "", 0, 0}; // create clientData with no information
    
    printf("Enter account number to view ( 1 - 100 ): ");
    scanf("%d", &accountNum);
    // move file pointer to correct record in file
    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    // read record from file
    fread(&client, sizeof(struct clientData), 1, fPtr);
    // display error if account does not exist
    if (client.acctNum == 0)
    {
        printf("Account #%d has no information.\n", accountNum);
    }
    else
    { // display record
        printf("\n******* ACCOUNT DETAILS *******\n");

        if (client.isActive == 1){
            printf("Status: Active\n");
        }
        else{
            printf("Status: Inactive\n");
        }
        printf("Account Number: %d\n", client.acctNum);
        printf("Name: %s %s\n", client.firstName, client.lastName);
        printf("Phone: %s\n", client.phnum);
        printf("Aadhar: %s\n", client.Aanum);
        printf("Balance: %.2f\n", client.balance);
        //If the string is empty or broken , print an error. Otherwise, print the time 
        if (strlen(client.lastAccessed) < 5){
            printf("Last Updated : Time Not Available\n");
        }
        else{
            printf("Last Updated : %s\n", client.lastAccessed);
        }
        printf("*********************************\n");
    } // end else
    
}
// end function enterChoice