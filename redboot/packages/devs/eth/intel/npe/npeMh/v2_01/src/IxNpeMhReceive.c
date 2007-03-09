/**
 * @file IxNpeMhReceive.c
 *
 * @author Intel Corporation
 * @date 18 Jan 2002
 *
 * @brief This file contains the implementation of the private API for the
 * Receive module.
 *
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

/*
 * Put the system defined include files required.
 */


/*
 * Put the user defined include files required.
 */
#include "IxOsal.h"
#include "IxNpeMhMacros_p.h"
#include "IxNpeMhConfig_p.h"
#include "IxNpeMhReceive_p.h"
#include "IxNpeMhSolicitedCbMgr_p.h"
#include "IxNpeMhUnsolicitedCbMgr_p.h"

/*
 * #defines and macros used in this file.
 */

/*
 * Typedefs whose scope is limited to this file.
 */

/**
 * @struct IxNpeMhReceiveStats
 *
 * @brief This structure is used to maintain statistics for the Receive
 * module.
 */

typedef struct
{
    UINT32 isrs;        /**< receive ISR invocations */
    UINT32 receives;    /**< receive messages invocations */
    UINT32 messages;    /**< messages received */
    UINT32 solicited;   /**< solicited messages received */
    UINT32 unsolicited; /**< unsolicited messages received */
    UINT32 callbacks;   /**< callbacks invoked */
} IxNpeMhReceiveStats;

/*
 * Variable declarations global to this file only.  Externs are followed by
 * static variables.
 */

PRIVATE IxNpeMhReceiveStats ixNpeMhReceiveStats[IX_NPEMH_NUM_NPES];

/*
 * Extern function prototypes.
 */

/*
 * Static function prototypes.
 */
PRIVATE
void ixNpeMhReceiveIsr (int npeId);

PRIVATE
void ixNpeMhReceiveIsr (int npeId)
{
    int lockKey;

    IX_NPEMH_TRACE0 (IX_NPEMH_FN_ENTRY_EXIT, "Entering "
                     "ixNpeMhReceiveIsr\n");

    lockKey = ixOsalIrqLock ();

    /* invoke the message receive routine to get messages from the NPE */
    ixNpeMhReceiveMessagesReceive (npeId);

    /* update statistical info */
    ixNpeMhReceiveStats[npeId].isrs++;

    ixOsalIrqUnlock (lockKey);

    IX_NPEMH_TRACE0 (IX_NPEMH_FN_ENTRY_EXIT, "Exiting "
                     "ixNpeMhReceiveIsr\n");
}

/*
 * Function definition: ixNpeMhReceiveInitialize
 */

void ixNpeMhReceiveInitialize (void)
{
    IxNpeMhNpeId npeId = 0;

    IX_NPEMH_TRACE0 (IX_NPEMH_FN_ENTRY_EXIT, "Entering "
                     "ixNpeMhReceiveInitialize\n");

    /* for each NPE ... */
    for (npeId = 0; npeId < IX_NPEMH_NUM_NPES; npeId++)
    {
        /* register our internal ISR for the NPE to handle "outFIFO not */
        /* empty" interrupts */
        ixNpeMhConfigIsrRegister (npeId, ixNpeMhReceiveIsr);
    }

    IX_NPEMH_TRACE0 (IX_NPEMH_FN_ENTRY_EXIT, "Exiting "
                     "ixNpeMhReceiveInitialize\n");
}

/*
 * Function definition: ixNpeMhReceiveMessagesReceive
 */

