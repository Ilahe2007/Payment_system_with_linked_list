#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "dsaproject.h"
#define MENU_ITEMS 11

//1
// Add a new transaction to the beginning of the linked list of transactions
// idStud - Student ID for the transaction
// amount - Transaction amount (can be positive or negative)
// descr - Description of the transaction
// listeTransaction - Pointer to the head of the transactions linked list
T_Transaction *addTransaction(int idStud, float amount, char *descr, T_Transaction *listeTransaction) {
    // Check if description is valid
    if (descr == NULL) {
        printf("Invalid description.\n");
        return listeTransaction;
    }
    
    // Allocate memory for new transaction
    T_Transaction *newTransaction = (T_Transaction *)malloc(sizeof(T_Transaction));
    if (newTransaction == NULL) {
        printf("Memory allocation failed.\n");
        return listeTransaction; // Return original list if fails
    }
    
    // Allocate memory for description string
    newTransaction->descr = (char *)malloc(strlen(descr) + 1);
    if (newTransaction->descr == NULL) {
        printf("Memory allocation failed.\n");
        free(newTransaction); // Free the transaction struct if fails
        return listeTransaction; // Return original list
    }
    
    // Initialize the new transaction
    newTransaction->studentID = idStud;
    newTransaction->amount = amount;
    strcpy(newTransaction->descr, descr); // Copy the description string
    newTransaction->next = listeTransaction; // Link to the existing list
    
    // Return the newTransaction as head of the list
    return newTransaction;
}

//2
// Add a new block to the blockchain
// bc - Pointer to the blockchain structure
// date - Date string in YYYYMMDD format for the new block
BlockChain addBlock(BlockChain *bc, char *date) {
    // Allocate memory for new block
    T_Block *newblock = (T_Block*) malloc(sizeof(T_Block));

    // Check if allocation was successful
    if (newblock == NULL) {
        printf("Memory allocation failed.\n");
        return *bc; // Return original blockchain if fails
    }

    // Set block identifier
    if (bc->blocks == NULL) {
        newblock->identifier = 0; // First block gets ID 0
    } else {
        newblock->identifier = bc->blocks->identifier + 1; // Increase ID from last block
    }
    
    // Set block data
    strcpy(newblock->date, date); // Copy the date string
    newblock->transactions = NULL; // Set transaction list empty
    
    // Insert new block at the beginning of the blockchain
    newblock->next = bc->blocks; // Link to previous block
    bc->blocks = newblock; // Update blockchain head to new block
    
    return *bc;
}

//3
// Calculate total transaction amount for a specific student in a block
// idStud - Student ID to calculate total 
// b - The block containing transactions for checking
float totalTransactionStudentBlock(int idStud, T_Block b) {
    float total = 0.0f;
    T_Transaction *current = b.transactions; // Start from first transaction

    // Look through all transactions in the block
    while (current != NULL) {
        // Add amount if student ID matches
        if (current->studentID == idStud) {
            total += current->amount;
        }
        current = current->next; // Move to next transaction
    }

    return total;
}

// 4
// Calculate the total balance for a specific student across all blocks in blockchain
// idStud - The ID of the student to calculate 
// bc - Pointer to the blockchain containing all transaction blocks
float soldeStudent(int idStud, BlockChain *bc) {
    float totalBalance = 0.0f; // Set total balance to zero
    T_Block *currentBlock = bc->blocks; // Start with the first block in the blockchain
    
    // Look through all blocks in the blockchain
    while (currentBlock != NULL) {
        // Add the transaction amount from block for the student
        totalBalance += totalTransactionStudentBlock(idStud, *currentBlock);
        
        currentBlock = currentBlock->next; // Move to the next block
    }
    
    return totalBalance; // Return the calculated total balance
}

