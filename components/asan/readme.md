


https://mcuoneclipse.com/2021/05/31/finding-memory-bugs-with-google-address-sanitizer-asan-on-microcontrollers/

https://github.com/ErichStyger/mcuoneclipse/tree/master/Examples/MCUXpresso/tinyK22/tinyK22_FreeRTOS_ASAN

# Using McuASAN
1. Add the following three files to your project:
~~~
McuASANconfig.h: configuration header file
McuASAN.h: interface to the McuASAN implementation
McuASAN.c: implementation with memory hooks, callbacks and shadow map
~~~
2. For each source file which shall be instrumented, add the following option to the compiler settings:
~~~
-fsanitize=kernel-address
~~~
3. Initialize the ASAN module with:
~~~
McuASAN_Init();
~~~
4. Configure ASAN with the defines in McuASANconfig.h. The most important ones are:
~~~
McuASAN_CONFIG_IS_ENABLED: set to 1 to enable ASAN
McuASAN_CONFIG_CHECK_MALLOC_FREE: set to 1 for catching malloc/free problems
McuASAN_CONFIG_APP_MEM_START: specify start address of covered memory for shadow map
McuASAN_CONFIG_APP_MEM_SIZE: size of memory to be covered in shadow map
McuASAN_CONFIG_MALLOC_RED_ZONE_BORDER: size of red zones
McuASAN_CONFIG_FREE_QUARANTINE_LIST_SIZE: size of quarantine list
~~~
5. Configure in the error hook what to do (log a message or halt the application). Below an example implementation:
~~~
static void ReportError(void *address, size_t kAccessSize, rw_mode_e mode) {
  McuLog_fatal("ASAN ptr failure: addr 0x%x, %s, size: %d", address, mode==kIsRead?"read":"write", kAccessSize);
  __asm volatile("bkpt #0"); /* stop application if debugger is attached */
}
~~~


6. end