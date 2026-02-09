# README-Skel.md

## Info/About

- Gen 2 is my favorite Pokémon game in so many ways and after finding Pokémon Heart & Soul and all it had to offer, it gave me the push to try making my own QoL features, changes, and additions that would make it my ideal Pokémon game. My goal for this repository is to make branches that are compatible with the [main](https://github.com/PokemonHnS-Development/pokemonHnS) branch of Pokémon Heart & Soul to allow other devs to take these branches and implement them hopefully easily.

- My `main` branch will contain changes so that my machine can compile (use `make modern`) and this document. I will attempt to keep my various branches up to date with my `main` so this document is updated with any changes when viewing those branches.

- The `build` branch will be the branch where I combine all my other branches to make a version with all my changes/additions.

- Occasionally, I may combine branches into groups of changes that are all similar in nature, merge branches into one - leaving the merged branch(es) as standalone - that I think would benefit from my other changes, or delete branches that have been merged into a grouping to keep things clean.

- I will probably have releases of my full build as I continue adding things, and possibly patch files for some of the changes/groups that I have made.

- Some branches will have screenshots in the `docs` folder if you wanted to see how a feature looked in game.

- Have any questions or notice any issues, please open an issue or I can be found on the Pokemon Heart & Soul Discord.

## Changes/Features/Additions

- Bug fixes from pokemonHnS branches cause I don't want to play with bugs if I don't have to. (collection of cherry-picked commits)

- Infinite Repel Nuzlocke Setting

- BST Viewer in Summary Page
  - View BST breakdown by pressing START on the Stats Summary screen
  - Planned: In battle viewer

- BST Distribution Randomizer Setting
  - future: BST Viewer merged into this feature

- Pokédex shortcuts
  - Party Menu's selection submenu
  - Info Summary screen (This means it can also be accessed when inspecting a Pokémon in the PC)

- Key Item DexNav that spits out the wild Pokêmon in the area
  - I gotta add "&"s to the lists cause its driving me nuts
  - Might implement a full UI based on [this DexNav UI](https://github.com/ghoulslash/pokeemerald/tree/dexnav).

- Field Moves submenu in party menu's selection menu (removes the limit on 4 usable field moves on a Pokémon)
  - **I also changed the logic on how the Challenge/Randomizer HM Override setting determines when to show FLY or FLASH in a Pokémon's FIELD MOVES. It now checks if the player has the HM in their bag for all slots in the party (the leading pokemon will still show FLY/FLASH regardless and non-leading slots will have a check for if they can learn it, due to how the original HM Override works)**



Thanks for reading & enjoy, Skel
