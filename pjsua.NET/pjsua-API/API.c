#include "API.h"


#define THIS_FILE	"API.c"
typedef int		API_status_t;



/** Buffer for error description */
static char errmsg[PJ_ERR_MSG_SIZE];

/** Some constants */
enum API_constants_
{
	/** Status is OK. */
	API_SUCCESS = 0,

	/** Error status. */
	API_ERROR = -1
};

enum API_service_constants_
{
	SC_Deflect,
	SC_CFU,
	SC_CFNR,
	SC_DND,
	SC_3Pty,
	SC_CFB
};

static void API_error(const char *title, pj_status_t status){
	int len = sprintf(errmsg, "%s [status=%d]: ", title, status);
	pj_strerror(status, errmsg + len, sizeof(errmsg - len));
}

static void API_error2(const char *msg){
	 sprintf(errmsg, "%s", msg);
 }


/* Get last error description */
const char* API_getErrorDesc(){
	return errmsg;
}

/* Register app callbacks*/
void API_registerCallbacks(API_Callback *callbacks){

	memcpy(&api_callbacks, callbacks, sizeof(API_Callback));
}

/* Init SIP app */
API_status_t API_init(int argc, char *argv[]){

	pjsua_app_cfg_t cfg;
	pj_status_t status;
	char **_argv;
	int	_argc;

	_argc = argc + 1;
	_argv = (char**)malloc(sizeof(char*)*_argc);
	_argv[0] = "This string will mimic the program path";

	//Set default for startup arguments (pjsua demands it).
	for (int i = 1; i <= argc; i++){
		_argv[i] = argv[i - 1];
	}

	pj_bzero(&cfg, sizeof(cfg));
	cfg.argc = _argc;
	cfg.argv = _argv;

	status = pjsua_app_init(&cfg);

	free(_argv);

	if (status != PJ_SUCCESS) {
		API_error("Error init app", status);
		return API_ERROR;
	}

	/* Hook application callbacks */
	pjsua_callbacks = app_config.cfg.cb;
	app_config.cfg.cb.on_call_state = &api_on_call_state;
	app_config.cfg.cb.on_call_media_state = &api_on_call_media_state;
	app_config.cfg.cb.on_incoming_call = &api_on_incoming_call;
	app_config.cfg.cb.on_call_tsx_state = &api_on_call_tsx_state;
	app_config.cfg.cb.on_dtmf_digit = &api_on_dtmf_digit;
	app_config.cfg.cb.on_call_redirected = &api_on_call_redirected;
	app_config.cfg.cb.on_reg_state = &api_on_reg_state;
	app_config.cfg.cb.on_incoming_subscribe = &api_on_incoming_subscribe;
	app_config.cfg.cb.on_buddy_state = &api_on_buddy_state;
	app_config.cfg.cb.on_buddy_evsub_state = &api_on_buddy_evsub_state;
	app_config.cfg.cb.on_pager = &api_on_pager;
	app_config.cfg.cb.on_typing = &api_on_typing;
	app_config.cfg.cb.on_call_transfer_status = &api_on_call_transfer_status;
	app_config.cfg.cb.on_call_replaced = &api_on_call_replaced;
	app_config.cfg.cb.on_nat_detect = &api_on_nat_detect;
	app_config.cfg.cb.on_mwi_info = &api_on_mwi_info;
	app_config.cfg.cb.on_transport_state = &api_on_transport_state;
	app_config.cfg.cb.on_ice_transport_error = &api_on_ice_transport_error;
	app_config.cfg.cb.on_snd_dev_operation = &api_on_snd_dev_operation;
	app_config.cfg.cb.on_call_media_event = &api_on_call_media_event;
#ifdef TRANSPORT_ADAPTER_SAMPLE
	app_config.cfg.cb.on_create_media_transport = &api_on_create_media_transport;
#endif

	/* Apply the new callbacks hooks. */
	memcpy(&pjsua_var.ua_cfg.cb, &app_config.cfg.cb, sizeof(pjsua_callback));
	
	/* Run the application */
	if ((status = pjsua_app_run()) != PJ_SUCCESS) {
		API_error("Error running app", status);
		return API_ERROR;
	}

	return API_SUCCESS;

}

