/** 
 * This file is intended to provide backward 
 * compatibility for main osService/OSSL 
 * APIs. 
 *
 * It shall be phased out gradually and users
 * are strongly recommended to use IX_OSAL API.
 *
 * @par
 * IXP400 SW Release version  2.0
 * 
 * -- Intel Copyright Notice --
 * 
 * @par
 * Copyright 2002-2005 Intel Corporation All Rights Reserved.
 * 
 * @par
 * The source code contained or described herein and all documents
 * related to the source code ("Material") are owned by Intel Corporation
 * or its suppliers or licensors.  Title to the Material remains with
 * Intel Corporation or its suppliers and licensors.
 * 
 * @par
 * The Material is protected by worldwide copyright and trade secret laws
 * and treaty provisions. No part of the Material may be used, copied,
 * reproduced, modified, published, uploaded, posted, transmitted,
 * distributed, or disclosed in any way except in accordance with the
 * applicable license agreement .
 * 
 * @par
 * No license under any patent, copyright, trade secret or other
 * intellectual property right is granted to or conferred upon you by
 * disclosure or delivery of the Materials, either expressly, by
 * implication, inducement, estoppel, except in accordance with the
 * applicable license agreement.
 * 
 * @par
 * Unless otherwise agreed by Intel in writing, you may not remove or
 * alter this notice or any other notice embedded in Materials by Intel
 * or Intel's suppliers or licensors in any way.
 * 
 * @par
 * For further details, please see the file README.TXT distributed with
 * this software.
 * 
 * @par
 * -- End Intel Copyright Notice --
 */


#ifndef IX_OSAL_BACKWARD_MEM_MAP_H
#define IX_OSAL_BACKWARD_MEM_MAP_H

#include "IxOsal.h"

#define IX_OSSERV_SWAP_LONG(wData) IX_OSAL_SWAP_LONG(wData)
#define IX_OSSERV_SWAP_SHORT(sData) IX_OSAL_SWAP_SHORT(sData)

#define IX_OSSERV_SWAP_SHORT_ADDRESS(sAddr) IX_OSAL_SWAP_SHORT_ADDRESS(sAddr)
#define IX_OSSERV_SWAP_BYTE_ADDRESS(bAddr) IX_OSAL_SWAP_BYTE_ADDRESS(bAddr)

#define IX_OSSERV_BE_XSTOBUSL(wData)  IX_OSAL_BE_XSTOBUSL(wData)
#define IX_OSSERV_BE_XSTOBUSS(sData)  IX_OSAL_BE_XSTOBUSS(sData)
#define IX_OSSERV_BE_XSTOBUSB(bData)  IX_OSAL_BE_XSTOBUSB(bData)
#define IX_OSSERV_BE_BUSTOXSL(wData)  IX_OSAL_BE_BUSTOXSL(wData)
#define IX_OSSERV_BE_BUSTOXSS(sData)  IX_OSAL_BE_BUSTOXSS(sData)
#define IX_OSSERV_BE_BUSTOXSB(bData)  IX_OSAL_BE_BUSTOXSB(bData)

#define IX_OSSERV_LE_AC_XSTOBUSL(wAddr) IX_OSAL_LE_AC_XSTOBUSL(wAddr)
#define IX_OSSERV_LE_AC_XSTOBUSS(sAddr) IX_OSAL_LE_AC_XSTOBUSS(sAddr)
#define IX_OSSERV_LE_AC_XSTOBUSB(bAddr) IX_OSAL_LE_AC_XSTOBUSB(bAddr)
#define IX_OSSERV_LE_AC_BUSTOXSL(wAddr) IX_OSAL_LE_AC_BUSTOXSL(wAddr)
#define IX_OSSERV_LE_AC_BUSTOXSS(sAddr) IX_OSAL_LE_AC_BUSTOXSS(sAddr)
#define IX_OSSERV_LE_AC_BUSTOXSB(bAddr) IX_OSAL_LE_AC_BUSTOXSB(bAddr)

