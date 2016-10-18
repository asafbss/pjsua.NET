#ifndef __API__
#define __API__

#include "pjsua_app.h"
#include <pjsua-lib/pjsua_internal.h>

#define DllExport   __declspec( dllexport ) 

typedef int		API_status_t;

/** Buffer for error description */
static char errmsg[PJ_ERR_MSG_SIZE];

/**
* This structure describes application callback to receive various event
* notification from PJSUA - API.All of these callbacks are OPTIONAL,
*although definitely application would want to implement some of
* the important callbacks(such as \a on_incoming_call).
*/
typedef struct API_Callback
{
	/**
	* Notify application when call state has changed.
	* Application may then query the call info to get the
	* detail call states by calling  pjsua_call_get_info() function.
	*
	* @param call_id	The call index.
	* @param e		Event which causes the call state to change.
	*/
	void(*on_call_state)(pjsua_call_id call_id, pjsip_event *e);

	/**
	* Notify application when media state in the call has changed.
	* Normal application would need to implement this callback, e.g.
	* to connect the call's media to sound device. When ICE is used,
	* this callback will also be called to report ICE negotiation
	* failure.
	*
	* @param call_id	The call index.
	*/
	void(*on_call_media_state)(pjsua_call_id call_id);

	/**
	* Notify application on incoming call.
	*
	* @param acc_id	The account which match the incoming call.
	* @param call_id	The call id that has just been created for
	*			the call.
	* @param rdata	The incoming INVITE request.
	*/
	void(*on_incoming_call)(pjsua_acc_id acc_id, pjsua_call_id call_id,
		pjsip_rx_data *rdata);

	/**
	* This is a general notification callback which is called whenever
	* a transaction within the call has changed state. Application can
	* implement this callback for example to monitor the state of
	* outgoing requests, or to answer unhandled incoming requests
	* (such as INFO) with a final response.
	*
	* @param call_id	Call identification.
	* @param tsx	The transaction which has changed state.
	* @param e		Transaction event that caused the state change.
	*/
	void(*on_call_tsx_state)(pjsua_call_id call_id,
		pjsip_transaction *tsx,
		pjsip_event *e);

	/**
	* Notify application upon incoming DTMF digits.
	*
	* @param call_id	The call index.
	* @param digit	DTMF ASCII digit.
	*/
	void(*on_dtmf_digit)(pjsua_call_id call_id, int digit);

	/**
	* This callback is called when the call is about to resend the
	* INVITE request to the specified target, following the previously
	* received redirection response.
	*
	* Application may accept the redirection to the specified target,
	* reject this target only and make the session continue to try the next
	* target in the list if such target exists, stop the whole
	* redirection process altogether and cause the session to be
	* disconnected, or defer the decision to ask for user confirmation.
	*
	* This callback is optional. If this callback is not implemented,
	* the default behavior is to NOT follow the redirection response.
	*
	* @param call_id	The call ID.
	* @param target	The current target to be tried.
	* @param e		The event that caused this callback to be called.
	*			This could be the receipt of 3xx response, or
	*			4xx/5xx response received for the INVITE sent to
	*			subsequent targets, or NULL if this callback is
	*			called from within #pjsua_call_process_redirect()
	*			context.
	*
	* @return		Action to be performed for the target. Set this
	*			parameter to one of the value below:
	*			- PJSIP_REDIRECT_ACCEPT: immediately accept the
	*			  redirection. When set, the call will immediately
	*			  resend INVITE request to the target.
	*			- PJSIP_REDIRECT_ACCEPT_REPLACE: immediately accept
	*			  the redirection and replace the To header with the
	*			  current target. When set, the call will immediately
	*			  resend INVITE request to the target.
	*			- PJSIP_REDIRECT_REJECT: immediately reject this
	*			  target. The call will continue retrying with
	*			  next target if present, or disconnect the call
	*			  if there is no more target to try.
	*			- PJSIP_REDIRECT_STOP: stop the whole redirection
	*			  process and immediately disconnect the call. The
	*			  on_call_state() callback will be called with
	*			  PJSIP_INV_STATE_DISCONNECTED state immediately
	*			  after this callback returns.
	*			- PJSIP_REDIRECT_PENDING: set to this value if
	*			  no decision can be made immediately (for example
	*			  to request confirmation from user). Application
	*			  then MUST call #pjsua_call_process_redirect()
	*			  to either accept or reject the redirection upon
	*			  getting user decision.
	*/
	pjsip_redirect_op(*on_call_redirected)(pjsua_call_id call_id,
		const pjsip_uri *target,
		const pjsip_event *e);

	/**
	* Notify application when registration status has changed.
	* Application may then query the account info to get the
	* registration details.
	*
	* @param acc_id	    The account ID.
	*/
	void(*on_reg_state)(pjsua_acc_id acc_id);

	/**
	* Notification when incoming SUBSCRIBE request is received. Application
	* may use this callback to authorize the incoming subscribe request
	* (e.g. ask user permission if the request should be granted).
	*
	* If this callback is not implemented, all incoming presence subscription
	* requests will be accepted.
	*
	* If this callback is implemented, application has several choices on
	* what to do with the incoming request:
	*	- it may reject the request immediately by specifying non-200 class
	*    final response in the \a code argument.
	*	- it may immediately accept the request by specifying 200 as the
	*	  \a code argument. This is the default value if application doesn't
	*	  set any value to the \a code argument. In this case, the library
	*	  will automatically send NOTIFY request upon returning from this
	*	  callback.
	*  - it may delay the processing of the request, for example to request
	*    user permission whether to accept or reject the request. In this
	*	  case, the application MUST set the \a code argument to 202, then
	*    IMMEDIATELY calls #pjsua_pres_notify() with state
	*    PJSIP_EVSUB_STATE_PENDING and later calls #pjsua_pres_notify()
	*    again to accept or reject the subscription request.
	*
	* Any \a code other than 200 and 202 will be treated as 200.
	*
	* Application MUST return from this callback immediately (e.g. it must
	* not block in this callback while waiting for user confirmation).
	*
	* @param srv_pres	    Server presence subscription instance. If
	*			    application delays the acceptance of the request,
	*			    it will need to specify this object when calling
	*			    #pjsua_pres_notify().
	* @param acc_id	    Account ID most appropriate for this request.
	* @param buddy_id	    ID of the buddy matching the sender of the
	*			    request, if any, or PJSUA_INVALID_ID if no
	*			    matching buddy is found.
	* @param from	    The From URI of the request.
	* @param rdata	    The incoming request.
	* @param code	    The status code to respond to the request. The
	*			    default value is 200. Application may set this
	*			    to other final status code to accept or reject
	*			    the request.
	* @param reason	    The reason phrase to respond to the request.
	* @param msg_data	    If the application wants to send additional
	*			    headers in the response, it can put it in this
	*			    parameter.
	*/
	void(*on_incoming_subscribe)(pjsua_acc_id acc_id,
		pjsua_srv_pres *srv_pres,
		pjsua_buddy_id buddy_id,
		const pj_str_t *from,
		pjsip_rx_data *rdata,
		pjsip_status_code *code,
		pj_str_t *reason,
		pjsua_msg_data *msg_data);

	/**
	* Notify application when the buddy state has changed.
	* Application may then query the buddy into to get the details.
	*
	* @param buddy_id	    The buddy id.
	*/
	void(*on_buddy_state)(pjsua_buddy_id buddy_id);

	/**
	* Notify application when the state of client subscription session
	* associated with a buddy has changed. Application may use this
	* callback to retrieve more detailed information about the state
	* changed event.
	*
	* @param buddy_id	    The buddy id.
	* @param sub	    Event subscription session.
	* @param event	    The event which triggers state change event.
	*/
	void(*on_buddy_evsub_state)(pjsua_buddy_id buddy_id,
		pjsip_evsub *sub,
		pjsip_event *event);

	/**
	* Notify application on incoming pager (i.e. MESSAGE request).
	* Argument call_id will be -1 if MESSAGE request is not related to an
	* existing call.
	*
	* See also \a on_pager2() callback for the version with \a pjsip_rx_data
	* passed as one of the argument.
	*
	* @param call_id	    Containts the ID of the call where the IM was
	*			    sent, or PJSUA_INVALID_ID if the IM was sent
	*			    outside call context.
	* @param from	    URI of the sender.
	* @param to	    URI of the destination message.
	* @param contact	    The Contact URI of the sender, if present.
	* @param mime_type	    MIME type of the message.
	* @param body	    The message content.
	*/
	void(*on_pager)(pjsua_call_id call_id, const pj_str_t *from,
		const pj_str_t *to, const pj_str_t *contact,
		const pj_str_t *mime_type, const pj_str_t *body);

	/**
	* Notify application about typing indication.
	*
	* @param call_id	    Containts the ID of the call where the IM was
	*			    sent, or PJSUA_INVALID_ID if the IM was sent
	*			    outside call context.
	* @param from	    URI of the sender.
	* @param to	    URI of the destination message.
	* @param contact	    The Contact URI of the sender, if present.
	* @param is_typing	    Non-zero if peer is typing, or zero if peer
	*			    has stopped typing a message.
	*/
	void(*on_typing)(pjsua_call_id call_id, const pj_str_t *from,
		const pj_str_t *to, const pj_str_t *contact,
		pj_bool_t is_typing);

	/**
	* Notify application of the status of previously sent call
	* transfer request. Application can monitor the status of the
	* call transfer request, for example to decide whether to
	* terminate existing call.
	*
	* @param call_id	    Call ID.
	* @param st_code	    Status progress of the transfer request.
	* @param st_text	    Status progress text.
	* @param final	    If non-zero, no further notification will
	*			    be reported. The st_code specified in
	*			    this callback is the final status.
	* @param p_cont	    Initially will be set to non-zero, application
	*			    can set this to FALSE if it no longer wants
	*			    to receie further notification (for example,
	*			    after it hangs up the call).
	*/
	void(*on_call_transfer_status)(pjsua_call_id call_id,
		int st_code,
		const pj_str_t *st_text,
		pj_bool_t final,
		pj_bool_t *p_cont);

	/**
	* Notify application that an existing call has been replaced with
	* a new call. This happens when PJSUA-API receives incoming INVITE
	* request with Replaces header.
	*
	* After this callback is called, normally PJSUA-API will disconnect
	* \a old_call_id and establish \a new_call_id.
	*
	* @param old_call_id   Existing call which to be replaced with the
	*			    new call.
	* @param new_call_id   The new call.
	* @param rdata	    The incoming INVITE with Replaces request.
	*/
	void(*on_call_replaced)(pjsua_call_id old_call_id,
		pjsua_call_id new_call_id);

	/**
	* Callback when the library has finished performing NAT type
	* detection.
	*
	* @param res	    NAT detection result.
	*/
	void(*on_nat_detect)(const pj_stun_nat_detect_result *res);

	/**
	* This callback is called when a NOTIFY request for message summary /
	* message waiting indication is received.
	*
	* @param acc_id	The account ID.
	* @param mwi_info	Structure containing details of the event,
	*			including the received NOTIFY request in the
	*			\a rdata field.
	*/
	void(*on_mwi_info)(pjsua_acc_id acc_id, pjsua_mwi_info *mwi_info);

	/**
	* This callback is called when transport state is changed. See also
	* #pjsip_tp_state_callback.
	*/
	void(*on_transport_state)(
		pjsip_transport *tp,
		pjsip_transport_state state,
		const pjsip_transport_state_info *info);

	/**
	* This callback is called to report error in ICE media transport.
	* Currently it is used to report TURN Refresh error.
	*
	* @param index	Transport index.
	* @param op	Operation which trigger the failure.
	* @param status	Error status.
	* @param param	Additional info about the event. Currently this will
	* 			always be set to NULL.
	*/
	void(*on_ice_transport_error)(int index, pj_ice_strans_op op,
		pj_status_t status, void *param);

	/**
	* Callback when the sound device is about to be opened or closed.
	* This callback will be called even when null sound device or no
	* sound device is configured by the application (i.e. the
	* #pjsua_set_null_snd_dev() and #pjsua_set_no_snd_dev() APIs).
	* This API is mostly useful when the application wants to manage
	* the sound device by itself (i.e. with #pjsua_set_no_snd_dev()),
	* to get notified when it should open or close the sound device.
	*
	* @param operation	The value will be set to 0 to signal that sound
	* 			device is about to be closed, and 1 to be opened.
	*
	* @return		The callback must return PJ_SUCCESS at the moment.
	*/
	pj_status_t(*on_snd_dev_operation)(int operation);

	/**
	* Notification about media events such as video notifications. This
	* callback will most likely be called from media threads, thus
	* application must not perform heavy processing in this callback.
	* Especially, application must not destroy the call or media in this
	* callback. If application needs to perform more complex tasks to
	* handle the event, it should post the task to another thread.
	*
	* @param call_id	The call id.
	* @param med_idx	The media stream index.
	* @param event 	The media event.
	*/
	void(*on_call_media_event)(pjsua_call_id call_id,
		unsigned med_idx,
		pjmedia_event *event);

} API_Callback;