/* Add SIP account */
pjsua_acc_id API_add_account(API_Acc_config *conf)
{
	pjsua_acc_config acc_cfg;
	pj_status_t status;
	pjsua_acc_id acc_id;

	pjsua_acc_config_default(&acc_cfg);
	acc_cfg.id = pj_str(conf->id);
	acc_cfg.reg_uri = pj_str(conf->reg_uri);
	acc_cfg.cred_count = 1;
	acc_cfg.cred_info[0].scheme = pj_str("Digest");
	acc_cfg.cred_info[0].realm = pj_str(conf->realm);
	acc_cfg.cred_info[0].username = pj_str(conf->username);
	acc_cfg.cred_info[0].data_type = 0;
	acc_cfg.cred_info[0].data = pj_str(conf->password);

	acc_cfg.rtp_cfg = app_config.rtp_cfg;
	app_config_init_video(&acc_cfg);

	if ((status = pjsua_acc_add(&acc_cfg, PJ_TRUE, &acc_id)) != PJ_SUCCESS) {
		API_error("Error adding new account", status);
		return API_ERROR;
	}

	return acc_id;
}

/* Delete account */
API_status_t API_del_account(pjsua_acc_id accId)
{
	pj_status_t status;

	if (!pjsua_acc_is_valid(accId)) {
		API_error2("Invalid account id");
		return API_ERROR;
	}
	else {

		if ((status = pjsua_acc_del(accId)) != PJ_SUCCESS) {
			API_error("Error adding new account", status);
			return API_ERROR;
		}
	}
	return API_SUCCESS;
}

/* Get account information */
API_status_t API_account_get_info(pjsua_acc_id accId, pjsua_acc_info *info)
{
	pj_status_t status;

	if (!pjsua_acc_is_valid(accId)) {
		API_error2("Invalid account id");
		return API_ERROR;
	}
	else {

		if ((status = pjsua_acc_get_info(accId, info)) != PJ_SUCCESS) {
			API_error("Error get account info", status);
			return API_ERROR;
		}
	}
	return API_SUCCESS;
}

/* Make single call */
pjsua_call_id API_make_single_call(char *dest)
{
	pj_status_t status;
	struct input_result result;
	pj_str_t tmp = pj_str(dest);

	/* input destination. */
	if (get_input_url(tmp.ptr, tmp.slen, &result) < 0){
		API_error2("Destination input invalid");
		return API_ERROR;
	}

	if (result.nb_result != PJSUA_APP_NO_NB) {
		pjsua_buddy_info binfo;
		pjsua_buddy_get_info(result.nb_result - 1, &binfo);
		pj_strncpy(&tmp, &binfo.uri, sizeof(dest));
	}
	else {
		tmp = pj_str(result.uri_result);
	}

	pjsua_msg_data_init(&msg_data);
	TEST_MULTIPART(&msg_data);

	if ((status = pjsua_call_make_call(current_acc, &tmp, &call_opt, NULL,
		&msg_data, &current_call)) != PJ_SUCCESS) {
		API_error("Error making call", status);
		return API_ERROR;
	}

	return current_call;
}

/* Get call details */
API_status_t API_call_get_info(pjsua_call_id call_id, pjsua_call_info *info){

	pj_status_t status;

	if ((status = pjsua_call_get_info(call_id, info)) != PJ_SUCCESS) {
		API_error("Error get call info", status);
		return API_ERROR;
	}

	return API_SUCCESS;
}

/* Hangup call */
API_status_t API_hangup_call(pjsua_call_id call_id)
{
	pj_status_t status;

	if (call_id != PJSUA_INVALID_ID) {
		if (pjsua_call_is_active(call_id)){
			if ((status = pjsua_call_hangup(call_id, 0, NULL, NULL)) != PJ_SUCCESS) {
				API_error("Error hangup call", status);
				return API_ERROR;
			}
		}
		else{
			API_error2("Call is not active");
			return API_ERROR;
		}
	}
	return API_SUCCESS;
}

