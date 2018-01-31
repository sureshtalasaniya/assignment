/**
 * The Initial Developer of the Original Code is
 * Suresh Talasaniya <Stalasaniya@netrixllc.com> [Netrixllc Worldwide Pvt Ltd]
 * 
 * Portions created by the Initial Developer are Copyright (C)
 * the Initial Developer. All Rights Reserved.
 * 
 * Contributor(s):
 * 
 * Suresh Talasaniya <Stalasaniya@netrixllc.com>
 * 
 * mod_routing.c  FreeSwitch routing
 * 
 */

//-------------------------------------------------------------------------------------//
//  To include Header Guard
//-------------------------------------------------------------------------------------//

#ifndef ROUTING_HEADER
#define ROUTING_HEADER

//-------------------------------------------------------------------------------------//
//  Include Header Files.
//-------------------------------------------------------------------------------------//

#include <switch.h> 

//-------------------------------------------------------------------------------------//
//  Function Prototype
//-------------------------------------------------------------------------------------//

SWITCH_MODULE_SHUTDOWN_FUNCTION(mod_routing_shutdown);
SWITCH_MODULE_RUNTIME_FUNCTION(mod_routing_runtime);
SWITCH_MODULE_LOAD_FUNCTION(mod_routing_load);

/* SWITCH_MODULE_DEFINITION(name, load, shutdown, runtime) 
 * Defines a switch_loadable_module_function_table_t and a static const char[] modname
 */
SWITCH_MODULE_DEFINITION(mod_routing, mod_routing_load, mod_routing_shutdown, NULL);

SWITCH_STANDARD_APP(switch_pbx_routing_app);


char banner[] = "\n"
"/**\n"
" * The Initial Developer of the Original Code is\n"
" * Suresh Talasaniya <Stalasaniya@netrixllc.com> [Netrixllc Worldwide Pvt Ltd].\n"
" * \n"
" * Contributor(s):\n"
" * \n"
" * Suresh Talasaniya <Stalasaniya@netrixllc.com> [Netrixllc Worldwide Pvt Ltd]\n"
" * \n"
" * mod_routing.c pbx FreeSwitch routing.\n"
" */\n";

static char *line = "++++++++++++++++++++++++++++++++++++++++++++++++++++++++";

/**
 * @var Configuration File name
 */

static const char *global_cf = "pbx_routing.conf";                  /*! Config File Name */

#define  SWITCH_PBX_SQLITE_DBNAME     "fusionpbx"        /*! DEFAULT DB NAME */

/**
 * @struct Global Configuartion structure.
 */

static struct {
	char *odbc_dsn;                          /*! ODBC DSN */
	char *dbname;                            /*! DBNAME */
	char *kam_odbc_dsn;                      /*! KAM ODBC DSN */
	char *kam_dbname;                        /*! KAM DBNAME */
	int count;
	char *sip_proxy_private;                 /*! Private SIP Proxy Server */
	char *sip_proxy_public;                  /*! Public SIP Proxy Server */
	switch_mutex_t *mutex;                   /*! MUTEX */  
	switch_memory_pool_t *pool;              /*! MEMORY POOL */
} globals;                                   /*! STRUCTURE VARIABLE */  

/**
 * @struct pbx routing Structure
 */

struct nvx_pbx_follow_me {
	char *follow_me_enabled;
	char *cid_name_prefix;
	char *cid_number_prefix;
};
typedef struct nvx_pbx_follow_me pbx_follow_me;

struct nvx_pbx_f_dst {
	char *follow_me_destination_uuid[15];
	char *follow_me_destination[15];
	char *follow_me_delay[15];
	char *follow_me_timeout[15];
	char *follow_me_prompt[15];
};
typedef struct nvx_pbx_f_dst pbx_f_dst;

struct nvx_pbx_dst_user_exist {
	char *extension_uuid;
	switch_channel_t *channel;
};
typedef struct nvx_pbx_dst_user_exist pbx_dst_user_exist;

struct nvx_pbx_call_block {
	char *call_block_uuid; 
	char *call_block_name; 
	char *call_block_number; 
	char *call_block_count; 
	char *call_block_action;
	switch_channel_t *channel;
};
typedef struct nvx_pbx_call_block pbx_call_block;




static switch_cache_db_handle_t *switch_get_db_handler(void);
static switch_bool_t switch_execute_sql_callback(switch_mutex_t *mutex, char *sql, switch_core_db_callback_func_t callback, void *pdata);
static char *switch_execute_sql2str(switch_mutex_t *mutex, char *sql, char *resbuf, size_t len);
static switch_cache_db_handle_t *switch_pbx_kam_db_handler(void);
// static switch_bool_t switch_pbx_kam_execute_sql_callback(switch_mutex_t *mutex, char *sql, switch_core_db_callback_func_t callback, void *pdata);
static char *switch_pbx_kam_execute_sql2str(switch_mutex_t *mutex, char *sql, char *resbuf, size_t len);

/**
 * @Function Get db handler from configuartion and connect db mysql. 
 * @return DB Handler.
 */

static switch_cache_db_handle_t *switch_pbx_kam_db_handler(void)
{
	switch_cache_db_handle_t *dbh = NULL;    /*! ODBC DSN HANDLER */
	char *dsn;                               /*! ODBC DSN STRING */

	/**
	 * @Section Load DSN and from config
	 */
	
	if (!zstr(globals.kam_odbc_dsn)) {
		dsn = globals.kam_odbc_dsn;
	} else {
		dsn = globals.kam_dbname;
	}
	
	/**
	 * @Function get odbc dsn db handler
	 */

	if (switch_cache_db_get_db_handle_dsn(&dbh, dsn) != SWITCH_STATUS_SUCCESS) {
		dbh = NULL;
	}

	return dbh;
}

/**
 * @Function Execute SQL with Callback Function 
 * @Return sql raw wise result 
 */

// static switch_bool_t switch_pbx_kam_execute_sql_callback(switch_mutex_t *mutex, char *sql, switch_core_db_callback_func_t callback, void *pdata)
// {
// 	switch_bool_t ret = SWITCH_FALSE;          /*! SQL Query Execution Status */
// 	char *errmsg = NULL;                       /*! SQL Query Execution ERROR String */
// 	switch_cache_db_handle_t *dbh = NULL;      /*! MySQL odbc dsn Handler */
// 
// 	/**
// 	 * @Section Process Synchronisation
// 	 */
// 	
// 	if (mutex) {
// 		switch_mutex_lock(mutex);
// 	} else {
// 		switch_mutex_lock(globals.mutex);
// 	}
// 
// 	/**
// 	 * @Function get db handler dsn object
// 	 */
// 	
// 	if (!(dbh = switch_pbx_kam_db_handler())) {
// 		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "[ TetRaVx ] : Error Opening DB\n");
// 		goto end;
// 	}
// 
// 	/**
// 	 * @Function Execute SQL in MYSQL and get result row wise in callback function
// 	 */
// 	
// 	switch_cache_db_execute_sql_callback(dbh, sql, callback, pdata, &errmsg);
// 	if (errmsg) {
// 		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "[ TetRaVx ] : SQL ERR: [%s] %s\n", sql, errmsg);
// 		free(errmsg);
// 	}
// 
// 	end:
// 
// 	/**
// 	 * @Function release dsn handler 
// 	 */
// 	
// 	switch_cache_db_release_db_handle(&dbh);
// 
// 	/**
// 	 * @Section Process Synchronisation
// 	 */
// 	
// 	if (mutex) {
// 		switch_mutex_unlock(mutex);
// 	} else {
// 		switch_mutex_unlock(globals.mutex);
// 	}
// 
// 	return ret;
// }

/**
 * @Function Execute MySQL Query and get result in string
 * @return resulting string
 */

