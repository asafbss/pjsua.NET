/*
* Copyright (C) 2016 Assaf Albo <asafbss@gmail.com>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;


namespace pjsua.NET
{
    internal abstract class IPInvoke
    {
        public abstract int init(string[] args);
        public abstract int add_account(IntPtr account);
        public abstract int delete_account(int accId);
        public abstract int account_get_info(int accId, IntPtr info);
        public abstract int make_single_call(string dest);
        public abstract int call_get_info(int call_id, IntPtr info);
        public abstract int hangup_call(int call_id);
        public abstract int hold_call(int call_id);
        public abstract int call_reinvite(int call_id);
        public abstract int answer_call(int call_id, int st_code);
        public abstract int transfer_call(int call_id, string dest);
        public abstract int transfer_replace_call(int call_id, int dest_call_id);
        public abstract int dtmf(int call_id, string digits);
        public abstract int serviceReq(int callId, int serviceCode, string destUri);
        public abstract int destroy();
        public abstract void registerCallbacks(IntPtr callbacks);
        public abstract IntPtr getErrorDesc();
    }
}