/* Hold call */
API_status_t API_hold_call(int call_id)
{
	pj_status_t status;

	if (call_id != PJSUA_INVALID_ID) {
		if (pjsua_call_is_active(call_id)){
			if ((status = pjsua_call_set_hold(call_id, NULL)) != PJ_SUCCESS) {
				API_error("Error hold call", status);
				return API_ERROR;
			}		
		}
		else{
			API_error2("Call is not active");
			return API_ERROR;
		}
	}

	return API_SUCCESS;
}

/* Call reinvite */
API_status_t API_call_reinvite(pjsua_call_id call_id)
{
	pj_status_t status;

	if (call_id != PJSUA_INVALID_ID) {
		if (pjsua_call_is_active(call_id)){
			call_opt.flag |= PJSUA_CALL_UNHOLD;
			if ((status = pjsua_call_reinvite2(call_id, &call_opt, NULL)) != PJ_SUCCESS) {
				API_error("Error reinvite call", status);
				return API_ERROR;
			}
		}
		else{
			API_error2("Call is not active");
			return API_ERROR;
		}
	}

	return API_SUCCESS;
}

/* Answer call */
API_status_t API_answer_call(pjsua_call_id call_id, int st_code)
{
	pj_status_t status;
	pjsua_call_info call_info;

	if (call_id != PJSUA_INVALID_ID) {
		pjsua_call_get_info(call_id, &call_info);
	}
	else {
		/* Make compiler happy */
		call_info.role = PJSIP_ROLE_UAC;
		call_info.state = PJSIP_INV_STATE_DISCONNECTED;
	}

	if (call_id == PJSUA_INVALID_ID ||
		call_info.role != PJSIP_ROLE_UAS ||
		call_info.state >= PJSIP_INV_STATE_CONNECTING)
	{
		API_error2("No pending incoming call");
		return API_ERROR;
	}
	else {
		//char contact[120];
		//pj_str_t hname = { "Contact", 7 };
		//pj_str_t hvalue;
		//pjsip_generic_string_hdr hcontact;

		if ((st_code < 100) || (st_code > 699))
			return PJ_SUCCESS;

		pjsua_msg_data_init(&msg_data);

		/* Not supported at this moment
		if (st_code / 100 == 3) {
			if (cval->argc < 3) {
				static const pj_str_t err_msg = { "Enter URL to be put "
					"in Contact\n", 32 };
				pj_cli_sess_write_msg(cval->sess, err_msg.ptr, err_msg.slen);
				return PJ_SUCCESS;
			}

			hvalue = pj_str(contact);
			pjsip_generic_string_hdr_init2(&hcontact, &hname, &hvalue);

			pj_list_push_back(&msg_data.hdr_list, &hcontact);
		}
		*/

		/*
		* Must check again!
		* Call may have been disconnected while we're waiting for
		* keyboard input.
		*/
		if (call_id == PJSUA_INVALID_ID) {
			API_error2("Call has been disconnected");
			return API_ERROR;
		}
		
		if ((status = pjsua_call_answer2(call_id, &call_opt,
			st_code, NULL, &msg_data)) != PJ_SUCCESS) {
			API_error("Error reinvite call", status);
			return API_ERROR;
		}
	}

	return API_SUCCESS;
}