static char *switch_pbx_kam_execute_sql2str(switch_mutex_t *mutex, char *sql, char *resbuf, size_t len)
{
	char *ret = NULL;                          /*! SQL Query Result */
	switch_cache_db_handle_t *dbh = NULL;      /*! MySQL ODBC DSN Handler */   

	/**
	 * @Section Process Synchronisation
	 */
	
	if (mutex) {
		switch_mutex_lock(mutex);
	} else {
		switch_mutex_lock(globals.mutex);
	}

	/**
	 * @Function get db handler dsn object
	 */
		
	if (!(dbh = switch_pbx_kam_db_handler())) {
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "[ TetRaVx ] : Error Opening DB\n");
		goto end;
	}

	/**
	 * @function Execute SQL in MySQL and get result.
	 */
	
	ret = switch_cache_db_execute_sql2str(dbh, sql, resbuf, len, NULL);

	end:
	
	/**
	 * @function release db handler.
	 */
	
	switch_cache_db_release_db_handle(&dbh);

	/**
	 * @Section Process Synchronisation.
	 */
	
	if (mutex) {
		switch_mutex_unlock(mutex);
	} else {
		switch_mutex_unlock(globals.mutex);
	}

	//return SQL result.
	return ret;
}

/**
 * @Function Get db handler from configuartion and connect db mysql. 
 * @return DB Handler.
 */

static switch_cache_db_handle_t *switch_get_db_handler(void)
{
	switch_cache_db_handle_t *dbh = NULL;    /*! ODBC DSN HANDLER */
	char *dsn;                               /*! ODBC DSN STRING */

	/**
	 * @Section Load DSN and from config
	 */
	
	if (!zstr(globals.odbc_dsn)) {
		dsn = globals.odbc_dsn;
	} else {
		dsn = globals.dbname;
	}
	
	/**
	 * @Function get odbc dsn db handler
	 */

	if (switch_cache_db_get_db_handle_dsn(&dbh, dsn) != SWITCH_STATUS_SUCCESS) {
		dbh = NULL;
	}

	return dbh;
}

/**
 * @Function Execute SQL with Callback Function 
 * @Return sql raw wise result 
 */

static switch_bool_t switch_execute_sql_callback(switch_mutex_t *mutex, char *sql, switch_core_db_callback_func_t callback, void *pdata)
{
	switch_bool_t ret = SWITCH_FALSE;          /*! SQL Query Execution Status */
	char *errmsg = NULL;                       /*! SQL Query Execution ERROR String */
	switch_cache_db_handle_t *dbh = NULL;      /*! MySQL odbc dsn Handler */

	/**
	 * @Section Process Synchronisation
	 */
	
	if (mutex) {
		switch_mutex_lock(mutex);
	} else {
		switch_mutex_lock(globals.mutex);
	}

	/**
	 * @Function get db handler dsn object
	 */
	
	if (!(dbh = switch_get_db_handler())) {
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "[ TetRaVx ] : Error Opening DB\n");
		goto end;
	}

	/**
	 * @Function Execute SQL in MYSQL and get result row wise in callback function
	 */
	
	switch_cache_db_execute_sql_callback(dbh, sql, callback, pdata, &errmsg);
	if (errmsg) {
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "[ TetRaVx ] : SQL ERR: [%s] %s\n", sql, errmsg);
		free(errmsg);
	}

	end:

	/**
	 * @Function release dsn handler 
	 */
	
	switch_cache_db_release_db_handle(&dbh);

	/**
	 * @Section Process Synchronisation
	 */
	
	if (mutex) {
		switch_mutex_unlock(mutex);
	} else {
		switch_mutex_unlock(globals.mutex);
	}

	return ret;
}

/**
 * @Function Execute MySQL Query and get result in string
 * @return resulting string
 */

static char *switch_execute_sql2str(switch_mutex_t *mutex, char *sql, char *resbuf, size_t len)
{
	char *ret = NULL;                          /*! SQL Query Result */
	switch_cache_db_handle_t *dbh = NULL;      /*! MySQL ODBC DSN Handler */   

	/**
	 * @Section Process Synchronisation
	 */
	
	if (mutex) {
		switch_mutex_lock(mutex);
	} else {
		switch_mutex_lock(globals.mutex);
	}

	/**
	 * @Function get db handler dsn object
	 */
		
	if (!(dbh = switch_get_db_handler())) {
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "[ TetRaVx ] : Error Opening DB\n");
		goto end;
	}

	/**
	 * @function Execute SQL in MySQL and get result.
	 */
	
	ret = switch_cache_db_execute_sql2str(dbh, sql, resbuf, len, NULL);

	end:
	
	/**
	 * @function release db handler.
	 */
	
	switch_cache_db_release_db_handle(&dbh);

	/**
	 * @Section Process Synchronisation.
	 */
	
	if (mutex) {
		switch_mutex_unlock(mutex);
	} else {
		switch_mutex_unlock(globals.mutex);
	}

	//return SQL result.
	return ret;
}


static switch_status_t load_config(void)
{
	switch_status_t status = SWITCH_STATUS_SUCCESS;     /*! Loading Config Status */
	switch_xml_t cfg, xml, settings, param;             /*! XML Config Params */

	/**
	 * @Section Open Configuration File To Read configuartion
	 */
	
	if (!(xml = switch_xml_open_cfg(global_cf, &cfg, NULL))) {
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "[ TetRaVx ] : Open of %s failed\n", global_cf);
		status = SWITCH_STATUS_TERM;
		goto end;
	}
	
	switch_mutex_lock(globals.mutex);
	
	/**
	 * @Section Load Configuartion into Global Structure
	 */
	
	if ((settings = switch_xml_child(cfg, "settings"))) {
		
		/**
		 * @Section Load config param into variable
		 */
	    
		for (param = switch_xml_child(settings, "param"); param; param = param->next) {
			char *var = (char *) switch_xml_attr_soft(param, "name");
			char *val = (char *) switch_xml_attr_soft(param, "value");

			// Load dbname in variable
			if (!strcasecmp(var, "dbname")) {
				globals.dbname = strdup(val);
				switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG5, "[ TetRaVx ] : dbname [ %s ]\n", globals.dbname);
			} else if (!strcasecmp(var, "odbc-dsn")) {
				globals.odbc_dsn = strdup(val);
				switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG5, "[ TetRaVx ] : odbc-dsn [ %s ]\n", globals.odbc_dsn);
			} else if (!strcasecmp(var, "sip-proxy-private")) {
				globals.sip_proxy_private = strdup(val);
				switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG5, "[ TetRaVx ] : Private SIP Proxy [ %s ]\n", globals.sip_proxy_private);
			} else if (!strcasecmp(var, "sip-proxy-public")) {
				globals.sip_proxy_public = strdup(val);
				switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG5, "[ TetRaVx ] : Public SIP Proxy [ %s ]\n", globals.sip_proxy_public);
			} else if (!strcasecmp(var, "kam-dbname")) {
				globals.kam_dbname = strdup(val);
				switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG5, "[ TetRaVx ] : Kamailio dbname [ %s ]\n", globals.kam_dbname);
			} else if (!strcasecmp(var, "kam-odbc-dsn")) {
				globals.kam_odbc_dsn = strdup(val);
				switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG5, "[ TetRaVx ] : Kamailio odbc-dsn [ %s ]\n", globals.kam_odbc_dsn);
			}
		}
	}

	/**
	 * @var Default Database name
	 */
	
	if (!globals.dbname) {
		//use default db name if not specify in configure
		globals.dbname = strdup(SWITCH_PBX_SQLITE_DBNAME);
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "[ TetRaVx ] : set default DB :%s\n", globals.dbname);
	}
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "[ TetRaVx ] : Loaded Configuration successfully\n");
	
	end:
	switch_mutex_unlock(globals.mutex);
	
	//free memory
	if (xml) {
		switch_xml_free(xml);
	}

	return status;
}