static void api_on_call_state(pjsua_call_id call_id, pjsip_event *e);
static void api_on_call_media_state(pjsua_call_id call_id);
static void api_on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id,
	pjsip_rx_data *rdata);
static void api_on_call_tsx_state(pjsua_call_id call_id,
	pjsip_transaction *tsx,
	pjsip_event *e);
static void api_on_dtmf_digit(pjsua_call_id call_id, int dtmf);
static pjsip_redirect_op api_on_call_redirected(pjsua_call_id call_id,
	const pjsip_uri *target,
	const pjsip_event *e);
static void api_on_reg_state(pjsua_acc_id acc_id);
static void api_on_incoming_subscribe(pjsua_acc_id acc_id,
	pjsua_srv_pres *srv_pres,
	pjsua_buddy_id buddy_id,
	const pj_str_t *from,
	pjsip_rx_data *rdata,
	pjsip_status_code *code,
	pj_str_t *reason,
	pjsua_msg_data *msg_data);
static void api_on_buddy_state(pjsua_buddy_id buddy_id);
static void api_on_buddy_evsub_state(pjsua_buddy_id buddy_id,
	pjsip_evsub *sub,
	pjsip_event *event);
static void api_on_pager(pjsua_call_id call_id, const pj_str_t *from,
	const pj_str_t *to, const pj_str_t *contact,
	const pj_str_t *mime_type, const pj_str_t *body);
