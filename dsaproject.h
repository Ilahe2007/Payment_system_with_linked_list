typedef struct Transaction {
    int studentID;           
    float amount; // Price of meal (negative) or credit (positive)        
    char* descr; // Description of operation (Sandwich, Soda, etc)     
    struct Transaction* next;
} T_Transaction;

typedef struct Block{
    int identifier; // ID of block (block 1, block 2, etc)
    char date[9]; // Date in which the block for trasnsaction was added
    T_Transaction* transactions; // Operations of students in a day 
    struct Block* next; // List of transactions
} T_Block;

typedef struct BlockChain{
    T_Block* blocks; // List of blocks
} BlockChain;

T_Transaction *addTransaction(int idStud, float amount, char *descr, T_Transaction *listeTransaction);

BlockChain addBlock(BlockChain *bc, char *date); // Add new block to the list of blocks

float totalTransactionStudentBlock(int idStud, T_Block b); // Calculate total amount of money student spent

float soldeStudent(int idStud, BlockChain *bc); // Student total balance

void credit(int idStud, float amount, char *descr, BlockChain *bc); // Add transaction with a positive amount to a student

int pay(int idStud, float amount, char *descr, BlockChain *bc); // Check and then create transaction

void consult(int idStud, BlockChain *bc); // View the students history

int transfer(int idSource, int idDestination, float amount, char *descr, BlockChain *bc); // Check and then make transfer to another student

void importTransactions(BlockChain *bc, char *filename); // Import transactions from a file

void exportTransactions(BlockChain *bc, char *filename); // Export transactions to a file

void displayAllBlocks(BlockChain *bc); // Show all blocks in blockchain

void displayTransactionsInBlock(BlockChain *bc, int blockId); // Show transactions in a specific block

void displayStudentTransactionsOnDate(BlockChain *bc, int studentID, char *date); // Show transactions of a specific student on a specific date

void displayMenu(int highlight); // Show menu

void menu(BlockChain *bc); // Main menu

void waitForKeyPress(); // Wait for key press

void displayAllStudentTransactions(BlockChain *bc, int studentID); // Show all transactions of a specific student

int removeTransaction(BlockChain *bc, int studentID, int transactionIndex); // Remove a specific transaction

void processRefund(BlockChain *bc); // Make refund

void searchByDescription(BlockChain *bc); // Search transactions by description
