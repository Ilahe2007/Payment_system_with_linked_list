# Manage a Student's EATCoin Using a Singly Linked List

A terminal-based payment system for a student cafeteria, written in C.
Students pay for meals, receive credits and transfer **EATCoins** to each other. Every operation is stored in a simple **blockchain** built entirely from singly linked lists.

## How the data is organised

The system is built from three structures, all singly linked lists:

```
BlockChain
   │
   ▼
 Block 2 (20240118) ──► Block 1 (20240117) ──► Block 0 (20240116) ──► NULL
   │                      │                      │
   ▼                      ▼                      ▼
 Transaction            Transaction            Transaction
   │                      │                      │
   ▼                      ▼                      ▼
 Transaction            ...                    ...
   │
   ▼
 NULL
```

| Structure | Contains |
|-----------|----------|
| `BlockChain` | Pointer to the most recent block |
| `T_Block` | Block ID, date (`YYYYMMDD`), list of transactions of that day, pointer to the previous block |
| `T_Transaction` | Student ID, amount, description, pointer to the next transaction |

- One **block** = one day of transactions. New blocks are inserted at the head of the list, so the newest day comes first.
- A **positive amount** is a credit (deposit, scholarship, transfer received).
- A **negative amount** is a payment (meal, snack, transfer sent).
- A student's **balance** is the sum of all their transactions across every block.

## Features

**Core operations**

- Show a student's balance and their last transactions
- Credit a student's account
- Pay for a meal, refused if the balance is too low
- Transfer EATCoins between two students, with a balance check

**Additional functions we added**

- Interactive menu navigated with the arrow keys and Enter (ncurses)
- Display all blocks, with their ID, date and number of transactions
- Display every transaction in a given block
- Display a student's transactions on a given date
- Refund: pick a past transaction and remove it from the list
- Search transactions by description (case-insensitive)
- Export all transactions to `Darkbuys.txt`, and import them back

## Time complexity

B = number of blocks, T = average number of transactions per block, k = length of a string (date or description), L = number of lines in the file.

| Function | Complexity |
|----------|------------|
| `importTransactions` | O(L) |
| `exportTransactions` | O(B × T × k) |
| `displayAllBlocks` | O(B × T) |
| `displayTransactionsInBlock` | O(B + T) |
| `displayStudentTransactionsOnDate` | O(B × T × k) |
| `removeTransaction` | O(1) best case, O(B × T × k) worst case |
| `processRefund` | O(B × T × k) |
| `displayMenu`, `waitForKeyPress` | O(1) |

## Design choices

- **`addBlock` takes a `date` parameter**, so the date is passed in directly instead of being created again each time a block is added.
- **The blockchain is passed by pointer (`BlockChain *bc`) instead of by value.** Changes made inside a function then update the real blockchain, not a copy. This applies to `addBlock`, `soldeStudent`, `credit`, `pay`, `consult`, `transfer`, `importTransactions` and `exportTransactions`.
- **Refunds use two pointers (`prev` and `current`)** to unlink the transaction from the list, then free its memory.

## Project structure

| File | Role |
|------|------|
| `main.c` | Starts ncurses, creates the blockchain and opens the menu |
| `dsaproject.h` | Structure definitions and function prototypes |
| `dsaproject.c` | All the functions: blocks, transactions, balance, payments, transfers, refunds, file import/export, menu |
| `Darkbuys.txt` | Sample transactions file used for import/export |
| `Makefile` | Builds and runs the program |

## File format

`Darkbuys.txt` holds one transaction per line:

```
date; studentID; amount; description
```

Example:

```
2024/01/15; 101; 50.00; Initial deposit
2024/01/16; 101; -12.50; Lunch cafeteria
2024/01/17; 101; 30.00; Parent transfer
```

When importing, every new date starts a new block.

## Build and run

The project needs a C compiler and the **ncurses** library.

Install ncurses if you don't have it:

```bash
# Ubuntu / Debian
sudo apt install libncurses-dev

# macOS (Homebrew)
brew install ncurses
```

Build and run with the Makefile:

```bash
make run
```

Or compile by hand:

```bash
gcc main.c dsaproject.c -o main -lncurses
./main
```

**Tip:** choose "Import all transactions from a file Darkbuys" first to load the sample data, then try the other options.

## Technologies

C · ncurses · singly linked lists
