using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace pjsua.NET
{
    public class pjsua
    {
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public struct Acc_config
        {
            /**
            * The full SIP URL for the account. The value can take name address or
            * URL format, and will look something like "sip:account@serviceprovider"
            * or "\"Display Name\" <sip:account@provider>".
            *
            * This field is mandatory.
            */
            public string id;

            /**
            * This is the URL to be put in the request URI for the registration,
            * and will look something like "sip:serviceprovider".
            *
            * This field should be specified if registration is desired. If the
            * value is empty, no account registration will be performed.
            */
            public string reg_uri;

            /**
            * Realm. Use "*" to make a credential that
            * can be used to authenticate against any
            * challenges.
            */
            public string realm;

            /**
            * User name and password
            */
            public string username;
            public string password;
        }


        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public class cb
        {
            [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
            public delegate void on_call_state(int call_id, IntPtr e);
            [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
            public delegate void on_call_media_state(int call_id);
            [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
            public delegate void on_incoming_call(int acc_id, int call_id, IntPtr rdata);
            [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
            public delegate void on_call_tsx_state(int call_id, IntPtr tsx, IntPtr e);
            [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
            public delegate void on_dtmf_digit(int call_id, int dtmf);
            [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
            public delegate sip_util.pjsip_redirect_op on_call_redirected(int call_id, IntPtr target, IntPtr e);
            [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
            public delegate void on_reg_state(int acc_id);
            [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
            public delegate void on_incoming_subscribe(int acc_id, IntPtr srv_pres, int buddy_id,
                IntPtr from, IntPtr rdata, IntPtr code, IntPtr reason, IntPtr msg_data);
            [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
            public delegate void on_buddy_state(int buddy_id);
            [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
            public delegate void on_buddy_evsub_state(int buddy_id, IntPtr sub, IntPtr _event);
            [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
            public delegate void on_pager(int call_id, IntPtr from,
                IntPtr to, IntPtr contact, IntPtr mime_type, IntPtr body);
            [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
            public delegate void on_typing(int call_id, IntPtr from, IntPtr to, IntPtr contact, int is_typing);
            [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
            public delegate void on_call_transfer_status(int call_id, int st_code, IntPtr st_text,
                int final, ref int p_cont);
            [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
            public delegate void on_call_replaced(int old_call_id, int new_call_id);
            [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
            public delegate void on_nat_detect(IntPtr res);
            [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
            public delegate void on_mwi_info(int acc_id, IntPtr mwi_info);
            [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
            public delegate void on_transport_state(IntPtr tp, sip_transport.pjsip_transport_state state, IntPtr info);
            [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
            public delegate void on_ice_transport_error(int index, ice_strans.pj_ice_strans_op op, int status, IntPtr param);
            [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
            public delegate int on_snd_dev_operation(int operation);
            [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
            public delegate void on_call_media_event(int call_id, uint med_idx, IntPtr _event);


            public event on_call_state _on_call_state;
            public event on_call_media_state _on_call_media_state;
            public event on_incoming_call _on_incoming_call;
            public event on_call_tsx_state _on_call_tsx_state;
            public event on_dtmf_digit _on_dtmf_digit;
            public event on_call_redirected _on_call_redirected;
            public event on_reg_state _on_reg_state;
            public event on_incoming_subscribe _on_incoming_subscribe;
            public event on_buddy_state _on_buddy_state;
            public event on_buddy_evsub_state _on_buddy_evsub_state;
            public event on_pager _on_pager;
            public event on_typing _on_typing;
            public event on_call_transfer_status _on_call_transfer_status;
            public event on_call_replaced _on_call_replaced;
            public event on_nat_detect _on_nat_detect;
            public event on_mwi_info _on_mwi_info;
            public event on_transport_state _on_transport_state;
            public event on_ice_transport_error _on_ice_transport_error;
            public event on_snd_dev_operation _on_snd_dev_operation;
            public event on_call_media_event _on_call_media_event;

        }

        /**
         * This structure describes the information and current status of a call.
         */
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public class pjsua_call_info
        {
            /** Call identification. */
            public int id;

            /** Initial call role (UAC == caller) */
            public sip_types.pjsip_role_e role;

            /** The account ID where this call belongs. */
            public int acc_id;

            /** Local URI */
            public types.pj_str_t local_info;

            /** Local Contact */
            public types.pj_str_t local_contact;

            /** Remote URI */
            public types.pj_str_t remote_info;

            /** Remote contact */
            public types.pj_str_t remote_contact;

            /** Dialog Call-ID string. */
            public types.pj_str_t call_id;

            /** Call setting */
            public pjsua_call_setting setting;

            /** Call state */
            public sip_inv.pjsip_inv_state state;

            /** Text describing the state */
            public types.pj_str_t state_text;

            /** Last status code heard, which can be used as cause code */
            public sip_msg.pjsip_status_code last_status;

            /** The reason phrase describing the status. */
            public types.pj_str_t last_status_text;

            /** Media status of the first audio stream. */
            public pjsua_call_media_status media_status;

            /** Media direction of the first audio stream. */
            public types.pjmedia_dir media_dir;

            /** The conference port number for the first audio stream. */
            public int conf_slot;

            /** Number of active media info in this call. */
            public uint media_cnt;

            /** Array of active media information. */
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 16)]
            public pjsua_call_media_info[] media;

            /** Number of provisional media info in this call. */
            public uint prov_media_cnt;

            /** Array of provisional media information. This contains the media info
             *  in the provisioning state, that is when the media session is being
             *  created/updated (SDP offer/answer is on progress).
             */
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 16)]
            public pjsua_call_media_info[] prov_media;

            /** Up-to-date call connected duration (zero when call is not 
             *  established)
             */
            public types.pj_time_val connect_duration;

            /** Total call duration, including set-up time */
            public types.pj_time_val total_duration;

            /** Flag if remote was SDP offerer */
            public int rem_offerer;

            /** Number of audio streams offered by remote */
            public uint rem_aud_cnt;

            /** Number of video streams offered by remote */
            public uint rem_vid_cnt;

            /** Internal */
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 128 * 6)]
            public byte[] buf;

        }

        /**
         * Call settings.
         */
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public struct pjsua_call_setting
        {
            /**
             * Bitmask of #pjsua_call_flag constants.
             *
             * Default: PJSUA_CALL_INCLUDE_DISABLED_MEDIA
             */
            public uint flag;

            /**
             * This flag controls what methods to request keyframe are allowed on
             * the call. Value is bitmask of #pjsua_vid_req_keyframe_method.
             *
             * Default: PJSUA_VID_REQ_KEYFRAME_SIP_INFO
             */
            public uint req_keyframe_method;

            /**
             * Number of simultaneous active audio streams for this call. Setting
             * this to zero will disable audio in this call.
             *
             * Default: 1
             */
            public uint aud_cnt;

            /**
             * Number of simultaneous active video streams for this call. Setting
             * this to zero will disable video in this call.
             *
             * Default: 1 (if video feature is enabled, otherwise it is zero)
             */
            public uint vid_cnt;

        }

        /**
        * This enumeration specifies the media status of a call, and it's part
        * of pjsua_call_info structure.
        */
        public enum pjsua_call_media_status
        {
            /**
             * Call currently has no media, or the media is not used.
             */
            PJSUA_CALL_MEDIA_NONE,

            /**
             * The media is active
             */
            PJSUA_CALL_MEDIA_ACTIVE,

            /**
             * The media is currently put on hold by local endpoint
             */
            PJSUA_CALL_MEDIA_LOCAL_HOLD,

            /**
             * The media is currently put on hold by remote endpoint
             */
            PJSUA_CALL_MEDIA_REMOTE_HOLD,

            /**
             * The media has reported error (e.g. ICE negotiation)
             */
            PJSUA_CALL_MEDIA_ERROR

        }

        /**
         * Call media information.
         */
        [StructLayout(LayoutKind.Explicit, CharSet = CharSet.Ansi)]
        public struct pjsua_call_media_info
        {

            /** Media index in SDP. */
            [FieldOffset(0)]
            public uint index;

            /** Media type. */
            [FieldOffset(4)]
            public types.pjmedia_type type;

            /** Media direction. */
            [FieldOffset(8)]
            public types.pjmedia_dir dir;

            /** Call media status. */
            [FieldOffset(12)]
            public pjsua_call_media_status status;

            /** The specific media stream info. */

            /** Audio stream */
            /** The conference port number for the call.  */
            [FieldOffset(16)]
            public int conf_slot;

            /** Video stream */

            /**
             * The window id for incoming video, if any, or
             * PJSUA_INVALID_ID.
             */
            [FieldOffset(16)]
            public int win_in;

            /** The video capture device for outgoing transmission,
             *  if any, or PJMEDIA_VID_INVALID_DEV
             */
            [FieldOffset(20)]
            public int cap_dev;

        }

        /**
         * Account info. Application can query account info by calling 
         * #pjsua_acc_get_info().
         */
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public class pjsua_acc_info
        {
            /** 
             * The account ID. 
             */
            public int id;

            /**
             * Flag to indicate whether this is the default account.
             */
            public int is_default;

            /** 
             * Account URI 
             */
            public types.pj_str_t acc_uri;

            /** 
             * Flag to tell whether this account has registration setting
             * (reg_uri is not empty).
             */
            public int has_registration;

            /**
             * An up to date expiration interval for account registration session.
             */
            public int expires;

            /**
             * Last registration status code. If status code is zero, the account
             * is currently not registered. Any other value indicates the SIP
             * status code of the registration.
             */
            public sip_msg.pjsip_status_code status;

            /**
             * Last registration error code. When the status field contains a SIP
             * status code that indicates a registration failure, last registration
             * error code contains the error code that causes the failure. In any
             * other case, its value is zero.
             */
            public int reg_last_err;

            /**
             * String describing the registration status.
             */
            public types.pj_str_t status_text;

            /**
             * Presence online status for this account.
             */
            public int online_status;

            /**
             * Presence online status text.
             */
            public types.pj_str_t online_status_text;

            /**
             * Extended RPID online status information.
             */
            public rpid.pjrpid_element rpid;

            /**
             * Buffer that is used internally to store the status text.
             */
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 80)]
            public byte[] buf_;

        }
    }

    public class sip_util
    {
        /**
        * These enumerations specify the action to be performed to a redirect
        * response.
        */
        public enum pjsip_redirect_op
        {
            /**
             * Reject the redirection to the current target. The UAC will
             * select the next target from the target set if exists.
             */
            PJSIP_REDIRECT_REJECT,

            /**
             * Accept the redirection to the current target. The INVITE request
             * will be resent to the current target.
             */
            PJSIP_REDIRECT_ACCEPT,

            /**
             * Accept the redirection to the current target and replace the To
             * header in the INVITE request with the current target. The INVITE
             * request will be resent to the current target.
             */
            PJSIP_REDIRECT_ACCEPT_REPLACE,

            /**
             * Defer the redirection decision, for example to request permission
             * from the end user.
             */
            PJSIP_REDIRECT_PENDING,

            /**
             * Stop the whole redirection process altogether. This will cause
             * the invite session to be disconnected.
             */
            PJSIP_REDIRECT_STOP

        };
    }

    public class sip_transport
    {
        /**
         * Enumeration of transport state types.
         */
        public enum pjsip_transport_state
        {
            PJSIP_TP_STATE_CONNECTED,	    /**< Transport connected, applicable only
					 to connection-oriented transports
					 such as TCP and TLS.		    */
            PJSIP_TP_STATE_DISCONNECTED,    /**< Transport disconnected, applicable
					 only to connection-oriented 
					 transports such as TCP and TLS.    */
            PJSIP_TP_STATE_SHUTDOWN,        /**< Transport shutdown, either
                                         due to TCP/TLS disconnect error
                                         from the network, or when shutdown
                                         is initiated by PJSIP itself.      */
            PJSIP_TP_STATE_DESTROY,         /**< Transport destroy, when transport
                                         is about to be destroyed.          */
        }
    }

    public class ice_strans
    {
        /** Transport operation types to be reported on \a on_status() callback */
        public enum pj_ice_strans_op
        {
            /** Initialization (candidate gathering) */
            PJ_ICE_STRANS_OP_INIT,

            /** Negotiation */
            PJ_ICE_STRANS_OP_NEGOTIATION,

            /** This operation is used to report failure in keep-alive operation.
             *  Currently it is only used to report TURN Refresh failure.
             */
            PJ_ICE_STRANS_OP_KEEP_ALIVE,

            /** IP address change notification from STUN keep-alive operation.
             */
            PJ_ICE_STRANS_OP_ADDR_CHANGE

        }
    }

    public class types
    {
        /**
         * This type is used as replacement to legacy C string, and used throughout
         * the library. By convention, the string is NOT null terminated.
         */
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public struct pj_str_t
        {
            /** Buffer pointer, which is by convention NOT null terminated. */
            [MarshalAsAttribute(UnmanagedType.LPStr)]
            public string ptr;

            /** The length of the string. */
            public Int32 slen;
        }

        /**
        * Top most media type. See also #pjmedia_type_name().
        */
        public enum pjmedia_type
        {
            /** Type is not specified. */
            PJMEDIA_TYPE_NONE,

            /** The media is audio */
            PJMEDIA_TYPE_AUDIO,

            /** The media is video. */
            PJMEDIA_TYPE_VIDEO,

            /** The media is application. */
            PJMEDIA_TYPE_APPLICATION,

            /** The media type is unknown or unsupported. */
            PJMEDIA_TYPE_UNKNOWN

        }

        /**
        * Media direction.
        */
        public enum pjmedia_dir
        {
            /** None */
            PJMEDIA_DIR_NONE = 0,

            /** Encoding (outgoing to network) stream, also known as capture */
            PJMEDIA_DIR_ENCODING = 1,

            /** Same as encoding direction. */
            PJMEDIA_DIR_CAPTURE = PJMEDIA_DIR_ENCODING,

            /** Decoding (incoming from network) stream, also known as playback. */
            PJMEDIA_DIR_DECODING = 2,

            /** Same as decoding. */
            PJMEDIA_DIR_PLAYBACK = PJMEDIA_DIR_DECODING,

            /** Same as decoding. */
            PJMEDIA_DIR_RENDER = PJMEDIA_DIR_DECODING,

            /** Incoming and outgoing stream, same as PJMEDIA_DIR_CAPTURE_PLAYBACK */
            PJMEDIA_DIR_ENCODING_DECODING = 3,

            /** Same as ENCODING_DECODING */
            PJMEDIA_DIR_CAPTURE_PLAYBACK = PJMEDIA_DIR_ENCODING_DECODING,

            /** Same as ENCODING_DECODING */
            PJMEDIA_DIR_CAPTURE_RENDER = PJMEDIA_DIR_ENCODING_DECODING

        }


        /**
         * Representation of time value in this library.
         * This type can be used to represent either an interval or a specific time
         * or date. 
         */
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]

        public class pj_time_val
        {
            /** The seconds part of the time. */
            public Int32 sec;

            /** The miliseconds fraction of the time. */
            public Int32 msec;

        }

    }

    public class sip_types
    {
        /**
        * Transaction role.
        */
        public enum pjsip_role_e
        {
            PJSIP_ROLE_UAC,	/**< Role is UAC. */
            PJSIP_ROLE_UAS,	/**< Role is UAS. */

            /* Alias: */

            PJSIP_UAC_ROLE = PJSIP_ROLE_UAC,	/**< Role is UAC. */
            PJSIP_UAS_ROLE = PJSIP_ROLE_UAS	/**< Role is UAS. */

        }
    }

    public class sip_inv
    {
        /**
        * This enumeration describes invite session state.
        */
        public enum pjsip_inv_state
        {
            PJSIP_INV_STATE_NULL,	    /**< Before INVITE is sent or received  */
            PJSIP_INV_STATE_CALLING,	    /**< After INVITE is sent		    */
            PJSIP_INV_STATE_INCOMING,	    /**< After INVITE is received.	    */
            PJSIP_INV_STATE_EARLY,	    /**< After response with To tag.	    */
            PJSIP_INV_STATE_CONNECTING,	    /**< After 2xx is sent/received.	    */
            PJSIP_INV_STATE_CONFIRMED,	    /**< After ACK is sent/received.	    */
            PJSIP_INV_STATE_DISCONNECTED,   /**< Session is terminated.		    */
        }
    }

    public class sip_msg
    {
        /**
        * This enumeration lists standard SIP status codes according to RFC 3261.
        * In addition, it also declares new status class 7xx for errors generated
        * by the stack. This status class however should not get transmitted on the 
        * wire.
        */
        public enum pjsip_status_code
        {
            PJSIP_SC_TRYING = 100,
            PJSIP_SC_RINGING = 180,
            PJSIP_SC_CALL_BEING_FORWARDED = 181,
            PJSIP_SC_QUEUED = 182,
            PJSIP_SC_PROGRESS = 183,

            PJSIP_SC_OK = 200,
            PJSIP_SC_ACCEPTED = 202,

            PJSIP_SC_MULTIPLE_CHOICES = 300,
            PJSIP_SC_MOVED_PERMANENTLY = 301,
            PJSIP_SC_MOVED_TEMPORARILY = 302,
            PJSIP_SC_USE_PROXY = 305,
            PJSIP_SC_ALTERNATIVE_SERVICE = 380,

            PJSIP_SC_BAD_REQUEST = 400,
            PJSIP_SC_UNAUTHORIZED = 401,
            PJSIP_SC_PAYMENT_REQUIRED = 402,
            PJSIP_SC_FORBIDDEN = 403,
            PJSIP_SC_NOT_FOUND = 404,
            PJSIP_SC_METHOD_NOT_ALLOWED = 405,
            PJSIP_SC_NOT_ACCEPTABLE = 406,
            PJSIP_SC_PROXY_AUTHENTICATION_REQUIRED = 407,
            PJSIP_SC_REQUEST_TIMEOUT = 408,
            PJSIP_SC_GONE = 410,
            PJSIP_SC_REQUEST_ENTITY_TOO_LARGE = 413,
            PJSIP_SC_REQUEST_URI_TOO_LONG = 414,
            PJSIP_SC_UNSUPPORTED_MEDIA_TYPE = 415,
            PJSIP_SC_UNSUPPORTED_URI_SCHEME = 416,
            PJSIP_SC_BAD_EXTENSION = 420,
            PJSIP_SC_EXTENSION_REQUIRED = 421,
            PJSIP_SC_SESSION_TIMER_TOO_SMALL = 422,
            PJSIP_SC_INTERVAL_TOO_BRIEF = 423,
            PJSIP_SC_TEMPORARILY_UNAVAILABLE = 480,
            PJSIP_SC_CALL_TSX_DOES_NOT_EXIST = 481,
            PJSIP_SC_LOOP_DETECTED = 482,
            PJSIP_SC_TOO_MANY_HOPS = 483,
            PJSIP_SC_ADDRESS_INCOMPLETE = 484,
            PJSIP_AC_AMBIGUOUS = 485,
            PJSIP_SC_BUSY_HERE = 486,
            PJSIP_SC_REQUEST_TERMINATED = 487,
            PJSIP_SC_NOT_ACCEPTABLE_HERE = 488,
            PJSIP_SC_BAD_EVENT = 489,
            PJSIP_SC_REQUEST_UPDATED = 490,
            PJSIP_SC_REQUEST_PENDING = 491,
            PJSIP_SC_UNDECIPHERABLE = 493,

            PJSIP_SC_INTERNAL_SERVER_ERROR = 500,
            PJSIP_SC_NOT_IMPLEMENTED = 501,
            PJSIP_SC_BAD_GATEWAY = 502,
            PJSIP_SC_SERVICE_UNAVAILABLE = 503,
            PJSIP_SC_SERVER_TIMEOUT = 504,
            PJSIP_SC_VERSION_NOT_SUPPORTED = 505,
            PJSIP_SC_MESSAGE_TOO_LARGE = 513,
            PJSIP_SC_PRECONDITION_FAILURE = 580,

            PJSIP_SC_BUSY_EVERYWHERE = 600,
            PJSIP_SC_DECLINE = 603,
            PJSIP_SC_DOES_NOT_EXIST_ANYWHERE = 604,
            PJSIP_SC_NOT_ACCEPTABLE_ANYWHERE = 606,

            PJSIP_SC_TSX_TIMEOUT = PJSIP_SC_REQUEST_TIMEOUT,
            /*PJSIP_SC_TSX_RESOLVE_ERROR = 702,*/
            PJSIP_SC_TSX_TRANSPORT_ERROR = PJSIP_SC_SERVICE_UNAVAILABLE,

            /* This is not an actual status code, but rather a constant
             * to force GCC to use 32bit to represent this enum, since
             * we have a code in PJSUA-LIB that assigns an integer
             * to this enum (see pjsua_acc_get_info() function).
             */
            PJSIP_SC__force_32bit = 0x7FFFFFFF

        }
    }

    public class rpid
    {
        /**
         * This structure describes person information in RPID document.
         */
        public struct pjrpid_element
        {
            /** Element type. */
            public pjrpid_element_type type;

            /** Optional id to set on the element. */
            public types.pj_str_t id;

            /** Activity type. */
            public pjrpid_activity activity;

            /** Optional text describing the person/element. */
            public types.pj_str_t note;

        }

        /**
         * This enumeration describes subset of standard activities as 
         * described by RFC 4880, RPID: Rich Presence Extensions to the 
         * Presence Information Data Format (PIDF). 
         */
        public enum pjrpid_activity
        {
            /** Activity is unknown. The activity would then be conceived
             *  in the "note" field.
             */
            PJRPID_ACTIVITY_UNKNOWN,

            /** The person is away */
            PJRPID_ACTIVITY_AWAY,

            /** The person is busy */
            PJRPID_ACTIVITY_BUSY

        }

        /**
         * This enumeration describes types of RPID element.
         */
        public enum pjrpid_element_type
        {
            /** RPID <person> element */
            PJRPID_ELEMENT_TYPE_PERSON

        }
    }

}
