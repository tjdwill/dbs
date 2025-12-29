# DBS Journal

## 27-28 December 2025

- Create a preference widget to control color scheme (and other preferences in
  the future).
- TODO: 
  - ~~Fix sizing issue between the QListWidget and the QStackedWidget in thePreferenceWidget.~~
  - There was no sizing issue between the widgets. The preference window was too small.

The next feature I want to implement is an "Open Recent" option. It should list
the last five account books that were loaded from most recent to least.

**Update**: Recent files support has been implemented.

## 22 December 2025

I figured out how to build and install with shared libraries (PY)! I'll write
notes later about the following:

- CMake error on Windows 
  - Building SHARED
  - Tries to link, but can't find `.lib` file
  - Windows uses `.lib` for comp-time linking and the `.dll` for runtime loading
  - `.lib` isn't produced if no symbols are exported
- Symbol visibility
- GenerateExportHeader (`generate_export_header()` command)
- CMake install
- John Lakos-style architecture while only producing package-level targets.
  - No more granular, component-level `add_library()` commands.
  - Components still require the inclusion of the headers on which they depend.
  - Test drivers can still be individual targets.

## 21 December 2025

I tweaked the window title and the about page. Finally, I updated the README with
documentation for using the application. With these changes, I was able to
release a test version (PY!).

## 20 December 2025

Experimented with CMake install to create a basic distribution for Windows.
I will need to learn how to build a shared library version of DBS on Windows as
well as correct the installation on Linux. The install script does not appear to
copy Qt's platform plugins into the installation directory.

- Learn how to create _declspec macro files.
- Learn to build w/ shared libraries with correct linking
- Debug the Linux installation.

## 19 December 2025

I completed the draft of the DBS application. I'm not too ecstatic with how some
of the logic in dbscqt_mainwindow has panned out thus far, but the application
does work, PY. I plan to continue to refine the application, especially since
the "About" page still needs to implemented. I intend to figure out deployment
via CMake as well. 

## 18 December 2025

Skeleton implementations of the MainWindow menu actions.

## 17 December 2025

Today, I began editing the main window's interface. I am not sure how this will evolve, so
my present goal is to create something functional.

## 13 December 2025

I now have a working widget that can load accounts, display information, make
transactions, and update the accounts' active/inactive status.

TODO:
- [ ] Add "add transaction" button to dbscqt_accountmodel.m.cpp for feature completeness.
- [x] dbscqt_accountbookwidget: Add logic for creating accounts.
- [x] Add button icons.
- [x] MainWindow 

## 12 December 2025

- Add test driver application for dbscqt_accountbooktreewidget
- Made tweaks to dbscqt_accountbooktreewidget
    - Sorts items on account status change.
    - Adds added item to the tree widget.

## 8 December 2025

I fixed that bug (PY). Turns out, `std::chrono::system_clock` duration precision is
not a guaranteed value, so there was a difference between clang and MSVC. I've
updated the code to use nanosecond precision, so now the display works as
expected.

## 7 December 2025

I figured I should get a Windows build up and running to ensure proper behavior,
and boy am I glad I did. Firstly, the build setup wasn't as long as the EWI
project, so I've grown (PY!). However, problems arose with CMake. Windows needs
to use `windeployqt` in order to link the correct Qt libraries to the generated
executable(s). The process of getting that to happen post-build was...involved.
CMake's variable handling makes very little sense, and behavior appears to change
depending on the function. It was very frustrating, and I was up all times of
the evening hacking away at the issue. 

Definitely need some CMake refreshers in the near future.

After getting the project built, I did some testing. I notice that there is a
behavioral issue in that the dates don't work as expected. I'll look into this
later.

## 6 December 2025

A *ton* of structural changes today, PY. Ideally, the transition to the QTreeWidget representation will be concluded in the near future.
I'm going to have to double-check the Model/View programming aspects of the code; I'm still new to the paradigm.

## 11 November 2025

I've continued work on the dbscqt_accountbookwidget, even getting a working test (PY). 
I am a bit concerned in that I'm not 100% sure there are no memory leaks in the
implementation. I will need to do more analysis to ensure all allocated pointers are
destoyed.

