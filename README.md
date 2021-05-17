# Installing the driver
In order to disable the signing checks of windows against our development driver:

`bcdedit /set testsigning on`

`bcdedit /set nointegritychecks on`


To return to the normal state of windows undo the changes by:

`bcdedit /set testsigning off`

`bcdedit /set nointegritychecks off`


Option 1:

To install the driver:

`sc create <driver_name> type= kenrel binPath= <full_path_to_sys_file>`

To Load the driver:

`sc start <driver_name>`

To Unload the driver:

`sc stop <driver_name>`

To uninstall the driver:

`sc delete <driver_name>`

Option 2 (use drive.c in this repo):

To install the driver:

`drive.exe install <driver_name> <full_path_to_sys_file>`

To Load the driver:

`drive.exe start <driver_name>`

To Unload the driver:

`drive.exe stop <driver_name>`

To uninstall the driver:

`drive.exe uninstall <driver_name>`