/* Transfer call */
API_status_t API_transfer_call(pjsua_call_id call_id, char *dest)
{
	pj_status_t status;

	if (call_id != PJSUA_INVALID_ID) {
		if (pjsua_call_is_active(call_id)){

			pj_str_t tmp = pj_str(dest);
			pj_str_t tmp2;
			struct input_result result;
			pjsip_generic_string_hdr refer_sub;
			pj_str_t STR_REFER_SUB = { "Refer-Sub", 9 };
			pj_str_t STR_FALSE = { "false", 5 };

			/* input destination. */
			if (get_input_url(tmp.ptr, tmp.slen, &result) < 0){
				API_error2("Destination input invalid");
				return API_ERROR;
			}

			pjsua_msg_data_init(&msg_data);
			if (app_config.no_refersub) {
				/* Add Refer-Sub: false in outgoing REFER request */
				pjsip_generic_string_hdr_init2(&refer_sub, &STR_REFER_SUB,
					&STR_FALSE);
				pj_list_push_back(&msg_data.hdr_list, &refer_sub);
			}

			if (result.nb_result != PJSUA_APP_NO_NB) {
				pjsua_buddy_info binfo;
				pjsua_buddy_get_info(result.nb_result - 1, &binfo);
				tmp2 = binfo.uri;
			}
			else if (result.uri_result) {
				tmp2 = pj_str(result.uri_result);
			}

			if ((status = pjsua_call_xfer(call_id, &tmp2, &msg_data)) != PJ_SUCCESS) {
				API_error("Error transfer call", status);
				return API_ERROR;
			}

		}
		else{
			API_error2("Call is not active");
			return API_ERROR;
		}
	}

	return API_SUCCESS;
}

/* Transfer call */
API_status_t API_transfer_replace_call(pjsua_call_id call_id, pjsua_call_id dest_call_id)
{
	pj_status_t status;

	if (call_id != PJSUA_INVALID_ID && dest_call_id != PJSUA_INVALID_ID) {
		if (pjsua_call_is_active(call_id) && pjsua_call_is_active(dest_call_id)){

			pjsip_generic_string_hdr refer_sub;
			pj_str_t STR_REFER_SUB = { "Refer-Sub", 9 };
			pj_str_t STR_FALSE = { "false", 5 };
			pjsua_call_id ids[PJSUA_MAX_CALLS];
			pjsua_msg_data msg_data_;
			unsigned count = PJ_ARRAY_SIZE(ids);
			pjsua_enum_calls(ids, &count);

			if (count <= 1) {
				API_error2("There are no other calls");
				return API_ERROR;
			}

			/* Check that destination call is valid. */
			if (dest_call_id == call_id) {
				API_error2("Destination call number must not be the "
				"same as the call being transferred");
				return API_ERROR;
			}

			if (dest_call_id >= PJSUA_MAX_CALLS) {		
				API_error2("Invalid destination call number");
				return API_ERROR;
			}

			if (!pjsua_call_is_active(dest_call_id) || 
				!pjsua_call_is_active(call_id)) {
				API_error2("One of the calls inactive");
				return API_ERROR;
			}

			pjsua_msg_data_init(&msg_data_);
			if (app_config.no_refersub) {
				/* Add Refer-Sub: false in outgoing REFER request */
				pjsip_generic_string_hdr_init2(&refer_sub, &STR_REFER_SUB,
					&STR_FALSE);
				pj_list_push_back(&msg_data_.hdr_list, &refer_sub);
			}


			if ((status = pjsua_call_xfer_replaces(call_id, dest_call_id,
				PJSUA_XFER_NO_REQUIRE_REPLACES, &msg_data_)) != PJ_SUCCESS) {
				API_error("Error transfer call", status);
				return API_ERROR;
			}

		}
		else{
			API_error2("Calls are not active");
			return API_ERROR;
		}
	}

	return API_SUCCESS;
}

/* Send DTMF (RFC2833) */
API_status_t API_dtmf(pjsua_call_id call_id, char *digits)
{

	pj_status_t status;
	pj_str_t _digits = pj_str(digits);

	if (call_id != PJSUA_INVALID_ID) {
		if (pjsua_call_is_active(call_id)){

			if (!pjsua_call_has_media(call_id)) {
				API_error2("Media is not established yet!");
				return API_ERROR;
			}

			if ((status = pjsua_call_dial_dtmf(call_id, &_digits)) != PJ_SUCCESS) {
				API_error("Error send dtmf", status);
				return API_ERROR;
			}
		}
		else{
			API_error2("Call is not active");
			return API_ERROR;
		}
	}

	return API_SUCCESS;
}

