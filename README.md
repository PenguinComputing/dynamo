# dynamo
Dynamo -- dynamic workload generator

This project aims to create a tool that can simulate the dynamic kinds of
workloads that we find weaknesses in power delivery to modern CPU and servers.

Programs like XHPL have proven to be an excellent way to generate the maximum
power draw of a CPU and server.  But the power profile of XHPL when running
over time is nearly flat.  It draws maximum power but doesn't vary.

In contrast, other workloads alternate quickly between low and high power
draws.  Sometimes as quickly as 1msec or less in each state with transitions
measured in micro-seconds.  With modern processors with turbo clock speeds and
enhanced idle for energy efficiency the swing from low to high power can be 10%
to 120% of TDP for such brief periods of time.  Responding to such large and
rapid changes in power can be a challenge for VRM's and switching power
supplies.  Average power and heat are not the problem as the average can be
30-50% of TDP.  It is the rapid changes and short durations.

This executable aims to produce these same rapid transitions between idle and
high power states.

As of version 0.1 only a single system is tested.  In a future version we aim
to be able to synchronize multiple nodes using MPI so that the aggregated load
of distributed MPI applications all switching between low and high power states
a the same microsecond can be simulated for testing.

## COMPILE AND RUN

```
make
dynamo >/dev/null
```

If you don't redirect the output, you'll get a bunch of details about each loop execution.

You may also want to try reducing the number of OpenMP threads to the actual number of physical cores

```
OMP_NUM_THREADS=40 dynamo >/dev/null
```
