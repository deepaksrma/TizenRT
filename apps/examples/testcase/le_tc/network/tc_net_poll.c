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

/*  @file tc_net_poll.c
 * @brief Test Case Example for lwip_poll() API
 */

#include <tinyara/config.h>
#include <stdio.h>
#include <poll.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netutils/netlib.h>
#include <lwip/sockets.h>
#include <tinyara/os_api_test_drv.h>
#include "tc_internal.h"

struct poll_s {
	int fd;
	int sd;
};

/**
* @testcase             :tc_net_poll_p
* @brief                :check for poll in multi-thread env
* @scenario             :
* @apicovered           :lwip_poll()
* @precondition         :
* @postcondition        :
*/

static void tc_net_multi_thread_poll_p(int ret, int sd)
{
	TC_ASSERT_GEQ_CLEANUP("lwip_poll", ret, 0, close(sd));
	TC_SUCCESS_RESULT();
}

void *wait_event(void *arg)
{
	struct pollfd fds[1];
	struct poll_test_args args;
	struct poll_s *poll = (struct poll_s *)arg;
	int sd = poll->sd;
	int fd = poll->fd;
	int ret = -1;

	fds[0].fd = sd;
	fds[0].events = POLLIN;

	args.fd = sd;
	args.fds = fds;
	args.setup = 1;

	ret = ioctl(fd, TESTIOC_NET_POLL, &args);
	tc_net_multi_thread_poll_p(ret, sd);

	return NULL;
}

void *multi_thread_poll(void *arg)
{
	int sd = -1;
	int fd = *(int *)arg;
	pthread_t thread1;
	struct poll_s args;

	sd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sd < 0) {
		printf("socket fail %s:%d", __FUNCTION__, __LINE__);
		return NULL;
	}

	args.fd = fd;
	args.sd = sd;
	pthread_create(&thread1, NULL, wait_event, (void *)&args);

	close(sd);

	return NULL;
}

/**
* @testcase             :tc_net_poll_p
* @brief                :check for poll
* @scenario             :
* @apicovered           :lwip_poll()
* @precondition         :
* @postcondition        :
*/

static void tc_net_poll_p(int fd)
{
	int ret = -1;
	int sd = -1;
	struct pollfd fds[1];
	struct poll_test_args args;

	sd = socket(AF_INET, SOCK_DGRAM, 0);

	fds[0].fd = sd;
	fds[0].events = POLLIN;

	args.fd = sd;
	args.fds = fds;
	args.setup = 1;

	ret = ioctl(fd, TESTIOC_NET_POLL, &args);
	TC_ASSERT_GEQ_CLEANUP("lwip_poll", ret, 0, close(sd));

	args.setup = 0;
	ret = ioctl(fd, TESTIOC_NET_POLL, &args);
	TC_ASSERT_GEQ_CLEANUP("lwip_poll", ret, 0, close(sd));

	close(sd);
	TC_SUCCESS_RESULT();
}

/**
* @testcase             :tc_net_poll_fds_n
* @brief                :test with wrong values of fds.
* @scenario             :
* @apicovered           :lwip_poll()
* @precondition         :
* @postcondition        :
*/

static void tc_net_poll_fds_n(int fd)
{
	int ret = -1;
	int sd = -1;
	struct poll_test_args args;

	sd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sd < 0) {
		printf("socket fail %s:%d", __FUNCTION__, __LINE__);
		return;
	}

	args.fd = sd;
	args.fds = NULL;
	args.setup = 1;

	ret = ioctl(fd, TESTIOC_NET_POLL, &args);
	TC_ASSERT_LEQ_CLEANUP("lwip_poll", ret, -1, close(sd));
	TC_SUCCESS_RESULT();
	close(sd);
}

/**
* @testcase             :tc_net_poll_events_n
* @brief                :test with wrong value of events
* @scenario             :
* @apicovered           :lwip_poll()
* @precondition         :
* @postcondition        :
*/

static void tc_net_poll_events_n(int fd)
{
	int ret = -1;
	int sd = -1;
	struct pollfd fds[1];
	struct poll_test_args args;

	sd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sd < 0) {
		printf("socket fail %s:%d", __FUNCTION__, __LINE__);
		return;
	}

	fds[0].fd = sd;
	args.fds = 0;

	args.fd = sd;
	args.fds = fds;
	args.setup = 1;

	ret = ioctl(fd, TESTIOC_NET_POLL, &args);
	TC_ASSERT_GEQ_CLEANUP("lwip_poll", ret, 0, close(sd));
	TC_SUCCESS_RESULT();
	close(sd);
}

/**
* @testcase             :tc_net_poll_sd_n
* @brief                :test with wrong value of socket descriptor
* @scenario             :
* @apicovered           :lwip_poll()
* @precondition         :
* @postcondition        :
*/

static void tc_net_poll_sd_n(int fd)
{
	int ret = -1;
	int sd = -1;
	struct pollfd fds[1];
	struct poll_test_args args;

	fds[0].fd = sd;
	fds[0].events = POLLIN;

	args.fd = -1;
	args.fds = fds;
	args.setup = 0;

	ret = ioctl(fd, TESTIOC_NET_POLL, &args);
	TC_ASSERT_LEQ("lwip_poll", ret, -1);
	TC_SUCCESS_RESULT();
}

int net_poll_main(void)
{
	int fd;
	fd = tc_get_fd();

	pthread_t thread_p;
	pthread_create(&thread_p, NULL, multi_thread_poll, (void *)&fd);
	pthread_join(thread_p, NULL);

	tc_net_poll_p(fd);
	tc_net_poll_fds_n(fd);
	tc_net_poll_events_n(fd);
	tc_net_poll_sd_n(fd);

	return 0;
}