IX_STATUS ixNpeMhReceiveMessagesReceive (
    IxNpeMhNpeId npeId)
{
    IxNpeMhMessage message = { { 0, 0 } };
    IxNpeMhMessageId messageId = 0;
    IxNpeMhCallback callback = NULL;
    IX_STATUS status;

    IX_NPEMH_TRACE0 (IX_NPEMH_FN_ENTRY_EXIT, "Entering "
                     "ixNpeMhReceiveMessagesReceive\n");

    /* update statistical info */
    ixNpeMhReceiveStats[npeId].receives++;

    /* while the NPE has messages in its outFIFO */
    while (!ixNpeMhConfigOutFifoIsEmpty (npeId))
    {
        /* read a message from the NPE's outFIFO */
        status = ixNpeMhConfigOutFifoRead (npeId, &message);

        if (IX_SUCCESS != status)
        {
            return status;
        }
        
        /* get the ID of the message */
        messageId = ixNpeMhConfigMessageIdGet (message);

	    IX_NPEMH_TRACE2 (IX_NPEMH_DEBUG,
			 "Received message from NPE %d with ID 0x%02X\n",
			 npeId, messageId);

        /* update statistical info */
        ixNpeMhReceiveStats[npeId].messages++;

        /* try to find a matching unsolicited callback for this message. */

        /* we assume the message is unsolicited.  only if there is no */
        /* unsolicited callback for this message type do we assume the */
        /* message is solicited.  it is much faster to check for an */
        /* unsolicited callback, so doing this check first should result */
        /* in better performance. */

        ixNpeMhUnsolicitedCbMgrCallbackRetrieve (
            npeId, messageId, &callback);

        if (callback != NULL)
        {
	    IX_NPEMH_TRACE0 (IX_NPEMH_DEBUG,
			     "Found matching unsolicited callback\n");

            /* update statistical info */
            ixNpeMhReceiveStats[npeId].unsolicited++;
        }

        /* if no unsolicited callback was found try to find a matching */
        /* solicited callback for this message */
        if (callback == NULL)
        {
            ixNpeMhSolicitedCbMgrCallbackRetrieve (
                npeId, messageId, &callback);

            if (callback != NULL)
            {
		IX_NPEMH_TRACE0 (IX_NPEMH_DEBUG,
				 "Found matching solicited callback\n");

                /* update statistical info */
                ixNpeMhReceiveStats[npeId].solicited++;
            }
        }

        /* if a callback (either unsolicited or solicited) was found */
        if (callback != NULL)
        {
            /* invoke the callback to pass the message back to the client */
            callback (npeId, message);

            /* update statistical info */
            ixNpeMhReceiveStats[npeId].callbacks++;
        }
        else /* no callback (neither unsolicited nor solicited) was found */
        {
	    IX_NPEMH_TRACE2 (IX_NPEMH_WARNING,
			     "No matching callback for NPE %d"
			     " and ID 0x%02X, discarding message\n",
			     npeId, messageId);

            /* the message will be discarded.  this is normal behaviour */
            /* if the client passes a NULL solicited callback when */
            /* sending a message.  this indicates that the client is not */
            /* interested in receiving the response.  alternatively a */
            /* NULL callback here may signify an unsolicited message */
            /* with no appropriate registered callback. */
        }
    }

    IX_NPEMH_TRACE0 (IX_NPEMH_FN_ENTRY_EXIT, "Exiting "
                     "ixNpeMhReceiveMessagesReceive\n");
    
    return IX_SUCCESS;
}

/*
 * Function definition: ixNpeMhReceiveShow
 */

void ixNpeMhReceiveShow (
    IxNpeMhNpeId npeId)
{
    /* show the ISR invocation counter */
    IX_NPEMH_SHOW ("Receive ISR invocations",
                   ixNpeMhReceiveStats[npeId].isrs);

    /* show the receive message invocation counter */
    IX_NPEMH_SHOW ("Receive messages invocations",
                   ixNpeMhReceiveStats[npeId].receives);

    /* show the message received counter */
    IX_NPEMH_SHOW ("Messages received",
                   ixNpeMhReceiveStats[npeId].messages);

    /* show the solicited message counter */
    IX_NPEMH_SHOW ("Solicited messages received",
                   ixNpeMhReceiveStats[npeId].solicited);

    /* show the unsolicited message counter */
    IX_NPEMH_SHOW ("Unsolicited messages received",
                   ixNpeMhReceiveStats[npeId].unsolicited);

    /* show the callback invoked counter */
    IX_NPEMH_SHOW ("Callbacks invoked",
                   ixNpeMhReceiveStats[npeId].callbacks);

    /* show the message discarded counter */
    IX_NPEMH_SHOW ("Received messages discarded",
                   (ixNpeMhReceiveStats[npeId].messages -
                    ixNpeMhReceiveStats[npeId].callbacks));
}

/*
 * Function definition: ixNpeMhReceiveShowReset
 */

void ixNpeMhReceiveShowReset (
    IxNpeMhNpeId npeId)
{
    /* reset the ISR invocation counter */
    ixNpeMhReceiveStats[npeId].isrs = 0;

    /* reset the receive message invocation counter */
    ixNpeMhReceiveStats[npeId].receives = 0;

    /* reset the message received counter */
    ixNpeMhReceiveStats[npeId].messages = 0;

    /* reset the solicited message counter */
    ixNpeMhReceiveStats[npeId].solicited = 0;

    /* reset the unsolicited message counter */
    ixNpeMhReceiveStats[npeId].unsolicited = 0;

    /* reset the callback invoked counter */
    ixNpeMhReceiveStats[npeId].callbacks = 0;
}