/* Service request */
API_status_t API_serviceReq(int callId, int serviceCode, char* destUri)
{
	int status = PJ_SUCCESS; //default status is ERROR!!
	
	switch (serviceCode)
	{
	case SC_3Pty:
	{
		//This is only local 3PTY that's all we have to do ....
		if ((status = API_call_reinvite(callId)) < 0){
			return API_ERROR;
		}
	}
		break;

	case SC_CFU:
	case SC_CFB:
	case SC_CFNR:
	case SC_Deflect:
	{
		//1.) build sip target Uri  
		pj_str_t contact_header_to_call = pj_str((char*)destUri);

		//2.) Fill pjsua_msg_data with correct Contact header ...
		pjsua_msg_data aStruct;
		pjsua_msg_data_init(&aStruct);//Initialize ...

		pjsip_generic_string_hdr warn;
		pj_str_t hname = pj_str("Contact");
		pj_str_t hvalue = contact_header_to_call;
		pjsip_generic_string_hdr_init2(&warn, &hname, &hvalue);
		warn.type = PJSIP_H_CONTACT;
		pj_list_push_back(&aStruct.hdr_list, &warn);

		//3.) Forward this call...
		//convert callId from abstract one (UI/CC) into concrete one (PJSIP) !!!
		if ((status = pjsua_call_hangup(callId, 302, NULL, &(aStruct))) != PJ_SUCCESS){
			API_error("Error service request", status);
			return API_ERROR;
		}
	}
		break;
	case SC_DND:
	{
		// sends 486 Busy here and releases this call instance ...
		if ((status = pjsua_call_hangup(callId, 486, NULL, NULL)) != PJ_SUCCESS){
			API_error("Error service request", status);
			return API_ERROR;
		}
	}
		break;
	}

	return API_SUCCESS;
}

/* Shutdown app */
API_status_t API_destroy(){

	pj_status_t status = PJ_TRUE;

	if ((status = pjsua_app_destroy()) != PJ_SUCCESS) {
		API_error("Error shutdown app", status);
		return API_ERROR;
	}

	/* This is on purpose */
	if ((status = pjsua_app_destroy()) != PJ_SUCCESS) {
		API_error("Error shutdown app", status);
		return API_ERROR;
	}

	return API_SUCCESS;
}



/* ------------------- MISC ------------------------------*/

/*
Not mine, taken from pjsua_app_cli.c,
This function will examine the input and will distinguish between buddy or regular call request.
*/
static int get_input_url(char *buf, pj_size_t len, struct input_result *result)
{
	static const pj_str_t err_invalid_input = { "Invalid input\n", 15 };
	result->nb_result = PJSUA_APP_NO_NB;
	result->uri_result = NULL;

	len = strlen(buf);

	/* Left trim */
	while (pj_isspace(*buf)) {
		++buf;
		--len;
	}

	/* Remove trailing newlines */
	while (len && (buf[len - 1] == '\r' || buf[len - 1] == '\n'))
		buf[--len] = '\0';

	if (len == 0 || buf[0] == 'q')
		return -1;

	if (pj_isdigit(*buf) || *buf == '-') {

		unsigned i;

		if (*buf == '-')
			i = 1;
		else
			i = 0;

		for (; i<len; ++i) {
			if (!pj_isdigit(buf[i])) {
				return -1;
			}
		}

		result->nb_result = my_atoi(buf);

		if (result->nb_result < 1 ||
			result->nb_result > (int)pjsua_get_buddy_count())
		{
			return -1;
		}
	}
	else {

		pj_status_t status;

		if ((status = pjsua_verify_url(buf)) != PJ_SUCCESS) {
			return -1;
		}

		result->uri_result = buf;
	}

	return 0;
}

/* ------------------- Callbacks ------------------------------*/

static void api_on_call_state(pjsua_call_id call_id, pjsip_event *e){

	if (pjsua_callbacks.on_call_state)
		pjsua_callbacks.on_call_state(call_id, e);

	if (api_callbacks.on_call_state)
		api_callbacks.on_call_state(call_id, e);
}