// 5
// Add a credit transaction for a student in the blockchain
// idStud - Student ID to credit
// amount - Positive amount to credit 
// descr - Description of the transaction
// bc - Pointer to the blockchain structure
void credit(int idStud, float amount, char *descr, BlockChain *bc) {
    // Check if blockchain pointer is valid
    if (bc == NULL) {
        printf("Error: Blockchain is not initialized.\n");
        return;
    }
    
    // Check if amount is positive
    if (amount <= 0) {
        printf("Invalid amount. Amount must be positive.\n");
        return;
    }
    
    // Create first block if blockchain is empty
    if (bc->blocks == NULL) {
        // Get current date in YYYYMMDD 
        time_t t = time(NULL);
        struct tm *tm_info = localtime(&t);
        char dateStr[9];
        strftime(dateStr, sizeof(dateStr), "%Y%m%d", tm_info);
        
        // Initialize blockchain with first block
        *bc = addBlock(bc, dateStr); 
    }
    
    // Add credit transaction to latest block
    bc->blocks->transactions = addTransaction(idStud, amount, descr, bc->blocks->transactions);
    printf("Successfully credited %.2f to student %d\n", amount, idStud);
}

// 6
// Process payment transaction for student
// idStud - Student ID making the payment
// amount - Positive amount to be paid
// descr - Description for the payment
// bc - Pointer to the blockchain structure
// Note: This function expects a positive amount parameter
int pay(int idStud, float amount, char *descr, BlockChain *bc) {
    // Check if amount is positive 
    if(amount < 0){
        printf("Error: Amount's value must be positive");
        return 0;
    }
    // Check if student has positive balance
    if (soldeStudent(idStud, bc) < amount) {
        printf("Error: Insufficient funds for student %d\n", idStud);
        return 0;  // Payment failed - insufficient funds
    }
    
    // Add payment transaction (stored as negative amount)
    bc->blocks->transactions = addTransaction(idStud, -amount, descr, bc->blocks->transactions);
    printf("Successfully processed payment of %.2f from student %d\n", amount, idStud);
    
    return 1; // Payment successful
} 

// 7
// Display the last 5 transactions for a specific student
// idStud - Student ID to consult
// bc - Pointer to the blockchain structure
void consult(int idStud, BlockChain *bc) {
    int count = 0;  // Counter for displayed transactions

    // Set block pointer to the first block in the blockchain
    T_Block* currentBlock = bc->blocks;

    // Check if blockchain is empty
    if (currentBlock == NULL) {
        printf("Error: No blocks found in the blockchain\n");
        return;
    }

    // Set transaction pointer to first transaction in first block
    T_Transaction* currentTransaction = currentBlock->transactions;

    // Display students total balance
    printw("Student's total balance: %.2f\n", soldeStudent(idStud, bc));

    // Look through blocks and transactions
    while (1) {
        // Move to next block if no more transactions in current block 
        if (currentTransaction == NULL) {
            currentBlock = currentBlock->next;
            
            // Exit if no more blocks
            if (currentBlock == NULL) {
                if (count == 0) {
                    printw("No transactions found for student %d\n", idStud);
                }
                return;
            }
            
            // Move to first transaction in next block
            currentTransaction = currentBlock->transactions;
            continue;
        }

        // Display it if transaction belongs to the student
        if (currentTransaction->studentID == idStud) {
            count++;
            printw("\nTransaction #%d\n", count);
            printw("Block ID: %d\n", currentBlock->identifier);
            printw("Description: %s\n", currentTransaction->descr);
            printw("Amount: %.2f\n", currentTransaction->amount);

            // Stop after 5 transactions
            if (count == 5) {
                break;
            }
        }
        
        // Move to next transaction
        currentTransaction = currentTransaction->next;
    }
}

// 8
// Transfer amount from source student to destination student
// idSource - ID of the student sending money
// idDestination - ID of the student receiving money
// amount - Positive amount to transfer 
// descr - Description of the transfer
// bc - Pointer to the blockchain structure
// Note: This function expects a positive amount parameter
int transfer(int idSource, int idDestination, float amount, char *descr, BlockChain *bc) {
    // Check if amount is positive
    if (amount <= 0) {
        printf("Error: Transfer amount must be positive\n");
        return 0;  // Failed if invalid amount
    }

    // Check if source has positive balance
    if (soldeStudent(idSource, bc) < amount) {
        printf("Error: Student %d has insufficient balance\n", idSource);
        return 0;  // Failed if negative balance  
    }
    
    // Process the transfer by adding two transactions:
    // 1. Debit from source (negative)
    bc->blocks->transactions = addTransaction(idSource, -amount, descr, bc->blocks->transactions);
    
    // 2. Credit to destination (positive)
    bc->blocks->transactions = addTransaction(idDestination, amount, descr, bc->blocks->transactions);
    
    printf("Successfully transferred %.2f from student %d to student %d\n", 
           amount, idSource, idDestination);
    
    return 1; // Transfer successful
}