/**
 * @function pbx routing information.
 */
static int switch_follow_me_callback(void *ptr, int argc, char **argv, char **col) 
{
	pbx_follow_me *pbx = (pbx_follow_me *)ptr;         /*! FOLLOW ME STRUCTURE POINTER */
	int index = 0;                                    /*! FOLLOW ME Query Coloum */

	/**
	 * @Section Log on console for DEBUG5 
	 */
	
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG5, "[ TetRaVx ] : %s\n", line);
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG5, "[ TetRaVx ] : ======== PBX FOLLOW ME DATA ========\n");
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG5, "[ TetRaVx ] : %s\n", line);

	for(index = 0; index < argc ; index++) {
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG5, "[ TetRaVx ] : %s : %s\n", col[index], argv[index]);
	}
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG5, "[ TetRaVx ] : %s\n", line);
	
	/* -------------------------------------------------------------- */
	
	/**
	 * @var copy pbx routing data in structure.
	 */

	pbx->follow_me_enabled = strdup(argv[0]);           /*! Follow me status */
	pbx->cid_name_prefix = strdup(argv[1]);             /*! Follow me cid name */
	pbx->cid_number_prefix = strdup(argv[2]);           /*! Follow me cid number */

	/* -------------------------------------------------------------- */
	
	return SWITCH_STATUS_SUCCESS;
}

/**
 * @function pbx routing information.
 */
static int switch_follow_me_dst_callback(void *ptr, int argc, char **argv, char **col) 
{
	pbx_f_dst *pbx = (pbx_f_dst *)ptr;         /*! FOLLOW ME STRUCTURE POINTER */
	int index = 0;                                    /*! FOLLOW ME Query Coloum */

	/**
	 * @Section Log on console for DEBUG5 
	 */
	
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG5, "[ TetRaVx ] : %s\n", line);
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG5, "[ TetRaVx ] : ======== PBX FOLLOW ME DESTINATION DATA ========\n");
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG5, "[ TetRaVx ] : %s\n", line);

	for(index = 0; index < argc ; index++) {
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG5, "[ TetRaVx ] : %s : %s\n", col[index], argv[index]);
	}
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG5, "[ TetRaVx ] : %s\n", line);
	
	/* -------------------------------------------------------------- */
	
	/**
	 * @var copy pbx routing data in structure.
	 */
	
	pbx->follow_me_destination_uuid[globals.count] = strdup(argv[0]);           /*! Follow me status */
	pbx->follow_me_destination[globals.count] = strdup(argv[1]);             /*! Follow me cid name */
	pbx->follow_me_delay[globals.count] = strdup(argv[2]);           /*! Follow me cid number */
	pbx->follow_me_timeout[globals.count] = strdup(argv[3]);           /*! Follow me cid number */
	pbx->follow_me_prompt[globals.count] = strdup(argv[4]);           /*! Follow me cid number */
	
	globals.count++;
	
	/* -------------------------------------------------------------- */
	
	return SWITCH_STATUS_SUCCESS;
}

static void switch_pbx_set_channel_variable(char *var, char *val, switch_channel_t *channel) {
// 	if(switch_true(val)) {
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG5, "[ TetRaVx ] : Set Channel Variable [ %s : %s ]\n", var, val);
		switch_channel_set_variable(channel, var, val);
// 	}
}

/**
 * @function pbx routing information.
 */
static int switch_destination_exists_callback(void *ptr, int argc, char **argv, char **col) 
{
	pbx_dst_user_exist *pbx = (pbx_dst_user_exist *)ptr;         /*! DESTINATION EXT STRUCTURE POINTER */
	int index = 0;                             

	/**
	 * @Section Log on console for DEBUG5 
	 */
	
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG5, "[ TetRaVx ] : %s\n", line);
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG5, "[ TetRaVx ] : ======== EXTENSION DATA ========\n");
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG5, "[ TetRaVx ] : %s\n", line);

	for(index = 0; index < argc ; index++) {
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG5, "[ TetRaVx ] : %s : %s\n", col[index], argv[index]);
		switch_pbx_set_channel_variable(col[index], argv[index], pbx->channel);
	}
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG5, "[ TetRaVx ] : %s\n", line);
	
	/* -------------------------------------------------------------- */
	
	/**
	 * @var copy pbx routing data in structure.
	 */
	
	pbx->extension_uuid = strdup(argv[0]);           /*! Destination UUID */
	/* -------------------------------------------------------------- */
	
	return SWITCH_STATUS_SUCCESS;
}


/**
 * @function pbx routing information.
 */
static int switch_source_exists_callback(void *ptr, int argc, char **argv, char **col) 
{
	pbx_dst_user_exist *pbx = (pbx_dst_user_exist *)ptr;         /*! DESTINATION EXT STRUCTURE POINTER */
	int index = 0;                             

	/**
	 * @Section Log on console for DEBUG5 
	 */
	
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG5, "[ TetRaVx ] : %s\n", line);
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG5, "[ TetRaVx ] : ======== EXTENSION DATA ========\n");
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG5, "[ TetRaVx ] : %s\n", line);

	for(index = 0; index < argc ; index++) {
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG5, "[ TetRaVx ] : %s : %s\n", col[index], argv[index]);
		
		if(index == 3) {
			switch_pbx_set_channel_variable("from_user_record", argv[index], pbx->channel);
		} else {
			switch_pbx_set_channel_variable(col[index], argv[index], pbx->channel);
		}
	}
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG5, "[ TetRaVx ] : %s\n", line);
	
	/* -------------------------------------------------------------- */
	
	/**
	 * @var copy pbx routing data in structure.
	 */
	
	pbx->extension_uuid = strdup(argv[0]);           /*! Destination UUID */
	/* -------------------------------------------------------------- */
	
	return SWITCH_STATUS_SUCCESS;
}


/**
 * @FUNCTION : Check Destination Number And Set Channel Variables.
 */
static int switch_pbx_dst_type(char *destination_number, char *domain_uuid, switch_channel_t *channel) {
	int ret_val = 0;    /*! Return 0 means Local Extension. Return 1 means PSTN Number */
	char *sql = NULL;
	pbx_dst_user_exist dst_user;
	
	memset(&dst_user, 0, sizeof(dst_user));        /*! Initializing structure variable */
	dst_user.channel = channel;
	dst_user.extension_uuid = "";

	/**
	 * @QUERY : GET Destination Extension Number Data
	 */
	sql = switch_mprintf("SELECT extension_uuid, number_alias, accountcode, effective_caller_id_name,  effective_caller_id_number,  outbound_caller_id_name,  outbound_caller_id_number, emergency_caller_id_name,  emergency_caller_id_number,  directory_full_name,  directory_visible,  directory_exten_visible,  limit_max as dst_limit_max,  limit_destination,  missed_call_app,  missed_call_data,    call_timeout,call_group,  call_screen_enabled,  user_record,  hold_music,do_not_disturb,  forward_all_destination,  forward_all_enabled,  forward_busy_destination,  forward_busy_enabled,  forward_no_answer_destination,  forward_no_answer_enabled,  forward_user_not_registered_destination,  forward_user_not_registered_enabled,  follow_me_uuid, forward_caller_id_uuid,absolute_codec_string FROM v_extensions WHERE domain_uuid = '%s' and extension = '%s' and enabled = 'true'", domain_uuid, destination_number);

	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG5, "[ TetRaVx ] : Destination Number SQL : \n%s\n", sql);
	switch_execute_sql_callback(globals.mutex, sql, switch_destination_exists_callback, &dst_user);
	switch_safe_free(sql);

	/**
	 * @VALUE : To Check Destination Number is LOCAL/PSTN
	 */
	if(!zstr(dst_user.extension_uuid)) {
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG, "[ TetRaVx ] : Destination Number [ %s ] is Local\n", destination_number);
		switch_pbx_set_channel_variable("user_exists", "true", channel);
		ret_val = 0;
	} else {
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG, "[ TetRaVx ] : Destination Number [ %s ] is PSTN\n", destination_number);
		switch_pbx_set_channel_variable("user_exists", "false", channel);
		ret_val = 1;
	}
	return ret_val;
}