static void api_on_typing(pjsua_call_id call_id, const pj_str_t *from,
	const pj_str_t *to, const pj_str_t *contact,
	pj_bool_t is_typing);
static void api_on_call_transfer_status(pjsua_call_id call_id,
	int st_code,
	const pj_str_t *st_text,
	pj_bool_t final,
	pj_bool_t *p_cont);
static void api_on_call_replaced(pjsua_call_id old_call_id,
	pjsua_call_id new_call_id);
static void api_on_nat_detect(const pj_stun_nat_detect_result *res);
static void api_on_mwi_info(pjsua_acc_id acc_id, pjsua_mwi_info *mwi_info);
static void api_on_transport_state(pjsip_transport *tp,
	pjsip_transport_state state,
	const pjsip_transport_state_info *info);
static void api_on_ice_transport_error(int index, pj_ice_strans_op op,
	pj_status_t status, void *param);
static pj_status_t api_on_snd_dev_operation(int operation);
static void api_on_call_media_event(pjsua_call_id call_id,
	unsigned med_idx,
	pjmedia_event *event);
#ifdef TRANSPORT_ADAPTER_SAMPLE
static void api_on_create_media_transport(pjsua_call_id call_id,
	unsigned media_idx,
	pjmedia_transport *base_tp,
	unsigned flags);
