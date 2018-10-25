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

As of Aug 14, 2018 Dynamo can be compiled with MPI support (see the Makefile) so
that multiple node workers can be syncronized to increase the effect on the power
infrastructure. 

## COMPILE AND RUN

```
make
dynamo
```

If you don't redirect the output, you'll get a bunch of details about each loop execution.

You may also want to try reducing the number of OpenMP threads to the actual number of physical cores

```
OMP_NUM_THREADS=40 dynamo
```

## Complete Command Line Options

```
usage: ./dynamo -u -h -d -A init -T task -R report -W wait -l loops -i idle -b busy
where:
   -u|-h  prints this usage message
   -d  increments the debug level
   init  is the value passed to the Init funciton (array length)
   task  is the name of the Task function (not implemented)
   report  is the name of the Report function (Drop, Print)
   wait  is the name of the Wait function (No, Static, MPI)
   loops  is the number of iterations of idle/busy times
   idle  is the idle time in seconds
   busy  is the busy time in seconds
```
