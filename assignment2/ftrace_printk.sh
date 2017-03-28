#!/bin/sh
DEBUGFS=`grep debugfs /proc/mounts | awk '{ print $2; }'`
sudo su -c "                                                        \
echo > $DEBUGFS/tracing/trace;                                      \
echo 0 > $DEBUGFS/tracing/tracing_on;                               \
echo SyS_printmsg > $DEBUGFS/tracing/set_ftrace_filter;             \
echo printk > $DEBUGFS/tracing/set_event;                           \
echo function > $DEBUGFS/tracing/current_tracer;                    \
echo 1 > $DEBUGFS/tracing/tracing_on;                               \
./a.out;                                                            \
echo 0 > $DEBUGFS/tracing/tracing_on"                                            
