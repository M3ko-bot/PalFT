/* 
 * Copyright (c) 2024, The PerformanC Organization
 * License available at LICENSE file (BSD 2-Clause)
*/

/* Lower layers (may) have a lower limit. This is the maximum limit for the TCP packet size. */

#ifndef TCPLIMITS_H
#define TCPLIMITS_H

#ifdef TCPLIMITS_EXPERIMENTAL_SAVE_MEMORY
/* Ethernet limit -- Lower level */
#define TCPLIMITS_PACKET_SIZE 1500
#else
#define TCPLIMITS_PACKET_SIZE 65536
#endif

#endif