//9
// Export all transactions from the blockchain to a file
// bc - Pointer to the blockchain structure
// filename - Name of the file to export transactions to
void exportTransactions(BlockChain *bc, char *filename) {
    // Open file for writing
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Error: Could not open file for writing.\n");
        return;
    }

    T_Block *currentBlock = bc->blocks;

    // Look through all blocks in blockchain
    while (currentBlock != NULL) {
        // Convert date from YYYYMMDD to DDMMYYYY format
        char day[3], month[3], year[5];
        strncpy(year, currentBlock->date, 4);  year[4] = '\0';
        strncpy(month, currentBlock->date + 4, 2); month[2] = '\0';
        strncpy(day, currentBlock->date + 6, 2);   day[2] = '\0';

        // Get transactions for current block
        T_Transaction *t = currentBlock->transactions;

        // Write each transaction to the file
        while (t != NULL) {
            fprintf(file, "%s/%s/%s; %d; %.2f; %s\n",
                    year, month, day,
                    t->studentID,
                    t->amount,
                    t->descr);

            t = t->next; // Move to next transaction
        }

        currentBlock = currentBlock->next; // Move to next block
    }

    fclose(file);
    printf("File exported successfully.\n");
}


// Import transactions from a file into the blockchain
// bc - Pointer to the blockchain structure
// filename - Name of the file containing transactions to import
void importTransactions(BlockChain *bc, char *filename) {
    // Check if blockchain is valid
    if (bc == NULL) {
        printf("Invalid blockchain.\n");
        return;
    }
    
    // Variable declarations for parsing
    char line[300], date[11], formattedDate[9] = "";
    int studentID;
    float amount;
    char descr[150];
    char day[3], month[3], year[5];

    // Open file for reading
    FILE *file = fopen(filename, "r");
    if (!file) { 
        printf("Cannot open file: %s\n", filename); 
        return; 
    }

    char lastDate[9] = ""; // Track the last processed date

    // Read file line by line
    while (fgets(line, sizeof(line), file)) {
        // Parse line into components
        if(sscanf(line, " %10[^;] ; %d ; %f ; %[^\n]", date, &studentID, &amount, descr) != 4) {
            printf("Warning: Skipping malformed line.\n");
            continue; // Skip lines that dont match expected format
        }
        
        // Parse date from YYYYMMDD format
        if(sscanf(date, " %4[^/]/%2[^/]/%2[^/]", year, month, day) != 3) {
            printf("Warning: Skipping line with invalid date format.\n");
            continue;
        }
        
        // Reformat date to YYYYMMDD
        sprintf(formattedDate, "%s%s%s", year, month, day);

        // Create new block if date changed
        if (strcmp(lastDate, formattedDate) != 0) {
            *bc = addBlock(bc, formattedDate);
            strcpy(lastDate, formattedDate);
        }

        // Add transaction to current block
        bc->blocks->transactions = addTransaction(studentID, amount, descr, bc->blocks->transactions);
    }

    fclose(file);
    printf("Transactions imported successfully from %s\n", filename);
}

// Display all transactions for a specific student across all blocks
// studentID - ID of the student whose transactions to display
// bc - Pointer to the blockchain structure
void displayAllStudentTransactions(BlockChain *bc, int studentID) {
    // Check if blockchain is valid and is not empty
    if (bc == NULL || bc->blocks == NULL) {
        printf("No blocks in the blockchain.\n");
        return;
    }

    T_Block *currentBlock = bc->blocks;
    int transactionCount = 0;
    
    // Print table header
    printf("\n=== All Transactions for Student ID: %d ===\n", studentID);
    printf("%-10s | %-10s | %-30s | %-10s\n", "Block ID", "Amount", "Description", "Date");
    printf("%s\n", "--------------------------------------------------");
    
    // Look through all blocks in blockchain
    while (currentBlock != NULL) {
        T_Transaction *currentTransaction = currentBlock->transactions;
        
        // Check all transactions in current block
        while (currentTransaction != NULL) {
            // Display transaction if studentID matches
            if (currentTransaction->studentID == studentID) {
                printf("%-10d | %-10.2f | %-30s | %s\n", 
                       currentBlock->identifier,
                       currentTransaction->amount,
                       currentTransaction->descr,
                       currentBlock->date);
                transactionCount++;
            }
            currentTransaction = currentTransaction->next; // Move to next transaction
        }
        
        currentBlock = currentBlock->next; // Move to next block
    }
    
    // Display summary of result
    if (transactionCount == 0) {
        printf("No transactions found for Student ID %d\n", studentID);
    } else {
        printf("\nTotal transactions found: %d\n", transactionCount);
    }
    printf("=== End of Transactions ===\n");
}

