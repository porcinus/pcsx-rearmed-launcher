# pcsx-rearmed-launcher
When running a PSX game from Retroarch, each game use its own memcard (located into the same folder as the game), this is not the case with PCSX-ReARMed.

This launcher is design to allow a bidirectionnal compatibility between Retroarch and PCSX-ReARMed (tested in Retropie).
In short, it will create a symlink from `pcsx/memcards/card1.mcd` to the memcard use by Retroarch.
This is done using somes "security" :
 - If the "blank" memcard file not exist, use `pcsx/memcards/card9.mcd` to create it.
 - If the game never been played with Retroarch, launcher will create a "blank" memcard.
 - If `card1.mcd` is a file, backup it before creating the symlink and restore it when closing the emulator.

# History
 - 0.1a : Initial release.

# Limitations
The launcher isn't design to work with save states.
If user load another game will into PCSX-ReARMed, the launcher will not be able to update the memcard filename.
For example, you start `GAME1`, the launcher will create `GAME1.srm` but if you load `GAME2` from PCSX-ReARMed menu, the memcard used will stay `GAME1.srm`.
Note : PCSX-ReARMed standalone version is not able to load .m3u file.

# Setup
 - in `/opt/retropie/configs/psx`, edit `emulators.cfg` and add `pcsx-rearmed-launcher = "pushd /opt/retropie/emulators/pcsx-rearmed; ./pcsx-rearmed-launcher -cdfile %ROM%; popd"`.
 - Copy `pcsx-rearmed-launcher.cpp` to `/opt/retropie/emulators/pcsx-rearmed` and compile it by running `g++ pcsx-rearmed-launcher.cpp -o pcsx-rearmed-launcher`.
 - Before starting any game, you should consider backup all `.srm` files located in `/home/pi/RetroPie/roms/psx`.
 - Start a PSX game into EmulationStation and press A to edit setting and set default emulator to `pcsx-rearmed-launcher`.

# Issues
If you have any troubles, please open a Issues, explain your problem as clearly as possible and attach `/dev/shm/runcommand.log` file to the post.
