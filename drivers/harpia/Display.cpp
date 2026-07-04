#include "AndroidPlatformDevice.h"

/* Harpia display driver — subclasses AndroidPlatformDevice
 * to drive the Qualcomm MDP4 display controller. */
class HarpiaDisplayDriver : public AndroidPlatformDevice
{
    OSDeclareDefaultStructors(HarpiaDisplayDriver)

public:
    bool start(IOService* provider) override
    {
        if (!AndroidPlatformDevice::start(provider))
            return false;

        /* MDP4 register offsets (from SD410E TRM). */
        const uint32_t MDP4_DISPLAY_CMD_TRIGGER   = 0x0003C;
        const uint32_t MDP4_DMA_P_QUEUE           = 0x02000;
        const uint32_t MDP4_DMA_P_CONFIG          = 0x02004;

        /* Enable DMA_P for framebuffer output. */
        write32(MDP4_DMA_P_CONFIG, 0x00000080); /* RGB888 mode */

        /* Set framebuffer base address. */
        /* write32(MDP4_DMA_P_BASE, fbPhysAddr); */

        /* Trigger display update. */
        write32(MDP4_DISPLAY_CMD_TRIGGER, 1);

        kprintf("[HarpiaDisplay] display started\n");
        return true;
    }
};

OSDefineMetaClassAndStructors(HarpiaDisplayDriver, AndroidPlatformDevice)