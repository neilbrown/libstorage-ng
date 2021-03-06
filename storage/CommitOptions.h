/*
 * Copyright (c) 2017 SUSE LLC
 *
 * All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, contact SUSE LLC.
 *
 * To contact SUSE LLC about this file by physical or electronic mail, you may
 * find current contact information at www.suse.com.
 */

#ifndef STORAGE_COMMIT_OPTIONS_H
#define STORAGE_COMMIT_OPTIONS_H

namespace storage
{

    // TODO move root_prefix here? only needed in do_mount and do_unmount
    // TODO but now force_rw should also be used in immediate_activate

    class CommitOptions
    {
    public:

	CommitOptions(bool force_rw)
	    : force_rw(force_rw) {}

	const bool force_rw;

    };

}

#endif