#define IX_OSSERV_LE_DC_XSTOBUSL(wData) IX_OSAL_LE_DC_XSTOBUSL(wData)
#define IX_OSSERV_LE_DC_XSTOBUSS(sData) IX_OSAL_LE_DC_XSTOBUSS(sData)
#define IX_OSSERV_LE_DC_XSTOBUSB(bData) IX_OSAL_LE_DC_XSTOBUSB(bData)
#define IX_OSSERV_LE_DC_BUSTOXSL(wData) IX_OSAL_LE_DC_BUSTOXSL(wData)
#define IX_OSSERV_LE_DC_BUSTOXSS(sData) IX_OSAL_LE_DC_BUSTOXSS(sData)
#define IX_OSSERV_LE_DC_BUSTOXSB(bData) IX_OSAL_LE_DC_BUSTOXSB(bData)

#define IX_OSSERV_READ_LONG(wAddr)          IX_OSAL_READ_LONG(wAddr)
#define IX_OSSERV_READ_SHORT(sAddr)         IX_OSAL_READ_SHORT(sAddr)
#define IX_OSSERV_READ_BYTE(bAddr)          IX_OSAL_READ_BYTE(bAddr)
#define IX_OSSERV_WRITE_LONG(wAddr, wData)  IX_OSAL_WRITE_LONG(wAddr, wData)
#define IX_OSSERV_WRITE_SHORT(sAddr, sData) IX_OSAL_WRITE_SHORT(sAddr, sData)
#define IX_OSSERV_WRITE_BYTE(bAddr, bData)  IX_OSAL_WRITE_BYTE(bAddr, bData)


#define IX_OSSERV_READ_NPE_SHARED_LONG(wAddr)            IX_OSAL_READ_BE_SHARED_LONG(wAddr)
#define IX_OSSERV_READ_NPE_SHARED_SHORT(sAddr)           IX_OSAL_READ_BE_SHARED_SHORT(sAddr)
#define IX_OSSERV_WRITE_NPE_SHARED_LONG(wAddr, wData)    IX_OSAL_WRITE_BE_SHARED_LONG(wAddr, wData)
#define IX_OSSERV_WRITE_NPE_SHARED_SHORT(sAddr, sData)   IX_OSAL_WRITE_BE_SHARED_SHORT(sAddr, sData)

#define IX_OSSERV_SWAP_NPE_SHARED_LONG(wData)            IX_OSAL_SWAP_BE_SHARED_LONG(wData)
#define IX_OSSERV_SWAP_NPE_SHARED_SHORT(sData)           IX_OSAL_SWAP_BE_SHARED_SHORT(sData)


/* Map osServ address/size */
#define IX_OSSERV_QMGR_MAP_SIZE			IX_OSAL_IXP400_QMGR_MAP_SIZE
#define IX_OSSERV_EXP_REG_MAP_SIZE		IX_OSAL_IXP400_EXP_REG_MAP_SIZE
#define IX_OSSERV_UART1_MAP_SIZE		IX_OSAL_IXP400_UART1_MAP_SIZE
#define IX_OSSERV_UART2_MAP_SIZE		IX_OSAL_IXP400_UART2_MAP_SIZE
#define IX_OSSERV_PMU_MAP_SIZE			IX_OSAL_IXP400_PMU_MAP_SIZE
#define IX_OSSERV_OSTS_MAP_SIZE			IX_OSAL_IXP400_OSTS_MAP_SIZE
#define IX_OSSERV_NPEA_MAP_SIZE			IX_OSAL_IXP400_NPEA_MAP_SIZE
#define IX_OSSERV_NPEB_MAP_SIZE			IX_OSAL_IXP400_NPEB_MAP_SIZE
#define IX_OSSERV_NPEC_MAP_SIZE			IX_OSAL_IXP400_NPEC_MAP_SIZE
#define IX_OSSERV_ETHA_MAP_SIZE			IX_OSAL_IXP400_ETHA_MAP_SIZE
#define IX_OSSERV_ETHB_MAP_SIZE			IX_OSAL_IXP400_ETHB_MAP_SIZE
#define IX_OSSERV_USB_MAP_SIZE			IX_OSAL_IXP400_USB_MAP_SIZE
#define IX_OSSERV_GPIO_MAP_SIZE			IX_OSAL_IXP400_GPIO_MAP_SIZE
#define IX_OSSERV_EXP_BUS_MAP_SIZE		IX_OSAL_IXP400_EXP_BUS_MAP_SIZE
#define IX_OSSERV_EXP_BUS_CS0_MAP_SIZE	IX_OSAL_IXP400_EXP_BUS_CS0_MAP_SIZE
#define IX_OSSERV_EXP_BUS_CS1_MAP_SIZE	IX_OSAL_IXP400_EXP_BUS_CS1_MAP_SIZE
#define IX_OSSERV_EXP_BUS_CS4_MAP_SIZE	IX_OSAL_IXP400_EXP_BUS_CS4_MAP_SIZE