#endif

typedef struct API_Acc_config
{
	/**
	* The full SIP URL for the account. The value can take name address or
	* URL format, and will look something like "sip:account@serviceprovider"
	* or "\"Display Name\" <sip:account@provider>".
	*
	* This field is mandatory.
	*/
	char	    *id;

	/**
	* This is the URL to be put in the request URI for the registration,
	* and will look something like "sip:serviceprovider".
	*
	* This field should be specified if registration is desired. If the
	* value is empty, no account registration will be performed.
	*/
	char	    *reg_uri;

	/**
	* Realm. Use "*" to make a credential that
	* can be used to authenticate against any
	* challenges.
	*/
	char    *realm;

	/**
	* User name and password
	*/
	char	*username;
	char	*password;

} API_Acc_config;



/* PJSUA application instance. */
extern struct pjsua_data pjsua_var;
API_Callback api_callbacks;
pjsua_callback pjsua_callbacks;

static void API_error(const char *title, pj_status_t status);
static void API_error2(const char *msg);
static int get_input_url(char *buf, pj_size_t len, struct input_result *result);


DllExport API_status_t API_init(int argc, char *argv[]);
DllExport pjsua_acc_id API_add_account(API_Acc_config *conf);
DllExport API_status_t API_del_account(pjsua_acc_id accId);
DllExport API_status_t API_account_get_info(pjsua_acc_id accId, pjsua_acc_info *info);
DllExport pjsua_call_id API_make_single_call(char *dest);
DllExport API_status_t API_call_get_info(pjsua_call_id call_id, pjsua_call_info *info);
DllExport API_status_t API_hangup_call(pjsua_call_id call_id);
DllExport API_status_t API_hold_call(pjsua_call_id call_id);
DllExport API_status_t API_call_reinvite(pjsua_call_id call_id);
DllExport API_status_t API_answer_call(pjsua_call_id call_id, int st_code);
DllExport API_status_t API_transfer_call(pjsua_call_id call_id, char *dest);
DllExport API_status_t API_transfer_replace_call(pjsua_call_id call_id, pjsua_call_id dest_call_id);
DllExport API_status_t API_dtmf(pjsua_call_id call_id, char *digits);
DllExport API_status_t API_serviceReq(int callId, int serviceCode, char* destUri);
DllExport API_status_t API_destroy();
DllExport void API_registerCallbacks(API_Callback* callbacks);
DllExport const char* API_getErrorDesc();

#endif