/**
 * @FUNCTION : Check Caller Id Number And Set Channel Variables.
 */
static int switch_pbx_caller_type(char *caller_id_number, char *domain_uuid, switch_channel_t *channel) {
	int ret_val = 0;    /*! Return 0 means Local Extension. Return 1 means PSTN Number */
	char *sql = NULL;
	pbx_dst_user_exist dst_user;
	
	memset(&dst_user, 0, sizeof(dst_user));        /*! Initializing structure variable */
	dst_user.channel = channel;
	dst_user.extension_uuid = "";

	/**
	 * @QUERY : Get Source Extension Number Data
	 */
	sql = switch_mprintf("SELECT extension_uuid ,extension,call_group ,user_record, hold_music,effective_caller_id_name, effective_caller_id_number,outbound_caller_id_name,outbound_caller_id_number, emergency_caller_id_name, emergency_caller_id_number, directory_first_name, directory_last_name ,directory_visible,directory_exten_visible, limit_max as caller_max_limit, call_timeout ,limit_destination, toll_allow FROM  v_extensions WHERE domain_uuid = '%s' and extension = '%s' and enabled = 'true'", domain_uuid, caller_id_number);

	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG5, "[ TetRaVx ] : Destination Number SQL : \n%s\n", sql);
	switch_execute_sql_callback(globals.mutex, sql, switch_source_exists_callback, &dst_user);
	switch_safe_free(sql);

	switch_pbx_set_channel_variable("record_stereo", "true", channel);
	switch_pbx_set_channel_variable("transfer_fallback_extension", "operator", channel);
	switch_pbx_set_channel_variable("export_vars", "domain_name", channel);
	switch_pbx_set_channel_variable("sip_from_user", caller_id_number, channel);
	switch_pbx_set_channel_variable("sip_from_number", caller_id_number, channel);
	
	/**
	 * @VALUE : To Check Source Number is LOCAL/PSTN
	 */
	if(!zstr(dst_user.extension_uuid)) {
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG, "[ TetRaVx ] : Source Number [ %s ] is Local\n", caller_id_number);
		switch_pbx_set_channel_variable("from_user_exists", "true", channel);
		ret_val = 0;
	} else {
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG, "[ TetRaVx ] : Source Number [ %s ] is PSTN\n", caller_id_number);
		switch_pbx_set_channel_variable("from_user_exists", "false", channel);
		ret_val = 1;
	}
	return ret_val;
}


	
/**
 * @function Call Blcok.
 */
static int switch_callblock_callback(void *ptr, int argc, char **argv, char **col) 
{
	pbx_call_block *pbx = (pbx_call_block *)ptr;         /*! DESTINATION EXT STRUCTURE POINTER */
	int index = 0;                             

	/**
	 * @Section Log on console for DEBUG5 
	 */
	
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG5, "[ TetRaVx ] : %s\n", line);
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG5, "[ TetRaVx ] : ======== Call Block Data ========\n");
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG5, "[ TetRaVx ] : %s\n", line);

	for(index = 0; index < argc ; index++) {
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG5, "[ TetRaVx ] : %s : %s\n", col[index], argv[index]);
// 		switch_pbx_set_channel_variable(col[index], argv[index], pbx->channel);
	}
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG5, "[ TetRaVx ] : %s\n", line);
	
	/* -------------------------------------------------------------- */
	
	/**
	 * @var copy pbx routing data in structure.
	 */
	
	pbx->call_block_uuid = strdup(argv[0]);           /*! Call Block UUID */
	pbx->call_block_name = strdup(argv[1]);           /*! Call Block Name */
	pbx->call_block_number = strdup(argv[2]);         /*! Call Block Number */
	pbx->call_block_count = strdup(argv[3]);          /*! Call Block Count */
	pbx->call_block_action = strdup(argv[4]);         /*! Call Block Action */

	/* -------------------------------------------------------------- */
	
	return SWITCH_STATUS_SUCCESS;
}



static int switch_pbx_ext_max_limit(char *backend, switch_core_session_t *session, char *realm, char *id, int max , int interval, char *xfer_exten) 
{
    int ret = 0;
    switch_channel_t *channel = switch_core_session_get_channel(session);

	if (switch_limit_incr(backend, session,  realm, id, max, interval) != SWITCH_STATUS_SUCCESS) {
	    if (*xfer_exten == '!') {
		    switch_channel_hangup(channel, switch_channel_str2cause(xfer_exten + 1));
	    } else {
		    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "[ TetRaVx ] : Your resource limit is exceeded...!!!\n");
		    ret = -1;
		    goto end;
	    }
    }
    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "[ TetRaVx ] : ********* LIMIT SET SUCCESSFULLY *********\n");
    
end:
	return ret;    
}


/**
 * @FUNCTION : PBX APPLICATION FUNCTION DEFINATION.
 */
