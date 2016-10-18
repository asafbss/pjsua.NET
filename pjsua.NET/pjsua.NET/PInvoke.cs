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
    public static class PInvoke
    {
        private static IPInvoke _dll
        {
            get
            {
                IPInvoke _p;

                if (IntPtr.Size == 8)
                {
                    _p = PInvoke64.Instance;
                }
                else
                {
                    _p = PInvoke32.Instance;
                }

                return _p;
            }
        }

        private static pjsua.cb _cb = new pjsua.cb();

        public static pjsua.cb callbacks
        {
            get
            {
                return _cb;
            }
        }


        public static void Init(string[] args)
        {
            if (_dll.init(args) < 0)
            {
                var _ptr = _dll.getErrorDesc();
                throw new Pjsua_Exception(Marshal.PtrToStringAuto(_ptr));
            }
        }

        public static int Add_account(pjsua.Acc_config account)
        {
            IntPtr pCellTable = Marshal.AllocHGlobal(Marshal.SizeOf(account));
            int Account;

            try
            {
                Marshal.StructureToPtr(account, pCellTable, false);

                if ((Account = _dll.add_account(pCellTable)) < 0)
                {
                    var _ptr = _dll.getErrorDesc();
                    throw new Pjsua_Exception(Marshal.PtrToStringAuto(_ptr));
                }

            }
            finally
            {
                Marshal.FreeHGlobal(pCellTable);
            }

            return Account;
        }

        public static void Delete_account(int accountId)
        {
            if (_dll.delete_account(accountId) < 0)
            {
                var _ptr = _dll.getErrorDesc();
                throw new Pjsua_Exception(Marshal.PtrToStringAuto(_ptr));
            }
        }

        public static void Account_get_info(int account_id, ref pjsua.pjsua_acc_info inf)
        {

            int f = Marshal.SizeOf(inf);
            IntPtr pCellTable = Marshal.AllocHGlobal(Marshal.SizeOf(inf));

            try
            {
                Marshal.StructureToPtr(inf, pCellTable, false);

                if (_dll.account_get_info(account_id, pCellTable) < 0)
                {
                    var _ptr = _dll.getErrorDesc();
                    throw new Pjsua_Exception(Marshal.PtrToStringAuto(_ptr));
                }

                Marshal.PtrToStructure(pCellTable, inf);

            }
            finally
            {
                Marshal.FreeHGlobal(pCellTable);
            }
        }

        public static int Make_single_call(string dest)
        {
            int res;

            if ((res = _dll.make_single_call(dest)) < 0)
            {
                var _ptr = _dll.getErrorDesc();
                throw new Pjsua_Exception(Marshal.PtrToStringAuto(_ptr));
            }

            return res;
        }

        public static void Call_get_info(int call_id, ref pjsua.pjsua_call_info inf)
        {
            IntPtr pCellTable = Marshal.AllocHGlobal(Marshal.SizeOf(inf));

            try
            {
                Marshal.StructureToPtr(inf, pCellTable, false);

                if (_dll.call_get_info(call_id, pCellTable) < 0)
                {
                    var _ptr = _dll.getErrorDesc();
                    throw new Pjsua_Exception(Marshal.PtrToStringAuto(_ptr));
                }

                Marshal.PtrToStructure(pCellTable, inf);

            }
            finally
            {
                Marshal.FreeHGlobal(pCellTable);
            }
        }

        public static void Hangup_call(int call_id)
        {
            if (_dll.hangup_call(call_id) < 0)
            {
                var _ptr = _dll.getErrorDesc();
                throw new Pjsua_Exception(Marshal.PtrToStringAuto(_ptr));
            }
        }

        public static void Hold_call(int call_id)
        {
            if (_dll.hold_call(call_id) < 0)
            {
                var _ptr = _dll.getErrorDesc();
                throw new Pjsua_Exception(Marshal.PtrToStringAuto(_ptr));
            }
        }

        public static void Call_reinvite(int call_id)
        {
            if (_dll.call_reinvite(call_id) < 0)
            {
                var _ptr = _dll.getErrorDesc();
                throw new Pjsua_Exception(Marshal.PtrToStringAuto(_ptr));
            }
        }

        public static void Answer_call(int call_id, int st_code)
        {
            if (_dll.answer_call(call_id, st_code) < 0)
            {
                var _ptr = _dll.getErrorDesc();
                throw new Pjsua_Exception(Marshal.PtrToStringAuto(_ptr));
            }
        }

        public static void Transfer_call(int call_id, string dest)
        {
            if (_dll.transfer_call(call_id, dest) < 0)
            {
                var _ptr = _dll.getErrorDesc();
                throw new Pjsua_Exception(Marshal.PtrToStringAuto(_ptr));
            }
        }

        public static void Transfer_replace_call(int call_id, int dest_call_id)
        {
            if (_dll.transfer_replace_call(call_id, dest_call_id) < 0)
            {
                var _ptr = _dll.getErrorDesc();
                throw new Pjsua_Exception(Marshal.PtrToStringAuto(_ptr));
            }
        }

        public static void Dtmf(int call_id, string digits)
        {
            if (_dll.dtmf(call_id, digits) < 0)
            {
                var _ptr = _dll.getErrorDesc();
                throw new Pjsua_Exception(Marshal.PtrToStringAuto(_ptr));
            }
        }

        public static void ServiceReq(int callId, int serviceCode, string destUri)
        {
            if (_dll.serviceReq(callId, serviceCode, destUri) < 0)
            {
                var _ptr = _dll.getErrorDesc();
                throw new Pjsua_Exception(Marshal.PtrToStringAuto(_ptr));
            }
        }

        public static void Destroy()
        {
            if (_dll.destroy() < 0)
            {
                var _ptr = _dll.getErrorDesc();
                throw new Pjsua_Exception(Marshal.PtrToStringAuto(_ptr));
            }
        }

        public static void RegisterCallbacks()
        {
            IntPtr pCellTable = Marshal.AllocHGlobal(Marshal.SizeOf(_cb));

            try
            {
                Marshal.StructureToPtr(callbacks, pCellTable, false);

                _dll.registerCallbacks(pCellTable);

            }
            finally
            {
                Marshal.FreeHGlobal(pCellTable);
            }
        }

        private static void GetErrorDesc()
        {
            throw new NotImplementedException();
        }
    }
}