static void api_on_call_media_state(pjsua_call_id call_id){

	if (pjsua_callbacks.on_call_media_state)
		pjsua_callbacks.on_call_media_state(call_id);

	if (api_callbacks.on_call_media_state)
		api_callbacks.on_call_media_state(call_id);
}

static void api_on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id,
	pjsip_rx_data *rdata){

	if (pjsua_callbacks.on_incoming_call)
		pjsua_callbacks.on_incoming_call(acc_id, call_id, rdata);

	if (api_callbacks.on_incoming_call)
		api_callbacks.on_incoming_call(acc_id, call_id, rdata);
}

static void api_on_call_tsx_state(pjsua_call_id call_id,
	pjsip_transaction *tsx,
	pjsip_event *e){

	if (pjsua_callbacks.on_call_tsx_state)
		pjsua_callbacks.on_call_tsx_state(call_id, tsx, e);

	if (api_callbacks.on_call_tsx_state)
		api_callbacks.on_call_tsx_state(call_id, tsx, e);
}

static void api_on_dtmf_digit(pjsua_call_id call_id, int dtmf){

	if (pjsua_callbacks.on_dtmf_digit)
		pjsua_callbacks.on_dtmf_digit(call_id, dtmf);

	if (api_callbacks.on_dtmf_digit)
		api_callbacks.on_dtmf_digit(call_id, dtmf);
}

static pjsip_redirect_op api_on_call_redirected(pjsua_call_id call_id,
	const pjsip_uri *target,
	const pjsip_event *e){

	pjsip_redirect_op res = 0;

	if (pjsua_callbacks.on_call_redirected)
		res = pjsua_callbacks.on_call_redirected(call_id, target, e);

	if (api_callbacks.on_call_redirected)
		api_callbacks.on_call_redirected(call_id, target, e);

	return res;
}

static void api_on_reg_state(pjsua_acc_id acc_id){

	if (pjsua_callbacks.on_reg_state)
		pjsua_callbacks.on_reg_state(acc_id);

	if (api_callbacks.on_reg_state)
		api_callbacks.on_reg_state(acc_id);
}

static void api_on_incoming_subscribe(pjsua_acc_id acc_id,
	pjsua_srv_pres *srv_pres,
	pjsua_buddy_id buddy_id,
	const pj_str_t *from,
	pjsip_rx_data *rdata,
	pjsip_status_code *code,
	pj_str_t *reason,
	pjsua_msg_data *msg_data){

	if (pjsua_callbacks.on_incoming_subscribe)
		pjsua_callbacks.on_incoming_subscribe(acc_id, srv_pres, buddy_id,
		from, rdata, code, reason, msg_data);

	if (api_callbacks.on_incoming_subscribe)
		api_callbacks.on_incoming_subscribe(acc_id, srv_pres, buddy_id,
		from, rdata, code, reason, msg_data);
}

static void api_on_buddy_state(pjsua_buddy_id buddy_id){

	if (pjsua_callbacks.on_buddy_state)
		pjsua_callbacks.on_buddy_state(buddy_id);

	if (api_callbacks.on_buddy_state)
		api_callbacks.on_buddy_state(buddy_id);
}

static void api_on_buddy_evsub_state(pjsua_buddy_id buddy_id,
	pjsip_evsub *sub,
	pjsip_event *event){

	if (pjsua_callbacks.on_buddy_evsub_state)
		pjsua_callbacks.on_buddy_evsub_state(buddy_id, sub, event);

	if (api_callbacks.on_buddy_evsub_state)
		api_callbacks.on_buddy_evsub_state(buddy_id, sub, event);
}

static void api_on_pager(pjsua_call_id call_id, const pj_str_t *from,
	const pj_str_t *to, const pj_str_t *contact,
	const pj_str_t *mime_type, const pj_str_t *body){

	if (pjsua_callbacks.on_pager)
		pjsua_callbacks.on_pager(call_id, from, to, contact, mime_type, body);

	if (api_callbacks.on_pager)
		api_callbacks.on_pager(call_id, from, to, contact, mime_type, body);
}