// Remove a specific transaction for a student from the blockchain
// bc - Pointer to the blockchain structure
// studentID - ID of the student whose transaction to remove
// transactionIndex - Index of the transaction to remove (start from 0)
int removeTransaction(BlockChain *bc, int studentID, int transactionIndex) {
    // Check if blockchain is valid and not empty
    if (bc == NULL || bc->blocks == NULL) {
        return 0; 
    }

    T_Block *currentBlock = bc->blocks;
    int currentIndex = 1; // Track the transaction index for the specific student
    
    // Look through all blocks in the blockchain
    while (currentBlock != NULL) {
        T_Transaction *current = currentBlock->transactions;
        T_Transaction *prev = NULL; // Track previous transaction for linked list operations
        
        // Check all transactions in the current block
        while (current != NULL) {
            // Only process transactions for the specified student
            if (current->studentID == studentID) {
                // Check if this is the transaction to remove
                if (currentIndex == transactionIndex) {
                    // Found the transaction to remove
                    if (prev == NULL) {
                        currentBlock->transactions = current->next;
                    } else {
                        prev->next = current->next;
                    }
                    
                    // Free the allocated memory
                    free(current->descr); // Free the description string
                    free(current); // Free the transaction structure
                    return 1; // Successfully removed
                }
                currentIndex++; // Move to next matching transaction
            }
            
            // Move to next transaction in the block
            prev = current;
            current = current->next;
        }
        
        // Move to next block in the blockchain
        currentBlock = currentBlock->next;
    }
    
    return 0; // Transaction not found
}

// Process refund by displaying transactions and allowing user to select one to remove
// bc - Pointer to the blockchain structure
void processRefund(BlockChain *bc) {
    int studentID;
    printf("Enter Student ID for refund: ");
    scanf("%d", &studentID);
    
    // Show all transactions for the student
    displayAllStudentTransactions(bc, studentID);
    
    // Get transaction index from user
    int transactionIndex;
    printf("\nEnter the index of the transaction to refund (or -1 to cancel): ");
    scanf("%d", &transactionIndex);
    
    // Check if user cancelled
    if (transactionIndex < 0) {
        printf("Refund cancelled.\n");
        return;
    }
    
    // Remove the specified transaction
    if (removeTransaction(bc, studentID, transactionIndex)) {
        printf("Transaction successfully refunded.\n");
    } else {
        printf("Error: Could not find the specified transaction.\n");
    }
}

// Display all blocks in the blockchain with their basic information
// bc - Pointer to the blockchain structure to display
void displayAllBlocks(BlockChain *bc) {
    // Check if blockchain is valid and not empty
    if (bc == NULL || bc->blocks == NULL) {
        printw("No blocks in the blockchain.\n");
        return;
    }

    T_Block *currentBlock = bc->blocks;
    printw("\n=== Blockchain Blocks ===\n");
    
    // Look through all blocks in the blockchain
    while (currentBlock != NULL) {
        // Display block info
        printw("\nBlock ID: %d\n", currentBlock->identifier);
        printw("Date: %s\n", currentBlock->date);
        
        // Count transactions in current block
        int transactionCount = 0;
        T_Transaction *currentTransaction = currentBlock->transactions;
        while (currentTransaction != NULL) {
            transactionCount++;
            currentTransaction = currentTransaction->next;
        }
        printw("Number of transactions: %d\n", transactionCount);
        
        currentBlock = currentBlock->next; // Move to next block
    }
    printw("\n=== End of Blockchain ===\n");
}