Also, I think it's time to learn to use the debugger on Linux platforms. I may take a
detour to learn gdb.

## 1 November 2025

- Reworked model value printing.
- Added new utility function package.
- Updated test to sort by descending transaction date.

### Architecture Discussion

When viewing discussions regarding GUI development online, the advice to separate your logic from
the GUI is ubiquitous. However, as with so many discussions online, actionable advice on *how* to do that is
scarce. Yes, Qt has a Model/View paradigm, but what does this mean in terms of architecture? Does
the Qt-related code have a physical dependency on the core package, or do we introduce interface
classes in terms of Qt types that a higher-level package then is responsible for performing
conversions?

Initially, I leaned toward the latter and architected the code thus far such that dbsqt wouldn't
depend on dbsc. However, I realized that not only is that unreasonable and inefficient, it was never
true in the first place! The test code for accountmodel, for example, includes dbsc code. It has to
in order to ensure the two packages work together. Therefore, dbsqt already depends on dbsc. Also,
thinking about how I've seen code structured elsewhere, it appears that a provable architecture is
one that isolates the core logic to a package and has an analogous Qt-dependent package that
directly uses the core code (types, functions, etc.). Basically, the Qt code needs to live on a level
*above* the core code rather than attempting to keep them on the same level. 

As a result, I've changed the name of `dbsqt` to `dbscqt`. This name change should imply that the qt
code depends directly on the `dbsc` package.

## 16 October 2025

### Qt/BDE Interaction

I opened an issue on the BDE Github, and the Bloomberg devs were kind enough to provide a
fix to the name collision (my hunch was correct; it was the clash with `emit`). They also
recommended setting the `QT_NO_KEYWORDS` macro to preclude additional naming collisions
that may appear. So, I can now mix dbsc code into dbsqt code if need be.

### What Model/View Scheme Should I Employ?

Currently, I've implemented dbsqt::AccountModel with the intent of using a QTableView.
However, there are a few problems with the current implementation:

0. Currently, the otherParty (I should think of a better name) is written as a UUID string, but I
   don't think that's particularly readable for non-technical viewers. I would like a label of some
   form like `${HumanReadableName} (${first X digits of UUID})`. However, the TransactionItem does
   not store this account name,—nor should it—and it doesn't have ready access to the account book
   to query the account name. 
1. The AccountBook → Account → Transaction flow forms a clear hierarchy, meaning it can be modeled
   by a tree. It would make sense to use a TreeView for this, allowing multiple accounts to be
   displayed if need be.
2. Again, there is no way to access upper-level information that would provide convenience
   for both the developer and the user.

Based on both the hierarchical nature of the account paradigm and the need to query
information from an upper level, I think the data should be modeled as a tree. The next
question to solve is: should I use the stock QTreeWidget and QTreeItem convenience classes
or roll out a custom implementation for a more flexible Model-View implementation? In the
spirit of learning, I want to try rolling out my own, but that may actually be an
inefficient use of time if the goal is to actually develop a product. Because the primary
programming goal of this project is to display what I've learned thus far, taking a detour
to learn how to develop custom models seems out-of-scope. Therefore, I will first attempt
to model the data via the convenience classes. If I determine that additional flexibility
is needed, I will then take a detour to learn custom tree model implementation. Basically,
I'm postponing the learning effort.

## 10 October 2025

It's been a month, but I haven't been particularly motivated to work. I'm back though.

I discovered a potential issue with Qt and BDE. Whenever a Qt component is included before
a BDE component, compilation fails (perhaps due to the `emit` keyword vs. its use in BDE as
a function?). The consequence of this behavior is that the Qt component essentially can't
include dbsc headers (dbsc_uuidstring is fine, but it would be better to simply prohibit
any direct dependency on that package).

I'm thinking that the Qt code will need to describe an iterface to represent the dbsc data
rather than take it directly. The upper-level code that depends on both dbsqt and dbsc will
be responsible for conversions. This also means that the only time both packages can be
used in a single component is in the final controller implementation. It *also* means that
all dbsc components must be included before any dbsqt component is included. Fortunately,
this will be the case by nature of alphabetic sorting. 

