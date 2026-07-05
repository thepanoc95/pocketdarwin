#include "../AndroidPlatformDevice.h"

#define USB_CI_BASE             0x07800000
#define USB_CI_SIZE             0x01000000

#define USB_CI_REVISION         0x0000
#define USB_CI_CTRL             0x0004
#define USB_CI_STATUS           0x0008
#define USB_CI_FRAME_INDEX      0x000C
#define USB_CI_SEGMENT          0x0010
#define USB_CI_FRAME_LIST       0x0014
#define USB_CI_ASYNC_NEXT       0x0018
#define USB_CI_CONFIG_FLAG      0x0050
#define USB_CI_PORTSC           0x0044
#define USB_CI_PORTSC_1         0x0044

#define USB_CI_CTRL_RUN         (1U << 0)
#define USB_CI_CTRL_HOST        (1U << 1)
#define USB_CI_CTRL_RESET       (1U << 2)
#define USB_CI_CTRL_IRQ_EN      (1U << 3)
#define USB_CI_CTRL_PERIODIC_EN (1U << 4)
#define USB_CI_CTRL_ASYNC_EN    (1U << 5)
#define USB_CI_CTRL_PE_EN       (1U << 6)

#define USB_CI_PORTSC_CCS       (1U << 0)
#define USB_CI_PORTSC_CSC       (1U << 1)
#define USB_CI_PORTSC_PE        (1U << 2)
#define USB_CI_PORTSC_PEC       (1U << 3)
#define USB_CI_PORTSC_FPR       (1U << 5)
#define USB_CI_PORTSC_RESET     (1U << 8)
#define USB_CI_PORTSC_LS_MASK   (3U << 10)
#define USB_CI_PORTSC_PP        (1U << 12)
#define USB_CI_PORTSC_PTC_MASK  (0x0F << 16)

#define USB_CI_OTGSC            0x0064
#define USB_CI_OTGSC_ID         (1U << 0)
#define USB_CI_OTGSC_BSV        (1U << 1)
#define USB_CI_OTGSC_ASV        (1U << 2)
#define USB_CI_OTGSC_AVV        (1U << 3)
#define USB_CI_OTGSC_IDIS       (1U << 16)
#define USB_CI_OTGSC_BSVIS      (1U << 17)
#define USB_CI_OTGSC_ASVIS      (1U << 18)
#define USB_CI_OTGSC_AVVIS      (1U << 19)
#define USB_CI_OTGSC_IDIE       (1U << 24)
#define USB_CI_OTGSC_BSVIE      (1U << 25)
#define USB_CI_OTGSC_ASVIE      (1U << 26)
#define USB_CI_OTGSC_AVVIE      (1U << 27)

#define USB_CI_ASYNCLISTADDR    0x0018
#define USB_CI_PERIODICLISTBASE 0x0014

#define USB_CI_STS_USBINT       (1U << 0)
#define USB_CI_STS_ERROR        (1U << 1)
#define USB_CI_STS_PORT_CHG     (1U << 2)
#define USB_CI_STS_RESET        (1U << 4)
#define USB_CI_STS_SOF          (1U << 5)
#define USB_CI_STS_SUSPEND      (1U << 6)

#define USB_QTD_TOKEN_STATUS_MASK  0xFF
#define USB_QTD_TOKEN_XACTERR      (1U << 7)
#define USB_QTD_TOKEN_BABBLE       (1U << 5)
#define USB_QTD_TOKEN_HALTED       (1U << 6)
#define USB_QTD_TOKEN_ACTIVE       (1U << 7)
#define USB_QTD_TOKEN_PID_IN       (0U << 8)
#define USB_QTD_TOKEN_PID_OUT      (1U << 8)
#define USB_QTD_TOKEN_PID_SETUP    (2U << 8)

#define USB_OTG_HC_AVAIL         0

enum MSM8916UsbRole {
    kUsbRoleHost = 0,
    kUsbRolePeripheral = 1,
};

class MSM8916USB : public AndroidPlatformDevice
{
    OSDeclareDefaultStructors(MSM8916USB)

private:
    UInt32      fOtgscValue;
    MSM8916UsbRole fRole;

    void resetController(void)
    {
        write32(USB_CI_CTRL, USB_CI_CTRL_RESET);
        IOSleep(10);
        int timeout = 1000;
        while (timeout--) {
            if (!(read32(USB_CI_CTRL) & USB_CI_CTRL_RESET))
                break;
        }
    }

public:
    bool start(IOService* provider) override
    {
        if (!AndroidPlatformDevice::start(provider))
            return false;

        kprintf("[MSM8916USB] starting USB 2.0 OTG controller\n");

        resetController();

        write32(USB_CI_OTGSC, 0);

        fOtgscValue = read32(USB_CI_OTGSC);
        if (fOtgscValue & USB_CI_OTGSC_ID) {
            fRole = kUsbRolePeripheral;
            kprintf("[MSM8916USB] ID pin high — peripheral mode\n");

            write32(USB_CI_CTRL, USB_CI_CTRL_PE_EN);
        } else {
            fRole = kUsbRoleHost;
            kprintf("[MSM8916USB] ID pin low — host mode\n");

            write32(USB_CI_PORTSC_1, USB_CI_PORTSC_PP);

            write32(USB_CI_CTRL, USB_CI_CTRL_RUN | USB_CI_CTRL_HOST |
                                 USB_CI_CTRL_IRQ_EN);
        }

        write32(USB_CI_INT_STATUS, 0xFFFFFFFF);

        UInt32 intMask = USB_CI_STS_USBINT | USB_CI_STS_ERROR |
                         USB_CI_STS_PORT_CHG | USB_CI_STS_RESET |
                         USB_CI_STS_SOF;

        registerService();
        kprintf("[MSM8916USB] USB %s mode active\n",
                fRole == kUsbRoleHost ? "host" : "peripheral");
        return true;
    }

    IOReturn enableVBUS(bool enable)
    {
        if (enable) {
            UInt32 otgsc = read32(USB_CI_OTGSC);
            otgsc |= USB_CI_OTGSC_AVVIS | USB_CI_OTGSC_AVVIE;
            write32(USB_CI_OTGSC, otgsc);
        } else {
            UInt32 otgsc = read32(USB_CI_OTGSC);
            otgsc &= ~(USB_CI_OTGSC_AVVIS | USB_CI_OTGSC_AVVIE);
            write32(USB_CI_OTGSC, otgsc);
        }
        return kIOReturnSuccess;
    }

    IOReturn resetPort(void)
    {
        UInt32 portsc = read32(USB_CI_PORTSC_1);
        portsc |= USB_CI_PORTSC_RESET;
        write32(USB_CI_PORTSC_1, portsc);
        IOSleep(10);
        portsc &= ~USB_CI_PORTSC_RESET;
        write32(USB_CI_PORTSC_1, portsc);
        return kIOReturnSuccess;
    }

    bool isDeviceConnected(void)
    {
        return (read32(USB_CI_PORTSC_1) & USB_CI_PORTSC_CCS) != 0;
    }

    MSM8916UsbRole getRole(void)
    {
        return fRole;
    }
};

OSDefineMetaClassAndStructors(MSM8916USB, AndroidPlatformDevice)
