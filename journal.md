# DBS Journal

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

- Attempted to implement serailization interface as a concept
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
0. a function to perform a withdrawl from a `dbsc::Account`.

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
- Transaction Amount (+Deposit, -Withdrawl)
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

- [ ] Create a serialization schema
- [ ] Implement monetary calculations
- [ ] Determine how to generate unique IDs.
- [ ] Design the UI (down the line)
- [ ] 

## Libraries

- [Bloomberg's BDE](https://github.com/bloomberg/bde) (spec. `bdl::bdldfp` for decimal floating point)
- [tomlplusplus](https://github.com/marzer/tomlplusplus)
- Qt (6.9.1)
- [stduuid](https://github.com/mariusbancila/stduuid/)
