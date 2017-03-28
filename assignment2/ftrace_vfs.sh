#!/bin/sh
DEBUGFS=`grep debugfs /proc/mounts | awk '{ print $2; }'`
sudo su -c "                                                        \
echo > $DEBUGFS/tracing/trace;                                      \
echo 0 > $DEBUGFS/tracing/tracing_on;                               \
echo vfs_write vfs_open vfs_read > $DEBUGFS/tracing/set_graph_function;             \
echo 10 > $DEBUGFS/tracing/max_graph_depth;             \
echo function_graph > $DEBUGFS/tracing/current_tracer;              \
echo 1 > $DEBUGFS/tracing/tracing_on;"
sleep 0.01
echo 0 > $DEBUGFS/tracing/tracing_on                                           
