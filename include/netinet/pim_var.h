/*
 * Copyright (c) 1998-2001
 * University of Southern California/Information Sciences Institute.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
/* $Id: pim_var.h,v 1.15 2003/05/21 10:24:39 pavlin Exp $ */

#ifndef _NETINET_PIM_VAR_H_
#define _NETINET_PIM_VAR_H_

/*
 * Protocol Independent Multicast (PIM),
 * kernel variables and implementation-specific definitions.
 *
 * Written by George Edmond Eddy (Rusty), ISI, February 1998.
 * Modified by Pavlin Radoslavov, USC/ISI, May 1998, Aug 1999, October 2000.
 * Modified by Hitoshi Asaeda, WIDE, August 1998.
 */

/*
 * PIM statistics kept in the kernel
 */
struct pimstat {
	u_quad_t pims_rcv_total_msgs;	   /* total PIM messages received    */
	u_quad_t pims_rcv_total_bytes;	   /* total PIM bytes received	     */
	u_quad_t pims_rcv_tooshort;	   /* rcvd with too few bytes	     */
	u_quad_t pims_rcv_badsum;	   /* rcvd with bad checksum	     */
	u_quad_t pims_rcv_badversion;	   /* rcvd bad PIM version	     */
	u_quad_t pims_rcv_registers_msgs;  /* rcvd regs. msgs (data only)    */
	u_quad_t pims_rcv_registers_bytes; /* rcvd regs. bytes (data only)   */
	u_quad_t pims_rcv_registers_wrongiif; /* rcvd regs. on wrong iif     */
	u_quad_t pims_rcv_badregisters;	   /* rcvd invalid registers	     */
	u_quad_t pims_snd_registers_msgs;  /* sent regs. msgs (data only)    */
	u_quad_t pims_snd_registers_bytes; /* sent regs. bytes (data only)   */
};

#ifndef __P
#ifdef __STDC__
#define __P(x)  x
#else
#define __P(x)  ()
#endif
#endif

#if (defined(KERNEL)) || (defined(_KERNEL))
extern struct pimstat pimstat;

#if defined(NetBSD) || defined(__OpenBSD__)
void pim_input __P((struct mbuf *, ...));
#elif (defined(__FreeBSD__) && (__FreeBSD_version >= 400000))
void pim_input __P((struct mbuf *, int, int));
#else
void pim_input __P((struct mbuf *, int));
#endif /* ! (NetBSD || OpenBSD) */
#endif /* KERNEL */

/*
 * Names for PIM sysctl objects
 */
#if (defined(__FreeBSD__)) || (defined(NetBSD)) || defined(__OpenBSD__) || (defined(__bsdi__))
#define PIMCTL_STATS		1	/* statistics (read-only) */
#define PIMCTL_MAXID		2

#define PIMCTL_NAMES {			\
	{ 0, 0 },			\
	{ "stats", CTLTYPE_STRUCT },	\
}
#endif /* FreeBSD || NetBSD || OpenBSD || bsdi */

#if (defined(__FreeBSD__) && (__FreeBSD_version >= 400000))
#if ((defined(KERNEL)) || (defined(_KERNEL)))
#if defined(SYSCTL_DECL)
SYSCTL_DECL(_net_inet_pim);
#endif /* SYSCTL_DECL	 */
#endif /* KERNEL	 */
#endif /* FreeBSD >= 4.0 */

#if (defined(NetBSD) || defined(__OpenBSD__) || defined(__bsdi__))
#define PIMCTL_VARS {			\
	0,				\
	0,				\
}
int	pim_sysctl __P((int *, u_int, void *, size_t *, void *, size_t));
#endif /* NetBSD || OpenBSD || bsdi */

#endif /* _NETINET_PIM_VAR_H_ */