static void api_on_typing(pjsua_call_id call_id, const pj_str_t *from,
	const pj_str_t *to, const pj_str_t *contact,
	pj_bool_t is_typing){

	if (pjsua_callbacks.on_typing)
		pjsua_callbacks.on_typing(call_id, from, to, contact, is_typing);

	if (api_callbacks.on_typing)
		api_callbacks.on_typing(call_id, from, to, contact, is_typing);
}

static void api_on_call_transfer_status(pjsua_call_id call_id,
	int st_code,
	const pj_str_t *st_text,
	pj_bool_t final,
	pj_bool_t *p_cont){

	if (pjsua_callbacks.on_call_transfer_status)
		pjsua_callbacks.on_call_transfer_status(call_id, st_code, st_text, final, p_cont);

	if (api_callbacks.on_call_transfer_status)
		api_callbacks.on_call_transfer_status(call_id, st_code, st_text, final, p_cont);
}

static void api_on_call_replaced(pjsua_call_id old_call_id,
	pjsua_call_id new_call_id){

	if (pjsua_callbacks.on_call_replaced)
		pjsua_callbacks.on_call_replaced(old_call_id, new_call_id);

	if (api_callbacks.on_call_replaced)
		api_callbacks.on_call_replaced(old_call_id, new_call_id);
}

static void api_on_nat_detect(const pj_stun_nat_detect_result *res){

	if (pjsua_callbacks.on_nat_detect)
		pjsua_callbacks.on_nat_detect(res);

	if (api_callbacks.on_nat_detect)
		api_callbacks.on_nat_detect(res);
}

static void api_on_mwi_info(pjsua_acc_id acc_id, pjsua_mwi_info *mwi_info){

	if (pjsua_callbacks.on_mwi_info)
		pjsua_callbacks.on_mwi_info(acc_id, mwi_info);

	if (api_callbacks.on_mwi_info)
		api_callbacks.on_mwi_info(acc_id, mwi_info);
}

static void api_on_transport_state(pjsip_transport *tp,
	pjsip_transport_state state,
	const pjsip_transport_state_info *info){

	if (pjsua_callbacks.on_transport_state)
		pjsua_callbacks.on_transport_state(tp, state, info);

	if (api_callbacks.on_transport_state)
		api_callbacks.on_transport_state(tp, state, info);
}

static void api_on_ice_transport_error(int index, pj_ice_strans_op op,
	pj_status_t status, void *param){

	if (pjsua_callbacks.on_ice_transport_error)
		pjsua_callbacks.on_ice_transport_error(index, op, status, param);

	if (api_callbacks.on_ice_transport_error)
		api_callbacks.on_ice_transport_error(index, op, status, param);
}

static pj_status_t api_on_snd_dev_operation(int operation){

	pj_status_t res = 0;

	if (pjsua_callbacks.on_snd_dev_operation)
		res = pjsua_callbacks.on_snd_dev_operation(operation);

	if (api_callbacks.on_snd_dev_operation)
		api_callbacks.on_snd_dev_operation(operation);

	return res;
}

static void api_on_call_media_event(pjsua_call_id call_id,
	unsigned med_idx,
	pjmedia_event *event){

	if (pjsua_callbacks.on_call_media_event)
		pjsua_callbacks.on_call_media_event(call_id, med_idx, event);

	if (api_callbacks.on_call_media_event)
		api_callbacks.on_call_media_event(call_id, med_idx, event);
}

#ifdef TRANSPORT_ADAPTER_SAMPLE
static void api_on_create_media_transport(pjsua_call_id call_id,
	unsigned media_idx,
	pjmedia_transport *base_tp,
	unsigned flags){

	if (pjsua_callbacks.on_create_media_transport)
		pjsua_callbacks.on_create_media_transport(call_id, media_idx, base_tp, flags);

	if (api_callbacks.on_create_media_transport)
		api_callbacks.on_create_media_transport(call_id, media_idx, base_tp, flags);
}
#endif