using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace pjsua.NET
{
    internal class PInvoke32 : IPInvoke
    {
#if DEBUG

        private const string _dll32bit = @"DLLS\pjsua_API--Win32-vc-Debug.dll";


#else
        private const string _dll32bit = @"DLLS\pjsua_API--Win32-vc-Release.dll";

#endif

        [DllImport(_dll32bit, EntryPoint = "API_init", CallingConvention = CallingConvention.Cdecl)]
        protected static extern int API_init(int argc,
            [MarshalAsAttribute(UnmanagedType.LPArray, ArraySubType = UnmanagedType.LPStr)]string[] argv);

        [DllImport(_dll32bit, EntryPoint = "API_add_account", CallingConvention = CallingConvention.Cdecl)]
        protected static extern int API_add_account(IntPtr account);

        [DllImport(_dll32bit, EntryPoint = "API_del_account", CallingConvention = CallingConvention.Cdecl)]
        protected static extern int API_del_account(int accId);

        [DllImport(_dll32bit, EntryPoint = "API_account_get_info", CallingConvention = CallingConvention.Cdecl)]
        protected static extern int API_account_get_info(int accId, IntPtr info);

        [DllImport(_dll32bit, EntryPoint = "API_make_single_call", CallingConvention = CallingConvention.Cdecl)]
        protected static extern int API_make_single_call([MarshalAsAttribute(UnmanagedType.LPStr)]string dest);

        [DllImport(_dll32bit, EntryPoint = "API_call_get_info", CallingConvention = CallingConvention.Cdecl)]
        protected static extern int API_call_get_info(int call_id, IntPtr info);

        [DllImport(_dll32bit, EntryPoint = "API_hangup_call", CallingConvention = CallingConvention.Cdecl)]
        protected static extern int API_hangup_call(int call_id);

        [DllImport(_dll32bit, EntryPoint = "API_hold_call", CallingConvention = CallingConvention.Cdecl)]
        protected static extern int API_hold_call(int call_id);

        [DllImport(_dll32bit, EntryPoint = "API_call_reinvite", CallingConvention = CallingConvention.Cdecl)]
        protected static extern int API_call_reinvite(int call_id);

        [DllImport(_dll32bit, EntryPoint = "API_answer_call", CallingConvention = CallingConvention.Cdecl)]
        protected static extern int API_answer_call(int call_id, int st_code);

        [DllImport(_dll32bit, EntryPoint = "API_transfer_call", CallingConvention = CallingConvention.Cdecl)]
        protected static extern int API_transfer_call(int call_id, [MarshalAsAttribute(UnmanagedType.LPStr)]string dest);

        [DllImport(_dll32bit, EntryPoint = "API_transfer_replace_call", CallingConvention = CallingConvention.Cdecl)]
        protected static extern int API_transfer_replace_call(int call_id, int dest_call_id);

        [DllImport(_dll32bit, EntryPoint = "API_dtmf", CallingConvention = CallingConvention.Cdecl)]
        protected static extern int API_dtmf(int call_id, [MarshalAsAttribute(UnmanagedType.LPStr)]string digits);

        [DllImport(_dll32bit, EntryPoint = "API_serviceReq", CallingConvention = CallingConvention.Cdecl)]
        protected static extern int API_serviceReq(int callId, int serviceCode, string destUri);

        [DllImport(_dll32bit, EntryPoint = "API_destroy", CallingConvention = CallingConvention.Cdecl)]
        protected static extern int API_destroy();

        [DllImport(_dll32bit, EntryPoint = "API_registerCallbacks", CallingConvention = CallingConvention.Cdecl)]
        protected static extern void API_registerCallbacks(IntPtr callbacks);

        [DllImport(_dll32bit, EntryPoint = "API_getErrorDesc", CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr API_getErrorDesc();


        private static volatile PInvoke32 instance;
        private static object syncRoot = new Object();

        private PInvoke32() { }

        public static PInvoke32 Instance
        {
            get
            {
                if (instance == null)
                {
                    lock (syncRoot)
                    {
                        if (instance == null)
                            instance = new PInvoke32();
                    }
                }

                return instance;
            }
        }


        public override int init(string[] args)
        {
            return API_init(args.Length, args);
        }

        public override int add_account(IntPtr account)
        {
            return API_add_account(account);
        }

        public override int account_get_info(int accId, IntPtr info)
        {
            return API_account_get_info(accId, info);
        }

        public override int delete_account(int accId)
        {
            return API_del_account(accId);
        }


        public override int make_single_call(string dest)
        {
            return API_make_single_call(dest);
        }

        public override int call_get_info(int call_id, IntPtr info)
        {
            return API_call_get_info(call_id, info);
        }

        public override int hangup_call(int call_id)
        {
            return API_hangup_call(call_id);
        }

        public override int hold_call(int call_id)
        {
            return API_hold_call(call_id);
        }

        public override int call_reinvite(int call_id)
        {
            return API_call_reinvite(call_id);
        }

        public override int answer_call(int call_id, int st_code)
        {
            return API_answer_call(call_id, st_code);
        }

        public override int transfer_call(int call_id, string dest)
        {
            return API_transfer_call(call_id, dest);
        }

        public override int transfer_replace_call(int call_id, int dest_call_id)
        {
            return API_transfer_replace_call(call_id, dest_call_id);
        }

        public override int dtmf(int call_id, string digits)
        {
            return API_dtmf(call_id, digits);
        }

        public override int serviceReq(int callId, int serviceCode, string destUri)
        {
            return API_serviceReq(callId, serviceCode, destUri);
        }

        public override int destroy()
        {
            return API_destroy();
        }

        public override void registerCallbacks(IntPtr callbacks)
        {
            API_registerCallbacks(callbacks);
        }

        public override IntPtr getErrorDesc()
        {
            return API_getErrorDesc();
        }
    }
}
