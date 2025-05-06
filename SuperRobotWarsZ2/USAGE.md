
#### Prebuild all projects  
1. Open `SuperRobotWarsZ2.sln`
1. Build whole solution.   
This should create `<project>\x64\Debug` folder with all tools.

#### Unpack EVENTBIN
- Create folder for workspace, ex. `Workspace\EVENTBIN`
- Copy original `EVENTBIN.BIN`/`EVENTBINBDY.BIN`/`EVENTBINHED.BIN` from `<iso>\PSP_GAME\USRDIR\KURO` folder into `Workspace\EVENTBIN`
- Unpack *.bin files  
From `Workspace\EVENTBIN` execute, that unpacks files into `EVENT` folder.
> <path_to_tools>\EVENTBIN_unpack.exe  
- Copy `EVENT` folder to `EVENT_dst` (just because all tools works like this...)

#### Extract text files
- From `Workspace\EVENTBIN` folder execute:  
> python <path_to_project_root>\MTFL_dump.py
- This should create `<file>.txt` and `<file>_point.txt` for each `MTFLz2_2`/`jstr` files (usually `0012_SDEMO` and `0013_DLGEV` files)
- Change text in `EVENT/..../<file>.txt` (not `EVENT_dst`!), and then...

#### Import text files back
- From `Workspace\EVENTBIN` folder execute:  
> python <path_to_project_root>\MTFL_import.py
- Then pack files into EVENTBIN\* again  
Ensure you have `EVENT` folder and `EVENT_dst` nearby
> EVENTBIN_pack.exe "<Workspace\EVENTBIN>\EVENT"
- This should create `EVENTBIN.new`/`EVENTBINBDY.new`/`EVENTBINHED.new` in `Workspace\EVENTBIN`
- Copy them into `<iso>\PSP_GAME\USRDIR\KURO`, remove old EVENTBIN files, copy `*.new` files, and change their extensions from `new` to `BIN`.