SWITCH_STANDARD_APP(switch_pbx_routing_app)
{
	/**
	 * @var Declared pbx routing application variables.
	 */

	char *destination_number = NULL;       /*! Destination Number */
	char *caller_id_number = NULL;         /*! Caller Id Number */
	char *mydata = NULL;                   /*! Application Arg Data */
	char *app_data = NULL;
	char *uuid = NULL;                     /*! Call UUID */
	char *domain_uuid = NULL;              /*! Domain UUID */
	char *domain_name = NULL;              /*! Domain Name */
	char *extension_uuid = NULL;           /*! Extension UUID */
	
	/**
	 * @var get channel from session
	 */
	switch_channel_t *channel = switch_core_session_get_channel(session);
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG, "[ TetRaVx ] : Free Switch Routing Started.\n");
	
	/**
	 * @Section Handler if channel is not available and respond originater with FACILITY_NOT_IMPLEMENTED	 
	 */
	
	if(!channel) {
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "[ TetRaVx ] : Hey I am unable to get channel..!!\n");
		switch_channel_set_variable(channel, "pbx_sip_code", "501");
		switch_channel_set_variable(channel, "pbx_sip_hangup_cause", "FACILITY_NOT_IMPLEMENTED");
		switch_channel_set_variable(channel, "transfer_string", "TetRaVx_PBX_HANGUP XML public");
		goto end;
	}

	/**
	 * @Section Get routing Application Data
	 */
	mydata = switch_core_session_strdup(session, data);
	if (!mydata) {
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "[ TetRaVx ] : Application Can't execute without data\n");
		switch_channel_set_variable(channel, "pbx_sip_code", "501");
		switch_channel_set_variable(channel, "pbx_sip_hangup_cause", "FACILITY_NOT_IMPLEMENTED");
		switch_channel_set_variable(channel, "transfer_string", "TetRaVx_PBX_HANGUP XML public");
		goto end;
	}
	app_data = switch_str_nil(switch_mprintf("%s", mydata));
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "[ TetRaVx ] : Application Data : %s\n", app_data);

	/**
	 * @var Get Destination Number from channel variable ${destination_number}
	 */
	destination_number = switch_str_nil(switch_mprintf("%s", switch_str_nil((char *)switch_channel_get_variable(channel, "destination_number"))));
	caller_id_number = switch_str_nil(switch_mprintf("%s", switch_str_nil((char *)switch_channel_get_variable(channel, "caller_id_number"))));

	/**
	 * @Section Get Channel Variables.
	 */
	uuid = switch_str_nil(switch_mprintf("%s", switch_str_nil((char *)switch_channel_get_variable(channel, "uuid"))));
	domain_uuid = switch_str_nil(switch_mprintf("%s", switch_str_nil((char *)switch_channel_get_variable(channel, "domain_uuid"))));
	domain_name = switch_str_nil(switch_mprintf("%s", switch_str_nil((char *)switch_channel_get_variable(channel, "domain_name"))));
	extension_uuid = switch_str_nil(switch_mprintf("%s", switch_str_nil((char *)switch_channel_get_variable(channel, "extension_uuid"))));

	if(!zstr(domain_name)) {
		char *sql = NULL;
		char result[250] = "";
		
		sql = switch_mprintf("SELECT domain_uuid FROM v_domains WHERE domain_name = '%s'", domain_name);
		switch_execute_sql2str(NULL, sql, result, sizeof(result));
		switch_safe_free(sql);
		
		if(zstr(result)) {
			switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "[ TetRaVx ] : Domain UUID Is Empty\n");
			switch_channel_set_variable(channel, "pbx_sip_code", "501");
			switch_channel_set_variable(channel, "pbx_sip_hangup_cause", "FACILITY_NOT_IMPLEMENTED");
			switch_channel_set_variable(channel, "transfer_string", "TetRaVx_PBX_HANGUP XML public");
			goto end;
		}
		domain_uuid = switch_mprintf(result);
	}
	
	/**
	 * @Log On Console
	 */
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "[ TetRaVx ] : Destination Number : %s\n", destination_number);
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "[ TetRaVx ] : Caller Id Number : %s\n", caller_id_number);
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "[ TetRaVx ] : Call UUID : %s\n", uuid);
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "[ TetRaVx ] : Domain UUID : %s\n", domain_uuid);
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "[ TetRaVx ] : Domain Name : %s\n", domain_name);
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "[ TetRaVx ] : Extension UUID : %s\n", extension_uuid);

	/**
	 * @ROUTING Application For FOLLOW-ME
	 */
	
	if(!strcasecmp(app_data, "FOLLOWME")) {
		char *follow_me_uuid = NULL;  /*! Follow Me UUID */
		char *sql = NULL;
		pbx_follow_me pbxfollowme;
		
		memset(&pbxfollowme, 0, sizeof(pbxfollowme));        /*! Initializing structure variable */
		
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "[ TetRaVx ] : !!! Routing for follow_me application !!!!\n");
		follow_me_uuid = switch_str_nil(switch_mprintf("%s", switch_str_nil((char *)switch_channel_get_variable(channel,"follow_me_uuid"))));
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "[ TetRaVx ] : follow_me_uuid : %s\n", follow_me_uuid);
		
		/**
		 * @QUERY :  SELECT Data From DB For FOLLOW-ME Application
		 */
		sql = switch_mprintf("SELECT follow_me_enabled, cid_name_prefix, cid_number_prefix FROM v_follow_me WHERE domain_uuid = '%s' AND follow_me_uuid = '%s'", domain_uuid, follow_me_uuid);
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG5, "[ TetRaVx ] : Follow Me Data SQL : \n%s\n", sql);
		switch_execute_sql_callback(globals.mutex, sql, switch_follow_me_callback, &pbxfollowme);
		switch_safe_free(sql);
		
		if(zstr(pbxfollowme.follow_me_enabled)) {
			switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "[ TetRaVx ] : No Follow me data\n");
			switch_channel_set_variable(channel, "transfer_string", "TetRaVx_PBX_HANGUP XML public");
			goto end;
		}
		
		/**
		 * @STATUS : To Check FOLLOW-ME Enabled/Disabled
		 */
		if(!strcmp(pbxfollowme.follow_me_enabled,"false")) {
			switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "[ TetRaVx ] : Follow me is disabled.\n");
