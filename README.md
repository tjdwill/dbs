# Digital Budgeting System (DBS)

Copyright &copy; 2025 Terrance Williams

This application is a digital implementation of the envelope budgeting system.
Given an amount of money, one is able to distribute it into different "accounts"
(envelopes), categories for which the money is earmarked.

## Usage Documentation

### Create an Account Book

When the program first begins, the user is met with a start screen prompting him
or her to load an account book (see the figure below). In DBS, an *account book*
is a file containing the data for all of the accounts of a given user. To create
an account book:

1. Select `File > New`
2. Enter the account book owner's name. 

The program will then create an empty book for that user. It can then be saved
to the desired location. DBS uses the [TOML](https://toml.io) language to store
the data.

<p align="center">
    <img src="https://github.com/tjdwill/dbs/blob/assets/dbsImages/startScreen.png" alt="DBS Start Screen" />
</p>

### Accounts

Once the account book is created, the user is then presented with the account
book view. This view lists all the accounts ("envelopes") the user has created.
A given account can be in one of two states: *active* or *inactive*. For active
accounts, users are able to make transactions (via the *Transact* button),
transferring money to and from said accounts. Inactive accounts are "read-only";
they are there for historical purposes, but users cannot make further
transactions with them. An account's state can be toggled at any time via the
*(De)Activate Account* button. 

To create an account:

1. Click the "Create Account" button at the bottom-left of the screen. 
2. Enter the desired account name.
3. (Optional) Enter the description that describes the account.
4. Press Ok

That's it! The account is given a unique identifier and listed among the active
accounts. From there, the user can deposit money into the account via the
*Transact* button.

Here's the view for inactive accounts. Notice the *Transact* button is greyed out:

<p align="center">
    <img src="https://github.com/tjdwill/dbs/blob/assets/dbsImages/inactiveAccount.png" alt="Inactive Accounts View" />
</p>

### Transactions

A *transaction* in DBS is a transfer of money. While an active account is
selected, pressing the *Transact* button will bring up the following dialog:

<p align="center">
    <img src="https://github.com/tjdwill/dbs/blob/assets/dbsImages/transactionDialog.png" alt="Transaction Dialog" />
</p>

To make a deposit into the selected account, enter a positive number into the
"Amount" field. A negative number is a withdrawal. The "Other Party" field
determines the other side of the transaction. Pressing the drop-down will list
the eligible accounts. The "External" drop down item represents a source outside
of DBS. It is intended to represent jobs, stores, people, etc. that one would
exchange currency with. Otherwise, the user can select from one of the active
accounts. Finally, the user can write notes to clarify the purpose of the
transaction.

The following image shows the result of two transactions on the "Groceries"
account:

<p align="center">
    <img src="https://github.com/tjdwill/dbs/blob/assets/dbsImages/activeAccountView.png" alt="Active Accounts View" />
</p>

If a transaction is made between two DBS accounts, the other party is
automatically updated with a transaction entry from its own perspective. So,
given Accounts A and B, a deposit into Account A will be seen as a withdrawal
when viewed from Account B.

> **NOTE**: Currently, the program does not guard against overdrafts, so use
caution when making transactions. This will be corrected in a future release.

### Shutdown

- `File > Close` will close the current account book, prompting the user to save
  any unsaved changes.
- `File > Exit` or pressing the X button will shut down the program, prompting
  the user to save any unsaved chagnges. The currently-loaded account book will
  be loaded the next time the program starts.
