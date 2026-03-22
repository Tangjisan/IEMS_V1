#include "memory_SDHC.h"

namespace n_memory_SDHC{
    // SD card memory related functions would be implemented here

    namespace n_cmd{
        const uint8_t CMD0  = 0x40 | 0;   // GO_IDLE_STATE
        const uint8_t CMD8  = 0x40 | 8;   // SEND_IF_COND
        const uint8_t CMD17 = 0x40 | 17;  // READ_SINGLE_BLOCK
        const uint8_t CMD24 = 0x40 | 24;  // WRITE_SINGLE_BLOCK
        const uint8_t CMD55 = 0x40 | 55;  // APP_CMD
        const uint8_t ACMD41= 0x40 | 41;  // SD_SEND_OP_COND

        void init(void){
            // Initialization code for SD card commands
            
        }
    }

    void init(void){

    }

} // namespace n_memory_SD