#define IX_OSSERV_GPIO_PHYS_BASE			IX_OSAL_IXP400_GPIO_PHYS_BASE
#define IX_OSSERV_UART1_PHYS_BASE			IX_OSAL_IXP400_UART1_PHYS_BASE
#define IX_OSSERV_UART2_PHYS_BASE			IX_OSAL_IXP400_UART2_PHYS_BASE
#define IX_OSSERV_ETHA_PHYS_BASE			IX_OSAL_IXP400_ETHA_PHYS_BASE
#define IX_OSSERV_ETHB_PHYS_BASE			IX_OSAL_IXP400_ETHB_PHYS_BASE
#define IX_OSSERV_NPEA_PHYS_BASE			IX_OSAL_IXP400_NPEA_PHYS_BASE
#define IX_OSSERV_NPEB_PHYS_BASE			IX_OSAL_IXP400_NPEB_PHYS_BASE
#define IX_OSSERV_NPEC_PHYS_BASE			IX_OSAL_IXP400_NPEC_PHYS_BASE
#define IX_OSSERV_PERIPHERAL_PHYS_BASE		IX_OSAL_IXP400_PERIPHERAL_PHYS_BASE
#define IX_OSSERV_QMGR_PHYS_BASE			IX_OSAL_IXP400_QMGR_PHYS_BASE
#define IX_OSSERV_OSTS_PHYS_BASE			IX_OSAL_IXP400_OSTS_PHYS_BASE
#define IX_OSSERV_USB_PHYS_BASE				IX_OSAL_IXP400_USB_PHYS_BASE
#define IX_OSSERV_EXP_BUS_PHYS_BASE			IX_OSAL_IXP400_EXP_BUS_PHYS_BASE
#define IX_OSSERV_EXP_BUS_BOOT_PHYS_BASE	IX_OSAL_IXP400_EXP_BUS_BOOT_PHYS_BASE
#define IX_OSSERV_EXP_BUS_CS0_PHYS_BASE		IX_OSAL_IXP400_EXP_BUS_CS0_PHYS_BASE
#define IX_OSSERV_EXP_BUS_CS1_PHYS_BASE		IX_OSAL_IXP400_EXP_BUS_CS1_PHYS_BASE
#define IX_OSSERV_EXP_BUS_CS4_PHYS_BASE		IX_OSAL_IXP400_EXP_BUS_CS4_PHYS_BASE
#define IX_OSSERV_EXP_BUS_REGS_PHYS_BASE	IX_OSAL_IXP400_EXP_BUS_REGS_PHYS_BASE

#define IX_OSSERV_MEM_MAP(physAddr, size)		IX_OSAL_MEM_MAP(physAddr, size)

#define IX_OSSERV_MEM_UNMAP(virtAddr) 			IX_OSAL_MEM_UNMAP(virtAddr)

#endif /* IX_OSAL_BACKWARD_MEM_MAP_H */