For posterity's sake (esp. if I decide to ask about this online at some point), here is the
output for the failed compilation:

```
FAILED: src/dbsqt/CMakeFiles/dbsqtAccountModel.dir/dbsqt_accountmodel.cpp.o 
/usr/bin/clang++-20 -DQT_CORE_LIB -DQT_GUI_LIB -DQT_WIDGETS_LIB -D_POSIX_PTHREAD_SEMANTICS -D_REENTRANT -I/home/dev/programming/cpp/dbs/build_Debug/src/dbsqt/dbsqtAccountModel_autogen/include -I/home/dev/programming/cpp/dbs/src/. -I/home/dev/programming/cpp/dbs/src/dbsqt -I/home/dev/programming/cpp/dbs/dbsc -I/home/dev/programming/cpp/dbs/src/dbsc -I/home/dev/programming/cpp/dbs/3rdparty -isystem /home/dev/programming/cpp/dbs/3rdparty/bde/include -isystem /home/dev/programming/cpp/dbs/3rdparty/Qt/6.9.1/gcc_64/include/QtCore -isystem /home/dev/programming/cpp/dbs/3rdparty/Qt/6.9.1/gcc_64/include -isystem /home/dev/programming/cpp/dbs/3rdparty/Qt/6.9.1/gcc_64/mkspecs/linux-g++ -isystem /home/dev/programming/cpp/dbs/3rdparty/Qt/6.9.1/gcc_64/include/QtGui -isystem /home/dev/programming/cpp/dbs/3rdparty/Qt/6.9.1/gcc_64/include/QtWidgets -g -std=gnu++23 -Wall -Wextra -Wpedantic -pthread -fPIC -MD -MT src/dbsqt/CMakeFiles/dbsqtAccountModel.dir/dbsqt_accountmodel.cpp.o -MF src/dbsqt/CMakeFiles/dbsqtAccountModel.dir/dbsqt_accountmodel.cpp.o.d -o src/dbsqt/CMakeFiles/dbsqtAccountModel.dir/dbsqt_accountmodel.cpp.o -c /home/dev/programming/cpp/dbs/src/dbsqt/dbsqt_accountmodel.cpp
In file included from /home/dev/programming/cpp/dbs/src/dbsqt/dbsqt_accountmodel.cpp:4:
In file included from /home/dev/programming/cpp/dbs/src/./dbsc/dbsc_account.h:18:
In file included from /home/dev/programming/cpp/dbs/src/dbsc/dbsc_transaction.h:20:
In file included from /home/dev/programming/cpp/dbs/3rdparty/bde/include/bdldfp_decimal.h:606:
In file included from /home/dev/programming/cpp/dbs/3rdparty/bde/include/bdldfp_decimalimputil.h:147:
In file included from /home/dev/programming/cpp/dbs/3rdparty/bde/include/bsl_iostream.h:18:
In file included from /home/dev/programming/cpp/dbs/3rdparty/bde/include/bsl_ostream.h:17:
In file included from /home/dev/programming/cpp/dbs/3rdparty/bde/include/bslstl_ostream.h:18:
/home/dev/programming/cpp/dbs/3rdparty/bde/include/bslstl_syncbufbase.h:99:34: error: expected ')'
   99 |     static bool emit(SyncBufBase *syncBuf);
      |                                  ^
/home/dev/programming/cpp/dbs/3rdparty/bde/include/bslstl_syncbufbase.h:99:21: note: to match this '('
   99 |     static bool emit(SyncBufBase *syncBuf);
      |                     ^
/home/dev/programming/cpp/dbs/3rdparty/bde/include/bslstl_syncbufbase.h:107:31: error: must use 'class' tag to refer to type 'SyncBufBase' in this scope
  107 |     static void setEmitOnSync(SyncBufBase *syncBuf, bool value);
      |                               ^
/home/dev/programming/cpp/dbs/3rdparty/bde/include/bslstl_syncbufbase.h:99:22: note: class 'SyncBufBase' is hidden by a non-type declaration of 'SyncBufBase' here
   99 |     static bool emit(SyncBufBase *syncBuf);
      |                      ^
/home/dev/programming/cpp/dbs/3rdparty/bde/include/bslstl_syncbufbase.h:117:23: error: expected unqualified-id
  117 | bool SyncBufBaseUtil::emit(SyncBufBase *syncBuf)
      |                       ^
/home/dev/programming/cpp/dbs/3rdparty/bde/include/bslstl_syncbufbase.h:124:23: error: variable has incomplete type 'void'
  124 | void SyncBufBaseUtil::setEmitOnSync(SyncBufBase *syncBuf, bool value)
      |                       ^
/home/dev/programming/cpp/dbs/3rdparty/bde/include/bslstl_syncbufbase.h:124:64: error: expected '(' for function-style cast or type construction
  124 | void SyncBufBaseUtil::setEmitOnSync(SyncBufBase *syncBuf, bool value)
      |                                                           ~~~~ ^
/home/dev/programming/cpp/dbs/3rdparty/bde/include/bslstl_syncbufbase.h:124:50: error: use of undeclared identifier 'syncBuf'
  124 | void SyncBufBaseUtil::setEmitOnSync(SyncBufBase *syncBuf, bool value)
      |                                                  ^
/home/dev/programming/cpp/dbs/3rdparty/bde/include/bslstl_syncbufbase.h:124:70: error: expected ';' after top level declarator
  124 | void SyncBufBaseUtil::setEmitOnSync(SyncBufBase *syncBuf, bool value)
      |                                                                      ^
In file included from /home/dev/programming/cpp/dbs/src/dbsqt/dbsqt_accountmodel.cpp:4:
In file included from /home/dev/programming/cpp/dbs/src/./dbsc/dbsc_account.h:18:
In file included from /home/dev/programming/cpp/dbs/src/dbsc/dbsc_transaction.h:20:
In file included from /home/dev/programming/cpp/dbs/3rdparty/bde/include/bdldfp_decimal.h:606:
In file included from /home/dev/programming/cpp/dbs/3rdparty/bde/include/bdldfp_decimalimputil.h:147:
In file included from /home/dev/programming/cpp/dbs/3rdparty/bde/include/bsl_iostream.h:18:
In file included from /home/dev/programming/cpp/dbs/3rdparty/bde/include/bsl_ostream.h:17:
/home/dev/programming/cpp/dbs/3rdparty/bde/include/bslstl_ostream.h:126:60: error: expected unqualified-id
  126 |             if (!BloombergLP::bslstl::SyncBufBaseUtil::emit(p)) {

```

