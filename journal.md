# DBS Journal

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

- Bloomberg's BDE (spec. `bdl::bdldfp` for decimal floating point)
- tomlplusplus
- Qt