// 			switch_channel_set_variable(channel, "transfer_string", "TetRaVx_PBX_HANGUP XML TetRaVx_PBX_HANGUP");
// 			goto end;
		}
		
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG, "[ TetRaVx ] : follow_me_enabled : %s\n", pbxfollowme.follow_me_enabled);
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG, "[ TetRaVx ] : cid_name_prefix : %s\n", pbxfollowme.cid_name_prefix);
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG, "[ TetRaVx ] : cid_number_prefix : %s\n", pbxfollowme.cid_number_prefix);
	
		/**
		 * @Section : To Process For FOLLOW-ME If Enabled.
		 */
		if(!strcmp(pbxfollowme.follow_me_enabled,"true")) {
			pbx_f_dst pbxfmedst;
			int c_dst_num_list = 0;
			
			memset(&pbxfmedst, 0, sizeof(pbxfmedst));        /*! Initializing structure variable */
			switch_mutex_lock(globals.mutex);
			globals.count = 0;

			/**
			 * @QUERY : To SELECT Data From DB For FOLLOW-ME Destination 
			 */
			sql = switch_mprintf("SELECT follow_me_destination_uuid, follow_me_destination, follow_me_delay, follow_me_timeout, follow_me_prompt, follow_me_order FROM v_follow_me_destinations where follow_me_uuid='%s' AND domain_uuid='%s'  ORDER BY follow_me_order ASC", follow_me_uuid,domain_uuid);
			switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG5, "[ TetRaVx ] : Get Follow me Destination Data SQL : \n%s\n", sql);
			switch_execute_sql_callback(globals.mutex, sql, switch_follow_me_dst_callback, &pbxfmedst);
			switch_safe_free(sql);

			c_dst_num_list =  globals.count;
			globals.count = 0;
			switch_mutex_unlock(globals.mutex);
			if(c_dst_num_list > 0) {
				int i = 0;
				char *dial_string = NULL, *temp_dial_string = NULL;
				char app_data[500] = "";
				char *pbx_str = NULL;
			
				/**
				 * @STRING : To Make Bridge Data String
				 */
				temp_dial_string = switch_mprintf("{sip_contact_user = '%s',sip_h_X-domain_uuid = '%s',sip_h_X-FROMFS='yes',fail_on_single_reject=USER_BUSY,instant_ringback=true,ignore_early_media=true,domain_uuid='%s',sip_invite_domain='%s',domain_name='%s',domain='%s',extension_uuid='%s',group_confirm_key=exec,group_confirm_file=lua confirm.lua,sip_h_X-accountcode='%s',accountcode='%s',toll_allow='%s'}",caller_id_number,domain_uuid, domain_uuid, domain_name, domain_name, domain_name, extension_uuid,caller_id_number, caller_id_number, switch_str_nil((char *)switch_channel_get_variable(channel, "toll_allow")));

				/**
				 * @Loop : Check Each Destination Number is Local/PSTN Number and Create Bridge Data.
				 */
				
				for (i = 0; i < c_dst_num_list ; i++) {
					char *group_confirm_key = NULL;
					char *tmp_app_data = NULL;
					int pbx_dst_type = 0;
					char *sip_proxy = NULL;
					char *x_call_type = "";
					char *do_not_disturb = NULL;
					
					if(atoi(pbxfmedst.follow_me_prompt[i]) == 1) {
						group_confirm_key = switch_mprintf("group_confirm_key=exec,group_confirm_file=lua confirm.lua,confirm=true");
					} else {
						group_confirm_key = switch_mprintf("");
					}

					/*! @To Check Destination Number is Local/PSTN */
					pbx_dst_type = switch_pbx_dst_type(pbxfmedst.follow_me_destination[i], domain_uuid, channel);
					if(pbx_dst_type == 0) {
						sip_proxy = switch_mprintf(globals.sip_proxy_public);
						x_call_type = switch_mprintf("EXT");
					} else {
						sip_proxy = switch_mprintf(globals.sip_proxy_private);
						x_call_type = switch_mprintf("PSTN");
					}
					
					if(!zstr(do_not_disturb) &&  !strcmp(do_not_disturb, "true")) {
						switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "[ TetRaVx ] : Destination Number is in DND.\n");
// 						switch_channel_set_variable(channel, "transfer_string", pbx_str);
// 						switch_safe_free(pbx_str);
						continue;
					}

		
					tmp_app_data = switch_mprintf("[sip_h_X-Call_Type = '%s',presence_id='%s@%s','%s',leg_delay_start='%s',leg_timeout='%s']sofia/internal/%s@%s;fs_path=sip:%s" ,x_call_type, pbxfmedst.follow_me_destination[i],domain_name, group_confirm_key, pbxfmedst.follow_me_delay[i], pbxfmedst.follow_me_timeout[i],pbxfmedst.follow_me_destination[i],domain_name,sip_proxy);

					strcat(app_data, tmp_app_data); 
					if(i+1 < c_dst_num_list) {
						strcat(app_data, ","); 
					}
					switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG5, "[ TetRaVx ] : app_data : %s\n", app_data);
					
					switch_safe_free(x_call_type);
					switch_safe_free(tmp_app_data);
					switch_safe_free(group_confirm_key);
					switch_safe_free(sip_proxy);
				}
				/**
				 * @Section : To Set Channel Variables.
				 */
				
				dial_string = switch_mprintf("%s%s",temp_dial_string,app_data);
				switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG5, "[ TetRaVx ] : dial_string : %s\n", dial_string);
				pbx_str = switch_mprintf("%s XML %s", destination_number, domain_name);
				switch_channel_export_variable(channel, "sofia_bridge_str", dial_string, SWITCH_BRIDGE_EXPORT_VARS_VARIABLE);
				switch_channel_set_variable(channel, "sofia_bridge_str", dial_string);
				switch_channel_set_variable(channel, "transfer_string", pbx_str);
				switch_channel_export_variable(channel, "transfer_string", pbx_str, SWITCH_BRIDGE_EXPORT_VARS_VARIABLE);
				
				/*! Free Dynamic Allocated Memory */
				switch_safe_free(pbx_str);
				switch_safe_free(temp_dial_string);
				switch_safe_free(dial_string);
			} else {
				switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "[ TetRaVx ] : Follow me Destination Not Found.\n");
				switch_channel_set_variable(channel, "transfer_string", "TetRaVx_PBX_HANGUP XML public");
				goto end;
			}
		}
	} else if(!strcasecmp(app_data, "PBX")) {
		char *call_direction = NULL;
		int pbx_dst_type = 0, pbx_usr_type = 0;
		char *sip_proxy = NULL;
		char *x_call_type = NULL;
		char *do_not_disturb = NULL;
		char *pbx_str = NULL;
		char *follow_me_uuid = NULL;
		char *user_record = NULL;
		char *from_user_record = NULL;
		char *forward_all_enabled = NULL;
		char *emergency_caller_id_name = NULL;
		char *outbound_caller_id_name = NULL;
		char *path = "";
		char *sql = NULL;
		char *caller_proto = NULL;
		char *media_webrtc = NULL;
		int call_recording = 0;
		pbx_call_block cblock;
		
		call_direction = switch_str_nil(switch_mprintf("%s", switch_str_nil((char *)switch_channel_get_variable(channel, "call_direction"))));
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "[ TetRaVx ] : !!!!! Executing PBX ROUTING !!!!!\n");
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "[ TetRaVx ] : Call Direction : %s\n", call_direction);
		if(zstr(call_direction)) {
			switch_pbx_set_channel_variable("call_direction", "local", channel);
		}
	
		/**!
		 * @SET : SET Channel Variables.
		 */
		switch_pbx_set_channel_variable("dialed_user", caller_id_number, channel);
		switch_pbx_set_channel_variable("caller_destination", destination_number, channel);
		switch_pbx_set_channel_variable("caller_id_name", caller_id_number, channel);
		switch_pbx_set_channel_variable("caller_id_number", caller_id_number, channel);
		switch_pbx_set_channel_variable("origination_callee_id_name", destination_number, channel);
		switch_pbx_set_channel_variable("limit_ignore_transfer", "true", channel);
