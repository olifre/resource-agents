/******************************************************************************
*******************************************************************************
**
**  Copyright (C) Sistina Software, Inc.  1997-2003  All rights reserved.
**  Copyright (C) 2004 Red Hat, Inc.  All rights reserved.
**
**  This copyrighted material is made available to anyone wishing to use,
**  modify, copy, or redistribute it subject to the terms and conditions
**  of the GNU General Public License v.2.
**
*******************************************************************************
******************************************************************************/

#include "gulm.h"

#include <linux/init.h>

#include "gulm_lock_queue.h"

MODULE_DESCRIPTION ("Grand Unified Locking Module " GULM_RELEASE_NAME);
MODULE_AUTHOR ("Red Hat, Inc.");
MODULE_LICENSE ("GPL");

gulm_cm_t gulm_cm;

struct lm_lockops gulm_ops = {
      lm_proto_name:PROTO_NAME,
      lm_mount:gulm_mount,
      lm_others_may_mount:gulm_others_may_mount,
      lm_unmount:gulm_unmount,
      lm_withdraw:gulm_withdraw,
      lm_get_lock:gulm_get_lock,
      lm_put_lock:gulm_put_lock,
      lm_lock:gulm_lock,
      lm_unlock:gulm_unlock,
      lm_cancel:gulm_cancel,
      lm_hold_lvb:gulm_hold_lvb,
      lm_unhold_lvb:gulm_unhold_lvb,
      lm_sync_lvb:gulm_sync_lvb,
      lm_plock_get:gulm_plock_get,
      lm_plock:gulm_plock,
      lm_punlock:gulm_punlock,
      lm_recovery_done:gulm_recovery_done,
      lm_owner:THIS_MODULE,
};

/**
 * init_gulm - Initialize the gulm module
 *
 * Returns: 0 on success, -EXXX on failure
 */
int __init
init_gulm (void)
{
	int error;

	memset (&gulm_cm, 0, sizeof (gulm_cm_t));
	gulm_cm.hookup = NULL;

	/* register with the lm layers. */
	error = gfs_register_lockproto (&gulm_ops);
	if (error)
		goto fail;

	error = glq_init();
	if (error != 0 )
		goto lm_fail;

	error = gulm_lt_init();
	if (error != 0)
		goto glq_fail;

	init_gulm_fs ();
	sig_watcher_init ();

	printk ("Gulm %s (built %s %s) installed\n",
		GULM_RELEASE_NAME, __DATE__, __TIME__);

	return 0;

glq_fail:
	glq_release();

   lm_fail:
	gfs_unregister_lockproto (&gulm_ops);

      fail:
	return error;
}

/**
 * exit_gulm - cleanup the gulm module
 *
 */

void __exit
exit_gulm (void)
{
	gulm_lt_release();
	glq_release();
	gfs_unregister_lockproto (&gulm_ops);
}

module_init (init_gulm);
module_exit (exit_gulm);

/* vim: set ai cin noet sw=8 ts=8 : */
