# Installing the driver
In order to disable the signing checks of windows against our development driver:

`bcdedit /set testsigning on`

`bcdedit /set nointegritychecks on`


To return to the normal state of windows undo the changes by:

`bcdedit /set testsigning off`

`bcdedit /set nointegritychecks off`


# Setup the driver:
Make sure you use the RELEASE version of the driver.
You won't be able to install and debug the DEBUG version.

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


# Debugging the driver:
Once we have 2 PCs that have an ethernet connection between them we can simplly debug by issuing 
on the target PC the following command:

`kdnet.exe <HostComputerIPAddress> <YourDebugPort>`

kdnet.exe is located normally at: `C:\Program Files (x86)\Windows Kits\10\Debuggers\x64`
I usually use port 50000.

Then from the host PC you can launch windbg and target the target maching by supplying the 
port and the key you get from the kdnet command we issued before.

Then in order for the host PC to connect to the target PC you will need to reboot the target.

In order to see prints from the target driver you will need to use the 
`DbgPrintEx()` function with the following parameters:

```
DbgPrintEx( DPFLTR_IHVDRIVER_ID,            
            0,                             
            "Hello World\n");             
```