// 		switch_pbx_set_channel_variable("RFC2822_DATE", "${strftime(%a, %d %b %Y %T %z)}", channel);
		
 		/*! @To Check Destination Number is Local/PSTN */
		pbx_usr_type = switch_pbx_caller_type(caller_id_number, domain_uuid, channel);
		if(pbx_usr_type == 0) {
			int retval = 0;
			char *caller_max_limit = NULL;
			switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "[ TetRaVx ] : Caller User is Local.\n");
			
			caller_max_limit = switch_str_nil(switch_mprintf("%s", switch_str_nil((char *)switch_channel_get_variable(channel, "caller_max_limit"))));
			if (!zstr(caller_max_limit) && atoi(caller_max_limit) > 0) {
				char *idname = switch_mprintf("%s_%s", caller_id_number, domain_name);
				retval = switch_pbx_ext_max_limit("hiredis", session, "devnvx", idname, atoi(caller_max_limit), 0, "EXTENSION_LIMIT_EXCEEDED"); 
				if(retval != 0) {
					switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "[ TetRaVx ] : Caller Id Number [ %s ] Max Call Limit Exceeded.\n", caller_id_number);
					pbx_str = switch_mprintf("%s XML %s", "PBX_EXT_LIMIT_EXCEEDED", domain_name);
					switch_channel_export_variable(channel, "destination_number", "PBX_EXT_LIMIT_EXCEEDED", SWITCH_BRIDGE_EXPORT_VARS_VARIABLE);
					switch_channel_export_variable(channel, "reset_limit", idname, SWITCH_BRIDGE_EXPORT_VARS_VARIABLE);
					switch_safe_free(pbx_str);
					switch_safe_free(idname);
					goto end;
				}
				switch_safe_free(idname);
			}
			switch_safe_free(caller_max_limit);
		} else {
			switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "[ TetRaVx ] : Caller User is PSTN.\n");
		}
	
		/*! @To Check Destination Number is Local/PSTN */
		pbx_dst_type = switch_pbx_dst_type(destination_number, domain_uuid, channel);
		if(pbx_dst_type == 0) {
			int retval = 0;
			char *dst_limit_max = NULL;

			sip_proxy = switch_mprintf(globals.sip_proxy_public);
			x_call_type = switch_mprintf("EXT");
			
			dst_limit_max = switch_str_nil(switch_mprintf("%s", switch_str_nil((char *)switch_channel_get_variable(channel, "dst_limit_max"))));
			if (!zstr(dst_limit_max) && atoi(dst_limit_max) > 0) {
				char *idname = switch_mprintf("%s_%s", destination_number, domain_name);
				retval = switch_pbx_ext_max_limit("hiredis", session, "devnvx", idname, atoi(dst_limit_max), 0, "EXTENSION_LIMIT_EXCEEDED"); 
				if(retval != 0) {
					switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "[ TetRaVx ] : Destination Number [ %s ] Max Call Limit Exceeded.\n", destination_number);
					pbx_str = switch_mprintf("%s XML %s", "PBX_EXT_LIMIT_EXCEEDED", domain_name);
					switch_channel_export_variable(channel, "destination_number", "PBX_EXT_LIMIT_EXCEEDED", SWITCH_BRIDGE_EXPORT_VARS_VARIABLE);
					switch_channel_export_variable(channel, "reset_limit", idname, SWITCH_BRIDGE_EXPORT_VARS_VARIABLE);
					switch_channel_set_variable(channel, "transfer_string", pbx_str);
					
					switch_safe_free(pbx_str);
					switch_safe_free(idname);
					goto end;
				}
				switch_safe_free(idname);
			}
			switch_safe_free(dst_limit_max);
			
			
		} else {
			sip_proxy = switch_mprintf(globals.sip_proxy_private);
			x_call_type = switch_mprintf("PSTN");
		}
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "[ TetRaVx ] : SIP Proxy : %s\n", sip_proxy);
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "[ TetRaVx ] : Destination Number is : %s\n", x_call_type);

		memset(&cblock, 0, sizeof(cblock));        /*! Initializing structure variable */
		/**
		 * @QUERY : To SELECT Data From DB For FOLLOW-ME Destination 
		 */
		sql = switch_mprintf("SELECT call_block_uuid, call_block_name, call_block_number, call_block_count, call_block_action  FROM v_call_block WHERE call_block_number = '%s' AND domain_uuid = '%s' AND call_block_enabled='true'", caller_id_number, domain_uuid); 
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG5, "[ TetRaVx ] : Check Caller Id Number in Call Block SQL : \n%s\n", sql);
		switch_execute_sql_callback(globals.mutex, sql, switch_callblock_callback, &cblock);
		switch_safe_free(sql);
			
		if(!zstr(cblock.call_block_uuid)) {
			char *pbx_str = NULL;
			switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "[ TetRaVx ] : Caller Id Number is in Call Block List.\n");
			if(!zstr(cblock.call_block_action) && !strcmp(cblock.call_block_action, "Reject") ) {
				pbx_str = switch_mprintf("%s XML %s", "PBX_HANGUP", domain_name);
				switch_channel_export_variable(channel, "destination_number", "PBX_HANGUP", SWITCH_BRIDGE_EXPORT_VARS_VARIABLE);
				switch_pbx_set_channel_variable("pbx_hangup", "CALL_REJECTED", channel);
			} else if(!zstr(cblock.call_block_action) && !strcmp(cblock.call_block_action, "Busy") ) {
				pbx_str = switch_mprintf("%s XML %s", "PBX_HANGUP", domain_name);
				switch_channel_export_variable(channel, "destination_number", "PBX_HANGUP", SWITCH_BRIDGE_EXPORT_VARS_VARIABLE);
				switch_pbx_set_channel_variable("pbx_hangup", "USER_BUSY", channel);
	
			} else if(!zstr(cblock.call_block_action) && !strcmp(cblock.call_block_action, "Hold") ) {
				pbx_str = switch_mprintf("%s XML %s", "*9664", domain_name);
				switch_channel_export_variable(channel, "destination_number", "*9664", SWITCH_BRIDGE_EXPORT_VARS_VARIABLE);
				
			} else if(!zstr(cblock.call_block_action) && !strncmp(cblock.call_block_action, "Voicemail", 9) ) {
				pbx_str = switch_mprintf("%s XML %s", "pbx_voicemail", domain_name);
				switch_channel_export_variable(channel, "destination_number", "pbx_voicemail", SWITCH_BRIDGE_EXPORT_VARS_VARIABLE);
			}
			switch_channel_set_variable(channel, "transfer_string", pbx_str);
			switch_safe_free(pbx_str);
			goto end;
		}

		/**
		 * @SECTION : To Check DND On Extension 
		 */
		do_not_disturb = switch_str_nil(switch_mprintf("%s", switch_str_nil((char *)switch_channel_get_variable(channel, "do_not_disturb"))));
		if(!zstr(do_not_disturb) &&  !strcmp(do_not_disturb, "true")) {
			pbx_str = switch_mprintf("DO_NOT_ALLOW XML %s", domain_name);
			switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "[ TetRaVx ] : Destination Number is in DND.\n");
			switch_channel_set_variable(channel, "transfer_string", pbx_str);
			switch_channel_set_variable(channel, "destination_number", "DO_NOT_ALLOW");
			switch_safe_free(pbx_str);
			goto end;
		}
		
		/**
		 * @SECTION : To Check FOLLOW-ME On Extension.
		 */
		follow_me_uuid = switch_str_nil(switch_mprintf("%s", switch_str_nil((char *)switch_channel_get_variable(channel, "follow_me_uuid"))));
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "[ TetRaVx ] : Follow Me UUID is : %s\n", follow_me_uuid);
		if(!zstr(follow_me_uuid)) {
			char *sql = NULL;
			char result[25] = "";
			
			sql = switch_mprintf("SELECT follow_me_enabled FROM v_follow_me WHERE domain_uuid = '%s' AND follow_me_uuid = '%s'", domain_uuid, follow_me_uuid);
			switch_execute_sql2str(NULL, sql, result, sizeof(result));
			switch_safe_free(sql);
			if(!zstr(result) && !strcmp(result, "true")) {
				switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "[ TetRaVx ] : Follow Me UUID is : %s\n", follow_me_uuid);
				switch_core_session_execute_application(session,"routing", "FOLLOWME");
				goto end;
			}
		}
		
		/**
		 * @SECTION : To Check Call Recording On Destination Number
		 */
		user_record = switch_str_nil(switch_mprintf("%s", switch_str_nil((char *)switch_channel_get_variable(channel, "user_record"))));
		if( (pbx_dst_type == 0 && !strcasecmp(user_record, "all")) || (pbx_dst_type == 0 && !strcasecmp(user_record, "inbound") && !strcasecmp(call_direction,"inbound") ) || (pbx_dst_type == 0 && !strcasecmp(user_record, "outbound") && !strcasecmp(call_direction,"outbound")) || (pbx_dst_type == 0 && !strcasecmp(user_record, "local") && !strcasecmp(call_direction,"local"))) {
			switch_channel_set_variable(channel, "record_session", "true");
			switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "[ TetRaVx ] : Destination Number Call Recording is enabled\n");
			call_recording = 1;
		}

		/**
		 * @SECTION : To Check Call Recording On Source Number
		 */
		from_user_record = switch_str_nil(switch_mprintf("%s", switch_str_nil((char *)switch_channel_get_variable(channel, "from_user_record"))));
		
		if( (pbx_usr_type == 0 && !strcasecmp(from_user_record, "all")) || (pbx_usr_type == 0 && !strcasecmp(from_user_record, "inbound") && !strcasecmp(call_direction,"inbound") ) || (pbx_usr_type == 0 && !strcasecmp(from_user_record, "outbound") && !strcasecmp(call_direction,"outbound")) || (pbx_usr_type == 0 && !strcasecmp(from_user_record, "local") && !strcasecmp(call_direction,"local"))) {
			switch_channel_set_variable(channel, "record_session", "true");
			switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "[ TetRaVx ] : Caller Id Number Call Recording is enabled\n");
			call_recording = 1;
		}

		/**
		 * @SECTION : To Set Call Recording Path
		 */
		if (call_recording == 1) {
			path = switch_mprintf("api_on_answer='uuid_record %s start /usr/local/freeswitch/recordings/%s/archive/%s.wav'", uuid, domain_name,uuid);
		}

		/**
		 * @SECTION : To Check Always Call Forwarding is Configured on Extension
		 */
		forward_all_enabled = switch_str_nil(switch_mprintf("%s", switch_str_nil((char *)switch_channel_get_variable(channel, "forward_all_enabled"))));
		if(!strcasecmp(forward_all_enabled, "true")) {
			char *forward_all_destination = switch_str_nil(switch_mprintf("%s", switch_str_nil((char *)switch_channel_get_variable(channel, "forward_all_enabled"))));
			switch_channel_set_variable(channel, "destination_number", forward_all_destination);
			destination_number = switch_mprintf("%s",forward_all_destination);
			switch_safe_free(forward_all_destination);
		}

		caller_proto = switch_str_nil(switch_mprintf("%s", switch_str_nil((char *)switch_channel_get_variable(channel, "sip_h_X-Caller-Proto"))));
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "[ TetRaVx ] : Caller Protocol : %s\n", caller_proto);
		
		if(!strcmp(caller_proto,"WebRTC")) {
			media_webrtc=switch_mprintf("media_webrtc=true");
		} else if(!strcmp(caller_proto,"SIP")) {
			media_webrtc=switch_mprintf("media_webrtc=false");
		} else {
			media_webrtc=switch_mprintf("media_webrtc=false");
		}

		if(!strcmp(x_call_type,"EXT")) {
			char *sql = NULL;
			char result[20] = "";
			
			sql = switch_mprintf("SELECT id from location where contact like '%%transport=ws%%' AND username='%s' AND domain='%s'", destination_number,domain_name);
			switch_pbx_kam_execute_sql2str(NULL, sql, result, sizeof(result));
			switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG5, "[ TetRaVx ] : EXT Location SQL : %s\n", sql);
			switch_safe_free(sql);
			if(!zstr(result)) {
				switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "[ TetRaVx ] : Extension Registered on WebRTC Client : %s\n", destination_number);
				media_webrtc=switch_mprintf("media_webrtc=true");
			} else {
				media_webrtc=switch_mprintf("media_webrtc=false");
			}
		} else {
			media_webrtc=switch_mprintf("media_webrtc=false");
		}
	
		
		
		/**
		 * @SECTION : Local Extension Call 
		 */
		
		if(pbx_dst_type == 0 && strcmp(forward_all_enabled, "true")) {
			char *dial_string = switch_mprintf("{%s,sip_h_X-domain_uuid = '%s',sip_h_X-Call_Type = 'EXT',sip_h_X-FROMFS='yes',sip_contact_user='%s','%s'}sofia/internal/%s@%s;fs_path=sip:192.216.107.162:9000", path,domain_uuid, caller_id_number, media_webrtc,destination_number, domain_name);
	
			switch_channel_set_variable(channel, "sofia_bridge_str", dial_string);
			switch_channel_export_variable(channel, "sofia_bridge_str", dial_string, SWITCH_BRIDGE_EXPORT_VARS_VARIABLE);
			
			switch_safe_free(media_webrtc);
		} else {
			/**
			 * @SECTION : PSTN Call
			 */
			
			char *dial_string = switch_mprintf("{%s,sip_h_X-Call_Type='PSTN',sip_h_X-FROMFS='yes',sip_contact_user='%s'}sofia/internal/%s@%s;fs_path=sip:172.28.230.126:9001", path,  caller_id_number, destination_number, domain_name);
			switch_channel_export_variable(channel, "sofia_bridge_str", dial_string, SWITCH_BRIDGE_EXPORT_VARS_VARIABLE);
			switch_channel_set_variable(channel, "sofia_bridge_str", dial_string);
		}
		
		/**
		 * @SECTION : To Set Channel Variables.
		 */
		switch_pbx_set_channel_variable("dialed_extension", destination_number, channel);
		emergency_caller_id_name = switch_str_nil(switch_mprintf("%s", switch_str_nil((char *)switch_channel_get_variable(channel, "emergency_caller_id_name"))));
		if(zstr(emergency_caller_id_name)) {
			switch_pbx_set_channel_variable("emergency_caller_id_name", switch_str_nil(switch_mprintf("%s", switch_str_nil((char *)switch_channel_get_variable(channel, "default_emergency_caller_id_name")))), channel);
			switch_pbx_set_channel_variable("emergency_caller_id_number", switch_str_nil(switch_mprintf("%s", switch_str_nil((char *)switch_channel_get_variable(channel, "default_emergency_caller_id_number")))), channel);
		}
		outbound_caller_id_name = switch_str_nil(switch_mprintf("%s", switch_str_nil((char *)switch_channel_get_variable(channel, "outbound_caller_id_name"))));
		if(zstr(outbound_caller_id_name)) {
			switch_pbx_set_channel_variable("outbound_caller_id_name", switch_str_nil(switch_mprintf("%s", switch_str_nil((char *)switch_channel_get_variable(channel, "default_outbound_caller_id_name")))), channel);
			switch_pbx_set_channel_variable("outbound_caller_id_number", switch_str_nil(switch_mprintf("%s", switch_str_nil((char *)switch_channel_get_variable(channel, "default_outbound_caller_id_number")))), channel);
		}
		
		if(!zstr(domain_name)) {
			char *xfer_str = NULL;
			xfer_str = switch_mprintf("%s XML %s", destination_number, domain_name);
			switch_channel_set_variable(channel, "transfer_string", xfer_str);
			switch_safe_free(xfer_str);
		}
		/**
		 * @SECTION : To Free Dynamic Memory
		 */
		switch_safe_free(outbound_caller_id_name);
		switch_safe_free(emergency_caller_id_name);
		switch_safe_free(forward_all_enabled);
		switch_safe_free(user_record);
		switch_safe_free(from_user_record);
		switch_safe_free(call_direction);
		switch_safe_free(do_not_disturb);
	}