// Display all transactions within a specific block
// bc - Pointer to the blockchain structure
// blockId - ID of the block to display transactions from
void displayTransactionsInBlock(BlockChain *bc, int blockId) {
    // Check if blockchain is valid and not empty
    if (bc == NULL || bc->blocks == NULL) {
        printw("No blocks in the blockchain.\n");
        return;
    }

    T_Block *currentBlock = bc->blocks;
    
    // Look for the block with matching ID
    while (currentBlock != NULL) {
        if (currentBlock->identifier == blockId) {
            break;
        }
        currentBlock = currentBlock->next;
    }
    
    // Check if block was found
    if (currentBlock == NULL) {
        printw("Block with ID %d not found.\n", blockId);
        return;
    }
    
    // Display block header
    printw("\n=== Transactions in Block %d (Date: %s) ===\n", 
           currentBlock->identifier, currentBlock->date);
    
    T_Transaction *currentTransaction = currentBlock->transactions;
    int transactionCount = 0;
    
    // Check if block has any transactions
    if (currentTransaction == NULL) {
        printw("No transactions in this block.\n");
    } else {
        // Display table header
        printw("%-10s | %-10s | %-30s\n", "Student ID", "Amount", "Description");
        printw("%s\n", "--------------------------------------------");
        
        // Display each transaction in the block
        while (currentTransaction != NULL) {
            printw("%-10d | %-10.2f | %-30s\n", 
                   currentTransaction->studentID,
                   currentTransaction->amount,
                   currentTransaction->descr);
            transactionCount++;
            currentTransaction = currentTransaction->next;
        }
    }
    
    // Display transaction count and block footer
    printw("\nTotal transactions in this block: %d\n", transactionCount);
    printw("=== End of Block %d ===\n\n", blockId);
}

// Display all transactions for a specific student on a given date
// bc - Pointer to the blockchain structure
// studentID - ID of the student whose transactions to display
// date - Date in YYYYMMDD format to search for transactions
void displayStudentTransactionsOnDate(BlockChain *bc, int studentID, char *date) {
    // Check if blockchain is valid and not empty
    if (bc == NULL || bc->blocks == NULL) {
        printw("No blocks in the blockchain.\n");
        return;
    }

    T_Block *currentBlock = bc->blocks;
    int found = 0; 
    
    // Print section header with student ID and date
    printw("\n=== Transactions for Student ID: %d on %s ===\n", studentID, date);
    
    // Look through all blocks in the blockchain
    while (currentBlock != NULL) {
        // Check if current block's date matches the needed date
        if (strcmp(currentBlock->date, date) == 0) {
            T_Transaction *currentTransaction = currentBlock->transactions;
            int transactionCount = 0; // Count of matching transactions in this block
            
            // Check all transactions in the current block
            while (currentTransaction != NULL) {
                // Check if transaction belongs to the needed student
                if (currentTransaction->studentID == studentID) {
                    // Print block header before first transaction
                    if (transactionCount == 0) {
                        printw("\nBlock ID: %d, Date: %s\n", 
                               currentBlock->identifier, currentBlock->date);
                        printw("----------------------------------------\n");
                    }
                    // Print transaction details
                    printw("Amount: %8.2f | Description: %s\n", 
                           currentTransaction->amount, currentTransaction->descr);
                    found = 1; 
                    transactionCount++;
                }
                currentTransaction = currentTransaction->next; // Move to next transaction
            }
            
            // Print transaction count for this block if any were found
            if (transactionCount > 0) {
                printw("Total transactions in this block: %d\n", transactionCount);
            }
        }
        currentBlock = currentBlock->next; // Move to next block
    }
    
    // Print message based on whether transactions were found
    if (!found) {
        printw("No transactions found for Student ID %d on %s\n", studentID, date);
    } else {
        printw("\n=== End of Transactions ===\n");
    }
}

