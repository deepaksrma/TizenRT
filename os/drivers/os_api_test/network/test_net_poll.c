/****************************************************************************
 *
 * Copyright 2021 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <debug.h>
#include <poll.h>
#include <sys/types.h>
#include <lwip/sockets.h>
#include <tinyara/os_api_test_drv.h>

typedef struct poll_test_args poll_test_args_t;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static int test_poll(unsigned long arg)
{
	int ret = -1;
	poll_test_args_t *args = NULL;
	args = (poll_test_args_t *)arg;
	if (args == NULL) {
		dbg("args is NULL\n");
		return -1;
	}

	ret = lwip_poll(args->fd, args->fds, args->setup);
	return ret;
}

int test_net_poll(int cmd, unsigned long arg)
{
	int ret = -1;

	switch (cmd) {
	case TESTIOC_NET_POLL:
		ret = test_poll(arg);
		break;
	}
	return ret;
}