end:
	
	if (app_data)
		switch_safe_free(app_data);
	
	switch_safe_free(domain_uuid);
	
	return ; 
}

/**
 * @API : PBX ROUTING API DEFINATION 
 */
SWITCH_STANDARD_API(switch_pbx_routing_api)
{
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG, "[ TetRaVx ] : FreeSWITCH API Module.\n");
	return SWITCH_STATUS_SUCCESS;
}

/**
 * @Function Loading module in kernal of FreeSWITCH
 */

SWITCH_MODULE_LOAD_FUNCTION(mod_routing_load)
{

	switch_application_interface_t *app_interface;        /* application interface object */
	switch_status_t status;                               /* Loading module status */
	switch_api_interface_t *api_interface;

	/**
	 * @Section initialize configuration structure.
	 */
	
	memset(&globals, 0, sizeof(globals));
	globals.pool = pool;

	/**
	 * @Section mutex initializing 
	 */
	
	switch_mutex_init(&globals.mutex, SWITCH_MUTEX_NESTED, globals.pool);

	/**
	 * @Section Loading Configuartion from config file
	 */
	
	if ((status = load_config()) != SWITCH_STATUS_SUCCESS) {
		return status;
	}
	
	/**
	 * @Section create module interface.
	 */
	
	*module_interface = switch_loadable_module_create_module_interface(pool, modname);

	/**
	 * @Section Log on console on Loading module
	 */
	
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG, "[ TetRaVx ] : Loading PBX Routing Module.\n");
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "[ TetRaVx ] : ======================================\n");
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG, "%s", banner);
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "[ TetRaVx ] : ======================================\n");
	
	/**
	 * @Section Define a routing application <action application="routing" data=""/> 
	 */
	
	SWITCH_ADD_APP(app_interface, "routing", "routing", "FreeSWITCH PBX Routing", switch_pbx_routing_app, NULL, SAF_SUPPORT_NOMEDIA | SAF_ROUTING_EXEC);
  
	//Define routing api
	SWITCH_ADD_API(api_interface, "pbx_routing_config", "FreeSWITCH PBX Routing", switch_pbx_routing_api, NULL);
	
	return SWITCH_STATUS_SUCCESS;
}

/**
 * @Function execute on module unload
 */

SWITCH_MODULE_SHUTDOWN_FUNCTION(mod_routing_shutdown)
{
	/**
	 * @SECTION : On Shutdown Free Dynamic Memory.
	 */
	switch_mutex_lock(globals.mutex);
	
	switch_safe_free(globals.odbc_dsn);
	switch_safe_free(globals.dbname);
	
	switch_mutex_unlock(globals.mutex);
	
	return SWITCH_STATUS_SUCCESS;
}
// End Header Guard.
#endif
