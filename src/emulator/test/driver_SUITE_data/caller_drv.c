/* ``The contents of this file are subject to the Erlang Public License,
 * Version 1.1, (the "License"); you may not use this file except in
 * compliance with the License. You should have received a copy of the
 * Erlang Public License along with this software. If not, it can be
 * retrieved via the world wide web at http://www.erlang.org/.
 * 
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and limitations
 * under the License.
 * 
 * The Initial Developer of the Original Code is Ericsson Utvecklings AB.
 * Portions created by Ericsson are Copyright 1999, Ericsson Utvecklings
 * AB. All Rights Reserved.''
 * 
 *     $Id$
 */

#include <stdlib.h>
#include <string.h>
#include "erl_driver.h"

static ErlDrvData start(ErlDrvPort port,
			char *command);
static void output(ErlDrvData drv_data,
		   char *buf, int len);
static void outputv(ErlDrvData drv_data,
		    ErlIOVec *ev);
static int control(ErlDrvData drv_data,
		   unsigned int command, char *buf, 
		   int len, char **rbuf, int rlen);
static int call(ErlDrvData drv_data,
		unsigned int command,
		char *buf, int len,
		char **rbuf, int rlen,
		unsigned int *flags);

static ErlDrvEntry caller_drv_entry = { 
    NULL /* init */,
    start,
    NULL /* stop */,
    output,
    NULL /* ready_input */,
    NULL /* ready_output */,
    "caller_drv",
    NULL /* finish */,
    NULL /* handle */,
    control,
    NULL /* timeout */,
    outputv,
    NULL /* ready_async */,
    NULL /* flush */,
    call,
    NULL /* event */,
    ERL_DRV_EXTENDED_MARKER,
    ERL_DRV_EXTENDED_MAJOR_VERSION,
    ERL_DRV_EXTENDED_MINOR_VERSION,
    ERL_DRV_FLAG_USE_PORT_LOCKING,
    NULL /* handle2 */,
    NULL /* handle_monitor */
};

DRIVER_INIT(caller_drv)
{
    char buf[10];
    size_t bufsz = sizeof(buf);
    char *use_outputv;
    use_outputv = (erl_drv_getenv("CALLER_DRV_USE_OUTPUTV", buf, &bufsz) == 0
		   ? buf
		   : "false");
    if (strcmp(use_outputv, "true") != 0)
	caller_drv_entry.outputv = NULL;
    return &caller_drv_entry;
}

void
send_caller(ErlDrvData drv_data, char *func)
{
    int res;
    ErlDrvPort port = (ErlDrvPort) drv_data;
    ErlDrvTermData msg[] = {
	ERL_DRV_ATOM,	driver_mk_atom("caller"),
	ERL_DRV_PORT,	driver_mk_port(port),
	ERL_DRV_ATOM,	driver_mk_atom(func),
	ERL_DRV_PID,	driver_caller(port),
	ERL_DRV_TUPLE,	(ErlDrvTermData) 4
    };
    res = driver_output_term(port, msg, sizeof(msg)/sizeof(ErlDrvTermData));
    if (res <= 0)
	driver_failure_atom(port, "driver_output_term failed");
}

static ErlDrvData
start(ErlDrvPort port, char *command)
{
    send_caller((ErlDrvData) port, "start");
    return (ErlDrvData) port;
}

static void
output(ErlDrvData drv_data, char *buf, int len)
{
    send_caller(drv_data, "output");
}

static void
outputv(ErlDrvData drv_data, ErlIOVec *ev)
{
    send_caller(drv_data, "outputv");
}

static int
control(ErlDrvData drv_data,
	unsigned int command, char *buf, 
	int len, char **rbuf, int rlen)
{
    send_caller(drv_data, "control");
    return 0;
}

static int
call(ErlDrvData drv_data,
     unsigned int command,
     char *buf, int len,
     char **rbuf, int rlen,
     unsigned int *flags)
{
    /* echo call */
    if (len > rlen)
	*rbuf = driver_alloc(len);
    memcpy((void *) *rbuf, (void *) buf, len);
    send_caller(drv_data, "call");
    return len;
}
