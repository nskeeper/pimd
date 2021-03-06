README
======
[![Build Status](https://travis-ci.org/troglobit/pimd.png?branch=master)](https://travis-ci.org/troglobit/pimd)[![Coverity Scan Status](https://scan.coverity.com/projects/3319/badge.svg)](https://scan.coverity.com/projects/3319)

pimd is a lightweight, stand-alone implementation of RFC 2362, available
under the 3-clause BSD license.  This is the restored original version
from University of Southern California, by Ahmed Helmy, Rusty Eddy and
Pavlin Ivanov Radoslavov.

pimd is maintained at GitHub.  Use its facilities to access the source,
report bugs, feature requests, send patches and for GIT pull requests:

  http://github.com/troglobit/pimd

pimd also has a homepage, mainly to distribute releases:

  http://troglobit.com/pimd.html

pimd is primarily developed on Linux and should work as-is out of the
box on all major distributions.  Other UNIX variants should also work,
but are not as thoroughly tested.  For some tips and details, see the
`configure` script.


Building
--------

When building pimd from source you first need to run the `configure`
script to generate the file `config.mk`.  The script relies on Bourne
shell standard features as well as expr and uname.  Any optional pimd
features, such as `--enable-scoped-acls` are activated here as well.

**Example:**

    ./configure --enable-scoped-acls
    make

    sudo make install

The Makefile supports de facto standard environment variables such as
`prefix` and `DESTDIR` for the install process.  E.g., to install pimd
to `/usr` instead of the default `/usr/local`, but redirect to a binary
package directory in `/tmp`:

    VERSION=2.2.0-1 prefix=/usr DESTDIR=/tmp/pimd-2.2.0-1 make clean install


Configuration
-------------

The configuration is kept in the file `/etc/pimd.conf`, the order of
the statements are in some cases important.

PIM-SM is a designed to be a _protocol independent_ multicast routing
protocol.  As such it relies on protocols like, e.g, OSPF, RIP, or
static routing entries, to figure out the path to all multicast capable
neighboring routers.  This information is necessary in setups with more
than one route between a multicast sender and a receiver to figure out
which PIM router should be the active forwarder.

However, pimd currently cannot retrieve the unicast routing distance
(preference) and metric of routes from the system, not from the kernel
nor a route manager like zebra.  Hence, pimd currently needs to be setup
statically on each router using the desired distance and metric for each
active interface.  If either the distance and/or the metric is missing
in an interface configuration, the following two defaults will be used:

    default_source_preference <1-255>     default: 101   (distance)
    default_source_metric     <1-1024>    default: 1024

By default pimd starts up on all interfaces it can find, using the above
defaults.  To configure individual interfaces use:

    phyint <address | ifname> ...

You can reference the interface via either its local IPv4 address or
its name, e.g., eth0.  Some common interface settings are:

   * `disable`: Disable pimd on this interface, i.e., do not send or
     listen for PIM-SM traffic

   * `preference <1-255>`: The interface's distance value (also
     confusingly referred to as metric preference) in PIM Assert
     messages.  Used with `metric` to elect the active multicast
     forwarding router.  Defaults to `default_source_preference`

   * `metric <1-1024>`: The cost for traversing this router.  Used with
     the `preference` value above. Defaults to `default_source_metric`

More interface settings are available, see the pimd(8) manual page for
the full details.

The most notable feature of PIM-SM is that multicast is distributed from
so called Rendezvous Points (RP).  Each RP handles distribution of one
or more multicast groups, pimd can be configured to advertise itself as
a candidate RP `cand_rp`, and request to be static RP `rp_address` for
one or more multicast groups.

    rp_address <address> [<group>[/<LENGTH> | masklen <LENGTH]

The `rp_address` setting is the same as the Cisco `ip pim rp-address`
setting to configure static Rendezvous Points.  The first argument can
be an IPv4 address or a multicast group address.  The default group and
prefix length is 224.0.0.0/16.  Static RP's always have priority 1.

    cand_rp [address | ifname] [time <10-16383>] [priority <0-255>]

The Rendezvous Point candidate, or CRP, setting is the same as the Cisco
`ip pim rp-candidate` setting.  Use it to control which interface that
should be used in RP elections.

   * `address | ifname`: Optional local IPv4 address, or interface name
     to acquire address from.  The default is to use the highest active
     IP address.

   * `time <10-16383>`: The interval, in seconds, between advertising
     this CRP. Default: 60 seconds

   * `priority <0-255>`: How important this CRP is compared to others.
     The lower the value here, the more important the CRP.  Like Cisco,
     pimd defaults to priority 0 when this is left out

In the CRP messages sent out by pimd, one or more multicast groups can
be advertised using the following syntax.

    group_prefix <group>[</LENGTH> | masklen <LENGTH>]

Each `group_prefix` setting defines one multicast group and an optional
mask length, which defaults to 16 if left out.  A maximum of 255
multicast group prefix records is possible for the CRP.

To keep track of all Rendezvous Points in a PIM-SM domain there exists a
feature called *Bootstrap Router*.  The elected BSR in a PIM-SM domain
periodically announces the RP set in Bootstrap messages.  For details on
PIM BSR operation, see [RFC 5059](http://tools.ietf.org/search/rfc5059).

    cand_bootstrap_router [address | ifname] [priority <0-255>]

The configuration of a Candidate BootStrap Router (CBSR) is very similar
to that of CRP, except for the interval time.  If either the address or
the interface name is left out pimd uses the highest active IP address.
If the priority is left out, pimd (like Cisco) defaults to priority 0.

In a PIM-SM domain there can be two, or more, paths from a designated
router (DR) for a multicast sender to reach a receiver.  When receivers
begin joining multicast groups all data is received via the *shared
tree* (RPT) from each Rendezvous Point (RP).  This is often not an
optimal route, so when the volume starts exceeding a configurable
threshold, on either the last-hop router or the RP itself, the router
will attempt to switch to the *shortest path tree* (SPT) from the
multicast source to the receiver.

In versions of pimd prior to 2.2.0 this threshold was confusingly split
in two different settings, one for the DR and one for the RP.  These
settings are still supported, for compatibility reasons and documented
in the man-page, but it is strongly recommended to change to the new
syntax instead:

    spt_threshold [rate <KBPS> | packets <NUM> | infinity] [interval <5-60>]

Only slightly different from the Cisco `ip pim spt-threshold` setting,
pimd can trigger a switch to SPT on a rate or number of packets and you
can also tweak the poll interval.  It's recommended to keep the interval
in the tens of seconds, the default is 100 sec.  The default threshold
is set to zero packets, which will cause a switch over to the SPT after
the first multicast packet is received.


Example
-------

    # This pimd.conf example assumes a router with four interfaces.
    # Interface eth0 is disabled, i.e., pimd will not run there.
    # The default interface preference 101 has been changed on all
    # the other interfaces.
    phyint eth0 disable
    phyint eth1 preference 255
    phyint eth2 preference 250
    phyint eth3 preference 251
    
    # Offer to be an RP for all of 224.0.0.0/4
    cand_rp eth1
    group_prefix 224.0.0.0 masklen 4
    
    # Partake in BSR elections as well
    cand_bootstrap_router eth1
    
    # This is the built-in defaults, switch to SPT on first packet
    spt_threshold packets 0 interval 100


Starting
--------

Having set up the configuration file, you are ready to run pimd.  As
usual, it is recommended that you start it manually first, to make sure
everything works as expected, before adding it to your system's startup
scripts,  with any startup flags it might need.

    pimd [-c file] [-d[level1,...,levelN]]

   * `-c file`: Utilize the specified configuration file rather than the
      default, `/etc/pimd.conf`

   * `-d[level1,...,levelN]`: Specifies the debug level(s) to utilize
      when running the daemon.  Type `pimd -h` for a full list of levels

**Example:**

    pimd -c /cfg/pimd.conf -digmp_proto,pim_jp,kernel,pim_register

Notice the lack of spaces in the option argument to `-d`, the
long-option `--debug=igmp_proto,pim_jp,kernel,pim_register`is slightly
more readable.


Monitoring
----------

To see the virtual interface table, including neighboring PIM routers,
and the multicast routing table:

    pimd -r

or to watch it continually:

    watch pimd -r

In addition, pimd logs important events to the system logfile, in
particular at startup when parsing the `pimd.conf` configuration file.

