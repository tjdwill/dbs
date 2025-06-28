# DBS Journal

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