// Search for transactions with specific description
// bc - Pointer to the blockchain 
void searchByDescription(BlockChain *bc) {
    if (bc == NULL) {
        printf("Error: Blockchain is not initialized.\n");
        return;
    }
    
    char searchTerm[256];
    printf("Enter search term: ");
    scanf(" %[^\n]", searchTerm); 
    
    if (strlen(searchTerm) == 0) {
        printf("Error: Empty search term.\n");
        return;
    }
    
    T_Block *currentBlock = bc->blocks;
    int matchCount = 0;
    
    printf("\nSearch results for: %s\n", searchTerm);
    printf("----------------------------------------\n");
    
    // Convert search term to lowercase for correct comparison
    char searchTermLower[256];
    strncpy(searchTermLower, searchTerm, sizeof(searchTermLower) - 1);
    searchTermLower[sizeof(searchTermLower) - 1] = '\0';
    for (int i = 0; searchTermLower[i]; i++) {
        searchTermLower[i] = tolower(searchTermLower[i]);
    }
    
    // Look through all blocks
    while (currentBlock != NULL) {
        T_Transaction *currentTransaction = currentBlock->transactions;
        
        // Look through all transactions in the current block
        while (currentTransaction != NULL) {
            if (currentTransaction->descr != NULL) {
                // Create a lowercase copy of the description for correct comparison
                char descLower[256];
                strncpy(descLower, currentTransaction->descr, sizeof(descLower) - 1);
                descLower[sizeof(descLower) - 1] = '\0';
                for (int i = 0; descLower[i]; i++) {
                    descLower[i] = tolower(descLower[i]);
                }
                
                // Check if searchTerm is in description
                if (strstr(descLower, searchTermLower) != NULL) {
                    // Print transaction details
                    printf("Block %d | Date: %s | Student ID: %d | Amount: %.2f | Description: %s\n",
                           currentBlock->identifier, currentBlock->date,
                           currentTransaction->studentID, currentTransaction->amount,
                           currentTransaction->descr);
                    matchCount++;
                }
            }
            currentTransaction = currentTransaction->next;
        }
        currentBlock = currentBlock->next;
    } 
    
    if (matchCount == 0) {
        printf("No transactions found matching the search term: %s\n", searchTerm);
    } else {
        printf("----------------------------------------\n");
        printf("Found %d matching transactions.\n", matchCount);
    }
}

// Wait for user to press any key before continuing
// Used for pausing program execution until user input
void waitForKeyPress() {
    printw("\nPress any key to continue...");
    refresh();
    getch(); 
}

// Display the main menu with navigation highlighting
// highlight - Index of the currently selected menu item (start from 0)
void displayMenu(int highlight) {
    // Define menu items as an array of strings
    const char *menuItems[MENU_ITEMS] = {
        "Display the list of blocks in the BlockChain",
        "Display all transactions in a block",
        "Display all transactions for a student on a given day",
        "Display the history for a student",
        "Credit an account",
        "Refund",
        "Pay for a meal",
        "Transfer EATCoins between two students",
        "Search transactions by description",
        "Export all transactions to a file Darkbuys",
        "Import all transactions from a file Darkbuys",
        "Exit"
    };

    clear(); // Clear the screen
    
    // Display each menu item
    for (int i = 0; i < MENU_ITEMS; i++) {
        move(11 + i, 11); // Position cursor for this menu item
        if (i == highlight) {
            printw(">%s<", menuItems[i]); // Highlight selected item
        } else {
            printw(" %s ", menuItems[i]); // Display for unselected items
        }
    }
    refresh(); // Update the screen
}

