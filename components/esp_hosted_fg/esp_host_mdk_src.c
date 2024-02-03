

#include ".\common\protobuf-c\protobuf-c\protobuf-c.c"
#include ".\common\esp_hosted_config.pb-c.c"



/**components*/
#include ".\host\components\src\esp_queue.c"
/**control lib*/
#include ".\host\control_lib\src\ctrl_api.c"
#include ".\host\control_lib\src\ctrl_core.c"
/** stm32 - app - control*/
#include ".\host\stm32\app\control\control_utils.c"
#include ".\host\stm32\app\control\control.c"
/** stm32 - app - data*/
#include ".\host\stm32\app\data\arp_server_stub.c"
/** stm32 - app*/
#include ".\host\stm32\app\app_main_api.c"
#include ".\host\stm32\app\app_main.c"

/** stm32 - common*/
#include ".\host\stm32\common\common.c"
#include ".\host\stm32\common\stats.c"
#include ".\host\stm32\common\util.c"
/** stm32 - driver*/
#include ".\host\stm32\driver\network\netdev_api.c"
#include ".\host\stm32\driver\network\netdev_stub.c"
#include ".\host\stm32\driver\serial\serial_ll_if.c"

#include ".\host\stm32\driver\transport\spi\spi_drv.c"
#include ".\host\stm32\driver\transport\transport_drv.c"
#include ".\host\stm32\port\src\platform_wrapper.c"

/*virtual_serial_if*/
#include ".\host\virtual_serial_if\src\serial_if.c"