## 11 September 2025

Let's begin designing the graphical user interface for this application. 

For icons, I'm strongly considering Icons8. They are free, and their attribution requirement
consists of: 
- Linking to the main website (icons8.com)
- Linking to every icon used. 
  - I thought this meant I needed to include a link exactly *where* the icon is used, but [this
    link](https://community.icons8.com/t/free-license-attribution-requirements/1708) proves that it
    is sufficient to simply link to a source that contains all of the icons used and their links.
    Note that the user, elena_icons8, is the same person who wrote [this
    article](https://intercom.help/icons8-7fb7577e8170/en/articles/5534926-universal-multimedia-license-agreement-for-icons8)
    about the website's licensing.

Teal color: #20C997
- U.S. Dollar Sign: https://icons8.com/icon/7172/us-dollar-circled
- Ledger: https://icons8.com/icon/65613/ledger
- Summary List: https://icons8.com/icon/48622/summary-list
- Transaction: https://icons8.com/icon/61743/exchange

I propose a main window design that has a tree widget pane, enabling the selection of various app
pages. There are a number of pages needed for the app's function:

1. Summary
  - Lists the accounts in the book as well as their balances. 
  - Displays recent transactions sorted by time(?)
0. Account View
  - Allows a deep-dive into a specific account.
  - View transaction details
  - (?) Make transactions
  - Change Account name and description
0. Open/Close Account  
  - Create new accounts
  - Change Account status (open vs. closed)
0. Make Transaction(s)


## 6 September 2025

It's been a while since I've made an entry, but I have been working on this from time to
time. I've implemented working serialization (PY), and learned a bit about templates and
concepts along the way. I consider this a great success. At this point, I think it may be
time to consider the GUI design and front-end/back-end interface more concretely.

## 24 July 2025

I realized that, should a user want to close an account, we need a way to prevent
additional transactions from being added to the account without actually deleting the data.
This will allow one to view "archived" accounts and even re-open accounts that have been
closed. As a result, the account status is to be serialized along with the account name and
description (should the key be`isOpen`)?

## 23 July 2025

I completed an initial implementation of dbsc::Account. Now, I need to create an
AccountBook that stores all accounts for a user.

### Serialization

After thinking about the serialization process, I've come to the conclusion that
transactions need to store both Uuids for the two parties.

Here is an example format. The entire table serves as the AccountBook:

```toml
owner = "Who owns this account?"

[<Account 1 UUID>]
name = "Account name"
description = "Some description of the account."

[[<Account 1 UUID>.transactions]]
otherPartyID = "Other Party UUID"
timeStamp = YYYY-MM-DD HH:MM:SS.s
amount = "1000.00"
notes = """Multi-line notes"""

```

For example, here is an example of TOML and its analogous JSON:

```toml
owner = "tjdwill"

[testAccount]
name = "foo"
description = ""

[[testAccount.transactions]]
otherParty = "1234"
timeStamp = 2025-07-23 18:11:56.5Z
amount = "1234.00"
notes = """"""

[[testAccount.transactions]]
otherParty = "2468"
timeStamp = 2025-07-23 18:12:56.5Z
amount = "1234.00"
notes = """"""
```

```json
{
  "owner": "tjdwill",
  "testAccount": {
    "name": "foo",
    "description": "",
    "transactions": [
      {
        "otherParty": "1234",
        "timeStamp": "2025-07-23T18:11:56.500Z",
        "amount": "1234.00",
        "notes": ""
      },
      {
        "otherParty": "2468",
        "timeStamp": "2025-07-23T18:12:56.500Z",
        "amount": "1234.00",
        "notes": ""
      }
    ]
  }
}
```

## 9 July 2025

- Finished initial draft of the serialization interface.

## 8 July 2025

- Attempted to implement serialization interface as a concept
    - Still much to learn. Doing it as an abstract base class instead.

## 7 July 2025

Today, I would like to think about DBS' serialization subsystem.

Goal: *Given multiple account objects, write them to file and load them from file.*

Ideally, we want to be able to add additional transactions without needing to rewrite the
entire file every time, but this is likely an implementation detail instead of a hard
requirement. Now, the naive solution is to assume this serialization is done using one
format (TOML, in this case). However, since this is also intended to be a learning project,
I'd like to take the time to make it more "robust".

So, what do I need? Currently, I have two "major" classes: `dbsc::Account` and
`dbsc::Transaction`. An account has a collection of transactions. At the top-level, I think
I would have a map whose keys are `dbsc::UuidString`s and whose values are
`dbsc::Account`s. That would essentially make a "Patron". For a given patron, one needs to
parse all of the Accounts (and therefore all Transactions). This means I need a way to
serialize:

- Accounts
    - ID
    - Name
    - Description
    - Balance (possibly)
- Transactions
    - Transaction ID
    - Other Party ID
    - Amount
    - DateTime
    - Extra Notes 

In the abstract, `DbscParser` would have:

- `readPatron`
- `readAccount`
- `readTransaction`
- `writePatron`
- `writeAccount`
- `writeTransaction`

Implementers would then be responsible for adhering to this interface, regardless of the
means used to do so.

## 5 July 2025

Another class to consider creating is a broker class, a class responsible for actually
making transactions for given accounts. This component would need to build
upon `dbsc::{UuidString, Transaction, and Account}` at minimum.

The broker would have at least three static functions:
 
1. a function to perform a transaction between two `dbsc::Account` objects.
0. a function to perform a deposit into a `dbsc::Account`.
0. a function to perform a withdrawal from a `dbsc::Account`.

## 4 July 2025

- Created a class that represents a UUID string (read-only, valid UUID).
  This will make programming easier to think about because UUID semantics are enforced programmatically instead of implicitly.
- Reorganized project tree a bit. The `tutorialSpace/` directory had nothing to do with the
  actual application, so it is now outside of the `src/` directory.

## 3 July 2025

Today, I continued yesterday's work by testing the formatting of the date time string.
The chrono library is still a bit confusing, but I have enough to do what I need.

## 2 July 2025

- Basic class design
- Edited clang format file.

## 28 June 2025

Now that I have the stduuid library, I can generate UUIDs for unique identification. How
will this work in practice, however? Obviously, since the data will need to be serialized,
each id will need to be converted to a string when written to file. However, when loading,
should I keep it as a string, or should I make the effort of converting to a UUID object?
As of now, there is no perceived benefit to using the class itself. The string
rerpesentation can be used as map keys just as readily as the class, and, if we are
considering the principle of minimizing dependencies, the design would transform from
depending on the specific library used for generating uuids to simply needing *some*
library that can produce a UUID as a text string. I prefer the former, so that is the
direction I will choose. The design can always be changed if need demands it.

### Accounts

What do I need to represent a given "account"?

- Account number (uuid)
- Account name (possibly non-unique)
- Account Description
- Balance
- Transaction History (a collection of transactions)

### Transactions

A transaction is a financial event that occurs for a given account. One can either transfer
money into an account or out of an account. For a transaction between two accounts, we need
for the accounts to have the same transaction IDs. 

- Transaction ID (uuid)
- Date Time
- Transaction Amount (+Deposit, -Withdrawal)
- Source (?)
- Additional notes

Both transactions and accounts need to be serializable.

## 21 June 2025

I have a means of working with decimal floating point and a means of serializing the data
via TOML. Another aspect of the project is generating unique identifiers for created
accounts and transaction IDs. After some research into the matter, I learned about
Universal Unique Identifiers (UUIDs), a 128-bit (16 byte) schema that enables unique
identification without a centralized registry. There are many versions of UUIDs as defined
by RFC4122 (and the much more recent RFC9562), but the primary ones of interest for my use
case are v4 (randomized generation), v5 (namespaced + SHA-1 hashing), and v7 (Unix time). 

Since the identifiers are really only intended to be used internally such that no
communication is necessary between instances of the program, I think I can get away with
using v4 throughout the program. 

While implementing  UUID generation myself would be a viable exercise (indeed, I may do
this as a project in the future for multiple languages), I have decided to use a [library
for this](https://github.com/mariusbancila/stduuid). 

## 17 June 2025

Though this is the first entry in this journal, I've been working on this project's design
for longer. So far, I've been approaching questions such as: "How will I serialize the
data" and "How do I perform calculations that are accurate with money"?

Already, I have experimented with Bloomberg's `bdl::bdldfp` decimal floating point library,
and I think it is perfectly suitable for my use case. However, I will need to create
facilities to easily convert between decimal floating point values and `QString` values. 
Additionally, because serialization methods use binary floating point, I will need to
remember to *serialize any and all monetary values as strings*. In this way, the conversion
between string and decimal floating point will be lossless.

I've decided to use the TOML language for serialization because, unlike JSON, it allows one
to append to a file, continuing to add data to the overall table without needing to
re-write completely. My thinking is that this will be more efficient for recording
transactions within the application.

I've been experimenting with the `tomlplusplus` library, and I am happy with it so far. I
think the goal at this point is to identify as many puzzle pieces for this project as
possible before beginning the work of solving it.

## Living List of To-Dos

- [x] Create a serialization schema
- [x] Implement monetary calculations
- [x] Determine how to generate unique IDs.
- [x] Design the UI (down the line)

## Libraries

- [Bloomberg's BDE](https://github.com/bloomberg/bde) (spec. `bdl::bdldfp` for decimal floating point)
- [tomlplusplus](https://github.com/marzer/tomlplusplus)
- Qt (6.9.1)
- [stduuid](https://github.com/mariusbancila/stduuid/)