// Handle user interaction and navigation
// bc - Pointer to the blockchain structure that stores all blocks and transactions
void menu(BlockChain *bc) {
    int highlight = 0;  // Currently highlighted menu item index
    int ch;             // Stores user key press

    // Enable special keyboard keys
    keypad(stdscr, TRUE);

    // Main menu loop
    while (1) {
        // Display the menu with current highlight
        displayMenu(highlight);
        
        // Get user input
        ch = getch();
        
        // Handle navigation keys
        switch (ch) {
            // Move highlight down
            case KEY_DOWN: 
                highlight = (highlight + 1) % MENU_ITEMS;  // Go to top if at bottom
                break;
                
            // Move highlight up
            case KEY_UP:   
                highlight = (highlight + MENU_ITEMS - 1) % MENU_ITEMS;  // Go to bottom if at top
                break;
                
            // Handle Enter key
            case '\n':
                // Execute selected menu option
                switch (highlight) {
                    // 0 - Display all blocks in blockchain
                    case 0: 
                        clear();
                        if(bc->blocks == NULL){
                            printw("Please first import a file.");
                        }else{
                            displayAllBlocks(bc);
                        }
                        waitForKeyPress();
                        break;
                        
                    // 1 - Display transactions in specific block
                    case 1: {
                        clear();
                        if(bc->blocks == NULL){
                            printw("Please first import a file.");
                        }else{
                            int blkId;
                            printw("Enter Block ID: "); scanw("%d", &blkId);
                            displayTransactionsInBlock(bc, blkId);
                        }
                        waitForKeyPress();
                        break;
                    }
                    
                    // 2 - Display transactions for student on specific date
                    case 2: {
                        clear();
                        if(bc->blocks == NULL){
                            printw("Please first import a file.");
                        }else{
                            int studentID; 
                            char date[10];
                            printw("Enter Student ID: "); scanw("%d", &studentID);
                            printw("Enter date (YYYYMMDD): "); scanw("%s", date);
                            displayStudentTransactionsOnDate(bc, studentID, date);
                        }
                        waitForKeyPress();
                        break;
                    }
                    
                    // 3 - Display transaction history for a student
                    case 3: {
                        clear();
                        if(bc->blocks == NULL){
                            printw("Please first import a file.");
                        }else{
                            int studentID; 
                            printw("Enter Student ID: "); scanw("%d", &studentID);
                            consult(studentID, bc);
                        }
                        waitForKeyPress();
                        break;
                    }
                    
                    // 4 - Credit student account
                    case 4: {
                        clear();
                        if(bc->blocks == NULL){
                            printw("Please first import a file.");
                        }else{
                            int studentID; 
                            float amount; 
                            char desc[100];
                            printw("Enter Student ID: "); scanw("%d", &studentID);
                            printw("Enter amount: "); scanw("%f", &amount);
                            printw("Enter description: "); scanw(" %[^\n]", desc);
                            credit(studentID, amount, desc, bc);
                        }
                        waitForKeyPress();
                        break;
                    }
                    
                    // 5 - Process refund (remove transaction)
                    case 5: {
                        clear();
                        if(bc->blocks == NULL){
                            printw("Please first import a file.");
                        }else{
                            int studentID; 
                            int transactionIndex;
                            printw("Enter Student ID: "); scanw("%d", &studentID);
                            printw("Enter transaction index: "); scanw(" %d", &transactionIndex);
                            removeTransaction(bc, studentID, transactionIndex);
                        }
                        waitForKeyPress();
                        break;
                    }
                    
                    // 6 - Process payment
                    case 6: {
                        clear();
                        if(bc->blocks == NULL){
                            printw("Please first import a file.");
                        }else{
                            int studentID; 
                            float amount; 
                            char desc[100];
                            printw("Enter Student ID: "); scanw("%d", &studentID);
                            printw("Enter amount: "); scanw("%f", &amount);
                            printw("Enter description: "); scanw(" %[^\n]", desc);
                            if (!pay(studentID, amount, desc, bc)) {
                                printw("Insufficient balance.\n");
                            }
                        }
                        waitForKeyPress();
                        break;
                    }
                    
                    // 7 - Transfer EATCoins between students
                    case 7: {
                        clear();
                        if(bc->blocks == NULL){
                            printw("Please first import a file.");
                        }else{
                            int idSrc, idDst; 
                            float amount; 
                            char desc[100];
                            printw("Enter source Student ID: "); scanw("%d", &idSrc);
                            printw("Enter destination Student ID: "); scanw("%d", &idDst);
                            printw("Enter amount: "); scanw("%f", &amount);
                            printw("Enter description: "); scanw(" %[^\n]", desc);
                            transfer(idSrc, idDst, amount, desc, bc);
                        }
                        waitForKeyPress();
                        break;
                    }
                    
                    // 8 - Search transactions by description
                    case 8: {
                        clear();
                        searchByDescription(bc);
                        waitForKeyPress();
                        break;
                    }
                    
                    // 9 - Export transactions to file
                    case 9: {
                        clear();
                        if(bc->blocks == NULL){
                            printw("Please first import a file.");
                        }else{
                            exportTransactions(bc, "Darkbuys.txt");
                        }
                        waitForKeyPress();
                        break;
                    }
                    
                    // 10 - Import transactions from file
                    case 10: {
                        clear();
                        importTransactions(bc, "Darkbuys.txt");
                        waitForKeyPress();
                        break;
                    }
                    
                    // 11 - Exit the program
                    case 11: {
                        endwin();  // Clean up ncurses
                        exit(0);   // Exit the program
                    }
                }
                break;
        }
    }
}
